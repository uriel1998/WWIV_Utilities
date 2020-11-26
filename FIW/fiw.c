/*YOU BLOODY MORON!  USE FTSEND FOR THE INTERNET STUFF!!!!!!!!!!!!!!!!!!!*/
/* storing sections broken */
/*Mark, do a search for the string "Marklook" (no quotes, screw case).  That
will probably be the area of interest.  I'll have a copy with me at school
too, so feel free to ask questions of me. - Surge */

/************************************************************************
* This file is (c) Steven Saus 1999 and is considered FREEWARE.  The author
* can be contacted at: surgicalsteel@mindspring.com (Surgical Steel)
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
#include <spawno.h>

#define VERSION "v2.92bugfix2"

/************************************************************************
* Multitasker defines - also ripped from PPP project
************************************************************************/
#define WAIT_TIME 10
#define TRIES 100
#define SHARE_LEVEL 10
#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40

/*global defines*/
#define FALSE  0
#define TRUE   1
#define OTHER  2
#define MIME64 1
#define UUE    2
#define ARCMAIL 1
#define BINKLEY 2

/* global strings */
int multitasker = 0;
char bbs[MAXPATH],filedir[MAXPATH],in[MAXPATH],from[80];
char out[MAXPATH],mail[MAXPATH],freqdir[MAXPATH],keyword[MAXPATH];
unsigned long targcrc;
int basezone,mailer,keepsent=0,failed=1;



/*the great big list of functions */
void send_ack(int success);
void import_seat (char *name, int wwiv);
void receiptfreq (int which, FILE *check);
void inbound_segment(void);
void write_control (void);

int import_pkt(char *name, int wwiv, int method);
int check_pkt (char *name, int wwiv);
void import (void);

int game_dir(char *drive, char *dir);
void export(void);
void write_email (char path[MAXPATH],char original[20]);
int read_route (void);

int readini(void);
void driveto (char *feed);
char crc32file(char *name);

 /*multitasker*/
FILE *fsh_open(char *path, char *fmode);
long sh_lseek(int handle, long offset, int fromwhere);
void giveup_timeslice(void);
void detect_multitask(void);
int get_nb_version(void);
int get_dos_version(void);
int get_win_version(void);
int get_dv_version(void);
/*************************************************************************
*  CRC 32 stuff, taken from snippet CRC.C - no attributation provided.
**************************************************************************/


typedef unsigned long int UNS_32_BITS;
static UNS_32_BITS crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet, crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

char crc32file(char *name)
{
	register FILE *fin;
	register unsigned long oldcrc32;
	register unsigned long oldcrc;
	register c;
	register long charcnt;

	oldcrc32 = 0xFFFFFFFF; charcnt = 0;
	if ((fin=fopen(name, "rb"))==NULL) {
		perror(name);
	return -1;
	}
	while ((c=getc(fin))!=EOF) {
		++charcnt;
		oldcrc32 = UPDC32(c, oldcrc32);
	}
	if (ferror(fin)) {
		perror(name);
		charcnt = -1;
	}
	fclose(fin);
    oldcrc = oldcrc32; /*= ~oldcrc32; */
    /* change as recommended by SEAT authors - SS */
    targcrc=oldcrc;  //targcrc is an unsigned long global variable
    return 0;
}


/************************************************************************
* SEAT message structure v 1.00
************************************************************************/

struct seat {
	char from[80],   /*duh*/
	     file[256],   /*filename*/
	     id[255],     /*msg-id*/
	     time[20];   /*unix-timestamp*/
	int  method,     /*uuencode or mime64*/
	     seg,        /*which segment is it?*/
	     total;      /*out of how many?*/
	char segid[255]; /*segment id*/
	unsigned long crc,    /*crc32*/
	     segcrc;
	int  freq,       /*is there a freq? */
	     receipt;    /*is there an ack/nak? */
} *seatin;

/************************************************************************
* Structure for linked list for control file for inbound multipart
* SEAT messages and keeping track of all sent SEAT messages
************************************************************************/
struct control {
	char ofile[256],    /*original filename*/
	     fileid[255],  /*used for inbound Ack/Nak request*/
	     from[80];
	unsigned long crc;
	long time;  /* this *should* work for time */
	int  total,        /* total number of segments - seg # tracked by extention*/
	     store;        /* renamed stored files */
	struct control *next;	   /* for linked list */
	int  finish,       /* TRUE||FALSE */
	     outbound;     /* TRUE||FALSE */
} *point,*start,*prior;

/************************************************************************
* Structure used for linked list in routing
************************************************************************/

struct route{
	int zone,	/*FTN attributes */
	    net,
	    node,
	    point,	/*route determines level of routing on-the-fly */
	    route,	/* 0=noroute/1=zone/2=net/3=node               */
	    method,	/* UUE||MIME64 */
	    seat;       /*is it seat? */
	long split;      /* in k */
	char to[121];	/* to string */
	struct route *next;	/* for linked list */
} *rpoint,*rprior,*rstart;


struct id{
       int zone,
	   net,
	   node,
	   point;
       struct id *nexter;  /* for linked list */
} *idpoint,*idprior,*idstart;

/************************************************************************
* driveto to switch drives, if needbe, readini for obvious reasons.
* readini modified off of PPPproject sourcecode by (mainly) Frank Reid
************************************************************************/
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

