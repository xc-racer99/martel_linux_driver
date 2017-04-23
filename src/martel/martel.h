/******************************************************************************
* COMPANY       : MARTEL ENGINEERING
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : martel.h
* DESCRIPTION   : MARTEL library header file
* CVS           : $Id: martel.h,v 1.1 2006/08/01 09:12:16 chris Exp $
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

#ifndef _MARTEL_H
#define _MARTEL_H

#include <martel/version.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <martel/models.def>

typedef enum {
        MARTEL_OK                          = 0,
        MARTEL_NOT_IMPLEMENTED             = -1,
        MARTEL_IO_ERROR                    = -2,
        MARTEL_INVALID_MODEL               = -3,
        MARTEL_INVALID_MODEL_TYPE          = -4,
        MARTEL_INVALID_PORT                = -5,
        MARTEL_INVALID_PORT_TYPE           = -6,
        MARTEL_NAME_TOO_LONG               = -7,
        MARTEL_INVALID_URI                 = -8,
        MARTEL_INVALID_BAUDRATE            = -9,
        MARTEL_INVALID_HANDSHAKE           = -10,
        MARTEL_INVALID_TIMEOUT             = -11,
        MARTEL_OPEN_FAILED                 = -12,
        MARTEL_CLOSE_FAILED                = -13,
        MARTEL_WRITE_FAILED                = -14,
        MARTEL_WRITE_TIMEOUT               = -15,
        MARTEL_READ_FAILED                 = -16,
        MARTEL_READ_TIMEOUT                = -17,
        MARTEL_SYNC_FAILED                 = -18,
        MARTEL_FLUSH_FAILED                = -19,
        MARTEL_INVALID_STATUS              = -20,
        MARTEL_PORT_NOT_OPEN               = -21,
        MARTEL_PORT_ALREADY_OPEN           = -22,
        MARTEL_INVALID_PARALLEL_MODE       = -23,
        MARTEL_INVALID_USB_PATH            = -24,
        MARTEL_USB_DEVICE_NOT_FOUND        = -25,
        MARTEL_USB_DEVICE_BUSY             = -26
} martel_error_t;

typedef struct {
        char    printing;
        char    online;
        char    end_of_paper;
        char    near_end_of_paper;
        char    head_up;
        char    cover_open;
        char    temp_error;
        char    supply_error;
        char    mark_error;
        char    cutter_error;
        char    mechanical_error;
        char    presenter_error;
        char    presenter_action;
        char    rcpt_front_exit;
        char    rcpt_retract_exit;
        char    rcpt_status;
        char    front_exit_jam;
        char    retract_exit_jam;
} martel_status_t;

typedef enum {
        MARTEL_PRESENTER_IDLE              = 0,
        MARTEL_PRESENTER_PRESENTING        = 1,
        MARTEL_PRESENTER_CUTTING           = 2,
        MARTEL_PRESENTER_RETRACTING        = 3,
        MARTEL_PRESENTER_REPRESENTING      = 4,
        MARTEL_PRESENTER_JAM_CLEARING      = 5
} martel_presenter_action_t;

typedef enum {
        MARTEL_RCPT_NOT_COLLECTED          = 0,
        MARTEL_RCPT_COLLECTED              = 1,
        MARTEL_RCPT_RETRACT_COMPLETE       = 2,
        MARTEL_RCPT_RETRACT_PARTIAL        = 3
} martel_rcpt_status_t;

typedef enum {
        MARTEL_UNKNOWN     = 0,
        MARTEL_MPP         = 1,
        MARTEL_MCP         = 2
} martel_model_type_t;

typedef enum {
        MARTEL_SERIAL      = 0,
        MARTEL_PARALLEL    = 1,
        MARTEL_USB         = 2
} martel_port_type_t;

#define MARTEL_IDENTITY_MAX        31      /*characters*/
#define MARTEL_URI_MAX             255     /*characters*/

typedef struct {
        int     model;
        char    identity[MARTEL_IDENTITY_MAX+1];
        char    uri[MARTEL_URI_MAX+1];
} martel_printer_t;

typedef enum {
        MARTEL_B1200       = 0,
        MARTEL_B2400       = 1,
        MARTEL_B4800       = 2,
        MARTEL_B9600       = 3,
        MARTEL_B19200      = 4,
        MARTEL_B38400      = 5,
        MARTEL_B57600      = 6,
        MARTEL_B115200     = 7
} martel_serial_baudrate_t;

typedef enum {
        MARTEL_NONE        = 0,
        MARTEL_XONXOFF     = 1,
        MARTEL_RTSCTS      = 2
} martel_serial_handshake_t;

typedef enum {
        MARTEL_POLL        = 0,
        MARTEL_IRQ         = 1
} martel_parallel_mode_t;

typedef struct {
        unsigned char   bRequestType;
        unsigned char   bRequest;
        unsigned int    wValue;
        unsigned int    wIndex;
        unsigned int    wLength;
        void *          data;
} martel_usb_ctrltransfer_t;

const char *    martel_get_model_name(int model);

int     martel_get_model_type(int model);
int     martel_get_model_width(int model);

int     martel_decode_status(int type,const void *buf,int size,martel_status_t *status);

int     martel_detect_printers(martel_printer_t *printers,int max);

void *  martel_create_port(const char *uri);
void *  martel_create_serial_port(const char *device);
void *  martel_create_parallel_port(const char *device);
void *  martel_create_usb_port(const char *device);
void *  martel_create_usb_port_from_address(const char *usbfs,int busnum,int devnum);

int     martel_destroy_port(void *port);

int     martel_get_port_type(void *port);
int     martel_get_port_uri(void *port,char *uri,int size);
        
int     martel_open(void *port);
int     martel_close(void *port);
int     martel_write(void *port,const void *buf,int size);
int     martel_write_rt(void *port,const void *buf,int size);
int     martel_read(void *port,void *buf,int size);
int     martel_sync(void *port);
int     martel_flush(void *port);

int     martel_gets(void *port,void *s,int size);

int     martel_serial_set_baudrate(void *port,int baudrate);
int     martel_serial_set_handshake(void *port,int handshake);
int     martel_serial_get_baudrate(void *port);
int     martel_serial_get_handshake(void *port);

int     martel_parallel_reset(void *port);
int     martel_parallel_set_mode(void *port,int mode);
int     martel_parallel_get_mode(void *port);

int     martel_usb_control(void *port,martel_usb_ctrltransfer_t *ctrl);

int     martel_set_write_timeout(void *port,int ms);
int     martel_set_read_timeout(void *port,int ms);

int     martel_get_error(void *port);

const char *    martel_strerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif /*_MARTEL_H*/

