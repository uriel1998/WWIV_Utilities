/**********************************************************************
This file is (c) Steven Saus 1997 and is distributed as FREEWARE
to any non-corporate or non-government users (though why either would
want this I dunno).    Support and/or suggestions (especially the latter)
should be directed to:  surgicalsteel@mindspring.com (Surgical Steel)
************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <conio.h>
#include <io.h>
#include <share.h>
#include <errno.h>
#include <dir.h>
#include <time.h>
#include <process.h>
#include <direct.h>
#include "fiw.h"

#define WAIT_TIME 10
#define TRIES 100
#define SHARE_LEVEL 10
#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40

char bbs[80],filedir[80],in[80],out[80],mail[80],from[80];
int multitasker = 0;
enum BOOLEAN {FALSE, TRUE};
enum CHECKING {ISNT, IS};

typedef struct {
	unsigned short	tosys,		/* destination system */
			touser,		/* destination user */
			fromsys,	/* originating system */
			fromuser;	/* originating user */
	unsigned short	main_type,	/* main message type */
			minor_type;	/* minor message type */
	unsigned short	list_len;	/* # of entries in system list */
	unsigned long	daten;		/* date/time sent */
	unsigned long	length;		/* # of bytes of msg after header */
	unsigned short	method;		/* method of compression */
} net_header_rec;


/*SHARE AND MULTITASKERS*/

void dv_pause(void)
{
  __emit__(0xb8, 0x1a, 0x10, 0xcd, 0x15);
  __emit__(0xb8, 0x00, 0x10, 0xcd, 0x15);
  __emit__(0xb8, 0x25, 0x10, 0xcd, 0x15);
}

void win_pause(void)
{
  __emit__(0x55, 0xb8, 0x80, 0x16, 0xcd, 0x2f, 0x5d);
}

int get_dos_version(void)
{
  _AX = 0x3000;
  geninterrupt(0x21);
  if (_AX % 256 >= 10) {
    multitasker |= MT_OS2;
  }
  return (_AX);
}

int get_dv_version(void)
{
  int v;

  if (multitasker & MT_OS2)
    return 0;
  _AX = 0x2b01;
  _CX = 0x4445;
  _DX = 0x5351;
  geninterrupt(0x21);
  if (_AL == 0xff) {
    return 0;
  } else {
    v = _BX;
    multitasker |= MT_DESQVIEW;
    return v;
  }
}

int get_win_version(void)
{
  int v = 0;

  __emit__(0x55, 0x06, 0x53);
  _AX = 0x352f;
  geninterrupt(0x21);
  _AX = _ES;
  if (_AX | _BX) {
    _AX = 0x1600;
    geninterrupt(0x2f);
    v = _AX;
    if (v % 256 <= 1)
      v = 0;
  }
  __emit__(0x5b, 0x07, 0x5d);
  if (v != 0)
    multitasker |= MT_WINDOWS;
  return (v);
}

int get_nb_version(void)
{
  _AX = 0;
  geninterrupt(0x2A);
  return (_AH);
}

void detect_multitask(void)
{
  get_dos_version();
  get_win_version();
  get_dv_version();
  if (multitasker < 2)
    if (get_nb_version())
      multitasker = MT_NB;
}

void giveup_timeslice(void)
{
  if (multitasker) {
    switch (multitasker) {
 case 1:
 case 3:
	dv_pause();
	break;
      case 2:
      case 4:
      case 5:
      case 6:
      case 7:
	win_pause();
        break;
      default:
        break;
    }
  }
}

long sh_lseek(int handle, long offset, int fromwhere)
{
  if (handle == -1) {
    return (-1L);
  }
  return (lseek(handle, offset, fromwhere));
}

