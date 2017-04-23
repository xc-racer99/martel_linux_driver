/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : serial.c
* DESCRIPTION   : MARTEL library - serial communication routines
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
static const char id_str[] = "$Id: serial.c,v 1.1 2006/08/01 09:12:23 chris Exp $";

#define SERIAL_DEFBAUDRATE      MARTEL_B9600
#define SERIAL_DEFHANDSHAKE     MARTEL_RTSCTS

static  void    serial_timer(int);

static  const char *    baudrate_to_string(int);
static  const char *    handshake_to_string(int);

static  int     string_to_baudrate(const char *);
static  int     string_to_handshake(const char *);

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  serial_timer
Purpose   :  Custom signal handler for SIGALRM signals
Inputs    :  signum : signal number
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void serial_timer(int signum)
{
        /* this function trmartel SIGALRM signals
         * this function does nothing
         */
}

/*-----------------------------------------------------------------------------
Name      :  baudrate_to_string
Purpose   :  Convert baudrate parameter to string
Inputs    :  baudrate : baudrate setting
Outputs   :  <>
Return    :  string representation of baudrate or "unknown"
-----------------------------------------------------------------------------*/
static const char *baudrate_to_string(int baudrate)
{
        const char *s;
        
        switch (baudrate) {
        case MARTEL_B1200:
                s = "1200";
                break;
        case MARTEL_B2400:
                s = "2400";
                break;
        case MARTEL_B4800:
                s = "4800";
                break;
        case MARTEL_B9600:
                s = "9600";
                break;
        case MARTEL_B19200:
                s = "19200";
                break;
        case MARTEL_B38400:
                s = "38400";
                break;
        case MARTEL_B57600:
                s = "57600";
                break;
        case MARTEL_B115200:
                s = "115200";
                break;
        default:
                s = "unknown";
                break;
        }

        return s;
}

/*-----------------------------------------------------------------------------
Name      :  string_to_baudrate
Purpose   :  Convert string to baudrate parameter
Inputs    :  s : string
Outputs   :  <>
Return    :  baudrate parameter (martel_baudrate_t) or error code
-----------------------------------------------------------------------------*/
static int string_to_baudrate(const char *s)
{
        int baudrate;

        if (strcmp(s,"1200")==0) {
                baudrate = MARTEL_B1200;
        }
        else if (strcmp(s,"2400")==0) {
                baudrate = MARTEL_B2400;
        }
        else if (strcmp(s,"4800")==0) {
                baudrate = MARTEL_B4800;
        }
        else if (strcmp(s,"9600")==0) {
                baudrate = MARTEL_B9600;
        }
        else if (strcmp(s,"19200")==0) {
                baudrate = MARTEL_B19200;
        }
        else if (strcmp(s,"38400")==0) {
                baudrate = MARTEL_B38400;
        }
        else if (strcmp(s,"57600")==0) {
                baudrate = MARTEL_B57600;
        }
        else if (strcmp(s,"115200")==0) {
                baudrate = MARTEL_B115200;
        }
        else {
                baudrate = MARTEL_INVALID_BAUDRATE;
        }
        
        return baudrate;
}

/*-----------------------------------------------------------------------------
Name      :  handshake_to_string
Purpose   :  Convert handshake parameter to string
Inputs    :  handshake : handshake setting
Outputs   :  <>
Return    :  string representation of handshake or "unknown"
-----------------------------------------------------------------------------*/
static const char *handshake_to_string(int handshake)
{
        const char *s;

        switch (handshake) {
        case MARTEL_NONE:
                s = "none";
                break;
        case MARTEL_RTSCTS:
                s = "rtscts";
                break;
        case MARTEL_XONXOFF:
                s = "xonxoff";
                break;
        default:
                s = "unknown";
                break;
        }

        return s;
}

