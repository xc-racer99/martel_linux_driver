/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : common.h
* DESCRIPTION   : Common filter/backend routines
* CVS           : $Id: common.h,v 1.1 2006/08/01 09:08:33 chris Exp $
*******************************************************************************
*   Copyright (C) 2006  MARTEL Instruments Ltd.
*   
*   This file is part of the MARTEL Linux Driver.
*
*   MARTEL Linux Driver is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   MARTEL Linux Driver is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with MARTEL Linux Driver; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************
* HISTORY       :
*   27-Jul-06   CML     Initial revision
******************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

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

/*final cut modes*/
typedef enum {
        FINALCUT_NONE           = 0,
        FINALCUT_PARTIAL        = 1,
        FINALCUT_FULL           = 2
} finalcut_t;

/*printer configuration*/
extern int      printer_model;
extern int      printer_type;
extern int      printer_width;          /*dots*/

/*common options*/
extern int      prbaudrate;
extern int      prhandshake;
extern int      parmode;
extern int      prtimeout;
extern int      dynadiv;                /*black bytes*/
extern int      maxspeed;               /*mm/s*/
extern int      intensity;              /*%*/
extern int      font;
extern int      process;
extern int      finalcut;
extern int      fwdfeed;                /*dotlines*/
extern int      backfeed;               /*dotlines*/

void    error(const char *s);
void    get_options(const char *opt);
void    write_prolog(void);
void    write_epilog(void);

#endif /*_COMMON_H*/