FILE *fsh_open(char *path, char *fmode)
{
  FILE *f;
  int count, share, md, fd;
  char drive[MAXDRIVE], dir[MAXDIR], file[MAXFILE], ext[MAXEXT];

  share = SH_DENYWR;
  md = 0;
  if (((char *) strchr(fmode, 'w')) != NULL) {
    share = SH_DENYRD;
    md = O_RDWR | O_CREAT | O_TRUNC;
  } else
    if (((char *) strchr(fmode, 'a')) != NULL) {
    share = SH_DENYRD;
    md = O_RDWR | O_CREAT;
  } else {
    md = O_RDONLY;
  }
  if (((char *) strchr(fmode, 'b')) != NULL) {
    md |= O_BINARY;
  }
  if (((char *) strchr(fmode, '+')) != NULL) {
    md &= ~O_RDONLY;
    md |= O_RDWR;
    share = SH_DENYRD;
  }
  fd = open(path, md | share, S_IREAD | S_IWRITE);
  if (fd < 0) {
    count = 1;
    fnsplit(path, drive, dir, file, ext);
    if ((access(path, 0)) != -1) {
      delay(WAIT_TIME);
      fd = open(path, md | share, S_IREAD | S_IWRITE);
      while (((fd < 0) && (errno == EACCES)) && (count < TRIES)) {
	if (count % 2)
	  delay(WAIT_TIME);
	else
	  giveup_timeslice();
	count++;
        fd = open(path, md | share, S_IREAD | S_IWRITE);
      }
    }
  }
  if (fd > 0) {
    if (((char *) strchr(fmode, 'a')) != NULL)
      sh_lseek(fd, 0L, SEEK_END);
    f = fdopen(fd, fmode);
    if (!f) {
      close(fd);
    }
  } else
    f = 0;
  return (f);
}


void driveto (char *feed)
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


int readini(void)
{

#define INI_DIRS   0x01
#define INI_ROUTE  0x02
#define INI_XMAIL  0x03
#define INI_MDIR   0x04

char cursect;
char line[121], *ss;
FILE *ini;
int ini_section;

sprintf(line,"%s\\FIW.INI",bbs);
if ((ini = fsh_open(line, "rt")) == NULL)
 return (0);
while (fgets(line, 80, ini))
 {
 ss = NULL;
 ss = strtok(line, "=");
 if (ss)
  ss = strtok(NULL, "\n");
 if ((line[0] == ';') || (line[0] == '\n') || (strlen(line) == 0))
  continue;
 if (strnicmp(line, "[DIRS]", 6) == 0)
  {
  cursect = INI_DIRS;
  ini_section |= INI_DIRS;
  continue;
  }
 if (strnicmp(line, "[ROUTE]", 7) == 0)
  {
  cursect = INI_ROUTE;
  ini_section |= INI_ROUTE;
  continue;
  }
 if (strnicmp(line, "[MOVEDIR]", 9) == 0)
  {
  cursect = INI_MDIR;
  ini_section |= INI_MDIR;
  continue;
  }
 if (strnicmp(line, "[X-MAILER]", 10) == 0)
  {
  cursect = INI_XMAIL;
  ini_section |= INI_XMAIL;
  continue;
  }
 if (cursect & INI_DIRS)
  {
  if (strnicmp(line, "FILEDIR", 7) == 0)
   {
   if (ss)
    strcpy(filedir, ss);
   strupr(filedir);
   continue;
   }
  if (strnicmp(line, "INDIR", 5) == 0)
   {
   if (ss)
    strcpy(in, ss);
   strupr(in);
   continue;
   }
  if (strnicmp(line, "OUTDIR", 6) == 0)
   {
   if (ss)
    strcpy(out, ss);
   strupr(out);
   continue;
   }
  if (strnicmp(line, "MAILDIR", 7) == 0)
   {
   if (ss)
    strcpy(mail, ss);
   strupr(mail);
   continue;
   }
  if (strnicmp(line, "FROM", 4) == 0)
   {
   if (ss)
    strcpy(from, ss);
   continue;
   }
  }
 }
if (ini != NULL)
 fclose(ini);
return (1);
}


enum BOOLEAN check_pkt (char *name, short c)
{

  FILE *check;
  size_t br;
  net_header_rec nh;
  short a,b;
  long l;
  char s[255],s1[255];

