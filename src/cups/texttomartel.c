/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : texttomartel.c
*
* DESCRIPTION   : CUPS filter for MARTEL printers
*                 Converts plain text into MARTEL commands
*                 MARTEL command set is selected depending on cupsModelNumber
*                 attribute of the PPD file
*
* CVS           : $Id: texttomartel.c,v 1.1 2006/08/01 09:08:49 chris Exp $
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

#include <martel/martel.h>

#include "common.h"

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: texttomartel.c,v 1.1 2006/08/01 09:08:49 chris Exp $";

#define BUFSIZE 4096    /*bytes*/

static enum {
        PROCESSING_IDLE = 0,
        PROCESSING_TAG  = 1,
} state;

#define TAG_BUFSIZE 256         /*bytes*/

static int      tag_index;
static char     tag_buf[TAG_BUFSIZE+1];

typedef struct {
        char *  text;
        int     value;
} alias_t;

static const alias_t alias_table[] = {
        {"NUL", NUL},
        {"SOH", SOH},
        {"STX", STX},
        {"ETX", ETX},
        {"EOT", EOT},
        {"ENQ", ENQ},
        {"ACK", ACK},
        {"BEL", BEL},
        {"BS",  BS},
        {"TAB", TAB},
        {"LF",  LF},
        {"VT",  VT},
        {"FF",  FF},
        {"CR",  CR},
        {"SO",  SO},
        {"SI",  SI},
        {"DLE", DLE},
        {"DC1", DC1},
        {"DC2", DC2},
        {"DC3", DC3},
        {"DC4", DC4},
        {"NAK", NAK},
        {"SYN", SYN},
        {"ETB", ETB},
        {"CAN", CAN},
        {"EM",  EM},
        {"SUB", SUB},
        {"ESC", ESC},
        {"FS",  FS},
        {"GS",  GS},
        {"RS",  RS},
        {"US",  US},
};

#define ALIAS_TABLE_SIZE        (sizeof(alias_table)/sizeof(alias_t))

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  tag_to_char
Purpose   :  Convert current tag to character value
Inputs    :  <>
Outputs   :  <>
Return    :  character value or -1 if conversion is impossible
-----------------------------------------------------------------------------*/
static int tag_to_char(void)
{
        int i;
        int n;

        /*lookup tag in alias table*/
        for (i=0; i<ALIAS_TABLE_SIZE; i++)
                if (strcmp(alias_table[i].text,tag_buf)==0)
                        return alias_table[i].value;

        /*try converting numerical value*/
        if (sscanf(tag_buf,"%i",&n)==1) {
                if (n<0 || n>255)
                        return -1;
                else
                        return n;
        }

        /*we tried everything we could*/
        return -1;
}

/*-----------------------------------------------------------------------------
Name      :  process_and_write
Purpose   :  Process buffer data and write on standard output
Inputs    :  buf : data buffer
             bufsize : data buffer size in bytes
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void process_and_write(char *buf,int bufsize)
{
        while (bufsize--) {
                char c = *buf++;
                
                switch (state) {
                case PROCESSING_IDLE:
                        if (c=='<') {
                                tag_index = 0;
                                state = PROCESSING_TAG;
                        }
                        else
                                fputc(c,stdout);
                        break;
                
                case PROCESSING_TAG:
                        if (c=='>') {
                                int n;

                                tag_buf[tag_index] = 0;
                                n = tag_to_char();

                                if (n==-1)
                                        printf("<%s>",tag_buf);
                                else
                                        fputc(n,stdout);

                                state = PROCESSING_IDLE;
                        }
                        else {
                                if (tag_index==TAG_BUFSIZE) {
                                        tag_buf[tag_index] = 0;
                                        printf("<%s",tag_buf);

                                        state = PROCESSING_IDLE;
                                }
                                else {
                                        tag_buf[tag_index] = c;
                                        tag_index++;
                                }
                        }
                        break;
                }
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
        char buf[BUFSIZE];

	setbuf(stderr,NULL);

	/*check arguments*/
	if (argc<6 || argc>7) {
		fputs("ERROR: texttomartel job-id user title copies options [file]\n",stderr);
		return 1 + 128;
	}

        /*retrieve options*/
        get_options(argv[5]);

	/*open page stream*/
	if (argc==7) {
		if ((fd = open(argv[6],O_RDONLY))==-1) {
			perror("ERROR: Unable to open text file - ");
			return 1 + 129;
		}
	}
	else
		fd = 0; /*stdin*/

        /*write ticket prolog*/
        write_prolog();

        /*perform simple page accounting*/
        fprintf(stderr,"PAGE: 1 1\n");

        /*pipe text file to standard output*/
        while ((n = read(fd,buf,sizeof(buf)))!=0) {
                if (process)
                        process_and_write(buf,n);
                else
                        write(1,buf,n);
        }

        /*write ticket epilog*/
        write_epilog();

        /*close input file*/
        if (fd!=0) {
                close(fd);
        }

	return 0;
}


