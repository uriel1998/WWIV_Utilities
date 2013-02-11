/************************************************************************
* WWIV 4.30 supposedly will only support DSZ, GSZ, or other "commercial"
* varieties of Zmodem for network transfers.  I don't like this.  So this 
* simple util will take DSZ formatted input, and pass it to CEXYZ, a freeware 
* Z-modem protocol.  
* If you wish to disable port locking, look for "LOCKED" in the source.
* A Urielsoft Production
*************************************************************************/
/*************************************************************************
		A COMPARISON OF COMMAND LINE PARAMETERS

	DSZ.EXE				  *        CEXYZ.EXE

   port <COM port #>                      * /P<number>
   speed <baud rate>                      * /L<locked baud>
					  * /B<true baud>
   pa<timeout value>                      * <null>
   pB<buffer size>                        * <null>
   pv<verbosity>                          * <null>
   pR1 (delete aborted)                   * <null>
   ha[ndshake] <on|off|both|slow>         * <null>
   -y  (overwrite existing)               * FAKEDSZ provides check
   -r (resume)                            * <null>
   -+ (append)                            * <null>
   [D/d] (carrier drop detect)            * <null>
   <s|r>x  (xmodem)                       * </S|/R>x
   <s|r>b  (ymodem)                       * </S|/R>y
   <s|r>z  (zmodem)                       * </S|/R>z
   <s|r>b -g (Y-g)                        * </S|/R>yg
   <s|r>x -c (X-crc 1024)                 * </S|/R>x1k
   est[imate] <rate>			  * <null>
   restrict   (pathnames)                 * CEXYZ provides this
   provided by FAKEDSZ (DIRECT IS DEFAULT)* /U<fos|dir|fas>

   The last argument is always the file or file list, and passed as is,
   save for the "restrict" check for file lists.

*************************************************************************/

#include <stdio.h>
#include <io.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <conio.h>
#include <dir.h>
#include <errno.h>
#include <process.h>

#define YES 0
#define NO 1

void main (int argc, char *argv[])
 {
 char passfile[255],scratch[255],action[7],commo[6];
 char path[255],sport[7],sspeed[10],lspeed[10];                                  /* LOCKED CHANGE */
 int placeholder,overwrite=NO;

 printf("þ FAKEDSZ, a UrielSoft production\nþ (c) 1999 Steve Saus\n");

 if (argc < 2)  /* if there's not enough to have rz filename */
  exit(100);

 sprintf(passfile,"%s",argv[argc-1]);  /* should be the filename/list */

 /*default settings */

 sprintf(action,"/SZ");
 sprintf(commo,"/UDIR");
 sprintf(sport,"/P1");
 sprintf(sspeed,"/B2400");
 sprintf(lspeed,"/L2400");  /* LOCKED CHANGE ONE */

 /* loop thru args   DON'T CATCH LAST 2 ARGS!   */
 for (placeholder=1;placeholder < argc-1;placeholder++)
  {
  printf("Place = %d, argc = %d\nCARG = %s\n",placeholder,argc,argv[placeholder]);
  sprintf(scratch,"%s",argv[placeholder]);
  switch (scratch[0])
   {
   case 'p':  /* port, and all the p? variables */
   case 'P':
    if (strncmpi("port",scratch,4) == 0)  /* everything else to /dev/null */
     {
     placeholder++;  /* the port number MUST be next */
     sprintf(sport,"/P%s",argv[placeholder]);
     }
    break;
   case 's':  /* speed and all the send variables */
   case 'S':
    if (strncmpi("speed",scratch,4) == 0)
     {
     placeholder++;  /* the port number MUST be next */
     sprintf(sspeed,"/B%s",argv[placeholder]);
     sprintf(lspeed,"/L%s",argv[placeholder]);  /* LOCKED CHANGE TWO */
     } else {   /* it's a send variable */
     switch (scratch[1])
      {
      case 'b':
      case 'B':
       if (strncmpi("-g",argv[placeholder+1],2)==0)
	 sprintf(action,"/SYG");
       else
	 sprintf(action,"/SY");
       break;
      case 'x':
      case 'X':
       if (strncmpi("-c",argv[placeholder+1],2)==0)
	 sprintf(action,"/SX1k");
       else
	 sprintf(action,"/SX");
       break;
      default:   /* assume Zmodem */
       sprintf(action,"/SZ");
      break;
      }
     }
    break;
   case 'r':  /* recieve variables and restrict */
   case 'R':
    if (strncmpi("rest",scratch,4) == 0)
     {
     /* it's a restrict, but CEXYZ provides that already */
     } else {   /* it's a recieve variable */
     switch (scratch[1])
      {
      case 'b':
      case 'B':
       if (strncmpi("-g",argv[placeholder+1],2)==0)
	sprintf(action,"/RYG");
       else
	sprintf(action,"/RY");
       break;
      case 'x':
      case 'X':
       if (strncmpi("-c",argv[placeholder+1],2)==0)
	sprintf(action,"/RX1K");
       else
	sprintf(action,"/RX");
       break;
      default:   /* assume Zmodem */
	sprintf(action,"/RZ");
      break;
      }
     }
    break;
   case '-':  /* overwrite, resume, and append */
    if (scratch[1]=='y')  /* overwrite */
     overwrite=YES;
    /* and everything else goes to /dev/null */
    break;
   default:   /* handshaking,estimate, and drop detect >> /dev/null */
    break;
   }  /* end of switch for each arg */
  }  /* end of looping through args */
 /*     overwrite      only on recieve    and if it's there... */
 if ((overwrite==YES)&&(action[1]=='R')&&(!access(passfile,0)))
  unlink(passfile);
 if (!access("FAKEDSZ.FAS",0))
  sprintf(commo,"/UFAS");
 if (!access("FAKEDSZ.FOS",0))
  sprintf(commo,"/UFOS");
 getcwd(scratch,80);
 sprintf(path,"%s\\CEXYZ.EXE",scratch);
 printf("í Your command line is: \n%s %s %s %s %s %s %s\n",path,action,sport,sspeed,lspeed,commo,passfile);
 spawnl(P_WAIT,path,"CEXYZ.EXE",action,sport,sspeed,lspeed,commo,passfile,NULL);
/*
 printf("í Your command line is: \n%s %s %s %s %s %s\n",path,action,sport,sspeed,commo,passfile);
 spawnl(P_WAIT,path,"CEXYZ.EXE",action,sport,sspeed,commo,passfile,NULL);
*/
 printf("þ Thank you for using Urielsoft!\n");
 }  /*EOF*/


