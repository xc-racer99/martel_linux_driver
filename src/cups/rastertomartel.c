/******************************************************************************
* COMPANY       : MARTEL Instruments Ltd.
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : rastertomartel.c
*
* DESCRIPTION   : CUPS filter for MARTEL printers
*                 Converts CUPS internal RIP format into MARTEL commands
*                 MARTEL command set is selected depending on cupsModelNumber
*                 attribute of the PPD file
*
* CVS           : $Id: rastertomartel.c,v 1.1 2006/08/01 09:08:43 chris Exp $
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
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include <cups/cups.h>
#include <cups/raster.h>

#include <martel/martel.h>

#include "common.h"

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: rastertomartel.c,v 1.1 2006/08/01 09:08:43 chris Exp $";


#define BLACK   1
#define WHITE   0

static unsigned char bit_no[8] =
                {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


/* PRIVATE FUNCTIONS --------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name      :  consecutive_bits
Purpose   :  count no of consecutive bit the same colour
Inputs    :  bitmap : dotline buffer
             max_bytes : width of dotline in bytes
             line_no : which of 24 lines are we printing  
Outputs   :  <>
Return    :  count of pixels or bit image
-----------------------------------------------------------------------------*/
static unsigned char consecutive_bits(unsigned char *bitmap, int bit_ptr, int max_bytes)
{
        int i, j = bit_ptr;
        int bit_clr;
        unsigned char bits;

        if ( bitmap[j / 8] & bit_no[j & 7])
                bit_clr = BLACK;
        else
                bit_clr = WHITE;
        for ( i = 0; (j < (max_bytes * 8)) && (i < 63); j++, i++ )
        {
                if ((( bitmap[j / 8] & bit_no[j & 7]) && (bit_clr == WHITE))
                || (!( bitmap[j / 8] & bit_no[j & 7]) && (bit_clr == BLACK)))
                        break; /* consecutive bits no longer the same */
        }
        if (i > 7)
        {
                switch (bit_clr)
                {
                        case BLACK:
                                bits = 0x40 | i;
                                break;
                        case WHITE:
                                bits = 0x00 | i;
                                break;
                }
        }
        else /* 7 bit bit map */
        {
                unsigned char bit_mask = 0x40;

                bits = 0x80;
                for (i = 0; i < 7; i++)
                {
                        if ( bitmap[(bit_ptr + i) / 8] & bit_no[(bit_ptr + i) & 7])
                        {
                                bits  |= bit_mask;
                        }
                        else
                        {
                        }
                        bit_mask >>= 1;
                }
        }
        return(bits);
}


/*-----------------------------------------------------------------------------
Name      :  convert_to_rle
Purpose   :  Converts a dotline bit map to Martel Run Length Encoded bit
             image gaphics
             d7 d6 d5 d4 d3 d2 d1 d0
             0  0  x  x  x  x  x  x RLE white pixels (0 to 63)
             0  1  x  x  x  x  x  x RLE black pixels (0 to 63)
             1  x  x  x  x  x  x  x Seven bit image pixels (0=white, 1=black)
Inputs    :  bmp_in : dotline buffer
             bytes_in : width of dotline in bytes
Outputs   :  bytes_out : width of converted line in bytes
Return    :  ptr to converted line (must free memory after use)
-----------------------------------------------------------------------------*/
/* 
 * 
 *
 *
 * returns rle bits
 */
static unsigned char *convert_to_rle(unsigned char *bmp_in, int bytes_in, int *bytes_out)
{
        unsigned char *rle_out = malloc(bytes_in * 2);  /* plenty of space */
        int bit_ptr;            /* start at bit zero (MSB), byte zero */
        unsigned char bits;
        int i, j;

        *bytes_out = 0;
        if (rle_out == NULL)
                return (NULL);
        memset(rle_out, 0x00, bytes_in * 2);

        /* check for blank line */
        for (i = 0, j = 0; i < bytes_in; i++)
        {
                if (bmp_in[i])
                {
                        j = 1;
                }
        }
        if (!j) /* no dots found so exit early */
        {
                rle_out[0] = 0;
                *bytes_out = 1;
                return(rle_out);
        }

        for (bit_ptr = 0; bit_ptr < (bytes_in * 8); )
        {
                bits = consecutive_bits(bmp_in, bit_ptr, bytes_in);
                if (bits & 0x80) /* 7 bit image byte */
                {
                        bit_ptr += 7;
                }
                else /* RLE byte */
                {
                        bit_ptr += (bits & 0x3f);
                }
                rle_out[(*bytes_out)++] = bits;
        }
        return(rle_out);
}

