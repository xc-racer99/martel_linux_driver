/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : testmartel.c
* DESCRIPTION   : Test program for MARTEL library
* CVS           : $Id: testmartel.c,v 1.1 2006/08/01 09:12:25 chris Exp $
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

#include <martel/martel.h>

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: testmartel.c,v 1.1 2006/08/01 09:12:25 chris Exp $";

#define ESC     27
#define GS      29

/*command-line options*/
static const char *     uri;
static int              opt_type;

static void *port;

static const char ticket[] = "This is a test ticket\n\n";
static const char test_B9600[] = "Printed at 9600 Baud\n\n\n";


/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  clean
Purpose   :  Clean resources
             This function is called by exit() to clean port ressources
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void clean(void)
{
        int errnum;
        
        if (port!=NULL) {

                if ((errnum = martel_flush(port))<0) {
                        fprintf(stderr,"clean: %s\n",martel_strerror(errnum));
                }

                if ((errnum = martel_close(port))<0) {
                        fprintf(stderr,"clean: %s\n",martel_strerror(errnum));
                }

                if ((errnum = martel_destroy_port(port))<0) {
                        fprintf(stderr,"clean: %s\n",martel_strerror(errnum));
                }
        }
}

/*-----------------------------------------------------------------------------
Name      :  check
Purpose   :  Check MARTEL library error code. Print error string on error
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void check(int errnum)
{
        if (errnum!=MARTEL_OK) {
                fprintf(stderr,"check: %s\n",martel_strerror(errnum));
                exit(1);
        }
}

/*-----------------------------------------------------------------------------
Name      :  parse_options
Purpose   :  Parse command-line options and update global variables
Inputs    :  argc : number of command-line arguments
             argv : array of command-line arguments
Outputs   :  Update global variables
Return    :  <>
-----------------------------------------------------------------------------*/
static void parse_options(int argc,char **argv)
{
        int i;
        
        /*set defaults*/
        uri = NULL;
        opt_type = MARTEL_MPP;

        /*parse options*/
        for (i=1; i<argc; i++) {
                if (strcmp(argv[i],"-t")==0) {
                        i++;

                        if (i==argc) {
                                fprintf(stderr,"parse_options: missing model type\n");
                                exit(1);
                        }
                        else if (strcmp(argv[i],"MPP")==0) {
                                opt_type = MARTEL_MPP;
                        }
                        else if (strcmp(argv[i],"MCP")==0) {
                                opt_type = MARTEL_MCP;
                        }
                        else {
                                fprintf(stderr,"parse_options: invalid model type (%s)\n",argv[i]);
                                exit(1);
                        }
                }
                else if (i==argc-1) {
                        uri = argv[i];
                }
                else {
                        fprintf(stderr,"parse_options: unrecognized option (%s)\n",argv[i]);
                        exit(1);
                }
        }

        /*URI string is mandatory*/
        if (uri==NULL) {
                fprintf(stderr,"parse_options: missing device uri\n");
                exit(1);
        }
}

/*-----------------------------------------------------------------------------
Name      :  test_common
Purpose   :  Test libmartel features common to all port types
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void test_common(void *port)
{
        char line[100];
        int i;

        /*write simple ticket*/
        check(martel_write(port,ticket,strlen(ticket)));

        /*get printer identity*/
        check(martel_set_read_timeout(port,1000));
        
        check(martel_sync(port));
        check(martel_flush(port));

        /*print a large amount of data with timeout*/
        check(martel_set_write_timeout(port,5000));
        for (i=0; i<20; i++) {
                sprintf(line,"line %d: ABCDEFGHIJKLMNO\n",i);
                check(martel_write(port,line,strlen(line)));
        }
}

/*-----------------------------------------------------------------------------
Name      :  test_serial
Purpose   :  Test libmartel features specific to serial ports
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void test_serial(void *port)
{
        /* default baudrate should be 9600 bauds
         * default handshake should be RTS/CTS
         */

        /* revert to 9600 bauds */
        check(martel_sync(port));
        check(martel_serial_set_baudrate(port,MARTEL_B9600));
        check(martel_write(port,test_B9600,strlen(test_B9600)));
}

/*-----------------------------------------------------------------------------
Name      :  test_parallel
Purpose   :  Test libmartel features specific to parallel ports
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void test_parallel(void *port)
{
        /*no specific features*/
}

/*-----------------------------------------------------------------------------
Name      :  test_usb
Purpose   :  Test libmartel features specific to USB ports
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void test_usb(void *port)
{
        /*no specific features*/
}

/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  main
Purpose   :  Program main function
Inputs    :  argc : number of command-line arguments
             argv : array of command-line arguments
Outputs   :  <>
Return    :  0 if success, 1 on error
-----------------------------------------------------------------------------*/
int main(int argc,char **argv)
{
        port = NULL;

        atexit(clean);

        if (argc<2) {
                printf("testmartel compiled with MARTEL library %d.%d.%d\n",
                                MARTEL_MAJOR,
                                MARTEL_MINOR,
                                MARTEL_BUGFIX);

                printf("usage: testmartel [-t MPP|MCP] uri\n");
                return 0;
        }

        parse_options(argc,argv);

        /*create port structure*/
        port = martel_create_port(uri);

        if (port==NULL) {
                fprintf(stderr,"main: error creating printer port\n");
                exit(1);
        }

        check(martel_get_error(port));
        
        /*open port*/
        check(martel_open(port));

        /*test features common to all port types*/
        test_common(port);

        /*test port type specific features*/
        switch (martel_get_port_type(port)) {
        case MARTEL_SERIAL:
                test_serial(port);
                break;
        case MARTEL_PARALLEL:
                test_parallel(port);
                break;
        case MARTEL_USB:
                test_usb(port);
                break;
        default:
                fprintf(stderr,"main: unknown printer port type\n");
                exit(1);
        }
                
        /*close port*/
        check(martel_close(port));

        /*destroy port structure*/
        check(martel_destroy_port(port));
        
        port = NULL;

        return 0;
}