int readini(void)    /*duh*/
{

#define INI_DIRS   0x01
#define INI_ROUTE  0x02
#define INI_XMAIL  0x03
#define INI_MDIR   0x04
#define INI_BINK   0x05
#define INI_ID     0x06

char cursect;
char line[121], *ss;
FILE *ini;
int ini_section;

sprintf(line,"%s\\FIW.INI",bbs);
/*sprintf(line,"c:\\uriel\\fiw\\fiw.ini");*/
if ((ini = fsh_open(line, "rt")) == NULL)
 return (0);
while (fgets(line, 80, ini)!=NULL)
 {
 strupr(line);
 ss = NULL;
 ss = strtok(line, "=");
 if (ss)
  ss = strtok(NULL, "\n");
 if ((line[0] == ';') || (line[0] == '\n') || (strlen(line) == 0))
  continue;
 if (strnicmp(line, "[BINKLEY]", 9) == 0)
  {
  cursect = INI_BINK;
  ini_section |= INI_BINK;
  mailer=BINKLEY;
  continue;
  }
 if (strnicmp(line, "[DIRS]", 6) == 0)
  {
  cursect = INI_DIRS;
  ini_section |= INI_DIRS;
  continue;
  }
 if (strnicmp(line, "[ID]", 4) == 0)
  {
  cursect = INI_ID;
  ini_section |= INI_ID;
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
   continue;
   }
  if (strnicmp(line, "FREQDIR", 7) == 0)
   {
   if (ss)
    strcpy(freqdir, ss);
   continue;
   }
  if (strnicmp(line, "KEYWORD", 7) == 0)
   {
   if (ss)
    strcpy(keyword, ss);
   continue;
   }
  if (strnicmp(line, "KEEPSENT", 8) == 0)
   {
   if (ss)
    keepsent=atoi(ss);
   continue;
   }
  if (strnicmp(line, "INDIR", 5) == 0)
   {
   if (ss)
    strcpy(in, ss);
   continue;
   }
  if (strnicmp(line, "OUTDIR", 6) == 0)  /* primary outbound of Binkley */
   {
   if (ss)
    {
    char q[MAXPATH],q1[5];   /* defines */
    sscanf(ss,"%s%s",q,q1);
    if (q1 != NULL)
     basezone=atoi(q1);      /* basezone for binkley */
    strcpy(out, q);
    }
   continue;
   }
  if ((strnicmp(line, "MAILDIR", 7) == 0) && (mailer==ARCMAIL))
   {
   if (ss)
    {
    strcpy(mail, ss);
    }
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


/************************************************************************
*  Inbound packet handling - import, then check_pkt, then import_pkt.
*  SEAT routines handled separately (though they utilize import_pkt)
************************************************************************/

void import (void)
{

struct ffblk ffblk;
char s[255];
short a;
                                        /* MARKLOOK - you may just need to */
sprintf(s,"%s\\spool\\unk*.*",filedir); /* change this line here ... to star*/ 
a=findfirst(s,&ffblk,0);                /* dot star... after that it passes */
while (!a)                              /* full filename */
 {
 switch(check_pkt(ffblk.ff_name,FALSE)) /*checking in spool*/
  {
  case OTHER:
   import_seat(ffblk.ff_name,FALSE);  /*SEAT protocols - if needed, will*/
   break;                             /*call import_pkt from there      */
  case TRUE:
   if (import_pkt(ffblk.ff_name,FALSE,MIME64)==FALSE)
    printf("í Something went seriously wrong!\n");
   break;
  default:
   break;
  }
 a=findnext(&ffblk);
 }
sprintf(s,"%s\\p0*.*",filedir);
a=findfirst(s,&ffblk,0);
while (!a)
 {
 switch(check_pkt(ffblk.ff_name,TRUE)) /*checking in WWIV*/
  {
  case OTHER:
   import_seat(ffblk.ff_name,TRUE);
   break;
  case TRUE:
   import_pkt(ffblk.ff_name,TRUE,MIME64); /* did have a check here, but gave */
   break;                                /* false error messages */
  default:
   break;
  }
 a=findnext(&ffblk);
 }
}

/************************************************************************
* This function checks for packet type (if any).  If need be, may
* perform check for FTN- in message body to also trigger as SEAT.
************************************************************************/

int check_pkt (char *name, int wwiv)    /*done*/
{
  FILE *check;
  short a,b=0;
  char s[255],s1[255];

 if (wwiv==TRUE)
  {
  /*declarations*/
 /************************************************************************
 *  WWIV's net message header structure - ver 4.24a (c) Wayne Bell
 ************************************************************************/

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
  long l;
  net_header_rec nh;
  size_t br;

  sprintf(s,"%s\\%s",filedir,name);
  check=fsh_open(s,"rb");
  if (check==NULL)
   return(FALSE);
  if (fread(&nh,1,sizeof(nh),check) < sizeof(nh))
   {
   fclose(check);
   return(FALSE);
   }
  l = sizeof(net_header_rec)+(2*nh.list_len);
  fseek(check,l,SEEK_SET);
  fread(&s1,61,1,check);
  l=ftell(check);
  fclose(check);
  strupr(s1);
  if (strnicmp(s1,"FTN MAIL TRANSPORT",18)==0)  /*inbound SEAT*/
   return(OTHER);
  if ((strstr(s1,"FI")) && (strstr(s1,"->")))   /*non-SEAT*/
   return(TRUE);
  check=fsh_open(s,"rt");
  fseek(check,l,SEEK_SET);
  } else {
  sprintf(s,"%s\\spool\\%s",filedir,name);
  check=fsh_open(s,"rt");
  if (check==NULL)
   return(FALSE);
  }
 s1[0]='\0';
 for(a=0;(a < 300) && (b<2);a++)
  {
  if (fgets(s,255,check)==NULL)
   b=3;
  strupr(s);
  if (strstr(s,"X-MAILER: "))
   {
   char *p;  /*declaration */

   p=NULL;
   p=strpbrk(s,":");
   p++;
   sprintf(s1,"%s",p);
   s1[(strlen(s1)-1)]='\0';
   b++;
   continue;
   }
  if (strstr(s,"SUBJECT: "))
   {
   if (strstr(s,"FTN MAIL TRANSPORT"))
    {
    fclose(check);
    return(OTHER);
    }
   b++;    /*found subject, not SEAT */
   continue;
   }
  }
 fclose(check);
 if ((strstr(s1,"FIDO")) || (strstr(s1,"FIW")))
  return(TRUE);
 sprintf(s,"%s\\FIW.INI",bbs);
// sprintf(s,"c:\\uriel\\fiw\\fiw.ini");
 check=fsh_open(s,"rt");
 if (check==NULL)
  return(FALSE);
 b=0;    /*b is serving as an INI_SECT tag here...*/
 while ((fgets(s,255,check)) && (!feof(check)))
  {
  strupr(s);
   s[(strlen(s)-1)]='\0';
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

/************************************************************************
* Function that actually imports packet (despite origin) - if SEAT
* segment, imports segment to get dealt with later by import_segment.
* it also kludges the filename!!!!!!
************************************************************************/

int import_pkt(char *name, int wwiv, int method)
{

FILE *read,*write;
char readpath[MAXPATH],writepath[MAXPATH],filename[255];
char s[255];
int application=FALSE;  /* serving as a "section" - avoids problems with text
			   header like Fraley's */

printf("ð Importing %s\n",name);
if (wwiv==FALSE)
 sprintf(readpath,"%s\\spool\\%s",filedir,name);
else
 sprintf(readpath,"%s\\%s",filedir,name);
sprintf(writepath,"%s\\%s",in,name);
write=fsh_open(writepath,"wt");
read=fsh_open(readpath,"rt");
if ((write==NULL) || (read==NULL))
 {
 printf("í  I cannot open a neccessary file!\n");
 fclose(read);fclose(write);return(FALSE);
 }
while ((fgets(s,255,read)) && (s!=NULL))  /*copying file*/
 {
 char *p; int sloopy=0;  /* declarations */
 int changed=FALSE;

 if (strnicmp(s,"BEGIN 6",7)==0)  /* default is mime - if finds this, and NOT seat, */
  {
  method=UUE;              /* handles UUencoded files properly */
  application=TRUE;
  }
 if (method==MIME64)
  {
  if (strnicmp(s,"CONTENT-TYPE: APPLICATION/",26)==0)
   {
   application=TRUE;
   changed=OTHER;
   filename[0]='\0';
   p=strpbrk(s,"\"");  /* gets you to the first quote */
   if (p==NULL)
    {
    p=strpbrk(s,"=");
    changed=TRUE;
    }
   p++;                /* gets you past the first quote */
   do {
    filename[sloopy]=*p++;
    sloopy++;
    } while ((*p!='\0') && (*p!='\"') && (*p!='\r') && (*p!='\n'));
   filename[sloopy]='\0';
   }
  } else {
  if (strnicmp(s,"BEGIN 6",7)==0)
   {
   changed=TRUE;  /* neccessary to put full path in file for uudecode */
   p=NULL;
   p=strrchr(s,'\\');  /* making sure no pathname */
   if (p==NULL)
    {
    p=strrchr(s,':');  /* no pathname */
    if (p==NULL)
     {
     p=strpbrk(s,"6");
     p++;p++; /* 0, 0 */
     }
    }
   p++;  /* \, :, space */
   do {
    filename[sloopy]=*p++;
    sloopy++;
    } while ((*p!='\0') && (*p!='\n') && (*p!='\r'));
   filename[sloopy]='\0';
   }
  }
 if (changed!=FALSE)
  {
  if (strlen(filename) > 13)   /* it is NECCESSARY to kludge filename using Frank's UU */
   {
   char tname[9],text[5];  /* declarations */

   printf("í Truncating filename ");
   changed=TRUE;
   p=NULL;
   p=strrchr(filename,'.');
   if (p!=NULL)
    sscanf(p,"%s",text); /* should include period */
   else
    sprintf(text,". ");
   text[4]='\0';
   for (sloopy=0;sloopy<=7;sloopy++)
    {
    /* shouldn't have any cr/lfs, since we already trimmed 'em above */
    if ((filename[sloopy]==' ') || (filename[sloopy]=='.') || (filename[sloopy]=='\0'))
     break;
    tname[sloopy]=filename[sloopy];
    tname[sloopy+1]='\0';  /* do it every time so not caught unawares */
    }
   sprintf(filename,"%s%s",tname,text);
   printf("to %s\n",filename);
   }

  sprintf(writepath,"%s\\%s",in,filename);
  if (!access(writepath,0))
   {
   char tname[9];  /* declaration */
   char scratch[MAXPATH];

   printf("í Already existant - renaming to: ");
   changed=TRUE;
   for (sloopy=0;sloopy<=8;sloopy++)
    {
    if ((filename[sloopy]=='.') || (filename[sloopy]=='\0'))
     break;
    tname[sloopy]=filename[sloopy];
    tname[sloopy+1]='\0';  /* do it every time so not caught unawares */
    }
   sloopy=0;
   do {
    sprintf(filename,"%s.%03d",tname,sloopy++);
    sprintf(scratch,"%s\\%s",in,filename);
    } while (!access(scratch,0));
   printf("%s\n",filename);
   }
  if (changed==TRUE)
   {
   if (method==MIME64)
    sprintf(s,"Content-Type: application/octet-stream; name=\"%s\"\n",filename);
   else
    sprintf(s,"begin 600 %s\\%s\n",in,filename);
   }
  }
 if (application==TRUE)
  fputs(s,write);
 }
fclose(read);fclose(write);
unlink(readpath);
sprintf(writepath,"%s\\%s",in,name); /* reassign, if needed */
driveto(in);
chdir(in);
if (method==MIME64)
 {
 sprintf(readpath,"%s\\MIME64.EXE",bbs);
 if (access(readpath,0))
  return(FALSE);
 else {
  spawnl(P_WAIT,readpath,"MIME64.EXE",writepath,NULL);
  sprintf(readpath,"%s\\%s",in,filename);  /*checking for results*/
  if (seatin != NULL)  /* is SEAT packet */
   {
   crc32file(readpath);
   if (targcrc!=seatin->segcrc)
    {
    remove(writepath);
    return(FALSE);
    } else {
    remove(writepath);
    return(TRUE);
    }
   }
  sprintf(readpath,"%s\\%s",in,filename);  /*checking for results*/
  if (!access(readpath,0))
   {
   remove(writepath);
   return(TRUE);
   } else {
   return(FALSE);
   }
  }
 } else {
 sprintf(readpath,"%s\\UUDECODE.COM",bbs);
 if (access(readpath,0))
  {
  return(FALSE);
  } else {
  /*uudecode infile */
  spawnl(P_WAIT,readpath,"UUDECODE.COM",writepath,NULL);
  sprintf(readpath,"%s\\%s",in,filename);  /*checking for results*/
  if (seatin != NULL)  /* is SEAT packet */
   {
   crc32file(readpath);
   if (targcrc!=seatin->segcrc)
    {
    remove(writepath);
    return(FALSE);
    } else {
    remove(writepath);
    return(TRUE);
    }
   }
  if (!access(readpath,0))
   {
   remove(writepath);
   return(TRUE);
   } else
   return(FALSE);
  }
 }
}



/************************************************************************
* SEAT inbound routines - import_seat, send_ack, send_nak, freq_it,
* receipt (even though freq called later, it counts for inbound in my
* logic.  Also, *true* support for kludged filenames.
************************************************************************/

void import_seat (char *name, int wwiv)
{

FILE *check;
char s[255],header[50],tiny[30];

seatin=(struct seat *)malloc(sizeof(struct seat));
seatin->freq=seatin->receipt=FALSE;               /*initial values*/
if (wwiv==FALSE)
 sprintf(s,"%s\\spool\\%s",filedir,name);
else
 sprintf(s,"%s\\%s",filedir,name);
check=fsh_open(s,"rt");
 if (check!=NULL)
 {
 while ((fgets(s,255,check)) && (!feof(check)))
  {
  if (strstr(s,"FROM:"))
   sscanf(s,"%s%s,",header,seatin->from);
  if (strstr(s,"FTN-FILE:"))
   sscanf(s,"%s%s",header,seatin->file);

  if (strstr(s,"FTN-FILE-ID:"))
   sscanf(s,"%s%s",header,seatin->id);
  if (strstr(s,"FTN-SEG-ID:"))
   sscanf(s,"%s%s",header,seatin->segid);
  if (strstr(s,"FTN-DATE:"))
   sscanf(s,"%s%s",header,seatin->time);
  if (strstr(s,"FTN-CRC32"))
   {
   sscanf(s,"%s%s",header,tiny);
   seatin->crc=strtol(tiny,NULL,16);
   }
  if (strstr(s,"FTN-SEG-CRC32"))
   sscanf(s,"%s%s",header,tiny);
   seatin->segcrc=strtol(tiny,NULL,16);
  if (strstr(s,"FTN-RECEIPT: "))
   seatin->receipt=TRUE;
  if (strstr(s,"FTN-FREQ: "))
   seatin->freq=TRUE;
  if (strstr(s,"FTN-ENCODING:"))
   {
   if (strstr(s,"UUE"))
    seatin->method=UUE;
   else
    seatin->method=MIME64;
   }
  if (strstr(s,"FTN-SEG"))
   {
   char *p;    /*declarations*/
   short i=0;

   p=tiny;
   while ((s[i]!=' ')&&(s[i-1]!=':'))  /*going past FTN-SEG: */
    i++;
   while ((i<=strlen(s))&&(s[i]!='-')) /*should just copy in first number*/
    {
    *p++=s[i]; i++;
    }
   *p='\0';i++;   /*ending segment, advancing past - */
   seatin->seg=atoi(tiny);  /*converting to integer*/
   sprintf(tiny,"\0");
   p=tiny;
   while ((i<=strlen(s))&&(s[i]!='\0'))
    {
    *p++=s[i]; i++;
    }
   *p='\0';   /*ending of */
   seatin->total=atoi(tiny);  /*converting to integer*/
   }
  strupr(s);
  if ((strstr(s,"BEGIN 6"))&&(strstr(s,"APPLICATION/OCTET")))
   break;				/*found beginning of attach*/
  }
 rewind(check);
 if (seatin->freq)
  receiptfreq(TRUE,check);
 if (seatin->receipt)
  receiptfreq(FALSE,check);
 fclose(check);
 if (import_pkt(name,wwiv,seatin->method)==TRUE)
  send_ack(TRUE);                         /* check in send_ack for */
 else                                             /* use of segid or id    */
  send_ack(FALSE);
 if (seatin->total==seatin->seg)
  inbound_segment();
 } else
 printf("í  Error accessing SEAT file %s\n",name);
free(seatin);
}

/************************************************************************
*  This merely *moves* the inbound FREQ and/or reciept to deal with it
*  at the end of the run.
*************************************************************************/

void receiptfreq(int which, FILE *check)
{
FILE *write;
int counting=0;
char scr[255];

do{
 counting++;
 if (which==TRUE)    /* FREQ */
  sprintf(scr,"%s\\fiw\FREQ.%03d",filedir,counting);
 else
  sprintf(scr,"%s\\fiw\RCPT.%03d",filedir,counting);
 } while (access(scr,0));
write=fsh_open(scr,"rt");
while ((fgets(scr,255,check)) && (scr!=NULL))  /* copying file */
 {
 strupr(scr);
 if ((strstr(scr,"CONTENT-TYPE: APPLICA")) || (strstr(scr,"BEGIN 6")))
  break;          /* we are past the body now */
 fputs(scr,write);
 }
fclose(write);
rewind(check);  /* since this is an ADDRESS, it should work */
}


/***********************************************************************
* Deals with incoming segments, combines if needbe, calls send_ack
* on its own, utilized the control structure.
***********************************************************************/

void inbound_segment(void)
{
char s[255],s1[MAXPATH];
FILE *ctrl;
int checker=0;

sprintf(s,"%s\\CONTROL.FIW",bbs);
ctrl=fsh_open(s,"rb");
start=(struct control *)malloc(sizeof(struct control));
if (ctrl!=NULL)
 {
 fread(&start,1,sizeof(start),ctrl);
 start->next=NULL;
 prior=start;
 /* note printout linewrap! */
 for (point=(struct control *)malloc(sizeof(struct control));fread(&point,1,sizeof(point),ctrl)==sizeof(point);point=(struct control *)malloc(sizeof(struct control)))
  {
  prior->next=point;
  point->next=NULL;
  prior=point;
  }
 /* checking for filename */
 fclose(ctrl);
 point=start;
 do {
  prior=point->next;
  if (strnicmp(point->ofile,seatin->file,(strlen(seatin->file)))==0)
   break;    /* point is pointing to applicable structure */
  point=point->next;
  } while (prior!=NULL);     /* quit when next is NULL */
 if (strnicmp(point->ofile,seatin->file,(strlen(seatin->file)))==0)
  {  /* double check neccessary w/o using additional variable */
  FILE *write;   /* defines */
  int c;

  sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,seatin->seg);
  if (access(s,0)) /*don't want to overwrite prior file */
   {
   write=fsh_open(s,"wb");
   sprintf(s,"%s\\%s",in,seatin->file);
   ctrl=fsh_open(s,"rb");
   c=fgetc(ctrl);
   while (!feof(ctrl))
    {
    fputc(c,write);
    c=fgetc(ctrl);
    }
   fclose(write);
   fclose(ctrl);
   unlink(s);
   time(&point->time);
   } else {
   sprintf(s,"%s\\%s",in,seatin->file);
   unlink(s);
   }
  for (c=1;c <= point->total;c++)
   {
   sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,c);
   if (!access(s,0))    /* tests sequentially, then breaks and uses */
    continue;           /* c as indicator of test                   */
   c=-1;
   break;
   }
  if (c)
   {
   sprintf(s,"%s\\%s",in,point->ofile);
   write=fsh_open(s,"wb");
   for (c=1;c <= point->total;c++)
    {
    int d;              /* defines */
    sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,c);
    ctrl=fsh_open(s,"rb");
    d=fgetc(ctrl);
    while(!feof(ctrl))
     {
     fputc(d,write);
     d=fgetc(ctrl);
     }
    fclose(ctrl);
    unlink(s);
    }
   fclose(write);
   sprintf(s,"%s\\%s",in,point->ofile);
   crc32file(s);
   if (targcrc != point->crc)
    {
    unlink(s);
    seatin->segcrc=seatin->segcrc;  /* so that send_ack will work */
    sprintf(seatin->segid,"%s",seatin->id);
    send_ack(FALSE);  /* only need to send ack/nak if need request */
    }                 /* whole file again */
   point->finish=TRUE;
   } /*not a full set */
  write_control();
  point=start;
  do {
   prior=point->next;
   free(point);
   point=prior;
   } while (prior != NULL);
  } else {   /* no match, prior is last, point is at null */
  point=(struct control *)malloc(sizeof(struct control));
  prior->next=point;
  }
 } else {  /* no file */
 point=start;
 }
if (start != NULL)   /* i.e. - didn't already free the memory */
 {
 strcpy(point->ofile,seatin->file);
 strcpy(point->fileid,seatin->id);
 point->crc=seatin->crc;
 point->total=seatin->total;
 point->next=NULL;
 strcpy(point->from,seatin->from);
 do {
  sprintf(s,"%s\\fiw\\%08d.%03d",filedir,checker,seatin->seg);
  checker++;
  } while (!access(s,0));
 point->store=checker--;    /* additional checker++ above, corrected here */
 time(&point->time);
 point->finish=FALSE;
 point->outbound=FALSE;
 write_control();
 point=start;
 do {
  prior=point->next;
  free(point);
  point=prior;
  } while (prior != NULL);
 }
}

/* This SEAT function is used both in inbound and outbound processing */

void write_control (void)
{
FILE *write;
char s[MAXPATH];

sprintf(s,"%s\\CONTROL.FIW",bbs);
if (!access(s,0))
 unlink(s);
write=fsh_open(s,"wb");
point=start;
do {
 prior=point->next;
 if (point->finish==FALSE)
  fwrite(&point,1,sizeof(point),write);
 point=point->next;
 } while (prior!=NULL);
fclose(write);
}

/***********************************************************************
* Deals with inbound acks
***********************************************************************/

void inbound_ack(void)
{
char s[255],s1[MAXPATH],s2[255],to[121];
FILE *ctrl;
int fack;
struct ffblk ffblk;
long place;

sprintf(s,"%s\\CONTROL.FIW",bbs);
ctrl=fsh_open(s,"rb");
start=(struct control *)malloc(sizeof(struct control));
if (ctrl!=NULL)
 {
 fread(&start,1,sizeof(start),ctrl);
 start->next=NULL;
 prior=start;
 /* note printout linewrap! */
 for (point=(struct control *)malloc(sizeof(struct control));fread(&point,1,sizeof(point),ctrl)==sizeof(point);point=(struct control *)malloc(sizeof(struct control)))
  {
  prior->next=point;
  point->next=NULL;
  prior=point;
  }
 fclose(ctrl);
 } else {
 free(start);
 }
 sprintf(s,"%s\\fiw\RCPT.*",filedir);
 fack=findfirst(s,&ffblk,0);
 while (!fack)
  {
  sprintf(s1,"%s\\fiw\\%s",filedir,ffblk.ff_name);
  ctrl=fsh_open(s1,"rt");
  to[0]='\0';
  while(!fgets(s,255,ctrl)!=NULL)
   {
   strupr(s);
   if (strstr(s,"FROM:"))
    {
    char *p;  /* defines */
    int walk=0;

    p=strpbrk(s,":");
    p++;p++;  /* get past the : and space */
    do {
     to[walk]=*p++;
     walk++;
     } while ((*p!='\0')&&(*p!='\n')&&(*p!='\r'));
    to[walk++]='\0';
    }
   if (strstr(s,"FTN-RECEIPT:")&&(to[0]!='\0'))
    {
    char *p;  /* defines */
    char small[5];
    int ack=TRUE;
    int walk=0,segment=FALSE;
    int segnum=1,total=1;

    place=ftell(ctrl);
    fclose(ctrl);    /* for safety's sake */
    p=strpbrk(s,"*");     /* fileid line is started by *, so can look for */
    do {                  /* that and then work from there, eh? */
     s2[walk]=*p++;
     walk++;
     } while ((*p!=' ')&&(*p!='\0')&&(*p!='\n')&&(*p!='\r'));
    s2[walk++]='\0';  /* s is now the fileid */
    sscanf(small,"%s",p);
    if (strnicmp(small,"n",1)==0)
     ack=FALSE;
    p=strpbrk(s,":");
    p++;p++;
    if (*p!='*')  /* is a seg-id */
     {
     segment=TRUE;
     for (walk=0;walk<=3;walk++) /* we always have 3 chars in seg-id */
      small[walk]=*p++;
     small[walk+1]='\0';
     segnum=(atoi(small)-1);
     p++;
     for (walk=0;walk<=3;walk++) /* we always have 3 chars in seg-id */
      small[walk]=*p++;
     small[walk+1]='\0';
     total=(atoi(small)-1);  /* both total and seg start from 0 */
     }
    if (start!=NULL)
     {
     point=start;
     do {
      prior=point->next;
      if (point->outbound==TRUE)  /* it was an outbound */
       {
       if ((strnicmp(point->fileid,s2,(strlen(s2)))==0) && (point->finish==FALSE))
	break;    /* point is pointing to applicable structure */
       }
      point=point->next;
      } while (prior!=NULL);     /* quit when next is NULL */
     if ((strnicmp(point->fileid,s2,(strlen(s2)))==0) && (point->finish==FALSE))
      {  /* double check neccessary w/o using additional variable */
      FILE *read;  /* declaration */

      if (ack==FALSE)
       {
       if (segment==TRUE)
	{
	sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,segnum);
	if (!access(s,0))
	 {
	 read=fsh_open(s,"rb");
	 sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,segnum);
	 ctrl=fsh_open(s,"wb");
	 while (!feof(ctrl))
	  fputc(fgetc(read),ctrl);
	 fclose(read);
	 fclose(ctrl);
	 time(&point->time);
	 } else {  /* for some reason the file isn't there!  Cancel the whole */
	 for(walk=0;walk<=point->total;walk++);
	  {
	  sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
	  unlink(s);          /* if we can't resend the whole file, kill it */
	  }
	 point->finish=TRUE;
	 seatin=(struct seat *)malloc(sizeof(struct seat));
	 sprintf(seatin->from,"%s",to);
	  sprintf(seatin->segid,"%03d-%03d%s",segnum,total,s2);
	 send_ack(OTHER);
	 free(seatin);
	 }
	} else {  /* resend entire file */
	for (walk=0;walk<=point->total;walk++)
	 {
	 sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,walk);
	 if (access(s,0))
	  {
	  walk=-1;
	  break;
	  }
	 }
	if (walk==-1)
	 {
	 for(walk=0;walk<=point->total;walk++)
	  {
	  sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
	  unlink(s);          /* if we can't resend the whole file, kill it */
	  }
	 point->finish=TRUE;
	 seatin=(struct seat *)malloc(sizeof(struct seat));
	 sprintf(seatin->from,"%s",to);
	 sprintf(seatin->segid,"%s",s2);
	 send_ack(OTHER);
	 free(seatin);
	 } else {
	 for(walk=0;walk<=point->total;walk++)
	  {
	  sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,walk);
	  read=fsh_open(s,"rb");
	  sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
	  ctrl=fsh_open(s,"wb");
	  while (!feof(ctrl))
	   fputc(fgetc(read),ctrl);
	  fclose(read);
	  fclose(ctrl);
	  time(&point->time);
	  }
	 }
	}
       } else {  /* it's an ack */
       if (segment==TRUE)
	{
	sprintf(s,"%s\\fiw\\%08d.%03d",filedir,point->store,segnum);
	unlink(s);  /* it got there */
	point->finish=TRUE;
	for(walk=0;walk<=point->total;walk++);
	 {
	 sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
	 if (!access(s,0))
	  {
	  point->finish=FALSE;
	  break;
	  }
	 }
	} else { /* ack of entire file */
	for(walk=0;walk<=point->total;walk++);
	 {
	 sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
	 unlink(s);          /* if we can't resend the whole file, kill it */
	 }
	point->finish=TRUE;
	}
       }
      } else {  /* it doesn't exist */
      seatin=(struct seat *)malloc(sizeof(struct seat));
      sprintf(seatin->from,"%s",to);
      if (segment==TRUE)
       sprintf(seatin->segid,"%03d-%03d%s",segnum,total,s2);
      else
       sprintf(seatin->segid,"%s",s2);
      send_ack(OTHER);
      free(seatin);
      }
     } else { /* no control structure! */
     seatin=(struct seat *)malloc(sizeof(struct seat));
     sprintf(seatin->from,"%s",to);
     if (segment==TRUE)
      sprintf(seatin->segid,"%03d-%03d%s",segnum,total,s2);
     else
      sprintf(seatin->segid,"%s",s2);
     send_ack(OTHER);
     free(seatin);
     }
    ctrl=fsh_open(s1,"rt");
    fseek(ctrl,place,SEEK_SET);
    }
   }
  unlink(s1);
  fack=findnext(&ffblk);
  }
 if (start!=NULL)  /* checking to see what's expired... */
  {
  long timenow;
  double diff;

  point=start;
  do {
   prior=point->next;
   if (point->finish!=TRUE)
    {
    time(&timenow);
    diff=difftime(timenow,point->time);
    diff=diff/60; /*minutes */
    diff=diff/60; /* hours */
    diff=diff/24; /* days */
    if (diff > keepsent)
     {
     point->finish=TRUE;
     if (point->outbound==TRUE)
      {
      int walk; /*declaration */

      for(walk=0;walk<=point->total;walk++)
       {
       sprintf(s,"%s\\mqueue\\%08d.%03d",filedir,point->store,walk);
       unlink(s);          /* if we can't resend the whole file, kill it */
       }
      seatin=(struct seat *)malloc(sizeof(struct seat)); /* if for some reason they haven't got it. */
      sprintf(seatin->from,"%s",to);  /* this is for completeness, if not much else */
      sprintf(seatin->segid,"%s",point->fileid);
      send_ack(OTHER);
      free(seatin);
      }
     }
    }
   point=prior;
   } while (prior != NULL);
  write_control();
  point=start;
  do {
   prior=point->next;
   free(point);
   point=prior;
   } while (prior != NULL);
  }
 }

