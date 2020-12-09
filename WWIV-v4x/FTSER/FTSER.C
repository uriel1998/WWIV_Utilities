/*************************************************************************
* FTS list updater and maintainer.  At least it's commented this time.
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
#include <spawno.h>
#include <datelib.h>

/* datelib.h is freeware code by Brady Tippit, and seems to be invulnerable
*  to both Y2K and the "binary millenium".  The full archive is included
*  with this distribution as a convenience if you wish to recompile this
*  source.  I owe him a great debt;  I doubt I could have coded his stuff.
*  Basically all non-ANSI C time coding is his.*/

/************************************************************************
* Share and Multitasking stolen wholesale from the PPP project around
* Beta-85 or so.
************************************************************************/

#define WAIT_TIME 10
#define TRIES 100
#define SHARE_LEVEL 10
#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40

/* my global structures, variables, etc.  I hate passing lots of stuff
*  when all my functions use the same things */
#define YES 0
#define NO 1

 int multitasker=0;
 int nodeindex,listindex,aged=30;
 int removedlists=0,missinglists=0,oldlists=0;
 int maxnode,maxlist; /* maximum record to look at */
 char filedir[256],listdir[256];

struct {
  int number; /* node number, silly */
  int list; /* does it exist? */
  long int age;  /* of file list */
  } node[300];  /* Storing node information here */

struct {        /* This is to store bbslist.* from bbslist.0 */
  int number;   /* as I found from GFLINK, is not save to assume that */
  } bbslist[30]; /* all existing files are used in bbslist.0 */

FILE *fsh_open(char *path, char *fmode);
long sh_lseek(int handle, long offset, int fromwhere);
void giveup_timeslice(void);
void detect_multitask(void);
int get_nb_version(void);
int get_dos_version(void);
int get_win_version(void);
int get_dv_version(void);
void extra_lists(void);
void request_missing(void);
void request_old(void);

/*************************************************************************
* Finds extra lists and nukes 'em.
************************************************************************/
void extra_lists(void)
 {
 struct ffblk ffblk;
 char scrap[256],scrap2[256];
 short qwerty,match=NO;

 sprintf(scrap,"%sL*.FTS",listdir);
 qwerty=findfirst(scrap,&ffblk,0);
 while (!qwerty)
  {
  match=NO;
  sprintf(scrap,"%s%s",listdir,ffblk.ff_name); /* the file name in question */
  for (nodeindex=0;(nodeindex<=maxnode)&&match==NO;nodeindex++)
   {
   sprintf(scrap2,"%sL%d.FTS",listdir,node[nodeindex].number);
   if (stricmp(scrap,scrap2)==YES)
    match=YES;
   }
  if (match==NO)
   {
   removedlists++;
   printf("þ Removing %s\n",scrap);
   unlink(scrap);
   }
  qwerty=findnext(&ffblk);
  }
 /*****************************************************************
 * Yes, I could have just split the path.  But this way I'm a little closer
 * to UNIX compatibility.  Sorta.
 **********************************************************************/
 sprintf(scrap,"%sDIRFILE\\L*.DIR",listdir);
 qwerty=findfirst(scrap,&ffblk,0);
 while (!qwerty)
  {
  match=NO;
  sprintf(scrap,"%sDIRFILE\\%s",listdir,ffblk.ff_name); /* the file name in question */
  for (nodeindex=0;(nodeindex<=maxnode)&&match==NO;nodeindex++)
   {
   sprintf(scrap2,"%sDIRFILE\\L%d.DIR",listdir,node[nodeindex].number);
   if (stricmp(scrap,scrap2)==YES)
    match=YES;
   }
  if (match==NO)
   {
   printf("þ Removing %s\n",scrap);
   unlink(scrap);
   }
  qwerty=findnext(&ffblk);
  }
 }

/*************************************************************************
* Requests any missing lists.  Simple spawning techniques.
************************************************************************/

