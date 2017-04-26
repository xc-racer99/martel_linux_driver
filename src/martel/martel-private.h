/******************************************************************************
* COMPANY       : MARTEL ENGINEERING
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : martel-private.h
* DESCRIPTION   : MARTEL library - private definitions
* CVS           : $Id: martel-private.h,v 1.1 2006/08/01 09:12:19 chris Exp $
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

#ifndef _MARTEL_PRIVATE_H
#define _MARTEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*ASCII control codes*/
typedef enum {
        NUL     = 0,
        SOH     = 1,
        STX     = 2,
        ETX     = 3,
        EOT     = 4,
        ENQ     = 5,
        ACK     = 6,
        BEL     = 7,
        BS      = 8,
        TAB     = 9,
        LF      = 10,
        VT      = 11,
        FF      = 12,
        CR      = 13,
        SO      = 14,
        SI      = 15,
        DLE     = 16,
        DC1     = 17,
        DC2     = 18,
        DC3     = 19,
        DC4     = 20,
        NAK     = 21,
        SYN     = 22,
        ETB     = 23,
        CAN     = 24,
        EM      = 25,
        SUB     = 26,
        ESC     = 27,
        FS      = 28,
        GS      = 29,
        RS      = 30,
        US      = 31
} martel_control_code_t;

/* URI routines -------------------------------------------------------------*/

#define URI_MAX         255
#define OPTS_MAX        8

struct martel_uri {
        char    buf[URI_MAX+1];
        int     bufpos;
        char *  device;
        int     nopts;
        struct {
                char *  key;
                char *  value;
        } opts[OPTS_MAX];
};

int     uri_split(struct martel_uri *su,const char *uri);
int     uri_join(struct martel_uri *su,char *uri,int size);

void    uri_reset(struct martel_uri *su);
int     uri_add_device(struct martel_uri *su,const char *device);
int     uri_add_opt(struct martel_uri *su,const char *key,const char *value);

const char *    uri_get_device(struct martel_uri *su);
const char *    uri_get_opt(struct martel_uri *su,const char *key);

/* Port definition ----------------------------------------------------------*/

#define DEVICE_MAX              255     /*characters*/

#define USBFS_MAX               255     /*characters*/
#define USBPATH_MAX             31      /*characters*/

#define USB_BULK_EP             2       /*bidirectionnal*/

#define USB_BULK_OUT_EP_SIZE    64      /*characters*/
#define USB_BULK_IN_EP_SIZE     64      /*characters*/

#define USB_READ_BUFSIZE        USB_BULK_IN_EP_SIZE

typedef union {
        struct {
                char    device[DEVICE_MAX+1];
                int     fd;
                int     baudrate;
                int     handshake;
        } serial;
        struct {
                char    device[DEVICE_MAX+1];
                int     fd;
                int     mode;
                int     irq_left;
        } par;
        struct {
                char    usbfs[USBFS_MAX+1];
                char    path[USBPATH_MAX+1];
                int     fd;
                int     driver;

                /*user mode read buffer*/
                unsigned char   read_buf[USB_READ_BUFSIZE];
                int             read_pos;
                int             read_len;
        } usb;
} martel_settings_t;

typedef struct {
        int             type;
        int             errnum;
        int             open;
        int             write_timeout;          /*milliseconds*/
        int             read_timeout;           /*milliseconds*/
        martel_settings_t  set;
} martel_port_t;

/* Serial port routines -----------------------------------------------------*/

int     serial_get_uri(martel_port_t *p,char *uri,int size);
int     serial_create(martel_port_t *p,const char *device);
int     serial_create_from_uri(martel_port_t *p,struct martel_uri *su);

int     serial_open(martel_port_t *p);
int     serial_close(martel_port_t *p);
int     serial_set_baudrate(martel_port_t *p,int baudrate);
int     serial_set_handshake(martel_port_t *p,int handshake);
int     serial_write(martel_port_t *p,const void *buf,int size);
int     serial_write_rt(martel_port_t *p,const void *buf,int size);
int     serial_read(martel_port_t *p,void *buf,int size);
int     serial_sync(martel_port_t *p);
int     serial_flush(martel_port_t *p);

/* Parallel port routines ---------------------------------------------------*/

int     par_get_uri(martel_port_t *p,char *uri,int size);
int     par_create(martel_port_t *p,const char *device);
int     par_create_from_uri(martel_port_t *p,struct martel_uri *su);

int     par_open(martel_port_t *p);
int     par_close(martel_port_t *p);
int     par_reset(martel_port_t *p);
int     par_set_mode(martel_port_t *p,int mode);
int     par_write(martel_port_t *p,const void *buf,int size);
int     par_write_rt(martel_port_t *p,const void *buf,int size);
int     par_read(martel_port_t *p,void *buf,int size);
int     par_sync(martel_port_t *p);
int     par_flush(martel_port_t *p);

/* USB port routines --------------------------------------------------------*/

int     usb_get_uri(martel_port_t *p,char *uri,int size);
int     usb_create(martel_port_t *p,const char *device);
int     usb_create_from_address(martel_port_t *p,const char *usbfs,int busnum,int devnum);
int     usb_create_from_uri(martel_port_t *p,struct martel_uri *su);

int     usb_list_ports(martel_port_t **p,int max);

int     usb_open(martel_port_t *p);
int     usb_close(martel_port_t *p);
int     usb_control(martel_port_t *p,martel_usb_ctrltransfer_t *ctrl);
int     usb_write(martel_port_t *p,const void *buf,int size);
int     usb_read(martel_port_t *p,void *buf,int size);
int     usb_sync(martel_port_t *p);
int     usb_flush(martel_port_t *p);

#ifdef __cplusplus
}
#endif

#endif /*_MARTEL_PRIVATE_H*/