/********************************************************************
* Sends ack/naks for inbound seat messages - writes own e-mail
*********************************************************************/

void send_ack (int success)
{
char s[MAXPATH],s1[255];
FILE *write;
int counter=0;

do {
 counter++;
 sprintf(s,"%s\\mqueue\\%d.ack",filedir,counter); /* limits to *.ack */
 write=fsh_open(s,"rt+");
 fgets(s1,254,write);
 if (strstr(s1,seatin->from))
  break;                  /* should break us out - with stream open! */
 fclose(write);
 } while (!access(s,0));
if (write==NULL)
 {
 write=fsh_open(s,"at");
 fprintf(write,"To: %s\n",seatin->from);
 fprintf(write,"From: %s\n",from);       /*global variable, remember? */
 fprintf(write,"X-Mailer: %s\n",VERSION);
 fprintf(write,"Subject: FTN Mail Transport\n");
 fprintf(write,"Reply-To: %s\n\n",from);
 } else
 fseek(write,0,SEEK_END);   /* EOF for appending */
switch (success) {
 case TRUE:
  fprintf(write,"FTN-Receipt: %s Ack\n",seatin->segid);
  break;
 case FALSE:
  fprintf(write,"FTN-Receipt: %s Nak\n",seatin->segid);
  break;
 case OTHER:
  fprintf(write,"FTN-Cancel: %s \n",seatin->segid);
  break;
 }
fclose(write);
}

