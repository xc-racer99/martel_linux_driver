/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : usb.c
* DESCRIPTION   : MARTEL library - usb ACM communication routines
* CVS           : $Id: serial.c,v 1.1 2006/08/01 09:12:23 chris Exp $
*******************************************************************************
*   Copyright (C) 2006  MARTEL Instruments Ltd
*
*   This file is part of libmartel.
*
*   libmartel is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public
*   License as published by the Free Software Foundation; either
*   version 2.1 of the License, or (at your option) any later version.
*
*   libmartel is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with libmartel; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************
* HISTORY       :
*   27-Jul-06   CML     Initial revision
*   26-Jan-15   CML     USB based on serial.c
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/serial.h>
#include <linux/version.h>

#include <martel/martel.h>
#include <martel/martel-private.h>

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: usb.c,v 1.1 2006/08/01 09:12:23 chris Exp $";

static  void    usb_timer(int);

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  usb_timer
Purpose   :  Custom signal handler for SIGALRM signals
Inputs    :  signum : signal number
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void usb_timer(int signum)
{
        /* this function trmartel SIGALRM signals
         * this function does nothing
         */
}

/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  usb_get_uri
Purpose   :  Get URI string defining usb port
Inputs    :  p    : port structure
             uri  : URI string buffer
             size : URI string buffer size (includes trailing zero)