void request_missing(void)
 {
 char misstemp[256],misstemp0[20];

 chdir(filedir);
 for (nodeindex=0;(nodeindex<=maxnode);nodeindex++)
  {
  if (node[nodeindex].number==32767) /* like you need one from the net */
   continue;
  if (node[nodeindex].list==NO) /* Ah-ha! */
   {
   missinglists++;
   sprintf(misstemp,"%s\\FTSREQ.EXE",filedir);
   sprintf(misstemp0,"-@%d",node[nodeindex].number);
   spawnl(P_WAIT,misstemp,"FTSREQ.EXE","-L",misstemp0,NULL);
   }
  }
 }

/*************************************************************************
* Requests any lists which have a value over passed "aged"
************************************************************************/

void request_old(void)
 {
 char oldtemp[256],oldtemp0[20];

 chdir(filedir);
 for (nodeindex=0;(nodeindex<=maxnode);nodeindex++)
  {
  if (node[nodeindex].number==32767) /* like you need one from the net */
   continue;
  if ((node[nodeindex].age>=aged)||(aged==0)) /* If "aged" is 0==request all */
   {
   oldlists++;
   sprintf(oldtemp,"%s\\FTSREQ.EXE",filedir);
   sprintf(oldtemp0,"-@%d",node[nodeindex].number);
   spawnl(P_WAIT,oldtemp,"FTSREQ.EXE","-L",oldtemp0,NULL);
   }
  }
 }

/*************************************************************************
* Main reads in arguments and node data, then passes to the relevant
* subprocesses.  In theory, anyway.
************************************************************************/

