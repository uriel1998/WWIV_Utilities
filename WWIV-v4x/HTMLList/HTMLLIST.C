/**************************************************************************
* HTML BBSlist generator v.1.0 (c)1998 and by Steven Saus.
* Multitasking/share code ripped wholesale from the PPP project (around B-76)
* Explanation just inside void main().  While this code is freeware,
* if you use it, please give me credit.  Distribution of a compiled executable
* is expressly FORBIDDEN.  No guarantees of suitability are provided;  use
* this code at your own risk.  This is a UrielSoft production.
***************************************************************************/

#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <ctype.h>
#include <conio.h>
#include <io.h>
#include <share.h>
#include <errno.h>
#include <dir.h>
#include <time.h>
#include <process.h>
#include <direct.h>

 /*multitasker*/
FILE *fsh_open(char *path, char *fmode);
long sh_lseek(int handle, long offset, int fromwhere);
void giveup_timeslice(void);
void detect_multitask(void);
int get_nb_version(void);
int get_dos_version(void);
int get_win_version(void);
int get_dv_version(void);
enum BOOLEAN is_fido(unsigned short type);

typedef struct {
     unsigned char type;
     char          name[16];
     char          dir[69];
     unsigned short sysnum;
     char          fake[12];
} networks_net;

#define net_type_WWIVnet 0
#define net_type_fidonet 1
#define WAIT_TIME 10
#define TRIES 100
#define SHARE_LEVEL 10
#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40

int multitasker = 0;
enum BOOLEAN { FALSE, TRUE };


enum BOOLEAN is_fido(unsigned short type)
{
char s[5];

sprintf(s,"%u",type);
if (strstr(s,"1"))
 return(TRUE);
else
 return(FALSE);
}