/*-----------------------------------------------------------------------------
Name      :  write_dotline
Purpose   :  Write MARTEL commands to print given dotline
Inputs    :  dotline : dotline buffer
             num_bytes : width of dotline in bytes
             line_no : which of 24 lines are we printing  
Outputs   :  <>
Return    :  <>
-----------------------------------------------------------------------------*/
static void write_dotline(unsigned char *dotline,int num_bytes, int line_no)
{
        unsigned char *rle_dotline;
        int rle_bytes;

        switch (printer_type) {
        case MARTEL_MPP:
        case MARTEL_MCP:
                {
                        unsigned char cmd[2] = { ESC,'Z' };

                        if (line_no == 0)
                                fwrite(cmd,sizeof(cmd),1,stdout);
                        rle_dotline =
                            convert_to_rle(dotline, num_bytes, &rle_bytes);
                        fprintf(stdout, "%c", rle_bytes);
                        fwrite(rle_dotline,rle_bytes,1,stdout);
                        if (line_no == 23)
                                fwrite("\n", 1, 1, stdout);
                        free(rle_dotline);
                }
                break;
        default:
                error("unknown model type");
                break;
        }
        
        fflush(stdout);
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
	cups_raster_t *ras;
	cups_page_header_t header;
	int page;

	setbuf(stderr,NULL);

	/*check arguments*/
	if (argc<6 || argc>7) {
		fputs("ERROR: rastertomartel job-id user title copies options [file]\n",stderr);
		return 1;
	}

        /*retrieve options*/
        get_options(argv[5]);

	/*open page stream*/
	if (argc==7) {
		if ((fd = open(argv[6],O_RDONLY))==-1) {
			perror("ERROR: Unable to open raster file - ");
			return 1;
		}
	}
	else
		fd = 0; /*stdin*/

	ras = cupsRasterOpen(fd,CUPS_RASTER_READ);
	if (ras==NULL)
		error("cupsRasterOpen failed");

        /*write ticket prolog*/
        write_prolog();

	/*read and process pages*/
	page = 0;
	while (cupsRasterReadHeader(ras,&header)) {
                int num_bytes;
		int y;
		unsigned char *dotline;
                int line_no = 0;

		page++;
		fprintf(stderr,"PAGE: %d 1\n",page);

                if (header.cupsBytesPerLine>printer_width)
                        num_bytes = printer_width;
                else
                        num_bytes = header.cupsBytesPerLine;

                dotline = malloc(header.cupsBytesPerLine);
                if (dotline==NULL) {
                        perror("ERROR: Cannot allocate dotline buffer - ");
                        return 1;
                }

		for (y=0; y<header.cupsHeight; y++) {
                        if (cupsRasterReadPixels(ras,dotline,header.cupsBytesPerLine) != header.cupsBytesPerLine)
                                error("cupsRasterReadPixels did not read enough data");
                        write_dotline(dotline,num_bytes, line_no++);
                        line_no %= 24;
		}
                /* finish printing 24 lines. */
                {
                        int left_over = (24 - (header.cupsHeight % 24)) % 24;
                        dotline[0] = 0;
                        num_bytes = 1;

                        while (left_over--) {
                                write_dotline(dotline,num_bytes, line_no++);
                                line_no %= 24;
                        }
                }
                free(dotline);
	}

        /*write ticket epilog*/
        write_epilog();

	/*close raster stream*/
	cupsRasterClose(ras);

        /*close input file*/
        if (fd!=0) {
                close(fd);
        }

	return 0;
}

