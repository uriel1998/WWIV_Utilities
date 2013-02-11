/**********************************************************************
* SPLTMAIL is another tinyutility for WWIV BBSes using the PPP project to
* overcome the 32k/message limitation.  It is fed the appropriate directory
* to search, and finds UNK?????.MSG files.  It then sees if they're over
* 32k (approximately).  If so, it then sees if they're a true binary
* MIME.  If NOT, it then proceeds to split the message in chunks of
* 100 lines, putting the header info at the top of each new message.
* Why 100 lines?  Simply because that way I can be SURE that it won't
* overflow again, as we are assuming 254 char per line, and 100 lines is
* 25,400 bytes.
************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <dir.h>

#define YES 0
#define NO 1

/* global define */
char *p;
char subject[255],returnpath[255],date[255],from[255];
char sender[255],replyto[255],to[255];
int subint=NO,rpathint=NO,dateint=NO,fromint=NO,sendint=NO,reptoint=NO,toint=NO;


void driveto (char *feed);
int headerinfo(char *rawline);
void header_write (FILE *writer,int time);

void main (int argc, char *argv[])
{
char fileline[255],path[MAXPATH],beginpath[MAXPATH],scratch[255];
int fileint,ffirstint,ismime=NO;
long fsize;
struct ffblk ffblk;
FILE *read,*write;

if (argc < 2)
 {
 printf("í Format is SPLTMAIL <path:\\to\\spool> \n");
 exit(100);
 }
printf("þ SPLTMAIL ver 1.1\nþ A UrielSoft Production.\n");
sprintf(path,"%s",argv[1]);
getcwd(beginpath,255);
printf("þ CWD: %s\nþ ENDWD: %s\n",path,beginpath);
driveto(path);
chdir(path);
sprintf(scratch,"%s\\UNK*.MSG",path);
ffirstint=findfirst(scratch,&ffblk,0);
do {
 sprintf(scratch,"%s\\%s",path,ffblk.ff_name);
 printf("þ %s - ",ffblk.ff_name);
 fileint=_open(scratch,O_RDONLY);
 fsize=filelength(fileint);
 printf(" %ld bytes\n",fsize);
 if (fsize > 31000)  /* Too big! It's MINE ALL MINE! */
  {
  read=fdopen(fileint,"rt");
  ismime=NO;			/* using reptoint as a line counter during*/
  reptoint=0;                   /* MIME checking, and using ismime as */
  if (read==NULL)  		/* as the universal abort */
   ismime=YES;
  printf("þ Testing to see if binary attachment...\n");
  do {   			/* CHECKING FOR MIME */
   fgets(fileline,255,read);
   reptoint++;
   strlwr(fileline);
   if ((strstr(fileline,"begin 6"))||(strstr(fileline,"base64"))||(strstr(fileline,"type: app")))
    ismime=YES;
   } while ((ismime==NO)&&(reptoint<=400));
  reptoint=NO;
  rewind(read);
  do {                          /* reading in header info that I want */
   fgets(fileline,255,read);
   if (strncmpi("0RReturn-Path:",fileline,16) == 0)
    {
    rpathint=headerinfo(fileline);
    sprintf(returnpath,"%s",p);
    }
   if (strncmpi("0RDate:",fileline,8) == 0)
    {
    dateint=headerinfo(fileline);
    sprintf(date,"%s",p);
    }
   if (strncmpi("0RFrom:",fileline,8) == 0)
    {
    fromint=headerinfo(fileline);
    sprintf(from,"%s",p);
    }
   if (strncmpi("0RSubject:",fileline,11) == 0)
    {
    subint=headerinfo(fileline);
    sprintf(subject,"%s",p);
    }
   if ((strncmpi("0RSender:",fileline,10) == 0)||(strncmpi("0RX-Sender:",fileline,12) == 0))
    {
    sendint=headerinfo(fileline);
    sprintf(sender,"%s",p);
    }
   if (strncmpi("0RReply-To:",fileline,12) == 0)
    {
    reptoint=headerinfo(fileline);
    sprintf(replyto,"%s",p);
    }
   if (strncmpi("0RTo:",fileline,6) == 0)
    {
    toint=headerinfo(fileline);
    sprintf(to,"%s",p);
    }
   } while ((fileline[0]=='')&&(ismime==NO));
	/* well formed inbound e-mail        that's not MIME */
  if (subint==YES&&fromint==YES&&toint==YES&&ismime==NO)
   {
   printf("þ Splitting file....\n");
   ismime=2999;  		/* ismime is now our file counter.  We start */
   fsize=2999;
   while (!feof(read))
    {
    do {        /*trying to make non-collision filenames */
     ismime++;
     sprintf(scratch,"%s\\UNK-%04d.MSG",path,ismime);
     } while (!access(scratch,0));
    write=fopen(scratch,"wb");
    if (fsize==2999)  /* first one */
     header_write(write,NO);
    else
      header_write(write,YES);
    fsize=0;
    do {
     long checker;
     static char buffer[1024];

     checker=fread(buffer,1,1024,read);
     fwrite(buffer,1,checker,write);      /* writes only read characters */
     if (checker<1024)  /* hit eof in the booger bit */
      {
      fprintf(write,"\n\nEnd of Message - SPLTMAIL 1.0\n");
      fclose(write);
      fsize=31;
      fgetc(read);    /* should return eof */
      break;
      }
     fsize++;
     } while (fsize <= 28);
    fprintf(write,"\n\nContinued in next message - SPLTMAIL 1.0\n");
    fclose(write);
    }
   fclose(read);
   sprintf(scratch,"%s\\%s",path,ffblk.ff_name);
   unlink(scratch);
   }
  } else {
  _close(fileint);  /* because never fdopened */
  }
 ffirstint=findnext(&ffblk);
 } while (!ffirstint);
driveto(beginpath);
chdir(beginpath);
printf("þ Thank you for using URIELSOFT!\n");
}

void header_write (FILE *writer,int time)
{
char scratchs[255];

sprintf(scratchs,"0RFrom: %s",from); 	/* these three are always there */
fputs(scratchs,writer);
sprintf(scratchs,"0RTo: %s",to);		/* should already be \n on each line */
fputs(scratchs,writer);
sprintf(scratchs,"0RSubject: %s",subject);
fputs(scratchs,writer);
if (rpathint==YES)
 {
 sprintf(scratchs,"0RReturn-Path: %s",returnpath);
 fputs(scratchs,writer);
 }
if (dateint==YES)
 {
 sprintf(scratchs,"0RDate: %s",date);
 fputs(scratchs,writer);
 }
if (reptoint==YES)
 {
 sprintf(scratchs,"0RReply-To: %s",replyto);
 fputs(scratchs,writer);
 }
if (sendint==YES)
 {
 sprintf(scratchs,"0RX-Sender: %s",sender);
 fputs(scratchs,writer);
 }
fprintf(writer,"\n");
if (time==YES)
 fprintf(writer,"Continued from previous message...\n");
}

int headerinfo(char *rawline)  /* used when reading in header info */
{
p=strpbrk(rawline,":");
*p++;
*p++;
return (YES);
}

void driveto (char *feed)  /* switch drives if needbe */
{
char drive;
int dto,dfrom;

strupr(feed);
drive=feed[0];
dto=(drive-65);
dfrom=getdisk();
if (dto!=dfrom)
 setdisk(dto);
}