void main(int argc, char *argv[])
 {
 FILE *read;
 int missing=NO,old=NO;

 struct ftime timecrap;   /* time declarations */
 tdtype *date1,*date2;
 int month, day, year;
 char timetemp[256];

 /* miscellaneous temp variables */
 char crap[256],smallcrap[20],smallcrap0[20];
 char *p;
 int junk;

 /* end of defines, program starts */
 printf("\nþ Fts'er v1.1 - A UrielSoft Product\n");
 if (argc < 2)
  {
  printf("í Usage is FTSER [path] -<a[age]/o[age]/m>\n");
  printf("í [path] is the DOS path for Filenet, withOUT trailing backslash.\n");
  printf("í Switches: \n");
  printf("í -a = Request lists older than [age] and missing lists\n");
  printf("í -o = Request lists older than [age]\n");
  printf("í -m = Request missing lists\n");
  printf("í [age] is a value in days, and defaults to 30.\n");
  printf("í [age] == 0 requests all lists.\n");
  printf("í Lists from non-existent nodes are automatically purged.\n");
  exit(100);
  }
 for (nodeindex = 0; nodeindex < 300;nodeindex++)
  {
  node[nodeindex].number = 32767; /* setting to a "stop" value */
  node[nodeindex].list=NO;
  node[nodeindex].age=0;
  }
 for (listindex = 0; listindex < 30;listindex++)
  bbslist[listindex].number = 32767; /* setting to "stop" value */

 if ((strncmpi("-a",argv[2],2)==YES)||(strncmpi("-m",argv[2],2)==YES))
  missing=YES;
 if ((strncmpi("-a",argv[2],2)==YES)||(strncmpi("-o",argv[2],2)==YES))
  {
  old=YES;
  sprintf(smallcrap,"%s",argv[2]); /*redundancy; sometimes have problems with argvs */
  for (junk=2;junk<=(strlen(smallcrap));junk++)
   smallcrap0[junk-2]=smallcrap[junk];  /*this should strip -? and leave age */
  if (atoi(smallcrap0)==0) /* error in atoi? */
   {
   if (smallcrap[3]=='0')
    aged=0; /* nope, atoi returned the correct value instead of an error */
   } else {  /* atoi returned non-zero number */
   aged=atoi(smallcrap0);
   }  /* If neither called, age remains at 30 */
  printf("þ Filelist Request Age set to %d days.\n",aged);
  }
 sprintf(filedir,"%s",argv[1]);
 if (chdir(filedir)!=YES)
  {
  printf("í Error in changing to Filenet directory! Check paths and drives!\n");
  exit(100);
  }
 /* must find out where filelist directory is located! */
 /* okay, so I could have this now run without telling it anything. */
 /* But I don't want to.  So there.  Ha.  I *LIKE* programs that tell you */
 /* about themselves when run without arguments and you're an idiot. =) */
 sprintf(crap,"%s\\FDLFTS.CFG",filedir);
 read=fsh_open(crap,"rt");
 while ((fgets(crap,254,read)!=NULL))
  {
  if (strnicmp(crap,"ALT_PATH",8)==YES)
   sscanf(crap,"%s%s",smallcrap,listdir);
//  if (strnicmp(crap,"LOG_PATH",8)==YES)  /* could easily be enabled for */
//   sscanf(crap,"%s%s",smallcrap,logpath); /* logging, but most is already done! */
  }
 fclose(read);
 /* reading in correct bbslists, etc */
 sprintf(crap,"%s\\BBSLIST.0",filedir);
 read=fsh_open(crap,"rt");
 if (read!=NULL)
  {
  maxlist=0;  /* starting at record zero - we also set maxlist here */
  maxnode=0;
  nodeindex=0; /* these start at zero as we first start this run */
  while ((fgets(crap,254,read)!=NULL))
   {
   sscanf(crap,"%s",smallcrap);  /* just want the first bit... */
   if ((strstr(smallcrap,"~"))||(strstr(smallcrap,":"))||(strstr(smallcrap,"99")))
    continue; /* control characters, other stuff I'm not interested in nixed.*/
   junk=atoi(smallcrap); /* make it an integer! */
   if (junk!=99) /* doublechecking */
    {
    bbslist[maxlist].number=junk;
    maxlist++;   /* saved the bbslist number in our structure */
    }
   }  /* last valid entry is maxlist record */
  fclose(read);
  } else {
  printf("í Your BBSLIST.0 is gone! Request a new set of network files!\n");
  exit(100);
  }
 date1 = CreateDate(); /* pulled outside loop to minimize allocations */
 date2 = TimeDate();   /* The current time/date info. */

 for (listindex=0;listindex<=maxlist;listindex++)
  {
  sprintf(crap,"%s\\BBSLIST.%d",filedir,bbslist[listindex].number);
  read=fsh_open(crap,"rt");
  while ((fgets(crap,254,read)) && (crap!=NULL)) /* while getting valid data from file */
   {
   if (crap[0]!='@') /* not a node for some reason */
    continue;
   sscanf(crap,"%s",smallcrap);
   p=NULL;
   p=smallcrap0;
   for (junk=1;junk<=((strlen(smallcrap))-1)&&smallcrap[junk]!='\0';junk++)
    {
    *p++=smallcrap[junk];
    }
   *p='\0';
   node[nodeindex].number=atoi(smallcrap0);
   sprintf(crap,"%sL%d.FTS",listdir,node[nodeindex].number);
   if (access(crap,0)==YES)  /* If it has a filelist */
    {
    int month,day,year;

    node[nodeindex].list=YES;
    junk=open(crap,O_RDONLY,O_BINARY);
    if (getftime(junk,&timecrap)==YES)  /* V1.1 Change - added extra error */
     {                                  /* checking here, and got paranoid re - format */
     close(junk); /* redundancy */
     sprintf(crap,"%u %u %u",timecrap.ft_month,timecrap.ft_day,(timecrap.ft_year+1980));
     sscanf(crap,"%d%d%d", &month, &day, &year);
     if (LoadDate(date1,month,day,year) == -1 )
      {
/*SURGE - change this from such an unelegant break! */
      printf("í DATE ERROR!\n");
      fcloseall();
      exit(100);
      }
     node[nodeindex].age = DateDiff(date1, date2);
     } else {
     close(junk);   /* moved this here from up above, where it was ineffectual */
     }
    }
   maxnode++;
   nodeindex++;  /* added a new nodenumber! */
   }
  }
 FreeDate(&date1); /* moved to outside the loop to try to avoid corruption */
 /*Right here we have read in all the information.  We have a list of
 * nodes, whether they have a nodelist or not, and if so, how old it is.*/
 extra_lists();
 if (missing==YES)
  request_missing();
 if (old==YES)
  request_old();
 printf("þ Removed %d unused file lists!\nþ Requested %d missing lists!\nþ Requested %d old lists!\n",removedlists,missinglists,oldlists);
 exit(0);
 }

/************************************************************************
* Share and Multitasking stolen wholesale from the PPP project around
* Beta-85 or so.
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
