/******************************************************************************
* COMPANY       : MARTEL ENGINEERING
* PROJECT       : LINUX DRIVER
*******************************************************************************
* NAME          : sample1.c
* DESCRIPTION   : Basic sample of libmartel use
* CVS           : $Id: sample1.c,v 1.1 2006/08/01 09:07:04 chris Exp $
*******************************************************************************
* HISTORY       :
*   27-Jul-06   CML     Initial revision
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <martel/martel.h>

/* PRIVATE DEFINITIONS ------------------------------------------------------*/
static const char id_str[] = "$Id: sample1.c,v 1.1 2006/08/01 09:07:04 chris Exp $";
static const char ticket[] = "Hello world!\n";

/* PRIVATE FUNCTIONS --------------------------------------------------------*/

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
        void *port;
        int errnum;
        
        /* create communication port from URI */
        port = martel_create_port(argv[1]);

        /* open communication port */
        errnum = martel_open(port);

        /* write to communication port */
        errnum = martel_write(port,ticket,sizeof(ticket)-1);
        
        /* close communication port */
        errnum = martel_close(port);

        /* destroy communication port */
        errnum = martel_destroy_port(port);

        return 0;
}

