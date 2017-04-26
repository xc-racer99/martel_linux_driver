/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : martel.c
* DESCRIPTION   : CUPS backend for MARTEL printers
*                 Use MARTEL library to send data to printers
* CVS           : $Id: martel.c,v 1.1 2006/08/01 09:08:38 chris Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <cups/cups.h>

#include <martel/martel.h>

#include "common.h"

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: martel.c,v 1.1 2006/08/01 09:08:38 chris Exp $";

#define PRINTERS_MAX    32

#define BUFSIZE         4096    /*bytes*/

static void *   port;

static martel_serial_baudrate_t    defbaudrate;
static martel_serial_handshake_t   defhandshake;
static martel_parallel_mode_t      defparmode;

#define CMD_BUFSIZE     8       /*bytes*/

typedef struct {
        unsigned char   buf[CMD_BUFSIZE];
        int             size;
        int             answer;
} command_t;

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  cmd_reset
Purpose   :  Build 'reset' command based on printer type
Inputs    :  cmd : command structure
Outputs   :  Fills command structure
Return    :  MARTEL_OK or error code
-----------------------------------------------------------------------------*/
static int cmd_reset(command_t *cmd)
{
        martel_error_t errnum;

        errnum = MARTEL_OK;

        switch (printer_type) {
        case MARTEL_MPP:
        case MARTEL_MCP:
                cmd->size = 2;
                cmd->answer = 0;
                cmd->buf[0] = ESC;
                cmd->buf[1] = '@';
                break;
        default:
                errnum = MARTEL_INVALID_MODEL_TYPE;
                break;
        }

        return errnum;
}

/*-----------------------------------------------------------------------------
Name      :  clean
Purpose   :  Clean printer structure
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void clean(void)
{
        int errnum;
        
        if (port!=NULL) {

                errnum = martel_close(port);

                if (errnum<0) {
                        fprintf(stderr,"ERROR: (clean) %s\n",martel_strerror(errnum));
                }

                errnum = martel_destroy_port(port);

                if (errnum<0) {
                        fprintf(stderr,"ERROR: (clean) %s\n",martel_strerror(errnum));
                }
        }
}

/*-----------------------------------------------------------------------------
Name      :  check
Purpose   :  Check return code of MARTEL library function and exit on error
Inputs    :  errnum : function return code
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void check(int errnum)
{
        if (errnum<0) {
                error(martel_strerror(errnum));
        }
}

/*-----------------------------------------------------------------------------
Name      :  setup
Purpose   :  Save current port settings (defaults) and setup port for printing
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void setup(void)
{
        int type;

        check(type = martel_get_port_type(port));
        
        switch (type) {
        case MARTEL_SERIAL:
                 /*save current settings*/
                check(defbaudrate = martel_serial_get_baudrate(port));
                check(defhandshake = martel_serial_get_handshake(port));
                /*setup printing settings as defaults if required*/
                if (prbaudrate==-1) {
                        prbaudrate = defbaudrate;
                }
                if (prhandshake==-1) {
                        prhandshake = defhandshake;
                }

                check(martel_sync(port));

                check(martel_serial_set_baudrate(port,prbaudrate));
                check(martel_serial_set_handshake(port,prhandshake));

                break;

        case MARTEL_PARALLEL:
                 /*save current settings*/
                check(defparmode = martel_parallel_get_mode(port));

                /*setup printing settings as defaults if required*/
                if (parmode==-1) {
                        parmode = defparmode;
                }

                /*update parallel settings*/
                check(martel_parallel_set_mode(port,parmode));

                break;

        case MARTEL_USB:
                /*nothing to set*/
                break;
        }        
}

/*-----------------------------------------------------------------------------
Name      :  setup_defaults
Purpose   :  Revert to default port settings
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void setup_defaults(void)
{
        int type;

        check(type = martel_get_port_type(port));
        
        switch (type) {
        case MARTEL_SERIAL:
                check(martel_sync(port));
                
                check(martel_serial_set_baudrate(port,defbaudrate));
                check(martel_serial_set_handshake(port,defhandshake));

                break;

        case MARTEL_PARALLEL:
                /*revert to default parallel settings*/
                check(martel_parallel_set_mode(port,defparmode));

                break;

        case MARTEL_USB:
                /*nothing to set*/
                break;
        }
}

/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  main
Purpose   :  Program main function
Inputs    :  argc : number of command-line arguments (including program name)
             argv : array of command-line arguments
Outputs   :  <>
Return    :  0 if successful, 1 if program failed
-----------------------------------------------------------------------------*/
int main(int argc,char** argv)
{
        int fd;
        int n;
        unsigned char buf[BUFSIZE];

        atexit(clean);
        
        setbuf(stderr,NULL);

        /*check arguments*/
        if (argc<6 || argc>7) {
                fputs("ERROR: martel job-id user title copies options [file]\n",stderr);
                return 1;
        }

        /*retrieve options*/
        get_options(argv[5]);

        /*open input file*/
        if (argc==7) {
                if ((fd = open(argv[6],O_RDONLY))==-1) {
                        perror("ERROR: Unable to open input file - ");
                        return 1;
                }
        }
        else
                fd = 0; /*stdin*/

        /*create printer port*/
        port = martel_create_port(getenv("DEVICE_URI"));

        if (port==NULL)
                error("error creating port");

        check(martel_get_error(port));

        /*open port*/
        check(martel_open(port));

        /*setup port settings for printing*/
        setup();

        /*setup printing timeout*/
        check(martel_set_write_timeout(port,prtimeout));

        /*write data to printer*/
        while ((n = read(fd,buf,BUFSIZE))!=0) {

                check(martel_write(port,buf,n));
        }

        check(martel_sync(port));

        /*revert port settings to defaults*/
        setup_defaults();

        /*close port*/
        check(martel_close(port));

        /*destroy printer port*/
        check(martel_destroy_port(port));

        port = NULL;

        /*close input file*/
        if (fd!=0) {
                close(fd);
        }

        return 0;
	cmd_reset(NULL);	/* shut compiler up */
}