/*-----------------------------------------------------------------------------
Name      :  string_to_handshake
Purpose   :  Convert string to handshake parameter
Inputs    :  s : string
Outputs   :  <>
Return    :  handshake parameter (martel_handshake_t) or error code
-----------------------------------------------------------------------------*/
static int string_to_handshake(const char *s)
{
        int handshake;

        if (strcmp(s,"none")==0) {
                handshake = MARTEL_NONE;
        }
        else if (strcmp(s,"rtscts")==0) {
                handshake = MARTEL_RTSCTS;
        }
        else if (strcmp(s,"xonxoff")==0) {
                handshake = MARTEL_XONXOFF;
        }
        else {
                handshake = MARTEL_INVALID_HANDSHAKE;
        }
        
        return handshake;
}

/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  serial_get_uri
Purpose   :  Get URI string defining serial port
Inputs    :  p    : port structure
             uri  : URI string buffer
             size : URI string buffer size (includes trailing zero)
Outputs   :  Fills URI string buffer
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_get_uri(martel_port_t *p,char *uri,int size)
{
        martel_error_t errnum;
        int len;

        len = snprintf(uri,size,"martel:%s?type=serial+baudrate=%s+handshake=%s",
                        p->set.serial.device,
                        baudrate_to_string(p->set.serial.baudrate),
                        handshake_to_string(p->set.serial.handshake));

        if (len>=size) {
                errnum = MARTEL_INVALID_URI;
        }
        else {
                errnum = MARTEL_OK;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  serial_create
Purpose   :  Create serial port from device. Initialize settings to defaults
Inputs    :  p      : port structure
             device : device name
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_create(martel_port_t *p,const char *device)
{
        /*copy device name*/
        if (strlen(device)>sizeof(p->set.serial.device)-1) {
                return MARTEL_NAME_TOO_LONG;
        }

        strcpy(p->set.serial.device,device);

        /*initialize default settings*/
        p->set.serial.baudrate = SERIAL_DEFBAUDRATE;
        p->set.serial.handshake = SERIAL_DEFHANDSHAKE;

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  serial_create_from_uri
Purpose   :  Create serial port from URI string
Inputs    :  p   : port structure
             uri : URI string
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_create_from_uri(martel_port_t *p,struct martel_uri *su)
{
        martel_error_t errnum;
        const char *device;
        const char *value;

        /*set device parameter*/
        device = uri_get_device(su);

        if (device==NULL) {
                return MARTEL_INVALID_URI;
        }
        if ((errnum = serial_create(p,device))<0) {
                return errnum;
        }

        /*set baudrate parameter (if it exists)*/
        value = uri_get_opt(su,"baudrate");

        if (value!=NULL) {
                if ((errnum = string_to_baudrate(value))<0) {
                        return errnum;
                }
                else {
                        p->set.serial.baudrate = errnum;
                }
        }
        
        /*set handshake parameter (if it exists)*/
        value = uri_get_opt(su,"handshake");

        if (value!=NULL) {
                if ((errnum = string_to_handshake(value))<0) {
                        return errnum;
                }
                else {
                        p->set.serial.handshake = errnum;
                }
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  serial_open
Purpose   :  Open serial port
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_open(martel_port_t *p)
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

        /*setup default serial settings*/
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

        if ((errnum = serial_set_baudrate(p,p->set.serial.baudrate))<0) {
                return errnum;
        }

        if ((errnum = serial_set_handshake(p,p->set.serial.handshake))<0) {
                return errnum;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  serial_close
Purpose   :  Close serial port
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_close(martel_port_t *p)
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
Name      :  serial_set_baudrate
Purpose   :  Set serial port baudrate. Port structure is not updated
Inputs    :  p        : port structure
             baudrate : baudrate setting
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_set_baudrate(martel_port_t *p,int baudrate)
{
        struct termios set;

        if (tcgetattr(p->set.serial.fd,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        switch (baudrate) {
        case MARTEL_B1200:
                cfsetispeed(&set,B1200);
                cfsetospeed(&set,B1200);
                break;
        case MARTEL_B2400:
                cfsetispeed(&set,B2400);
                cfsetospeed(&set,B2400);
                break;
        case MARTEL_B4800:
                cfsetispeed(&set,B4800);
                cfsetospeed(&set,B4800);
                break;
        case MARTEL_B9600:
                cfsetispeed(&set,B9600);
                cfsetospeed(&set,B9600);
                break;
        case MARTEL_B19200:
                cfsetispeed(&set,B19200);
                cfsetospeed(&set,B19200);
                break;
        case MARTEL_B38400:
                cfsetispeed(&set,B38400);
                cfsetospeed(&set,B38400);
                break;
        case MARTEL_B57600:
                cfsetispeed(&set,B57600);
                cfsetospeed(&set,B57600);
                break;
        case MARTEL_B115200:
                cfsetispeed(&set,B115200);
                cfsetospeed(&set,B115200);
                break;
        default:
                return MARTEL_INVALID_BAUDRATE;
        }
        
        if (tcsetattr(p->set.serial.fd,TCSANOW,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        return MARTEL_OK;
 }

/*-----------------------------------------------------------------------------
Name      :  serial_set_handshake
Purpose   :  Set serial handshake mode. Port structure is not updated
Inputs    :  p         : port structure
             handshake : handshake mode
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_set_handshake(martel_port_t *p,int handshake)
{
        struct termios set;

        if (tcgetattr(p->set.serial.fd,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        switch (handshake) {
        case MARTEL_NONE:
                /*disable flow control*/
                set.c_cflag &= ~CRTSCTS;
                set.c_iflag &= ~(IXON|IXOFF|IXANY);
                break;
        case MARTEL_XONXOFF:
                /*software flow control*/
                set.c_cflag &= ~CRTSCTS;
                set.c_iflag |= IXON|IXOFF|IXANY;
                break;
        case MARTEL_RTSCTS:
                /*hardware flow control*/
                set.c_cflag |= CRTSCTS;
                set.c_iflag &= ~(IXON|IXOFF|IXANY);
                break;
        default:
                return MARTEL_INVALID_HANDSHAKE;
        }

        if (tcsetattr(p->set.serial.fd,TCSANOW,&set)<0) {
                return MARTEL_IO_ERROR;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  serial_write
Purpose   :  Write data buffer to serial port
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_write(martel_port_t *p,const void *buf,int size)
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
Name      :  serial_write_rt
Purpose   :  Write data buffer to serial port with handshaking disabled
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_write_rt(martel_port_t *p,const void *buf,int size)
{
        martel_error_t errnum;
        
        /*flush pending input and output data*/
        if ((errnum = serial_flush(p))<0) {
                return errnum;
        }

        /*disable handshaking*/
        if ((errnum = serial_set_handshake(p,MARTEL_NONE))<0) {
                return errnum;
        }

        /*write data*/
        /*transmission starts immediatly because we flushed output buffer*/
        if ((errnum = serial_write(p,buf,size))<0) {
                return errnum;
        }

        /*wait until data is completely transmitted*/
        if ((errnum = serial_sync(p))<0) {
                return errnum;
        }

        /*enable handshaking*/
        if ((errnum = serial_set_handshake(p,p->set.serial.handshake))<0) {
                return errnum;
        }

        return MARTEL_OK;
}

/*-----------------------------------------------------------------------------
Name      :  serial_read
Purpose   :  Read data buffer from serial port
Inputs    :  p    : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  Fills data buffer
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_read(martel_port_t *p,void *buf,int size)
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
Name      :  serial_sync
Purpose   :  Block until all data pending in output buffer are transmitted
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_sync(martel_port_t *p)
{
        martel_error_t errnum;
        struct sigaction sa,old_sa;
        struct itimerval timer;

        if (p->write_timeout!=0) {
                /*install signal handler*/
                memset(&sa,0,sizeof(sa));
                sa.sa_handler = &serial_timer;
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
Name      :  serial_flush
Purpose   :  Clear input and output buffers
Inputs    :  p : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int serial_flush(martel_port_t *p)
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