/************************************************************************
* See whether or not the exporting directory is a "game directory" for
* arcmail systems.
************************************************************************/

int game_dir(char *drive, char *dir)
{

char s[MAXPATH],s1[MAXPATH],s2[MAXPATH];
FILE *check;
short b;

sprintf(s,"\0");
sprintf(s1,"%s%s",drive,dir);
strncat(s,s1,(strlen(s1)-1));
sprintf(s1,"%s\\FIW.INI",bbs);
//sprintf(s1,"c:\\uriel\\fiw\\fiw.ini");
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

/**************************************************************************
*   write_email has the original OUTNOW dir and original filename passed
*   to it to deal with Binkley systems.  Splitting occurs here, as does
*   encoding and keeping sent, if requested.
***************************************************************************/

void write_email (char path[MAXPATH],char original[20])
{
 FILE *write,*read;
 char g[255],g1[255],outpath[MAXPATH],c;
 int seg=0,a,booger;
 unsigned long segcrc;

 sprintf(outpath,"%s\\%s",path,original);
 if (rpoint->seat==TRUE)
  {
  int checker=0; /*declaration*/

  start=(struct control *)malloc(sizeof(struct control)); /* setting up control */
  crc32file(outpath);
  start->crc=segcrc=targcrc;
  sprintf(start->fileid,"*%s-%lX-%s",original,start->crc,from);
  strupr(start->fileid);
  sprintf(start->ofile,"%s",original);
  do {
   checker++;
   for (booger=0;booger<=20;booger++)
    {
    sprintf(g,"%s\\fiw\\%08d.%03d",filedir,checker,booger);
    if (!access(g,0))     /* if anything in the range ext 000-020 for that */
     {                    /* filename still remains, it'll trigger */
     checker++;booger=1;  /* THIS line, which will boot us up, and keep */
     }                    /* the for loop going */
    }
   } while (!access(g,0));
  start->store=checker;
  start->total=0;  /* tracking from 0, just like seg */
  start->finish=FALSE;
  sprintf(start->from,"%s",rpoint->to);
  start->next=NULL;
  start->outbound=TRUE;
  if (keepsent!=0)
   time(&start->time);
  }
 if (rpoint->split!=0)  /* just need to add a line here to keep split seat only */
  {
  unsigned long length;

  a=open(outpath,O_RDONLY,O_BINARY);
  length=filelength(a);
  close(a);
  length=(length / 1024);  /* getting size in k to compare to rpoint */
  if ((rpoint->split <= length) && (rpoint->split != 0))  /* bigger than split value */
   {
   long checker=0;

   printf("þ Splitting segment: ");
   read=fsh_open(outpath,"rb");
   while (!feof(read))
    {
    printf("%d ",seg+1);
    sprintf(g,"%s\\segment.%03x",path,seg);
    write=fsh_open(g,"wb");
    for (a=0;a<=rpoint->split;a++)
     {
     static char buffer[1024];

     checker=fread(buffer,1,1024,read);
     fwrite(buffer,1,1024,write);
     if (checker<1024)  /* hit eof in the booger bit */
      {
      fclose(write);
      fgetc(read);    /* should return eof */
      break;
      }
     }
    fclose(write);
    giveup_timeslice();
    seg++;  /* moving segments *AFTER* encoding, etc, */
    } /* end of read file */
   fcloseall();
   unlink(outpath);
   if (start!=NULL)
    start->total=seg;
   } /* not bigger than split */
  } /* no split anyway */
 if (seg!=0)
  {
  printf("\nþ Mailing segment: ");
  seg--;
  }
 for (booger=0;booger<=seg;booger++)  /* for each segment will loop */
  {
  if (seg!=0) /* it was split */
   {
   giveup_timeslice();
   printf("%d ",booger+1);
   sprintf(outpath,"%s\\%s",path,original);
   sprintf(g,"%s\\segment.%03x",path,booger);
   if (rpoint->seat==TRUE)
    {
    crc32file(g);
    segcrc=targcrc;
    }
   read=fsh_open(g,"rb");
   write=fsh_open(outpath,"wb");
   c=fgetc(read);
   while (!feof(read))
    {
    fputc(c,write);
    c=fgetc(read);
    }
   flushall();
   fcloseall();
   unlink(g);  /* it's now back as the "original" */
   }
  a=0;
  do {
   a++;
   sprintf(g,"%s\\mqueue\\FIW.%03x",filedir,a);
   } while (!access(g,0));
  sprintf(g,"%s\\FIW\\OUTFIW.%03x",filedir,a);
  if (!access(g,0))
   remove(g);
  if (rpoint->method==UUE)
   {
   /* uu -encode [fullpath source] [fullpath target] */
   sprintf(g1,"%s\\UU.EXE",bbs);
   if (!access(g1,0))
    spawnlp(P_WAIT,g1,"UU.EXE","-encode",outpath,g,NULL);
   else
    printf("í UU.EXE is not residing in the BBS directory!\n");
   } else {   /* MIME64 */
   sprintf(g1,"%s\\MIME64.EXE",bbs);
   if (!access(g1,0))
    spawnl(P_WAIT,g1,"MIME64.EXE","-e",outpath,g,NULL);
   else
    printf("í MIME64.EXE must be in your BBS directory!\n");
   }
  if (access(g,0))
   printf("í Error in encoding! Mail not sent!\n");
  else {
   char bound[30];  /* defines */
   flushall();
   write=fsh_open(outpath,"wb");  /* there goes un-encoded bit - should be okay since game stuff should be copied already */
   fclose(write);
   sprintf(outpath,"%s",g);  /* this is the file that's in bbs now... */
   sprintf(g,"%s\\mqueue\\FIW.%03x",filedir,a);
   write=fsh_open(g,"wt");
   randomize();
   sprintf(g,"Message-ID :<%04x%04x@FIW%03x>\n",rand()*a,rand()+a,a);
   fputs(g,write);
   fprintf(write,"Reply-To: %s\n",from);
   fprintf(write,"X-Mailer: FIW %s\n",VERSION);
   if (rpoint->method!=UUE)
    {
    sprintf(g,"Mime-Version: 1.0\n");
    fputs(g,write);
    sprintf(bound,"Message-Boundary-%06x",(rand()*rand()+a));
    fprintf(write,"Content-type: Multipart/Mixed; boundary=%s\n",bound);
    }
   fprintf(write,"To: %s",rpoint->to);
   fprintf(write,"From: %s\n",from);
   if (rpoint->seat==TRUE)
    sprintf(g,"Subject: FTN Mail Transport -> FIW -> %s\n",original);
   else
    sprintf(g,"Subject: FIW -> %s",original);
   fputs(g,write);
   fprintf(write,"\n\n");
   if (rpoint->method!=UUE)
    {
    fprintf(write,"--%s\n",bound);
    fprintf(write,"Content-type: text/plain; charset=US-ASCII\n");
    fprintf(write,"Content-transfer-encoding: 7BIT\n");
    fprintf(write,"Content-description: Mail message body\n\n");
    }
   fprintf(write,"This is a ");
   if (rpoint->method!=UUE)
    fprintf(write,"MIME64 encoded ");
   else
    fprintf(write,"UUEncoded ");
   fprintf(write,"message generated by FIW %s\n",VERSION);
   fprintf(write,"If your mailer doesn't support this type of coding,\n");
   fprintf(write,"you can decode the file attached using a program\n");
   if (rpoint->method==UUE)
    fprintf(write,"like UU.EXE in the PPP project for WWIV systems.\n");
   else
    fprintf(write,"like 'munpack'.  'munpack' is available for anonymous\nFTP from ftp.andrew.cmu.edu:pub/mpack\n\n");
   if (seg != 0)
    {
    fprintf(write,"Your file has been split into %d sections of %dkb as requested.\n",(seg+1),rpoint->split);
    fprintf(write,"To recombine these sections, it is possible to use DOS Copy\n");
    fprintf(write,"by decoding each section separately, then copying all together\n");
    fprintf(write,"by using the format:\n");
    fprintf(write,"COPY [source] /B+[source] /B+[source]... [destination]\n");
    fprintf(write,"This is section %03d of your file.  \n",booger+1);
    }
   fprintf(write,"\n\n");
   if (rpoint->seat==TRUE)
    {
    fprintf(write,"Ftn-File: %s\n",original);
    fprintf(write,"Ftn-CRC-32: %lX\n",start->crc);
    if (rpoint->method!=UUE)
     fprintf(write,"Ftn-Encoding: Base64\n");
    else
     fprintf(write,"Ftn-Encoding: uuencode\n");
    fprintf(write,"Ftn-File-Id: %s\n",start->fileid);
    fprintf(write,"Ftn-Seg: %d-%d\n",(booger+1),(seg+1));
    fprintf(write,"Ftn-Seg-Id: %03d-%03d%s\n",(booger+1),(seg+1),start->fileid);
    fprintf(write,"Ftn-Seg-CRC-32: %lX\n",segcrc);
    fprintf(write,"Ftn-Date: %lX\n",start->time);
    }
   fprintf(write,"\n");
   read=fsh_open(outpath,"rt");
   fgets(g,100,read);
   if (rpoint->method!=UUE)
    {
    fprintf(write,"--%s\n",bound);
    sprintf(g,"Content-type: application/octet-stream; name=\x22%s\x22\n",original);
    fputs(g,write);
    fgets(g,100,read);
    fprintf(write,"Content-transfer-encoding: BASE64\nContent-Disposition: inline; filename=\x22%s\x22\n",original);
    } else {
    sprintf(g,"begin 644 %s\n",original);  /* in case it's fucked up */
    fputs(g,write);
    }
   c=0;
   c=fgetc(read);
   while ((!feof(read)))
    {
    fputc(c,write);
    c=fgetc(read);
    }    /*done with copying MIME portion*/
   if (rpoint->method!=UUE)
    fprintf(write,"--%s--",bound);
   flushall();
   fcloseall();
   remove(outpath);
   /*storing sections */
/*   if ((start!=NULL)&&(keepsent!=0))
    {
    sprintf(g,"%s\\FIW\\%s.%03d",filedir,start->store,booger);
    sprintf(g1,"%s\\mqueue\\FIW.%03x",filedir,a);
    read=fsh_open(g1,"rt");
    write=fsh_open(g,"wt");
    while (!feof(read))
     fputc(fgetc(read),write);
    fclose(read);
    fclose(write);
    } */
   }
/*  if ((start!=NULL)&&(keepsent!=0))
   {
   sprintf(g,"%s\\CONTROL.FIW",bbs);
   write=fsh_open(g,"ab");
   fwrite(&start,1,sizeof(start),write);
   fclose(write);
   }*/
  }
 fcloseall();
 printf("\n");
 if (start!=NULL)
  free(start);
 }

/************************************************************************
*   Global routine for reading routing into memory
*************************************************************************/

#define ZONE 3
#define NET 2
#define NODE 1
#define POINT 0

int read_route (void)
{

FILE *ini;
char x[255],x1[5],x2[5],x3[5],x4[5],x5[10],x6[10],x7[10],x8[10];
char y1[255];
char *p;
short a=0;

 sprintf(x,"%s\\FIW.INI",bbs);
//sprintf(x,"c:\\uriel\\fiw\\fiw.ini");
 ini=fsh_open(x,"rt");
 while (a==0)
  {
  fgets(x,254,ini);
  if (feof(ini))
   {
   fclose(ini);
   return(FALSE);
   }
  if (strnicmp(x,"[ROUTE]",7)==0)
   {
   a=1;
   continue;
   }
  }
  rstart=(struct route *)malloc(sizeof(struct route));
  fgets(x,255,ini);
  a=0;
  p=NULL; p=y1;
  do
   {
   if ((x[a]=='/') || (x[a]==':') || (x[a]=='.')) /* allowing sscanf */
    {
    *p++=' ';
    } else {
    *p++=x[a];
    }
   a++;
   } while ((x[a]!='\0') && (x[a]!='\n'));
  *p='\0';
  sscanf(y1,"%s%s%s%s%s%s%s%s",x1,x2,x3,x4,x8,x5,x7,x6);
  /* zone net node point routelvl method seat split */
  if (strnicmp(x5,"U",1)==0)
   rstart->method=UUE;
  else
   rstart->method=MIME64;
  if (strnicmp(x7,"seat",4)==0)
   rstart->seat=TRUE;
  else
   rstart->seat=FALSE;
  rstart->route=POINT;
  if (strnicmp(x8,"zone",4)==0)
   rstart->route=ZONE;
  if (strnicmp(x8,"net",3)==0)
   rstart->route=NET;
  if (strnicmp(x8,"node",4)==0)
   rstart->route=NODE;
  rstart->split=atol(x6);  /* should return 0 if nothing there */
  rstart->zone=atoi(x1); rstart->net=atoi(x2); rstart->node=atoi(x3); rstart->point=atoi(x4);
  rstart->next=NULL;
  fgets(rstart->to,78,ini);
  strupr(rstart->to);
  rprior=rstart;
  do
   {
   fgets(x,255,ini);
   if ((x[0]=='[') || (feof(ini)))
    {
    fseek(ini,0,SEEK_END);
    fgets(x,255,ini);
    continue;
    }
   rpoint=(struct route *)malloc(sizeof(struct route));
   a=0;
   p=NULL; p=y1;
   do
    {
    if ((x[a]=='/') || (x[a]==':') || (x[a]=='.')) /* allowing sscanf */
     {
     *p++=' ';
     } else {
     *p++=x[a];
     }
    a++;
    } while ((x[a]!='\0') && (x[a]!='\n'));
   *p='\0';
   sscanf(y1,"%s%s%s%s%s%s%s%s",x1,x2,x3,x4,x8,x5,x7,x6);
   /* zone net node point routelvl method seat split */
   if (strnicmp(x5,"U",1)==0)
    rpoint->method=UUE;
   else
    rpoint->method=MIME64;
   if (strnicmp(x7,"seat",4)==0)
    rpoint->seat=TRUE;
   else
    rpoint->seat=FALSE;
   rpoint->route=POINT;
   if (strnicmp(x8,"zone",4)==0)
    rpoint->route=ZONE;
   if (strnicmp(x8,"net",3)==0)
    rpoint->route=NET;
   if (strnicmp(x8,"node",4)==0)
    rpoint->route=NODE;
   rpoint->split=atol(x6);  /* should return 0 if nothing there */
   rpoint->zone=atoi(x1); rpoint->net=atoi(x2); rpoint->node=atoi(x3); rpoint->point=atoi(x4);
   rpoint->next=NULL;
   rprior->next=rpoint;
   fgets(rpoint->to,78,ini);
   strupr(rpoint->to);
   rprior=rpoint;
   } while (!feof(ini));
  fclose(ini);
/* read in ID section - who are we? */
  sprintf(x,"%s\\FIW.INI",bbs);
 //sprintf(x,"c:\\uriel\\fiw\\fiw.ini");
  ini=fsh_open(x,"rt");
  a=0;
  while (a==0)
  {
  fgets(x,254,ini);
  if (feof(ini))
   {
   fclose(ini);
   return(FALSE);
   }
  if (strnicmp(x,"[ID]",4)==0)
   {
   a=1;
   continue;
   }
  }
  idstart=(struct id *)malloc(sizeof(struct id));
  fgets(x,255,ini);
  a=0;
  p=NULL; p=y1;
  do {
   if ((x[a]=='/') || (x[a]==':') || (x[a]=='.')) // allowing sscanf
    {
    *p++=' ';
    } else {
    *p++=x[a];
    }
   a++;
   } while ((x[a]!='\0') && (x[a]!='\n'));
  *p='\0';
  sscanf(y1,"%s%s%s%s",x1,x2,x3,x4);  // zone net node point
  idstart->zone=atoi(x1); idstart->net=atoi(x2); idstart->node=atoi(x3); idstart->point=atoi(x4);
  idstart->nexter=NULL;
  idprior=idstart;
  do {
   fgets(x,255,ini);
   if ((x[0]=='[') || (feof(ini)))
    {
    fseek(ini,0,SEEK_END);
    fgets(x,255,ini);
    continue;
    }
   idpoint=(struct id *)malloc(sizeof(struct id));
   a=0;
   p=NULL; p=y1;
   do
    {
    if ((x[a]=='/') || (x[a]==':') || (x[a]=='.')) // allowing sscanf
     {
     *p++=' ';
     } else {
     *p++=x[a];
     }
    a++;
    } while ((x[a]!='\0') && (x[a]!='\n'));
   *p='\0';
   sscanf(y1,"%s%s%s%s",x1,x2,x3,x4); // zone net node point
   idpoint->zone=atoi(x1); idpoint->net=atoi(x2); idpoint->node=atoi(x3); idpoint->point=atoi(x4);
   idpoint->nexter=NULL;
   idprior->nexter=idpoint;
   idprior=idpoint;
   } while (!feof(ini));
  fclose(ini);
  return (TRUE);
  }

/**************************************************************************
*  Message structures for arcmail systems (and packets, too - very
*  deriviative of PKTHDR and stoneage, but what the hey.)
***************************************************************************/

typedef struct {
	char from[36],
	     to[36],
	     subject[72],
	     date[20];
	unsigned int tread,
	     dnode,         /*destination node*/
	     onode;         /*originating node*/
	int  cost;         /*same logic dominates*/
	unsigned int onet,
	     dnet,
	     dzone,
	     ozone,
	     dpoint,
	     opoint,
	     repto,
	     attr,
	     nextrep;
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

/************************************************************************
* Send_Freq () - finds the file, sets it up as a FREQ, to node
* 999:999/999.999.  If export finds that it's to 999:999/999.999, it'll
* look at the subject to find the filename, and the first line of message
* text to find out what the e-mail address is.
*************************************************************************/
void send_freq(void)
{
char h[255],h1[MAXPATH],original[MAXPATH];
FILE *read, *write;
int ffreq,fmethod=MIME64,fsplit=0;
struct ffblk ffblk;
char to[36],to1[36];                   /* may be able to remove to as global */

sprintf(h,"%s\\fiw\\FREQ.*",filedir);
ffreq=findfirst(h,&ffblk,0);
while (!ffreq)
 {
 to[0]='\0';
 sprintf(h1,"%s\\fiw\\%s",filedir,ffblk.ff_name);
 read=fsh_open(h1,"rt");
 while (!fgets(h,255,read) != NULL)
  {
  strupr(h);
  if (strstr(h,"FROM: "))
   {
   rpoint=rstart;
   do {
    rprior=rpoint->next;
    if (strstr(h,strupr(rpoint->to)))
     break;
    rpoint=rpoint->next;
    } while (prior!=NULL);  /* need to ensure this leaves point as null */
   if (rpoint==NULL)
    {
    int walk=0,finger=0; /*defines */

    while ((h[walk-1]!=' ')&&(h[walk-2]!=':')) /* take us to 1st char of from */
     walk++;
    while ((h[walk]!='\n') || (h[walk]!='\0') || (h[walk]!='\r') || (h[walk]!='@'))
     {
     to[finger]=h[walk];
     walk++;finger++;
     }
    to[finger+1]='\0';to[(strlen(to)-1)]='\0';
    walk++;
    while ((h[walk]!='\n') || (h[walk]!='\0') || (h[walk]!='\r'))
     {
     to1[finger]=h[walk];
     walk++;finger++;
     }
    to1[finger+1]='\0';to1[(strlen(to1)-1)]='\0';
    }
   continue;
   }
  if (strstr(h,"FTN-ENCODING:"))
   {
   if (strstr(h,"MIME64"))
    fmethod=MIME64;
   else
    fmethod=UUE;
   }
  if (strstr(h,"FTN-SPLIT: "))
   {
   char *pooky,petit[15];
   pooky=strpbrk(h,":");
   sscanf(pooky,"%s",petit);
   fsplit=atoi(petit);
   }
  if (strstr(h,"FTN-FREQ:"))
   {
   int walk=0,finger=0; /*defines */

   while ((h[walk-1]!=' ')&&(h[walk-2]!=':')) /* take us to 1st char of from */
    walk++;
   while ((h[walk]!='\n') || (h[walk]!='\0') || (h[walk]!='\r'))
    {
    original[finger]=h[walk];
    walk++;finger++;
    }
   original[finger+1]='\0';
   walk=finger=0;
   if (keyword[0]!='\0')   /* note - keyword and freqdir are new globals! */
    {
    write=fsh_open(keyword,"rt");
    while ((fgets(h,255,write)!=NULL) && (finger==0))
     {
     if (strnicmp(h,original,strlen(original))==0)
      {
      char quickie[MAXPATH],quicker[MAXPATH];  /* defines */

      sscanf(h,"%s%s",quickie,quicker);
      sprintf(original,"%s",quicker);
      finger=1;
      }
     }
    fclose(write);
    }
   if ((freqdir[0]!='\0') && (finger==0))
    {
    write=fsh_open(freqdir,"rt");
    while ((fgets(h,255,write)!=NULL) && (finger==0))
     {
     h[(strlen(h)-1)]='\0';  /* removing \n at end of string */
     sprintf(h1,"%s%s",h,original);
     if (!access(h1,0))
      {
      sprintf(original,"%s",h1);
      finger=1;
      }
     }
    fclose(write);
    }
   if ((finger==1) && (mailer==ARCMAIL))  /* there was a successful match */
    {
    MSGHDR mh;  /*defines */

    do {
     walk++;
     sprintf(h,"%s\\%d.MSG",mail,walk);
     }  while (!access(h,0));
    write=fsh_open(h,"wb");
    mh.onode=mh.onet=mh.ozone=mh.opoint=999;
    sprintf(mh.from,"%s",to);
    sprintf(mh.to,"%s",to1);
    sprintf(mh.date,"%d %d",fmethod,fsplit);
    sprintf(mh.subject,"%s",original);
    mh.tread=0;mh.cost=0;mh.repto=0;mh.attr=432;mh.nextrep=0;
    if (point==NULL)
     mh.dnode=mh.dnet=mh.dzone=mh.dpoint=mh.onode;
    else {
     mh.dzone=rpoint->zone;mh.dnet=rpoint->net;mh.dnode=rpoint->node;mh.dpoint=rpoint->point;
     }
    fwrite(&mh,1,sizeof(mh),write);
    fclose(write);  /*now its own file attach!*/
    }
   if ((finger==1) && (mailer==BINKLEY))  /* there was a successful match */
    {
    if (rpoint!=NULL)
     {
     driveto(out);
     if (rpoint->zone!=basezone)
      sprintf(h,"%s.%3ux",rpoint->zone);
     else
      sprintf(h,"%s",out);
     chdir(h);
     for (walk=0;walk<=2;walk++)
      {
      switch (walk)
       {
       case 2:
	sprintf(h1,"%s\\%4ux%4ux.DLO",rpoint->net,rpoint->node);
	break;
       case 1:
	sprintf(h1,"%s\\%4ux%4ux.CLO",rpoint->net,rpoint->node);
	break;
       case 0:
	sprintf(h1,"%s\\%4ux%4ux.FLO",rpoint->net,rpoint->node);
	break;
       }
      if (!access(h1,0))  /* default *.FLO */
       break;
      }
     write=fsh_open(h1,"at");
     fprintf(write,"%s\n",original);
     fclose(write);
     } else {
     driveto(out);
     chdir(out);
     walk=999;
     sprintf(h1,"%4ux%4ux.FLO",walk,walk);
     write=fsh_open(h1,"at");
     fprintf(write,"%s  %s@%s  %d %d\n",original,to,to1,fmethod,fsplit);
     fclose(write);
     }
    }
   }
  }
 fclose(read);
 ffreq=findnext(&ffblk);
 }
}

/************************************************************************
* determines if export (depending on mailer type), then passes outbound
* directory to write_email along with filename.  Write_email will use
* rpoint (switches in there to support the internet freq - it takes over
* rpoint for a moment), the passed filename, and the passed outbound
* directory (due to binkley systems, that seems easiest).  Write_email
* is taking care of all the bs stuff like splitting, writing control
* structures, etc, simply because it makes passing less stuff neccessary.
*************************************************************************/
void export (void)
{
char s[255],s1[255],nameb[15],name[15];
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];
FILE *f,*write;
int export;

if (mailer==ARCMAIL)
 {
 int a,b;   /* define */
 struct ffblk ffblk;
 MSGHDR mh;

 for (b=0;b<=1;b++)
  {
  driveto(mail);
  chdir(mail);
  sprintf(s,"%s\\*.*",mail);
  a=findfirst(s,&ffblk,0);
  while (!a)
   {
   char szone[10];
   char scanner[255],mini[10],*point,t2[5];

   export=FALSE;
   sprintf(nameb,"%s",ffblk.ff_name);
   sprintf(s,"%s\\%s",mail,nameb);
   f=fsh_open(s,"rb");
   if (strstr(s,".BBS"))   /*this will not let it attempt to check Hudson*/
    fclose(f);             /*message areas, closing f to make mh the wrong size*/
   if (fread(&mh,1,sizeof(mh),f) != sizeof(mh))
    fclose(f);
    else {


printf("%u\n",mh.attr);


    printf("þ Examining %s\n",s);
    scanner[0]='\0';
    fgets(scanner,254,f);
    do {                   /* get the point from ^A kludges */
     strupr(scanner);
     point=NULL;
     point=strstr(scanner,"FMPT");
     if (point!=NULL)
      {
      sscanf(point,"%s%s",mini,t2);
      mh.opoint=atoi(t2);
      }
     else
      mh.opoint=0;
     point=NULL;
     point=strstr(scanner,"TOPT");
     if (point!=NULL)
      {
      sscanf(point,"%s%s",mini,t2);
      mh.dpoint=atoi(t2);
      }
     else
      mh.dpoint=0;
     fgets(scanner,254,f);
     } while (!feof(f));
    fseek(f,190,SEEK_SET);
    while (!feof(f))
     {
     fscanf(f,"%s",s);
     if (strstr(s,"MSGID:"))
      {
      fscanf(f,"%s",szone);
      if (atoi(szone)!=0)
       mh.ozone=mh.dzone=atoi(szone);      /*get the MSGID zone*/
      break;
      }
/*added for TDS*/
     if ((strstr(s,"INTL"))&&(atoi(szone)==0))
      {
      fscanf(f,"%s",szone);
      if (atoi(szone)!=0)
       mh.ozone=mh.dzone=atoi(szone);      /*get the MSGID zone*/
      break;
      }
     }
    printf("þ Message from %d:%d/%d.%d to %d:%d/%d.%d\n",mh.ozone,mh.onet,mh.onode,mh.opoint,mh.dzone,mh.dnet,mh.dnode,mh.dpoint);
    if (mh.onode==mh.onet==mh.dnode==mh.dnet==999) /*internet freq */
     {
     char pooka[5],pooka2[5]; /*defines */

     sprintf(rpoint->to,"%s@%s",mh.from,mh.to);
     rpoint->seat=FALSE;
     sscanf(mh.date,"%s%s",pooka,pooka2);
     rpoint->method=atoi(pooka);
     rpoint->split=atoi(pooka2);
     export=TRUE;
     } else {
     idpoint=idstart;
     do {
      idprior=idpoint->nexter;
      if ((mh.dzone==idpoint->zone)&&(mh.dnet==idpoint->net)&&(mh.dnode==idpoint->node)&&(mh.dpoint==idpoint->point))
       {
       export=OTHER;
       }
      idpoint=idpoint->nexter;
      } while (idprior!=NULL);
     if (export!=OTHER)
      {
      rpoint=rstart;
      do {
       rprior=rpoint->next;
       switch(rpoint->route) {
	case ZONE:
	 if (mh.dzone==rpoint->zone)
	  export=TRUE;
	 break;
	case NET:
	 if ((mh.dzone==rpoint->zone)&&(mh.dnet==rpoint->net))
	  export=TRUE;
	 break;
	case NODE:
	 if ((mh.dzone==rpoint->zone)&&(mh.dnet==rpoint->net)&&(mh.dnode==rpoint->node))
	  export=TRUE;
	 break;
	case POINT:
	 if ((mh.dzone==rpoint->zone)&&(mh.dnet==rpoint->net)&&(mh.dnode==rpoint->node)&&(mh.dpoint==rpoint->point))
	  export=TRUE;
	 break;
	}
       if (export==TRUE)
	break;  /* to break out of loop */
       rpoint=rpoint->next;
       } while (rprior!=NULL);
      } else
      export=FALSE;  /* it's to us! */
     }
    fclose(f);
    if (export==TRUE)
     {
     int walk;
     /*--------------------------------------------------------------*/
     /* FIDO Message attributes (attr) (informational)                           */
     /*--------------------------------------------------------------*/
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
     static char  attrstr[16][11] = { "MSGPRIVATE","MSGCRASH","MSGREAD","MSGSENT","MSGFILE","MSGFWD",
		 "MSGORPHAN","MSGKILL","MSGLOCAL","MSGXX1","MSGXX2","MSGFRQ",
		 "MSGRRQ","MSGCPT","MSGARQ","MSGURQ"};

     export=OTHER;
     for(walk = 0;walk < 16;walk++)
      {
      sprintf(s,NULL);
      if(mh.attr & (1 << walk))
       sprintf(s,"%s",attrstr[walk]);
      else
       sprintf(s,"boogedy");  /*gotta have a sense of humor!*/
      if(strstr(s,"MSGFILE"))       /*is it attach bit?*/
       export=TRUE;
      }
     if (export==TRUE)
      {
      printf("ð Packet File Attach.  Exporting.\n");
      sprintf(s,"%s\\%s",mail,nameb);
      f=fsh_open(s,"rb");
      fseek(f,72, SEEK_SET);
      fscanf(f,"%s",s);
      strupr(out);strupr(s);
      if (strstr(s,out))
       {
       int c;  /* define */

       c=(strlen(out));
       fseek(f,(c+73), SEEK_SET);
       fscanf(f,"%s",name);
       fclose(f);
       fclose(write);
       } else {
       fclose(f);
       fclose(write);
       fnsplit(s,drive,dir,file,ext);
       sprintf(name,"%s%s",file,ext);
       sprintf(s1,"%s\\%s",out,name);
       printf("í Copying file to outbound\n");
       if (!access(s1,0))
	remove(s1);
       if (game_dir(drive,dir)==TRUE)
	export=OTHER;
       f=fsh_open(s,"rb");
       write=fsh_open(s1,"wb");
       while (!feof(f))
	fputc(fgetc(f),write);
       fclose(f);
       fclose(write);
       }
      flushall();
      fcloseall();
      if (export==TRUE)   /* was regular file attach, remove netmail msg, truncate outbound file */
       {
       sprintf(s,"%s\\%s",mail,nameb);
       unlink(s);
       } else {  /* was game directory outbound file, truncate *original* */
       printf("truncating %s",s);
       f=fsh_open(s,"wb");
       fclose(f);
       }                 /* the reason this is separate instead of an else is to catch */
      write_email(out,name);
      }                  /* needed netmails from game_directory */
     if (export==OTHER)  /* will NOT send out this run! Findnext should catch */
      {
      PKTHDR pkt;  /*defines */
      PMHDR pm;
      struct tm   *tim;
      time_t secs_now;

      fcloseall();
      printf("ð Converting netmail to file attach. \n");
      walk=0;
      sprintf(s,"%s\\%s",mail,nameb);
      f=fsh_open(s,"rb");
      do {
       sprintf(s1,"%s\\%04X%04d.PKT",out,rpoint->net,walk++);
       } while (!access(s1,0));
      write=fsh_open(s1,"wb");
      pkt.onode=mh.onode;pkt.dnode=rpoint->node;
      pkt.onet=mh.onet;pkt.dnet=rpoint->net;
      pkt.qozone=pkt.qdzone=pkt.ozone=pkt.dzone=rpoint->zone;
      pkt.opoint=mh.opoint;pkt.dpoint=rpoint->point;
/* steve change*/
      printf("%d:%d/%d.%d\n%d\n",rpoint->zone,rpoint->net,rpoint->node,rpoint->point,pkt.dpoint);

      time(&secs_now);
      tim = localtime(&secs_now);
      pkt.year=tim->tm_year;pkt.month=tim->tm_mon;pkt.day=tim->tm_mday;
      pkt.hour=tim->tm_hour;pkt.min=tim->tm_min;pkt.sec=tim->tm_sec;
      pkt.rate=0;pkt.ver=2;
      pkt.product=0;pkt.rev_lev=0;
      sprintf(pkt.password,"\0\0\0\0\0\0\0\0");
      sprintf(pkt.domain,"\0\0\0\0\0\0\0\0");
      fwrite(&pkt,1,sizeof(pkt),write);
      fputs("XPKT",write);
      fputc('\0',write);
      pm.onode=mh.onode;pm.dnode=mh.dnode;pm.onet=mh.onet;pm.dnet=mh.dnet;
      pm.attr=mh.attr;pm.cost=0;
      fwrite(&pm,1,sizeof(pm),write);
      fprintf(write,"%s",mh.date);
      fputc('\0',write);
      fprintf(write,"%s",mh.to);
      fputc('\0',write);
      fprintf(write,"%s",mh.from);
      fputc('\0',write);
      fprintf(write,"%s",mh.subject);
      fputc('\0',write);
      fseek(f,190,SEEK_SET);
      walk=0;
      walk=fgetc(f);
      while(!feof(f))
       {  /*copying text*/
       fputc(walk,write);
       walk=fgetc(f);
       }
      fputc('\0',write);
      fputc('\0',write);
      fputc('\0',write);
      fclose(f);
      fclose(write);
      walk=1;
      do {
       sprintf(s,"%s\\%d.MSG",mail,walk++);
       } while (!access(s,0));
      f=fsh_open(s,"wb");
      sprintf(mh.from,"FIW\0");sprintf(mh.to,"SYSOP\0");
      printf("%s - file attach created\n",s1);
      sprintf(mh.subject,"%s\0",s1);
/*      sprintf(mh.date,"%s",ctime(&secs_now));mh.tread=0;*/
      mh.onode=pkt.onode;mh.dnode=rpoint->node;
      mh.onet=pkt.onet;mh.dnet=rpoint->net;
      mh.dzone=mh.ozone=atoi(szone);
      mh.opoint=mh.opoint;mh.dpoint=rpoint->point;
//      mh.cost=0;
      mh.repto=0;mh.attr=432;mh.nextrep=0;



/* steve change*/
      printf("%d:%d/%d.%d\n%d\n",rpoint->zone,rpoint->net,rpoint->node,rpoint->point,pkt.dpoint);

      fwrite(&mh,1,sizeof(mh),f);
      fseek(f,190,SEEK_SET);
      fprintf(f,"TOPT %d\r",rpoint->point);
      fprintf(f,"PID: FIW %s\rFLAGS DIR\r",VERSION);

      fclose(f);  /*now its own file attach!*/
      flushall();
      fcloseall();
      sprintf(s,"%s\\%s",mail,nameb);
      unlink(s);
      }
     }
    }
   fcloseall();
   rpoint=rstart;
   a=findnext(&ffblk);
   }
  }
 } else {  /* binkley systems */
 long fpointer;
 int walking=999,kill=FALSE;
 char outfile[MAXPATH],outnow[MAXPATH];

 driveto(out);   /* may figure out some way to combine these.... */
 chdir(out);
 sprintf(s,"%s\\%4ux%4ux.FLO",out,walking,walking);  /* internet freqs */
 if (!access(s,0))
  {
  f=fsh_open(s,"rt");
  while (fgets(s1,255,f)!=NULL)
   {
   char hooka[10],hooka2[10];  /* defines */

   fpointer=ftell(f);
   fclose(f);
   if (s1[0]=='#')
    kill=OTHER;
   if (s1[0]=='^')
    kill=TRUE;
   s1[0]=' ';
   sscanf(s1,"%s%s%s%s",outfile,rpoint->to,hooka,hooka2);
   rpoint->seat=FALSE;
   rpoint->method=atoi(hooka);
   rpoint->split=atoi(hooka2);
   fnsplit(outfile,drive,dir,file,ext);
   sprintf(name,"%s%s",file,ext);
   sprintf(s1,"%s\\%s",out,name);
   if (!access(s1,0))
    remove(s1);
   f=fsh_open(outfile,"rb");
   write=fsh_open(s1,"wb");
   while (!feof(f))
    fputc(fgetc(f),write);
   fclose(f);
   fclose(write);
   if (kill==TRUE)
    unlink(outfile);
   if (kill==OTHER)
    {
    f=fsh_open(outfile,"wb");
    fclose(f);
    }
   write_email(outnow,name);
   f=fsh_open(s,"rt");
   fseek(f,fpointer,SEEK_SET);
   }
  fclose(f);
  unlink(s);
  }
 rpoint=rstart;  /* looping through routing structure to find matches for */
 do {            /* binkley systems */
  int bull;    /* define */

  rprior=rpoint->next;
  if (rpoint->zone!=basezone)
   sprintf(outnow,"%s.%3ux",out,rpoint->zone);
  else
   sprintf(outnow,"%s",out);
  driveto(outnow);
  chdir(outnow);
  for (bull=0;bull<=3;bull++)
   {
   switch (bull) {
    case 0:
     sprintf(s,"%s\\%4ux%4ux.FLO",outnow,rpoint->net,rpoint->node);
     break;
    case 1:
     sprintf(s,"%s\\%4ux%4ux.CLO",outnow,rpoint->net,rpoint->node);
     break;
    case 2:
     sprintf(s,"%s\\%4ux%4ux.DLO",outnow,rpoint->net,rpoint->node);
     break;
    case 3:
     sprintf(s,"%s\\%4ux%4ux.HLO",outnow,rpoint->net,rpoint->node);
     break;
    }
   if (!access(s,0))
    {
    f=fsh_open(s,"rt");
    while (fgets(s1,255,f)!=NULL)
     {
     fpointer=ftell(f);
     fclose(f);
     if (s1[0]=='#')
      kill=OTHER;
     if (s1[0]=='^')
      kill=TRUE;
     s1[0]=' ';
     sscanf(s1,"%s",outfile);
     fnsplit(outfile,drive,dir,file,ext);
     sprintf(name,"%s%s",file,ext);
     sprintf(s1,"%s\\%s",outnow,name);
     if (!access(s1,0))
      unlink(s1);
     f=fsh_open(outfile,"rb");
     write=fsh_open(s1,"wb");
     while (!feof(f))
      fputc(fgetc(f),write);
     fclose(f);
     fclose(write);
     if (kill==TRUE)
      unlink(outfile);
     if (kill==OTHER)
      {
      f=fsh_open(outfile,"wb");
      fclose(f);
      }
     write_email(outnow,name);
     f=fsh_open(s,"rt");
     fseek(f,fpointer,SEEK_SET);
     }
    fclose(f);
    unlink(s);
    }
   }
  for (bull=0;bull<=3;bull++)
   {
   switch (bull) {
    case 0:
     sprintf(s,"%s\\%4ux%4ux.OUT",outnow,rpoint->net,rpoint->node);
     break;
    case 1:
     sprintf(s,"%s\\%4ux%4ux.HUT",outnow,rpoint->net,rpoint->node);
     break;
    case 2:
     sprintf(s,"%s\\%4ux%4ux.CUT",outnow,rpoint->net,rpoint->node);
     break;
    case 3:
     sprintf(s,"%s\\%4ux%4ux.DUT",outnow,rpoint->net,rpoint->node);
     break;
    }
   if (!access(s,0))
    {
    fnsplit(s,drive,dir,file,ext);
    sprintf(outfile,"%s%s",file,ext);
    write_email(outnow,outfile);
    unlink(s);
    }
   }
  rpoint=rpoint->next;
  } while (rprior!=NULL);
 }
}


/**************************************************************************
*                                 Main.
***************************************************************************/

void main (void)
{
int c;
char *p,s[MAXPATH];
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];

printf("\n\n\n");
printf("þ Fido2Internet2WWIV Import/Export %s\n", VERSION);
printf("þ A UrielSoft Production\nþ http:\\\\www.mindspring.com\\~surgicalsteel\\home.htm\n");
printf("\n");

detect_multitask();

if ((p=searchpath("CONFIG.DAT"))==NULL)
 exit(100);
fnsplit(p,drive,dir,file,ext);
sprintf(s,"%s%s",drive,dir);
strncat(bbs,s,(strlen(s)-1));
giveup_timeslice();
mailer=ARCMAIL;
if ((readini())==0)
 {
 printf("í Cannot find FIW.INI!  Fatal Error! \n");
 exit(100);
 }
giveup_timeslice();
import();
giveup_timeslice();
if (read_route()==FALSE)
 printf("í A memory error has occurred!\ní Routing unable to load!\n");
send_freq();
export();
giveup_timeslice();
for (c=0;c<=500;c++)
 {
 sprintf(s,"%s\\fiw\\OUTFIW.%d",c);
 if (!access(s,0))
 remove(s);
 }
rpoint=rstart;
do {
 rprior=rpoint->next;
 free(rpoint);
 rpoint=rprior;
 } while (rprior != NULL);
idpoint=idstart;
do {
 idprior=idpoint->nexter;
 free(idpoint);
 idpoint=idprior;
 } while (idprior != NULL);
driveto(bbs);
chdir(bbs);
printf("þ Thanks for using FIW!\n");
flushall();
fcloseall();
exit(0);
}

/************************************************************************
* Share and multitasker sections stolen wholesale from the PPP project
* code, primarily by Frank Reid (though I didn't use ALL of it... <grin>
************************************************************************/


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