 if (c==1)
  {
  sprintf(s,"%s\\%s",filedir,name);
  check=fsh_open(s,"rb");
  if (check==NULL)
   return(FALSE);
  if (fread(&nh,1,sizeof(nh),check) < sizeof(nh))
   {
   fclose(check);
   return(FALSE);
   }// if (fread(&nh,1,sizeof(nh),check) < sizeof(nh))
  l = sizeof(net_header_rec)+(2*nh.list_len);
  fseek(check,l,SEEK_SET);
  fread(&s1,61,1,check);
  fclose(check);
  strupr(s1);
  if ((strstr(s1,"FI")) && (strstr(s1,"->")))
   return(TRUE);
  }
  sprintf(s,"%s\\spool\\%s",filedir,name);
  check=fsh_open(s,"rt");
  if (check==NULL)
   return(FALSE);
  while ((fgets(s,255,check)) && (!feof(check)))
   {
   strupr(s);
   if (strstr(s,"X-MAILER: "))
    {
    fclose(check);
    if ((strstr(s,"FIDO")) || (strstr(s,"FIW")))
     return(TRUE);
    sprintf(s1,"%s\\FIW.INI",bbs);
    check=fsh_open(s1,"rt");
    if (check==NULL)
     return(FALSE);
    b=0;
    while ((fgets(s1,255,check)) && (!feof(check)))
     {
     strupr(s);
     if (s[0]==';')
      continue;
     if (strnicmp(s,"[X-MAILER]",10) == 0)
      {
      b=1;
      continue;
      }
     if ((s[0]=='[') && (s[1]!='X'))
      {
      b=0;
      continue;
      }
     if ((b==1) && (strstr(s1,s)))
      {
      fclose(check);
      return(TRUE);
      }
     }
    fclose(check);
    return(FALSE);
    }
    if (strstr(s,"SUBJECT: "))
     {
     fclose(check);
     if ((strstr(s,"FI")) && (strstr(s,"->")))
      return(TRUE);
     else
      return(FALSE);
     }
    }
   fclose(check);
   return(FALSE);
  }


void import_pkt(char *name,short WWIV)
{

FILE *read,*write;
short a=0,b=WWIV;
char readpath[MAXPATH],writepath[MAXPATH];
char s[255];

printf("ð Importing %s\n",name);
if (b==0)
 {
 sprintf(readpath,"%s\\spool\\%s",filedir,name);
 } else {
 sprintf(readpath,"%s\\%s",filedir,name);
 }
sprintf(writepath,"%s\\%s",in,name);
write=fsh_open(writepath,"wt");
read=fsh_open(readpath,"rt");
if ((write==NULL) || (read==NULL))
 {
 printf("í  I cannot open a neccessary file!\n");
 fclose(read);fclose(write);exit(100);
 }
while ((fgets(s,255,read)) && (a!=1) && (!feof(read)))
 {
 strlwr(s);
 if (strstr(s,"application/octe"))
  {
  fputs(s,write);
  a=1;
  }
 }
while ((fgets(s,255,read)) && (!feof(read)))
 {
 if (strstr(s,"-"))
  continue;
 fputs(s,write);
 }
fclose(read);fclose(write);
unlink(readpath);
driveto(in);
chdir(in);
sprintf(readpath,"%s\\MIME64.EXE",bbs);
if (access(readpath,0))
 {
 printf("  You need to put the MIME64.EXE file in your BBS directory.\n");
 exit(100);
 } else {
 spawnl(P_WAIT,readpath,"MIME64.EXE",writepath,NULL);
 remove(writepath);
 }
}

void import (void)
{

struct ffblk ffblk;
char s[255];
short a;

sprintf(s,"%s\\spool\\unk*.*",filedir);
a=findfirst(s,&ffblk,0);
while (!a)
 {
 if (check_pkt(ffblk.ff_name,0) ==TRUE)
  import_pkt(ffblk.ff_name,0);
 a=findnext(&ffblk);
 }
sprintf(s,"%s\\p0*.*",filedir);
a=findfirst(s,&ffblk,0);
while (!a)
 {
 if (check_pkt(ffblk.ff_name,1) ==TRUE)
  import_pkt(ffblk.ff_name,1);
 a=findnext(&ffblk);
 }
}

enum BOOLEAN game_dir(char *drive, char *dir)
{

char s[MAXPATH],s1[MAXPATH],s2[MAXPATH];
FILE *check;
short b;

sprintf(s,"\0");
sprintf(s1,"%s%s",drive,dir);
strncat(s,s1,(strlen(s1)-1));
sprintf(s1,"%s\\FIW.INI",bbs);
check=fsh_open(s1,"rt");
if (check==NULL)
 return(FALSE);
b=0;
while ((fgets(s1,255,check)) && (!feof(check)))
 {
 strupr(s1);
 sscanf(s1,"%s",s2);
 if (s1[0]==';')
  continue;
 if (strnicmp(s1,"[MOVEDIR]",9) == 0)
  {
  b=1;
  continue;
  }
 if ((s1[0]=='[') && (s1[1]!='M'))
  {
  b=0;
  continue;
  }
 if ((b==1) && (strstr(s,s2)))
  {
  fclose(check);
  return(TRUE);
  }
 }
fclose(check);
return(FALSE);
}


