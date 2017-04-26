/******************************************************************************
* COMPANY       : Martel Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : martel.c
* DESCRIPTION   : MARTEL library - exported functions
* CVS           : $Id: martel.c,v 1.1 2006/08/01 09:12:03 chris Exp $
*******************************************************************************
*   Copyright (C) 2006  Martel Instruments Ltd.
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

#include <martel/martel.h>
#include <martel/martel-private.h>

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: martel.c,v 1.1 2006/08/01 09:12:03 chris Exp $";

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  build_port
Purpose   :  Build port structure
Inputs    :  <>
Outputs   :  <>
Return    :  port structure or NULL on error
-----------------------------------------------------------------------------*/
static martel_port_t *build_port(void)
{
        martel_port_t *p;

        /*allocate memory*/
        p = malloc(sizeof(martel_port_t));
        if (p==NULL)
                return NULL;

        /*zero out structure*/
        memset(p,0,sizeof(martel_port_t));

        return p;
}

/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  martel_get_model_name
Purpose   :  Retrieve name of model
Inputs    :  model : model number
Outputs   :  <>
Return    :  printer model name or NULL if model is invalid
-----------------------------------------------------------------------------*/
const char *martel_get_model_name(int model)
{
        const char *s;

        switch (model) {
        case MODEL_UNKNOWN:
                s = "unknown";
                break;
        case MODEL_MCP7810:
                s = "MCP7810";
                break;
        case MODEL_MCP8810:
                s = "MCP8810";
                break;
        case MODEL_MPP5510:
                s = "MPP5510";
                break;
        case MODEL_MPP5610:
                s = "MPP5610";
                break;
        case MODEL_MPP2000:
                s = "MPP2000";
                break;
        case MODEL_MPP3000:
                s = "MPP3000";
                break;
        case MODEL_MPP4000:
                s = "MPP4000";
                break;
        default:
                s = NULL;
                break;
        }

        return s;
}

