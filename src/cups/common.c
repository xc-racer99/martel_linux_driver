/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : common.c
* DESCRIPTION   : Common filter/backend routines
* CVS           : $Id: common.c,v 1.2 2006/08/01 09:20:30 chris Exp $
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
#include <cups/raster.h>
#include <cups/ppd.h>

#include <martel/martel.h>

#include "common.h"

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: common.c,v 1.2 2006/08/01 09:20:30 chris Exp $";

/*printer configuration*/
int     printer_model;
int     printer_type;
int     printer_width;          /*bytes*/

/*common options*/
int     prbaudrate;
int     prhandshake;
int     parmode;
int     prtimeout;
int     dynadiv;                /*black bytes*/
int     maxspeed;               /*mm/s*/
int     intensity;              /*%*/
int     font;
int     process;
int     finalcut;
int     fwdfeed;                /*dotlines*/
int     backfeed;               /*dotlines*/

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  get_opt_int
Purpose   :  Retrieve option value as an integer
Inputs    :  ppd : PPD file structure
             keyword : option name
Outputs   :  <>
Return    :  option value or -1 if option is not found
-----------------------------------------------------------------------------*/
static int get_opt_int(ppd_file_t *ppd,const char *keyword)
{
        ppd_choice_t *choice;

        choice = ppdFindMarkedChoice(ppd,keyword);

        if (choice!=NULL)
                return atoi(choice->choice);
        else
                return -1;
}

/*-----------------------------------------------------------------------------
Name      :  get_opt_bool
Purpose   :  Retrieve option value as a boolean
Inputs    :  ppd : PPD file structure
             keyword : option name
Outputs   :  <>
Return    :  1 if value is true, 0 if value is false or -1 if error
-----------------------------------------------------------------------------*/
static int get_opt_bool(ppd_file_t *ppd,const char *keyword)
{
        ppd_choice_t *choice;

        choice = ppdFindMarkedChoice(ppd,keyword);

        if (choice!=NULL) {
                if (strcmp(choice->choice,"False")==0)
                        return 0;
                else
                        return 1;
        }
        else
                return -1;
}
 
/* PUBLIC FUNCTIONS ---------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  error
Purpose   :  Exit program with error code
             Log error in /var/log/cups/error_log before exit
Inputs    :  s : custom error string
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
void error(const char *s)
{
	fprintf(stderr,"ERROR: %s (%s)\n",s,ippErrorString(cupsLastError()));
        exit(1);
}

/*-----------------------------------------------------------------------------
Name      :  get_options

Purpose   :  Retrieve current printing options
             Retrieve marked options from PPD file
             Override options with command-line string

Inputs    :  opt : command-line options
             
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
void get_options(const char *opt)
{
        ppd_file_t *ppd;
        int num_options;
        cups_option_t *options;
        int errnum;

        /*open printer PPD file and mark options*/
        ppd = ppdOpenFile(getenv("PPD"));
        if (ppd==NULL)
                error("ppdOpenFile failed");

        ppdMarkDefaults(ppd);

        options = NULL;
        num_options = cupsParseOptions(opt,0,&options);

        if (options!=NULL && num_options!=0) {
                cupsMarkOptions(ppd,num_options,options);
                cupsFreeOptions(num_options,options);
        }

        /*retrieve printer configuration*/
        printer_model = ppd->model_number;
        
        errnum = martel_get_model_type(printer_model);
        if (errnum<0)
                error(martel_strerror(errnum));
        else
                printer_type = errnum;

        errnum = martel_get_model_width(printer_model);
        if (errnum<0)
                error(martel_strerror(errnum));
        else
                printer_width = errnum/8;
        
        /*retrieve common options*/
        prbaudrate      = get_opt_int(ppd,"prbaudrate");
        prhandshake     = get_opt_int(ppd,"prhandshake");
        prtimeout       = get_opt_int(ppd,"prtimeout");
        dynadiv         = get_opt_int(ppd,"dynadiv");
        maxspeed        = get_opt_int(ppd,"maxspeed");
        intensity       = get_opt_int(ppd,"intensity");
        font            = get_opt_int(ppd,"font");
        process         = get_opt_bool(ppd,"process");
        fwdfeed         = get_opt_int(ppd,"fwdfeed");
        backfeed        = get_opt_int(ppd,"backfeed");
        
        /*retrieve printer-specific options*/
        /*TODO: not implemented!*/
        
        ppdClose(ppd);

}

/*-----------------------------------------------------------------------------
Name      :  write_prolog
Purpose   :  Write ticket prolog
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
void write_prolog(void)
{
        switch (printer_type) {
        case MARTEL_MPP:
        case MARTEL_MCP:
                if (font!=-1) {
                        unsigned char cmd[3] = {ESC,'!',font % 3};
                        fwrite(cmd,sizeof(cmd),1,stdout);
                }

                break;
        default:
                break;
        }

        fflush(stdout);
}

/*-----------------------------------------------------------------------------
Name      :  write_epilog
Purpose   :  Write ticket epilog
Inputs    :  <>
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
void write_epilog(void)
{
        switch (printer_type) {
        case MARTEL_MPP:
        case MARTEL_MCP:
                if (fwdfeed!=0) {
                        unsigned char cmd[3] = {ESC,'J',fwdfeed};
                        fwrite(cmd,sizeof(cmd),1,stdout);
                }
                if (backfeed!=0) {
                        unsigned char cmd[3] = {ESC,'j',backfeed};
                        fwrite(cmd,sizeof(cmd),1,stdout);
                }
                break;
        default:
                error("unknown model type");
                break;
        }

        fflush(stdout);
}