void export (void)
{

typedef struct
 {
 char from[36], to[36], subject[72], date[20];
 unsigned int tread, dnode, onode;
 int cost;
 unsigned int onet, dnet, dzone, ozone, dpoint, opoint, repto, attr, nextrep;
 } MSGHDR;

typedef struct
 {
 unsigned int onode, dnode, onet, dnet, attr;
 int cost;
 } PMHDR;

typedef struct
 {
 unsigned int onode, dnode, year, month, day, hour, min, sec,
  rate, ver, onet, dnet;
 char product, rev_lev, password[8];
 unsigned int ozone, dzone;
 char domain[8];
 unsigned int qozone, qdzone, opoint, dpoint;
 } PKTHDR;

/*--------------------------------------------------------------------------*/
/* FIDO Message attributes (attr) (informational)                           */
/*--------------------------------------------------------------------------*/
#define MSGPRIVATE 0x0001  /* private message,          0000 0000 0000 0001 */
#define MSGCRASH   0x0002  /* accept for forwarding     0000 0000 0000 0010 */
#define MSGREAD    0x0004  /* read by addressee         0000 0000 0000 0100 */
#define MSGSENT    0x0008  /* sent OK (remote)          0000 0000 0000 1000 */
#define MSGFILE    0x0010  /* file attached to msg      0000 0000 0001 0000 */
#define MSGFWD     0x0020  /* being forwarded           0000 0000 0010 0000 */
#define MSGORPHAN  0x0040  /* unknown dest node         0000 0000 0100 0000 */
#define MSGKILL    0x0080  /* kill after mailing        0000 0000 1000 0000 */
#define MSGLOCAL   0x0100  /* FidoNet vs. local         0000 0001 0000 0000 */
#define MSGXX1     0x0200  /*                           0000 0010 0000 0000 */
#define MSGXX2     0x0400  /* STRIPPED by FidoNet<tm>   0000 0100 0000 0000 */
#define MSGFRQ     0x0800  /* file request              0000 1000 0000 0000 */
#define MSGRRQ     0x1000  /* receipt requested         0001 0000 0000 0000 */
#define MSGCPT     0x2000  /* is a return receipt       0010 0000 0000 0000 */
#define MSGARQ     0x4000  /* audit trail requested     0100 0000 0000 0000 */
#define MSGURQ     0x8000  /* update request            1000 0000 0000 0000 */


static char  attrstr[16][11] = {
		 "MSGPRIVATE","MSGCRASH","MSGREAD","MSGSENT","MSGFILE","MSGFWD",
		 "MSGORPHAN","MSGKILL","MSGLOCAL","MSGXX1","MSGXX2","MSGFRQ",
		 "MSGRRQ","MSGCPT","MSGARQ","MSGURQ"};

MSGHDR       mh;
PMHDR        pm;
PKTHDR      pkt;
struct tm   *tim;
time_t secs_now;

struct ffblk ffblk;
char s[120],s1[120],s2[MAXPATH],*p,name[13],nameb[13],bound[20];
size_t    br;
size_t    er;
size_t    cr;
int exp,attach,kill;
int a,b,c,g,i,loop;
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT], *str_now;
char dzone[5],dnet[5],dnode[5],dpoint[5],rzone[5],rnet[5],rnode[5],rpoint[5],to[80];
unsigned int x;
long d;
FILE *t,*m,*f,*o;

 str_now = ctime(&secs_now);
 srand(time(NULL) % 37);
 sprintf(s,"%s\\FIW.INI",bbs);
 t=fsh_open(s,"rt");
/*note: t not closed!*/
 a = 1;