Outputs   :  Fills URI string buffer
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_get_uri(martel_port_t *p,char *uri,int size)
{
        martel_error_t errnum;
        int len;

        len = snprintf(uri,size,"martel:%s?type=usb",
                        p->set.serial.device);

        if (len>=size) {
                errnum = MARTEL_INVALID_URI;
        }
        else {
                errnum = MARTEL_OK;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  usb_create
Purpose   :  Create usb port from device. Initialize settings to defaults
Inputs    :  p      : port structure
             device : device name
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_create(martel_port_t *p,const char *device)
{
        /*copy device name*/
        if (strlen(device)>sizeof(p->set.serial.device)-1) {
                return MARTEL_NAME_TOO_LONG;
        }

        strcpy(p->set.serial.device,device);

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  usb_create_from_uri
Purpose   :  Create usb port from URI string
Inputs    :  p   : port structure
             uri : URI string
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_create_from_uri(martel_port_t *p,struct martel_uri *su)
{
        martel_error_t errnum;
        const char *device;
        const char *value;

        /*set device parameter*/
        device = uri_get_device(su);

        if (device==NULL) {
                return MARTEL_INVALID_URI;
        }
        if ((errnum = usb_create(p,device))<0) {
                return errnum;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  usb_open
Purpose   :  Open usb port
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_open(martel_port_t *p)
{
        int fd;
        martel_error_t errnum;
        struct termios set;

        /*open device
         * O_RDWR       open for read and write operations
         * O_NOCTTY     program does not want to be controlling terminal
         * O_NONBLOCK   use non-blocking system calls
         * O_EXCL       open in exclusive mode
         */
        if ((fd = open(p->set.serial.device,O_RDWR|O_NOCTTY|O_NONBLOCK|O_EXCL))<0) {
                return MARTEL_OPEN_FAILED;
        }

        p->set.serial.fd = fd;

        /*setup default usb settings*/
        if (tcgetattr(fd,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        cfmakeraw(&set);

        /*set 8 bits, no parity, enable receiver*/
        set.c_cflag &= ~PARENB;
        set.c_cflag &= ~CSTOPB;
        set.c_cflag &= ~CSIZE;
        set.c_cflag |= CS8;
        set.c_cflag |= CLOCAL|CREAD;

        if (tcsetattr(fd,TCSANOW,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  usb_close
Purpose   :  Close usb port
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_close(martel_port_t *p)
{
        martel_error_t errnum;

        /*close device*/
        if (close(p->set.serial.fd)<0) {
                errnum = MARTEL_CLOSE_FAILED;
        }
        else {
                errnum = MARTEL_OK;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  usb_write
Purpose   :  Write data buffer to usb port
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_write(martel_port_t *p,const void *buf,int size)
{
        martel_error_t errnum = MARTEL_OK;
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(p->set.serial.fd,&fds);

        while (size) {
                int n;

                /*wait until some room is available in kernel write buffer*/
                if (p->write_timeout==0) {
                        n = select(p->set.serial.fd+1,NULL,&fds,NULL,NULL);
                }
                else {
                        struct timeval tv;

                        tv.tv_sec = p->write_timeout/1000;
                        tv.tv_usec = (p->write_timeout%1000)*1000;

                        n = select(p->set.serial.fd+1,NULL,&fds,NULL,&tv);
                }

                if (n<0) {
                        errnum = MARTEL_WRITE_FAILED;
                        break;
                }
                else if (n==0) {
                        errnum = MARTEL_WRITE_TIMEOUT;
                        break;
                }

                /*write some bytes*/
                n = write(p->set.serial.fd,buf,size);

                if (n<0) {
                        errnum = MARTEL_WRITE_FAILED;
                        break;
                }
                else {
                        buf += n;
                        size -= n;
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  usb_write_rt
Purpose   :  Write data buffer to serial port with handshaking disabled
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_write_rt(martel_port_t *p,const void *buf,int size)
{
        martel_error_t errnum;
        
        /*flush pending input and output data*/
        if ((errnum = usb_flush(p))<0) {
                return errnum;
        }

        /*write data*/
        /*transmission starts immediatly because we flushed output buffer*/
        if ((errnum = usb_write(p,buf,size))<0) {
                return errnum;
        }

        /*wait until data is completely transmitted*/
        if ((errnum = usb_sync(p))<0) {
                return errnum;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  usb_read
Purpose   :  Read data buffer from usb port
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  Fills data buffer
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_read(martel_port_t *p,void *buf,int size)
{
        martel_error_t errnum = MARTEL_OK;
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(p->set.serial.fd,&fds);

        while (size) {
                int n;

                /*wait until some bytes are available in kernel read buffer*/
                if (p->read_timeout==0) {
                        n = select(p->set.serial.fd+1,&fds,NULL,NULL,NULL);
                }
                else {
                        struct timeval tv;

                        tv.tv_sec = p->read_timeout/1000;
                        tv.tv_usec = (p->read_timeout%1000)*1000;

                        n = select(p->set.serial.fd+1,&fds,NULL,NULL,&tv);
                }

                if (n<0) {
                        errnum = MARTEL_READ_FAILED;
                        break;
                }
                else if (n==0) {
                        errnum = MARTEL_READ_TIMEOUT;
                        break;
                }

                /*read some bytes*/
                n = read(p->set.serial.fd,buf,size);

                if (n<0) {
                        errnum = MARTEL_READ_FAILED;
                        break;
                }
                else {
                        buf += n;
                        size -= n;
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  usb_sync
Purpose   :  Block until all data pending in output buffer are transmitted
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_sync(martel_port_t *p)
{
        martel_error_t errnum;
        struct sigaction sa,old_sa;
        struct itimerval timer;

        if (p->write_timeout!=0) {
                /*install signal handler*/
                memset(&sa,0,sizeof(sa));
                sa.sa_handler = &usb_timer;
                sigaction(SIGALRM,&sa,&old_sa);

                /*start timer*/
                timer.it_interval.tv_sec = 0;
                timer.it_interval.tv_usec = 0;
                timer.it_value.tv_sec = p->write_timeout/1000;
                timer.it_value.tv_usec = (p->write_timeout%1000)*1000;

                setitimer(ITIMER_REAL,&timer,NULL);
        }

        if (tcdrain(p->set.serial.fd)<0) {
                if (errno==EINTR) {
                        errnum = MARTEL_WRITE_TIMEOUT;
                }
                else {
                        errnum = MARTEL_SYNC_FAILED;
                }
        }
        else {
                errnum = MARTEL_OK;
        }

        if (p->write_timeout!=0) {
                /*stop timer*/
                memset(&timer,0,sizeof(timer));
                setitimer(ITIMER_REAL,&timer,NULL);

                /*uninstall signal handler*/
                sigaction(SIGALRM,&old_sa,NULL);
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  usb_flush
Purpose   :  Clear input and output buffers
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int usb_flush(martel_port_t *p)
{
        martel_error_t errnum;

        if (tcflush(p->set.serial.fd,TCIOFLUSH)<0) {
                errnum = MARTEL_FLUSH_FAILED;
        }
        else {
                errnum = MARTEL_OK;
        }

        return errnum;
}