void main (void)
{

int a,b,mail,web,count;
char s[255],s1[255],name[80],email[255],www[255],phone[20],sm[50];
char filelist[100],olist[100],*p,temp[20],node[10],outfile[20];
char region[100];
FILE *data,*write,*read;
networks_net  anet;

/*************************************************************************
*  FTN nodelists must be copied to the appropriate fake WWIV directory
*  and renamed NODELIST.NET.
*  This code produces a number of *.txt files dependent upon a modified
*  REGIONS.DAT file (commented at the end of this file, with BBS listings
*  for each area.  These files can then be handled by a simple batch file
*  with a for %%f in (*.txt) do copier %%f, which calls batch file COPIER:
*  type myheader.htm > c:\mysite\%1
*  :myheader needs this html markup <FONT SIZE=+2> <TABLE Border=1>
*  :as well as the <HTML><HEAD>, etc - everything except the actual table
*  :note that we'll eventually have to rename *.txt *.htm...
*  type %1 >> c:\mysite\%1
*  type ender.htm >> c:\mysite\%1
**************************************************************************/

sprintf(s,"c:\\faith\\datum\\NETWORKS.DAT");
data=fsh_open(s,"rb");
fread(&anet,1,sizeof(anet),data);
while (!feof(data))
 {
 if (is_fido(anet.type)==FALSE)
  {
  printf("%s\n",anet.name);
  for (count=1;count<=98;count++)
   {
   sprintf(s,"%sBBSLIST.%d",anet.dir,count);
   if (access(s,0))
    continue;
   printf("%s",s);
   read=fsh_open(s,"rt");
   while ((fgets(s,254,read)) && (s!=NULL))
    {
    if (s[0]!='@')
     continue;
    sscanf(s,"%s",node);
    mail=FALSE;
    web=FALSE;
    sprintf(s1,"%saddress.net",anet.dir);
    if (!access(s1,0))
     {
     FILE *addy;

     addy=fsh_open(s1,"rt");
     while (!feof(addy))
      {
      fgets(s1,254,addy);
      if (strnicmp(s1,node,strlen(node))==0)
       {
       sscanf(s1,"%s%s",node,email);
       mail=TRUE;
       fseek(addy,0,SEEK_END);
       fgets(s1,254,addy);
       }
      }
     fclose(addy);
     }
    p=strpbrk(s,"*");
    p++;  /* should take us past the asterisk */
    sscanf(p,"%s",phone);
    strupr(phone);
    p=strpbrk(s,"\x22");
    sprintf(name,"%s",p);
    for (b=0;b<=strlen(name);b++)
     {
     if (name[b]=='\x22')
      name[b]=' ';
     if ((name[b]=='\n') || (name[b]=='\r'))
      name[b]=' ';
     }
    b=0;
    if (name!=NULL)
     {
     FILE *addy;
     int i;
     sprintf(s,"c:\\uriel\\regions.dat");
     addy=fsh_open(s,"rt");
     while (fgets(s,254,addy)!=NULL)
      {
      if (isalpha(s[0])!=0)
      sscanf(s,"%s%s",region,outfile);
      if (strnicmp(s,phone,3)==0)
      fseek(addy,0,SEEK_END);
      }
     fclose(addy);
     }
    sprintf(s,"c:\\uriel\\temp\\%s.txt",outfile);
    write=fsh_open(s,"rt");
    while (fgets(s,254,write)!=NULL)
     {
     if ((strstr(s,phone)) || (strstr(s,name)) || (strstr(phone,"PUBLISH"))|| (strstr(phone,"XXX")) || (strstr(phone,"NET")))
      {
      b=1;
      printf("Dupe %s",name);
      break;
      }
     }
    fclose(write);
    if (b==0)
     {
     sprintf(s,"c:\\faith\\www.net");
     if (!access(s,0))
      {
      FILE *addy;

      addy=fsh_open(s,"rt");
      while (!feof(addy))
       {
       fgets(s,254,addy);
       if (strnicmp(s,phone,strlen(phone))==0)
	{
	sscanf(s,"%s%s",sm,www);
	web=TRUE;
	fseek(addy,0,SEEK_END);
	fgets(s,254,addy);
	}
       }
      fclose(addy);
      }
     if (name!=NULL)
      {
      sprintf(s,"c:\\uriel\\temp\\%s.txt",outfile);
      write=fsh_open(s,"at");
      fprintf(write,"<TR><TD>");
      if (mail==TRUE)
       fprintf(write,"<A HREF=\x22mailto:%s\x22> ",email);
      fprintf(write," %s",phone);
      if (mail==TRUE)
       fprintf(write,"</A>");
      fprintf(write,"</TD><TD>");
      if (web==TRUE)
       fprintf(write,"<A HREF=\x22%s\x22>",www);
      fprintf(write,"%s",name);
      if (web==TRUE)
       fprintf(write,"</A>");
      fprintf(write,"</TD><TD>");
      fprintf(write,"%s",region);
      fprintf(write,"</TD><TR>\n");
      fclose(write);
      }
     }
    } /* end of bbslist! */
   fclose(read);
   }   /* end of bbslists */
  } else {
  int c,zone=1;

  fclose(read);
  sprintf(s,"%snodelist.txt",anet.dir);
  if (access(s,0))
   {
   fread(&anet,1,sizeof(anet),data);
   continue;
   }
  printf("%s",s);
  read=fsh_open(s,"rt");
  if (read==NULL)
   {
   fread(&anet,1,sizeof(anet),data);
   continue;
   }
  while (fgets(s,254,read)&&(s!=NULL))
   {
   if (strnicmp(s,"Down",4)==0)
    continue;
   if (s[0]==';')
    continue;
   sprintf(region,"\0");
   if (strnicmp(s,"Zone,2,",7)==0)
    {
    sprintf(region,"Europe");
    sprintf(outfile,"europe");
    zone=2;
    }
   if (strnicmp(s,"Zone,3,",7)==0)
    {
    sprintf(region,"Australia/NZ");
    sprintf(outfile,"aust");
    zone=3;
    }
   if (strnicmp(s,"Zone,4,",7)==0)
    {
    sprintf(region,"Latin America");
    sprintf(outfile,"latin");
    zone=4;
    }
   if (strnicmp(s,"Zone,5,",7)==0)
    {
    sprintf(region,"Africa");
    sprintf(outfile,"africa");
    zone=5;
    }
   if (strnicmp(s,"Zone,6,",7)==0)
    {
    sprintf(region,"Asia");
    sprintf(outfile,"asia");
    zone=6;
    }
   if (zone!=1)   /* shouldn't affect other zones outside of fidonet */
    continue;
   mail=FALSE;
   web=FALSE;
   c=0;
   for (b=0;b<strlen(s)&&c<2;b++)
    {
    if (s[b]==',')
     c++;
    }
   c=0;
   while (s[b]!=',')
    {
    if (s[b]=='_')
     name[c]=' ';
    else
     name[c]=s[b];
    c++;
    b++;
    }
   name[c]='\0';
   for (b=c=0;b<strlen(s)&&c<5;b++)
    {
    if (s[b]==',')
     c++;
    }
   b++;
   b++;
   c=0;
   while (s[b]!=',')
    {
    phone[c]=s[b];
    c++;
    b++;
    }
   phone[c]='\0';
   if (name!=NULL)
    {
    FILE *addy;
    int i;
    sprintf(s,"c:\\uriel\\regions.dat");
    addy=fsh_open(s,"rt");
    while (fgets(s,254,addy)!=NULL)
     {
     if (isalpha(s[0])!=0)
     sscanf(s,"%s%s",region,outfile);
     if (strnicmp(s,phone,3)==0)
     fseek(addy,0,SEEK_END);
     }
    fclose(addy);
    }
   sprintf(s,"c:\\uriel\\temp\\%s.txt",outfile);
   fclose(write);
   write=fsh_open(s,"rt");
   b=0;
   strupr(phone);
   while (fgets(s,254,write)!=NULL)
    {
//    printf("%s\nphone:%s/name:%s",s,phone,name);
    if ((strstr(s,phone)) || (strstr(s,name)) || (strstr(phone,"PUBLISH")) || (strstr(phone,"XXX")) || (strstr(phone,"NET")))
     {
     b=1;
     printf("Dupe %s",name);
     break;
     }
    }
   fclose(write);
   if ((b==0) && (name!=NULL) && (zone <=1))
    {
    sprintf(s,"c:\\faith\\www.net");
    if (!access(s,0))
     {
     FILE *addy;
     addy=fsh_open(s,"rt");
     while (!feof(addy))
      {
      fgets(s,254,addy);
      if (strnicmp(s,phone,strlen(phone))==0)
       {
       sscanf(s,"%s%s",sm,www);
       web=TRUE;
       fseek(addy,0,SEEK_END);
       fgets(s,254,addy);
       }
      }
     fclose(addy);
     }
    sprintf(s,"%saddress.net",anet.dir);
    if (!access(s,0))
     {
     FILE *addy;
     addy=fsh_open(s,"rt");
     while (!feof(addy))
      {
      fgets(s,254,addy);
      if (strnicmp(s,phone,strlen(phone))==0)
       {
       sscanf(s,"%s%s",sm,email);
       mail=TRUE;
       fseek(addy,0,SEEK_END);
       fgets(s,254,addy);
       }
      }
     fclose(addy);
     }
    sprintf(s,"c:\\uriel\\temp\\%s.txt",outfile);
    write=fsh_open(s,"at");
    fprintf(write,"<TR><TD>");
    if (mail==TRUE)
     fprintf(write,"<A HREF=\x22mailto:%s\x22> ",email);
    fprintf(write," %s",phone);
    if (mail==TRUE)
     fprintf(write,"</A>");
    fprintf(write,"</TD><TD>");
     if (web==TRUE)
    fprintf(write,"<A HREF=\x22%s\x22>",www);
     fprintf(write," %s ",name);
    if (web==TRUE)
     fprintf(write,"</A>");
    fprintf(write,"</TD><TD>");
    fprintf(write,"%s",region);
    fprintf(write,"</TD><TR>\n");
    fclose(write);
    }
   }
  fclose(read);
  }   /* end of bbslists */
 fread(&anet,1,sizeof(anet),data);
 }   /* end of networks.dat */
fclose(data);
}

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