/*testing for prior messages*/
 while (b != 1)
  {
  sprintf(s,"%s\\mqueue\\FIW.%d",filedir,a);
  f = fsh_open(s,"r");
  if (f == NULL)
   {
   b = 1;
   } else {
   b = 0;
   fclose(f);
   a = (a+1);
   }
  } /*endtest - a is == to 1st new msg number*/
 for (loop=0;loop < 2;loop++)
  {
  driveto(mail);
  chdir(mail);
  sprintf(s,"%s\\*.*",mail);
  b=findfirst(s,&ffblk,0);
  while (!b)
   {
   sprintf(nameb,"%s",ffblk.ff_name);
   sprintf(s,"%s\\%s",mail,nameb);
   f=fsh_open(s,"rb");
   cr = fread(&mh,1,sizeof(mh),f);
   if(cr < sizeof(mh))
    {
    fclose(f);
    } else {
    exp=0;
    kill=0;
    attach=0;
    for(x = 0;x < 16;x++)
     {
     sprintf(s,NULL);
     if(mh.attr & (1 << x))
      {
      sprintf(s,"%s",attrstr[x]);
      } else {
      sprintf(s,"boogedy");  /*gotta have a sense of humor!*/
      }
      if(strstr(s,"MSGFILE"))       /*is it attach bit?*/
       {
       attach=1;
       }
      if(strstr(s,"MSGKILL"))
       {
       kill=1;
       }
     } /*loop attr*/
    fseek(f,190,SEEK_SET);
    g=0;
    while ((!feof(f)) && (g!=1))
     {
     fscanf(f,"%s",s);
     if (strstr(s,"MSGID:"))
      {
      fscanf(f,"%s",s);
      mh.dzone=atoi(s);      /*get the MSGID zone*/
      g=1;
      }
     }
    g=0;
    b=0;
    while (b!=1)
     {
     fgets(s,80,t);
     if (strstr(s,"[ROUTE]")) /*places us at beginning of route section*/
      b=1;
     }
    fgets(s1,120,t);
    fgets(to,78,t);
    while ((g!=1) && (!feof(t)))
     {
     if (s1[0]!=';') /*commenting check, sorta*/
     {
     if (s1[0]=='[')
      {
      g=1;
      break;
      }
     b=0;
     p=NULL;
     p=s;
     do
      {
      if ((s1[b]=='/') || (s1[b]==':') || (s1[b]=='.'))
       {
       *p++=' ';
       } else {
       *p++=s1[b];
       }
       b++;
      } while ((s1[b]!='\0') && (s1[b]!='\n'));
     *p='\0';
     sscanf(s,"%s%s%s%s%s%s%s%s",rzone,rnet,rnode,rpoint,dzone,dnet,dnode,dpoint);
     if (strstr(rnet,"*"))
      {
      if (atoi(rzone)==mh.dzone)
       {
       exp=1;g=1;
       break;
	}
       }
      if (strstr(rnode,"*"))
       {
       if ((atoi(rzone)==mh.dzone) && (atoi(rnet)==mh.dnet))
	{
	exp=1;g=1;
	break;
	}
       }
      if (strstr(rpoint,"*"))
       {
       if ((atoi(rzone)==mh.dzone) && (atoi(rnet)==mh.dnet) && (atoi(rnode)==mh.dnode))
	{
	exp=1;g=1;
	break;
	}
       }
      if ((atoi(rzone)==mh.dzone) && (atoi(rnet)==mh.dnet) && (atoi(rnode)==mh.dnode))
       {
       exp=1;g=1;
       break;
       }
      fgets(s1,120,t);
      fgets(to,78,t);
      }
      } /*end of configuration*/
    rewind(t);
    if ((attach==1) && (exp==1))
     {  /*preparing packet file attach*/
     printf("ð Packet File Attach.  Exporting.\n");
     fseek(f,72, SEEK_SET);
     fscanf(f,"%s",s);
     strupr(s);
     if (strstr(s,out))
      {
      c=(strlen(out));
      fseek(f,(c+73), SEEK_SET);
      fscanf(f,"%s",name);
      fclose(f);
      exp=2;
      } else {
      fclose(f);
      fnsplit(s,drive,dir,file,ext);
      sprintf(name,"%s%s",file,ext);
      sprintf(s1,"%s\\%s",out,name);
      if (!access(s1,0))
	remove(s1);
      if (game_dir(drive,dir)==TRUE)
       {
       rename(s,s1);
       exp=2;attach=0;             /*setting up export of netmail*/
       } else {
       f=fsh_open(s,"rb");
       m=fsh_open(s1,"wb");
       while (!feof(f))
	fputc(fgetc(f),m);
       fclose(f);
       fclose(m);
       exp=2;attach=0;             /*setting up export of netmail*/
       }
      }
     }
     if ((attach==0) && (exp > 0)) /*export netmail*/
      {
      printf("ð Converting netmail to file attach. \n");
      g=0;
      sprintf(s,"%s\\%s",mail,nameb);
      fclose(f);
      f=fsh_open(s,"rb");
      while (g!=1)
       {
       sprintf(s1,"%d.PKT",rand());
       sprintf(s,"%s\\%s",out,s1);
       if (access(s,0))
	{
	g=1;
	}
       i++;
       }
      m=fsh_open(s,"wb");
      pkt.onode=mh.onode;pkt.dnode=atoi(dnode);
      pkt.onet=mh.onet;pkt.dnet=atoi(dnet);
      pkt.ozone=atoi(dzone);pkt.dzone=atoi(dzone);
      pkt.qozone=atoi(dzone);pkt.qdzone=atoi(dzone);
      pkt.opoint=mh.opoint;pkt.dpoint=atoi(dpoint);
      time(&secs_now);
      tim = localtime(&secs_now);
      pkt.year=tim->tm_year;pkt.month=tim->tm_mon;pkt.day=tim->tm_mday;
      pkt.hour=tim->tm_hour;pkt.min=tim->tm_min;pkt.sec=tim->tm_sec;
      pkt.rate=0;pkt.ver=2;
      pkt.product=0;pkt.rev_lev=0;
      sprintf(pkt.password,"\0\0\0\0\0\0\0\0");
      sprintf(pkt.domain,"\0\0\0\0\0\0\0\0");
      cr = fwrite(&pkt,1,sizeof(pkt),m);
      fputs("XPKT",m);
      fputc('\0',m);
      pm.onode=mh.onode;pm.dnode=mh.dnode;pm.onet=mh.onet;pm.dnet=mh.dnet;
      pm.attr=mh.attr;pm.cost=0;
      cr = fwrite(&pm,1,sizeof(pm),m);
      fprintf(m,"%s",mh.date);
      fputc('\0',m);
      fprintf(m,"%s",mh.to);
      fputc('\0',m);
      fprintf(m,"%s",mh.from);
      fputc('\0',m);
      fprintf(m,"%s",mh.subject);
      fputc('\0',m);
      fseek(f,190,SEEK_SET);
      i=0;
      i=fgetc(f);
      while(!feof(f))
       {  /*copying text*/
       fputc(i,m);
       i=fgetc(f);
       }
      fputc('\0',m);
      fputc('\0',m);
      fputc('\0',m);
      fclose(f);
      fclose(m);
      kill=1;
      i = 1; b = 0;
      while (b != 1)
       {
       sprintf(s,"%s\\%d.MSG",mail,i);
       if (access(s,0))
	{
	b=1;
	}
       i++;
       }
      f=fsh_open(s,"wb");
      sprintf(mh.from,"FIW 2.0");sprintf(mh.to,"SYSOP");
      sprintf(mh.subject,"%s\\%s",out,s1);
      sprintf(mh.date,"%s",str_now);mh.tread=0;
      mh.onode=pkt.onode;mh.dnode=atoi(dnode);
      mh.onet=pkt.onet;mh.dnet=atoi(dnet);
      mh.ozone=atoi(dzone);mh.dzone=atoi(dzone);
      mh.opoint=mh.opoint;mh.dpoint=atoi(dpoint);
      mh.cost=0;mh.repto=0;mh.attr=432;mh.nextrep=0;
      cr = fwrite(&mh,1,sizeof(mh),f);
      fclose(f);  /*now its own file attach!*/
      }
     if ((kill==1)&&(exp > 0))
      {
      sprintf(s,"%s\\%s",mail,nameb);
      remove(s);
      }
     /*encode the sucker in the BBS directory as msg.a*/
     if (exp==2)
     {
      sprintf(s,"%s\\%s",out,name);
      c=_open(s,O_RDONLY);
      d=filelength(c);
      _close(c);
      if (d >= 0)
       {
       sprintf(s,"%s\\FIW.%d",bbs,a);
       sprintf(s1,"%s\\%s",out,name);
       sprintf(s2,"%s\\MIME64.EXE",bbs);
       if (!access(s2,0))
	spawnl(P_WAIT,s2,"MIME64.EXE","-e",s1,s,NULL);
       else {
	printf("í MIME64.EXE must be in your BBS directory!\n");
	exit(100);
	}
       /*The MIME is in the main bbs dir as MSG.a  Orig filename is NAME,
       file length is %ld d*/
       driveto(bbs);
       chdir(bbs);
       sprintf(s,"%s\\FIW.%d",bbs,a);
       o = fsh_open(s,"rt");
       if (o==NULL)
	exit(100);
       sprintf(s,"%s\\MQUEUE\\FIW.%d",filedir,a);
       while (!access(s,0))
	{
	sprintf(s,"%s\\MQUEUE\\FIW.%d",filedir,a);
	a++;
        }
       printf("ð Encoding as %s\n",s);
       f = fsh_open(s,"wt");
       sprintf(s,"Message-ID :<%d%d@random-pc>\n",rand(),rand());
       fputs(s,f);
       fprintf(f,"Reply-To: %s\n",from);
       fprintf(f,"X-Mailer: FIW %s\n",VERSION);
       sprintf(s,"Mime-Version: 1.0\n");
       fputs(s,f);
       sprintf(bound,"Message-Boundary-%d",rand());
       fprintf(f,"Content-type: Multipart/Mixed; boundary=%s\n",bound);
       fprintf(f,"To: %s",to);
       fprintf(f,"From: %s\n",from);
       sprintf(s,"Subject:  FIW -> %s (%ld bytes)\n",name,d);
       fputs(s,f);
       fprintf(f,"\n\n");
       fprintf(f,"--%s\n",bound);
       fprintf(f,"Content-type: text/plain; charset=US-ASCII\n");
       fprintf(f,"Content-transfer-encoding: 7BIT\n");
       fprintf(f,"Content-description: Mail message body\n");
       fprintf(f,"\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
       fprintf(f,"File: %s (%ld bytes)\n",name,d);
       fprintf(f,"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
       fprintf(f,"               Fido2Internet2WWIV\n");
       fprintf(f,"                   %s\n",VERSION);
       fprintf(f,"  	  All software is a kludge.  Some are just  \n");
       fprintf(f,"                 more honest about it. \n");
       fprintf(f,"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n\n");
       fprintf(f,"--%s\n",bound);
       fgets(s,100,o);
       sprintf(s,"Content-type: application/octet-stream; name=\x22%s\x22\n",name);
       fputs(s,f);
       fgets(s,100,o);
       fprintf(f,"Content-transfer-encoding: BASE64\n\n");
       c=0;
       c=fgetc(o);
       while ((!feof(o)))
	{
	fputc(c,f);
	c=fgetc(o);
	}/*done with copying MIME portion*/
       fprintf(f,"--%s--",bound);
       fclose(o);
       fclose(f);
       if (kill==1)
	{
	sprintf(s,"%s\\%s",out,name);
	creat(s,S_IWRITE);
	}
       sprintf(s,"%s\\FIW.%d",bbs,a);
       remove(s);
       a=a+1;
       }
      }
     sprintf(s,"%s",mail);
     driveto(s);
     chdir(s);
     }
    b=findnext(&ffblk);
    } /*out of files*/
   }
  fclose(t);
  driveto(bbs);
  chdir(bbs);
  }

void main (void)
{

char *p,s[MAXPATH];
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];

printf("\n\n\n");
printf("þ Fido2Internet2WWIV Import/Export %s", VERSION);
printf("\n");

detect_multitask();

if ((p=searchpath("CONFIG.DAT"))==NULL)
 exit(100);
fnsplit(p,drive,dir,file,ext);
sprintf(s,"%s%s",drive,dir);
strncat(bbs,s,(strlen(s)-1));
giveup_timeslice();
if ((readini())==0)
 {
 printf("í Cannot find FIW.INI!  Fatal Error! \n");
 exit(100);
 }
giveup_timeslice();
import();
giveup_timeslice();
export();
giveup_timeslice();

printf("þ Thanks for using FIW!\n");
}//end of file