/*-----------------------------------------------------------------------------
Name      :  martel_get_model_type
Purpose   :  Retrieve type of model
Inputs    :  model : model number
Outputs   :  <>
Return    :  printer model type (martel_model_type_t) or error code
-----------------------------------------------------------------------------*/
int martel_get_model_type(int model)
{
        martel_error_t errnum;

        switch (model) {
        case MODEL_UNKNOWN:
                errnum = MARTEL_UNKNOWN;
                break;
        case MODEL_MCP7810:
        case MODEL_MCP8810:
                errnum = MARTEL_MCP;
                break;
	case MODEL_MPP2000:
	case MODEL_MPP3000:
	case MODEL_MPP4000:
        case MODEL_MPP5510:
        case MODEL_MPP5610:
                errnum = MARTEL_MPP;
                break;
        default:
                errnum = MARTEL_INVALID_MODEL;
                break;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_get_model_width
Purpose   :  Retrieve width of model TPH
Inputs    :  model : model number
Outputs   :  <>
Return    :  TPH width in pixels or error code
-----------------------------------------------------------------------------*/
int martel_get_model_width(int model)
{
        martel_error_t errnum;

        switch (model) {
        case MODEL_MCP7810:
        case MODEL_MCP8810:
        case MODEL_MPP5510:
        case MODEL_MPP5610:
        case MODEL_MPP2000:
                errnum = 384;
                break;
        case MODEL_MPP3000:
                errnum = 576;
                break;
        case MODEL_MPP4000:
                errnum = 832;
                break;
        default:
                errnum = MARTEL_INVALID_MODEL;
                break;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_create_port
Purpose   :  Create port structure from URI
Inputs    :  uri : uniform resource identifier
Outputs   :  <>
Return    :  port structure or NULL on error (check port error code)
-----------------------------------------------------------------------------*/
void *martel_create_port(const char *uri)
{
        martel_port_t *p;
        struct martel_uri su;
        const char *value;

        /*build port structure*/
        p = build_port();

        if (p==NULL) {
                return NULL;
        }

        /*parse URI*/
        if (uri==NULL || uri_split(&su,uri)<0) {
                p->errnum = MARTEL_INVALID_URI;
                fprintf(stderr,"martel_create_port: uri==NULL or uri_split\n");
                return p;
        }

        /*get port type*/
        value = uri_get_opt(&su,"type");

        if (value==NULL) {
                fprintf(stderr,"martel_create_port: uri_get_opt\n");
                p->errnum = MARTEL_INVALID_URI;
                return p;
        }

        /*setup port settings according to type*/
        if (strcmp(value,"serial")==0) {
                p->type = MARTEL_SERIAL;
                p->errnum = serial_create_from_uri(p,&su);
        }
        else if (strcmp(value,"parallel")==0) {
                p->type = MARTEL_PARALLEL;
                p->errnum = par_create_from_uri(p,&su);
        }
        else if (strcmp(value,"usb")==0) {
                p->type = MARTEL_USB;
                p->errnum = usb_create_from_uri(p,&su);
        }
        else {
                p->errnum = MARTEL_INVALID_PORT_TYPE;
        }

        return p;
}

/*-----------------------------------------------------------------------------
Name      :  martel_create_serial_port
Purpose   :  Create serial port structure
Inputs    :  device : device name
Outputs   :  <>
Return    :  serial port structure or NULL on error (check port error code)
-----------------------------------------------------------------------------*/
void *martel_create_serial_port(const char *device)
{
        martel_port_t *p;

        /*build port structure*/
        p = build_port();

        if (p==NULL) {
                return NULL;
        }

        p->type = MARTEL_SERIAL;

        /*setup port settings*/
        p->errnum = serial_create(p,device);
        
        return p;
}

/*-----------------------------------------------------------------------------
Name      :  martel_create_parallel_port
Purpose   :  Create parallel port structure
Inputs    :  device : device name
Outputs   :  <>
Return    :  parallel port structure or NULL on error (check port error code)
-----------------------------------------------------------------------------*/
void *martel_create_parallel_port(const char *device)
{
        martel_port_t *p;

        /*build port structure*/
        p = build_port();

        if (p==NULL) {
                return NULL;
        }

        p->type = MARTEL_PARALLEL;

        /*setup port settings*/
        p->errnum = par_create(p,device);
        
        return p;
}

/*-----------------------------------------------------------------------------
Name      :  martel_create_usb_port
Purpose   :  Create usb port structure
Inputs    :  device : device name
Outputs   :  <>
Return    :  usb port structure or NULL on error (check port error code)
-----------------------------------------------------------------------------*/
void *martel_create_usb_port(const char *device)
{
        martel_port_t *p;

        /*build port structure*/
        p = build_port();

        if (p==NULL) {
                return NULL;
        }

        p->type = MARTEL_USB;

        /*setup port settings*/
        p->errnum = usb_create(p,device);
        
        return p;
}

/*-----------------------------------------------------------------------------
Name      :  martel_destroy_port
Purpose   :  Destroy port structure
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_destroy_port(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (p->open) {
                        errnum = martel_close(p);
                }
                else {
                        errnum = MARTEL_OK;
                }
                
                if (errnum==MARTEL_OK) {
                        free(p);
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_get_port_type
Purpose   :  Retrieve type of port
Inputs    :  port : port structure
Outputs   :  <>
Return    :  printer port type (martel_port_type_t) or error code
-----------------------------------------------------------------------------*/
int martel_get_port_type(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                errnum = p->type;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_get_port_uri
Purpose   :  Retrieve URI string of port
Inputs    :  port : port structure
             uri  : URI string buffer
             size : URI string buffer size (includes trailing zero)
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_get_port_uri(void *port,char *uri,int size)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                switch (p->type) {
                case MARTEL_SERIAL:
                        errnum = serial_get_uri(p,uri,size);
                        break;
                case MARTEL_PARALLEL:
                        errnum = par_get_uri(p,uri,size);
                        break;
                case MARTEL_USB:
                        errnum = usb_get_uri(p,uri,size);
                        break;
                default:
                        errnum = MARTEL_INVALID_PORT;
                        break;
                }
                
                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_open
Purpose   :  Open port
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_open(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (p->open) {
                        errnum = MARTEL_PORT_ALREADY_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_open(p);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_open(p);
                                break;
                        case MARTEL_USB:
                                errnum = usb_open(p);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }

                        if (errnum==MARTEL_OK) {
                                p->open = 1;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_close
Purpose   :  Close port
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_close(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_close(p);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_close(p);
                                break;
                        case MARTEL_USB:
                                errnum = usb_close(p);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }

                        if (errnum==MARTEL_OK) {
                                p->open = 0;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_write
Purpose   :  Write data buffer to port
Inputs    :  port : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_write(void *port,const void *buf,int size)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_write(p,buf,size);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_write(p,buf,size);
                                break;
                        case MARTEL_USB:
                                errnum = usb_write(p,buf,size);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_write_rt
Purpose   :  Write data buffer to port in real-time (ignore handshake signals)
Inputs    :  port : port structure

             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_write_rt(void *port,const void *buf,int size)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_write_rt(p,buf,size);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_write_rt(p,buf,size);
                                break;
                        case MARTEL_USB:
                                errnum = usb_write_rt(p,buf,size);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT_TYPE;
                                break;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_read
Purpose   :  Read data buffer from port
Inputs    :  port : port structure
             buf  : data buffer
             size : data buffer size in bytes
Outputs   :  Fills data buffer
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_read(void *port,void *buf,int size)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_read(p,buf,size);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_read(p,buf,size);
                                break;
                        case MARTEL_USB:
                                errnum = usb_read(p,buf,size);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_sync
Purpose   :  Block until all data pending in output buffer are transmitted
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_sync(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_sync(p);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_sync(p);
                                break;
                        case MARTEL_USB:
                                errnum = usb_sync(p);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_flush
Purpose   :  Clear input and output buffers
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_flush(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else {
                        switch (p->type) {
                        case MARTEL_SERIAL:
                                errnum = serial_flush(p);
                                break;
                        case MARTEL_PARALLEL:
                                errnum = par_flush(p);
                                break;
                        case MARTEL_USB:
                                errnum = usb_flush(p);
                                break;
                        default:
                                errnum = MARTEL_INVALID_PORT;
                                break;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_gets
Purpose   :  Read a null-terminated string
Inputs    :  port : port structure
             s    : string buffer
             size : string buffer size (includes trailing zero)
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_gets(void *port,void *s,int size)
{
        martel_error_t errnum;
        unsigned char *buf = s;

        if (size==0) {
                errnum = MARTEL_OK;
        }
        else {
                errnum = MARTEL_OK;

                while (size>1) {
                        if ((errnum = martel_read(port,buf,1))<0) {
                                break;
                        }
                        else if (*buf==0) {
                                break;
                        }
                        else {
                                buf++;
                                size--;
                        }
                }

                if (errnum==MARTEL_OK) {
                        *buf = 0;
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_serial_set_baudrate
Purpose   :  Set port baudrate. Only available on serial type ports
Inputs    :  port     : port structure
             baudrate : baudrate setting
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_serial_set_baudrate(void *port,int baudrate)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else if (p->type!=MARTEL_SERIAL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        if ((errnum = serial_set_baudrate(p,baudrate))>=0) {
                                p->set.serial.baudrate = baudrate;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_serial_set_handshake
Purpose   :  Set handshake mode. Only available on serial type ports
Inputs    :  port      : port structure
             handshake : handshake mode
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_serial_set_handshake(void *port,int handshake)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else if (p->type!=MARTEL_SERIAL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        if ((errnum = serial_set_handshake(p,handshake))>=0) {
                                p->set.serial.handshake = handshake;
                        }
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_serial_get_baudrate
Purpose   :  Get port baudrate. Only available on serial type ports
Inputs    :  port : port structure
Outputs   :  <>
Return    :  baudrate (martel_baudrate_t) or error code
-----------------------------------------------------------------------------*/
int martel_serial_get_baudrate(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) { 
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (p->type!=MARTEL_SERIAL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        errnum = p->set.serial.baudrate;
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_serial_get_handshake 
Purpose   :  Get handshake mode. Only available on serial type ports
Inputs    :  port : port structure
Outputs   :  <>
Return    :  handshake (martel_handshake_t) or error code
-----------------------------------------------------------------------------*/
int martel_serial_get_handshake(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (p->type!=MARTEL_SERIAL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        errnum = p->set.serial.handshake;
                }
        }

        return errnum;
}


/*-----------------------------------------------------------------------------
Name      :  martel_parallel_reset
Purpose   :  Hardware reset printer. Only available on parallel type ports
Inputs    :  port : port structure
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_parallel_reset(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (!p->open) {
                        errnum = MARTEL_PORT_NOT_OPEN;
                }
                else if (p->type!=MARTEL_PARALLEL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        errnum = par_reset(p);
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_parallel_set_mode
Purpose   :  Set parallel port mode. Only available on parallel type ports
Inputs    :  port : port structure
             mode : mode setting
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_parallel_set_mode(void *port,int mode)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (p->type!=MARTEL_PARALLEL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        errnum = par_set_mode(p,mode);
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_parallel_get_mode
Purpose   :  Get parallel port mode. Only available on parallel type ports
Inputs    :  port : port structure
Outputs   :  <>
Return    :  mode (martel_parallel_mode_t) or error code
-----------------------------------------------------------------------------*/
int martel_parallel_get_mode(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (p==NULL) {
                errnum = MARTEL_INVALID_PORT;
        } 
        else {
                if (p->type!=MARTEL_PARALLEL) {
                        errnum = MARTEL_INVALID_PORT_TYPE;
                }
                else {
                        errnum = p->set.par.mode;
                }
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_set_write_timeout
Purpose   :  Set write timeout in milliseconds
Inputs    :  port : port structure
             ms   : timeout in milliseconds
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_set_write_timeout(void *port,int ms)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (port==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (ms<0) {
                        errnum = MARTEL_INVALID_TIMEOUT;
                }
                else {
                        p->write_timeout = ms;
                        errnum = MARTEL_OK;
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_set_read_timeout
Purpose   :  Set read timeout in milliseconds
Inputs    :  port : port structure
             ms   : timeout in milliseconds
Outputs   :  <>
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
int martel_set_read_timeout(void *port,int ms)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (port==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                if (ms<0) {
                        errnum = MARTEL_INVALID_TIMEOUT;
                }
                else {
                        p->read_timeout = ms;
                        errnum = MARTEL_OK;
                }

                p->errnum = errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_get_error
Purpose   :  Get last error on port
Inputs    :  port : port structure
Outputs   :  <>
Return    :  error code of last error
-----------------------------------------------------------------------------*/
int martel_get_error(void *port)
{
        martel_error_t errnum;
        martel_port_t *p = port;

        if (port==NULL) {
                errnum = MARTEL_INVALID_PORT;
        }
        else {
                errnum = p->errnum;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  martel_strerror
Purpose   :  Convert error code in printable string
Inputs    :  errnum : error code
Outputs   :  <>
Return    :  printable string
-----------------------------------------------------------------------------*/
const char *martel_strerror(int errnum)
{
        const char *s;
        
        switch (errnum) {
        case MARTEL_OK:
                s = "Success";
                break;
        case MARTEL_NOT_IMPLEMENTED:
                s = "Function not implemented";
                break;
        case MARTEL_IO_ERROR:
                s = "Low-level input/output error";
                break;
        case MARTEL_INVALID_MODEL:
                s = "Invalid model parameter";
                break;
        case MARTEL_INVALID_MODEL_TYPE:
                s = "Invalid model type parameter";
                break;
        case MARTEL_INVALID_PORT:
                s = "Invalid port parameter";
                break;
        case MARTEL_INVALID_PORT_TYPE:
                s = "Invalid port type parameter";
                break;
        case MARTEL_NAME_TOO_LONG:
                s = "Name or string too long";
                break;
        case MARTEL_INVALID_URI:
                s = "Invalid URI parameter";
                break;
        case MARTEL_INVALID_BAUDRATE:
                s = "Invalid baudrate parameter";
                break;
        case MARTEL_INVALID_HANDSHAKE:
                s = "Invalid handshake parameter";
                break;
        case MARTEL_INVALID_TIMEOUT:
                s = "Invalid timeout parameter";
                break;
        case MARTEL_OPEN_FAILED:
                s = "Open operation failed";
                break;
        case MARTEL_CLOSE_FAILED:
                s = "Close operation failed";
                break;
        case MARTEL_WRITE_FAILED:
                s = "Write operation failed";
                break;
        case MARTEL_WRITE_TIMEOUT:
                s = "Write operation timed out";
                break;
        case MARTEL_READ_FAILED:
                s = "Read operation failed";
                break;
        case MARTEL_READ_TIMEOUT:
                s = "Read operation timed out";
                break;
        case MARTEL_SYNC_FAILED:
                s = "Sync operation failed";
                break;
        case MARTEL_FLUSH_FAILED:
                s = "Flush operation failed";
                break;
        case MARTEL_INVALID_STATUS:
                s = "Invalid status buffer";
                break;
        case MARTEL_PORT_NOT_OPEN:
                s = "Port is not open";
                break;
        case MARTEL_PORT_ALREADY_OPEN:
                s = "Port is already open";
                break;
        case MARTEL_INVALID_PARALLEL_MODE:
                s = "Invalid parallel mode parameter";
                break;
        case MARTEL_INVALID_USB_PATH:
                s = "Invalid USB path";
                break;
        case MARTEL_USB_DEVICE_NOT_FOUND:
                s = "USB device not found";
                break;
        case MARTEL_USB_DEVICE_BUSY:
                s = "USB device busy (cannot unregister current driver)";
                break;
        default:
                s = "Unknown error";
                break;
        }

        return s;
}

