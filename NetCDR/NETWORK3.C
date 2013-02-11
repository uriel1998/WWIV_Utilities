/***********************************************************************
*  This is NetCommander4.  It is currently written with static arrays of
*  structures instead of dynamics because, well, frankly I'm not that good.
*  If you *are* and want to rewrite it using 'em, go for it.
*
*  The two main areas that you may wish to alter are the total number
*  of nodes (or networks) allowed [search for NODEME] or to add another
*  utility [search for UTILME].  Easy mods.
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

#define WAIT_TIME 10
#define TRIES 100
#define SHARE_LEVEL 10
#define MT_DESQVIEW 0x01
#define MT_WINDOWS  0x02
#define MT_OS2      0x04
#define MT_NB       0x40


#define YES 0
#define NO 1

FILE *fsh_open(char *path, char *fmode);
long sh_lseek(int handle, long offset, int fromwhere);
void giveup_timeslice(void);
void detect_multitask(void);
int get_nb_version(void);
int get_dos_version(void);
int get_win_version(void);
int get_dv_version(void);
short int readini (void);
void driveto (char *feed);
int is_even(int query);
void run_autocon(void);
int run_globalflink(void);
int copy_de555(void);

struct {
     unsigned char fido;   //(1==YES!)
     char name[16];
     char dir[69];    /* I would have 256 here, but it's not my structure */
     unsigned short ournode;
     char fake[12];  /* after reading, put in if it has a bbslist.0 here */
		     /* "BBSLIST  YES" "BBSLIST   NO" */
} netdata[20];       /* memory allocation makes things so much more diff. */

/* NODEME - you must change the [20] at the end of netdata, above, and
also at the end of long number in the structure below.  This must be the
SAME NUMBER FOR BOTH, and will change the number of max networks.  To
increase the number of nodes allowed in primenet, change the [250] after
node, below. */

struct {
 char name[30];
 char phonenum[15]; /* if no registration number - only FULL matches */
 long number[20]; /* array of nodes that this is, arranged by netnum */
 long regnum; /* registration number of the node */
 int marked; /* is it "marked" flinkable? */
} node[250];

/************************ GLOBAL VARIABLES *******************************/
 char bbs[256],bbsdir[256];   /* bbs w/backslash, ???dir w/o backslash */
 char primename[16];  /* duh */
 short primenet=-1;  /* until primenet is found */
 short sort=0;  /* 0=by node in flink.ini, 1=by network in flink.ini */
 short uninstall=NO,diagall=NO,specific=NO,checkonly=YES;
 short global=NO,acon=0,use_marking=YES;  /* see INI section for ACON values */
 int netindex=0,maxnet=0,nodeindex=0,maxnode=0; /* self-explanatory */
 int multitasker = 0;

/************************ PROGRAM CODE *******************************/
void main (short argc, char *argv[])
 {
 char cmdline0[20];
 char *p,scrap[256],scrap0[256];
 char drive[MAXDRIVE],dir[MAXDIR],smallcrap[MAXFILE],tinycrap[MAXEXT];
 FILE *read;
 int junkint;

 /* mod 4.1 */
 printf("\nþ NetCommander (aka GFLINK) v4.0\n");
 printf("þ A UrielSoft Utility for WWIV networks.\n");

 junkint=argc;
 sprintf(cmdline0,"%s",argv[1]);
 switch (cmdline0[0])   /* parsing the commandline */
  {
  case '/':
   if (strncmpi("/?",cmdline0,strlen(cmdline0))==YES)
    {
    printf("í NetCommander should be invoked either as a Network3 pre-processor\n");
    printf("í - see the docs about that - or by the following syntax:\n");
    printf("í NETWORK3 -[switches]\n");
    printf("í -D = Diagnose all networks and send feedback\n");
    printf("í -[networkname] = Diagnose and send feedback about that net\n");
    printf("í -UNINSTALL = Uninstall Gflink routines\n");
    exit(100);
    }
  case '-':
   if (strncmpi("-?",cmdline0,strlen(cmdline0))==YES)
    {
    printf("í NetCommander should be invoked either as a Network3 pre-processor\n");
    printf("í - see the docs about that - or by the following syntax:\n");
    printf("í NETWORK3 -[switches]\n");
    printf("í -D = Diagnose all networks and send feedback\n");
    printf("í -[networkname] = Diagnose and send feedback about that net\n");
    printf("í -UNINSTALL = Uninstall Gflink routines\n");
    exit(100);
    }
   if (strncmpi("-UNINSTALL",cmdline0,strlen(cmdline0))==YES)
    {
    uninstall=YES;
    break;
    }
   if (strncmpi("-D",cmdline0,strlen(cmdline0))==YES)
    {
    diagall=YES;  /* feedback all networks */
    break;
    }
   specific=YES;  /* just one network, please. */
   break;
  case '.':
   break;
  default:
   break;
  }
 if ((p=searchpath("WWIV.INI"))==NULL)
  {  printf("í File Error - WWIV.INI\n");  exit(100);  }
 fnsplit(p,drive,dir,smallcrap,tinycrap);
 sprintf(bbs,"%s%s",drive,dir);
 strncat(bbsdir,bbs,(strlen(bbs)-1));  /* removing the backslash for chdir */
 /* reading in options first so can determine primenet */
 if (readini()==NO)
  {  printf("í Error reading WWIV.INI\n");  exit(100);  }
 sprintf(scrap,"%sCONFIG.DAT",bbs);
 read=fsh_open(scrap,"rb");
 if (read==NULL)
  { printf("í File Error - CONFIG.DAT\n");  exit(100);  }
 fseek(read,204,SEEK_SET);
 fscanf(read,"%s",scrap);
 fclose(read);
 if (scrap[1]!=':')  /* If not path, make it so. */
  {
  if (scrap[0]!='\\') /* no leading backslash either! */
   {
   sprintf(scrap0,"%s",bbs,scrap);
   } else {
   sprintf(scrap0,"%s",bbsdir,scrap); /* had aleading backslash */
   }
  sprintf(scrap,"%s",scrap0);
  }
 sprintf(scrap0,"%s",scrap);  /* handle all possibles for dir problems */
 sprintf(scrap,"%sNETWORKS.DAT",scrap0);
 read=fsh_open(scrap,"rb");
 if (read==NULL)
  { printf("í File Error - NETWORKS.DAT\n");  exit(100);  }
 fread(&netdata[netindex],1,sizeof(netdata[netindex]),read);
 /* note the lack of incrementation here.  We want to have the count for
 * netindex and maxnet to start at ZERO. */
 while (!feof(read))
  {			/* is it our "prime network"? */
  if (strnicmp(netdata[netindex].name,primename,strlen(netdata[netindex].name))==YES)
   {
   for (junkint=0;junkint<=(strlen(netdata[netindex].name));junkint++)
    {   			/*Changing the format of PrImEnAmE! */
    if (is_even(junkint)==NO)    /* if even... */
     primename[junkint]=tolower(netdata[netindex].name[junkint]);
    else
     primename[junkint]=toupper(netdata[netindex].name[junkint]);
    }
   sprintf(netdata[netindex].name,"%s",primename);
   primenet=netindex; /* now we have our reference netnum as well */
   }
  if (netdata[netindex].dir[1]!=':')  /* If not path, make it so. */
   {
   if (netdata[netindex].dir[0]!='\\') /* no leading backslash either! */
    {
    sprintf(scrap,"%s%s",bbs,netdata[netindex].dir);
    sprintf(netdata[netindex].dir,"%s",scrap);
    } else {
    sprintf(scrap,"%s%s",bbsdir,netdata[netindex].dir); /* leading backslash */
    sprintf(netdata[netindex].dir,"%s",scrap);
    }
   }
  netindex++; maxnet++;
  fread(&netdata[netindex],1,sizeof(netdata[netindex]),read);
  }
 fclose(read);  /* read all network data! Woo Hoo!*/
 /* diagnostics! */
 driveto(bbsdir);
 sprintf(scrap0,"%sNET3.EXE",bbs);
 if (diagall==YES) /* global diagnostics does NOT call any utilities */
  {
  for (netindex=0;netindex<maxnet;netindex++)
   {
   sprintf(tinycrap,".%d",netindex);
   if (netdata[netindex].fido!=1)  /* does not process fido networks - NOT SURE IF THIS WILL WORK HERE */
    spawnl(P_WAIT,scrap0,"NET3.EXE",tinycrap,"Y",NULL);
   }
  exit(0);
  }
 if (specific==YES) /* named net */
  {
  for (junkint=1;junkint<=(strlen(cmdline0));junkint++)
   scrap[junkint-1]=cmdline0[junkint];  /* strip the "-" off the net name */
  for (netindex=0;netindex<maxnet;netindex++)
   {   /* off of -w will analyze WWIVlink, wwivnet, wwivdatanet, etc. */
   sprintf(tinycrap,".%d",netindex);
   if (strnicmp(netdata[netindex].name,scrap,(strlen(scrap)))==0)
    {
    sprintf(scrap0,"%sNET3.EXE",bbs);
    spawnl(P_WAIT,scrap0,"NET3.EXE",tinycrap,"Y",NULL);
    }
   }
  exit(0);
  }
 if ((acon > 0) && (uninstall==NO) && (primenet!=-1))
  run_autocon();
 if (((uninstall==YES) || (global==YES)) && (primenet!=-1)) /* gotta call gflink to uninstall it */
  {
  if (run_globalflink()==NO)
   {
   printf("í Program Error - GLOBALFLINK\n");
   }
  }
 for (netindex=0;netindex<maxnet;netindex++)
  {
  sprintf(tinycrap,".%d",netindex);
  if (strnicmp(tinycrap,cmdline0,(strlen(cmdline0)))==YES)
   {
   spawnl(P_WAIT,scrap0,"NET3.EXE",tinycrap,"Y",NULL);
   } else {
   if (netdata[netindex].fido!=1)  /* does not process fido networks - NOT SURE IF THIS WILL WORK HERE */
    {
    spawnl(P_WAIT,scrap0,"NET3.EXE",tinycrap,NULL);
    }
   }  /* supernesting! */
  }

 driveto(bbs);
 sprintf(scrap,"%sAUTOSEND.EXE",bbs);
 sprintf(smallcrap,"/REMOVE");
 if (access(scrap,0)==YES)
  spawnl(P_WAIT,scrap,"AUTOSEND.EXE",smallcrap,NULL);
/**********************************************************************
*  UTILME
*  If you want to run other utilities, add them here.  The example of
*  AUTOSEND is given as a demonstration.  If you need more than one
*  switch, scrap0 and tinycrap are still open, though the latter is
*  only four spaces.  You'd just stack them inbetween the filename
*  and before the NULL.
***********************************************************************/

 exit(0);
 } /* END OF MAIN */

/***********************************************************************
* Reads the configuration section in WWIV.INI  Token [NETCDR]
************************************************************************/
short int readini (void)
 {
 #define INI_CONFIG 0x01
 #define INI_OTHER 0x02

 char cursect;
 char line[256], *ss;
 FILE *ini;
 short int ini_section,found=NO;

 sprintf(line,"%sWWIV.INI",bbs);
 ini = fsh_open(line,"rt");
 if (ini == NULL)
  return(NO);
 while (fgets(line,256,ini))
  {
  ss=NULL;
  ss=strtok(line,"=");
  if (ss)
   ss=strtok(NULL,"\n");
  if ((line[0]==';')||(line[0]=='\n')||(strlen(line)==0))
   continue;
  if ((line[0]=='[')&&(strnicmp(line,"[NETCDR]",8)==YES))
   {
   cursect = INI_CONFIG;
   ini_section |= INI_CONFIG;
   found=YES;
   continue;
   }
  if ((line[0]=='[')&&(strnicmp(line,"[NETCDR]",8)!=YES))
   {
   cursect = INI_OTHER;
   ini_section |= INI_OTHER;
   continue;
   }
  if (cursect & INI_CONFIG)
   {
   strupr(ss);  /* should do it for all values, save cycles */
   if (strnicmp(line,"GFLINK",6)==0)  /* run globalflinking? */
    {
    if (strstr(ss,"YES")!=NULL)
     {
     global=YES;
     }
    continue;
    }
   if (strnicmp(line,"MARKED_ONLY",11)==0) /* only nodes marked for flink? */
    {
    if (strstr(ss,"NO")!=NULL)
     use_marking=NO;
    continue;
    }
   if (strnicmp(line,"PRIMENET",8)==0) /* What is the primary network? */
    {
    sscanf(ss,"%s",primename);   /* not sure if I need anything else here */
    continue;
    }
   if (strnicmp(line,"CHECKONLY",9)==YES)
    {
    if (strstr(ss,"NO")!=NULL)
     checkonly=NO;
    continue;
    }
   if (strnicmp(line,"SORT",4)==0) /* How to sort it? */
    {
    if (strstr(ss,"NETWORK")!=NULL)
     sort=1;
    continue;
    }
   if (strnicmp(line,"AUTOCON",7)==0) /* run autocon? */
    {
    /* ACON values:   (oh, this is slick)
    *  0 = don't run
    *  1 = run with only primenet
    *  2 = run with -N switch
    *  3 = run with -M switch
    *  4 = idiots can't configure nuthin.
    */
    if (strstr(ss,"YES")!=NULL)
     acon++;
    if (strstr(ss,"NEWS")!=NULL)
     acon=acon+1;
    if (strstr(ss,"MAIL")!=NULL)
     acon=acon+2;
    continue;
    }
   }
  }
 if (ini!=NULL)
  fclose(ini);
 if (found==YES)  /* Was WWIV.INI, with NETCDR token */
  {
  return(YES);
  } else {
  sprintf(line,"%sWWIV.INI",bbs); /* creating entry */
  ini = fsh_open(line,"at");
  fprintf(ini,"\n[NETCDR]\n;inserted by NetCommander v4.0\nGFLINK=NO\nMARKED_ONLY=YES\nAUTOCON=NO\nCHECKONLY=YES\nSORT=NODES\nPRIMENET=Insert a Network for GFLINK and Autocon!\n");
  fclose(ini);
  return(YES);
  }
 }

/***********************************************************
*  Pretty self-explanatory, wot?
***********************************************************/
void run_autocon(void)
 {
 char s[256],s1[10];

 sprintf(s,"%sAUTOCON.EXE",bbs);
 sprintf(s1,".%d",primenet);
 if (access(s,0)==YES)
  {
  switch (acon)
   {
   case 3:
    spawnl(P_WAIT,s,"AUTOCON.EXE",s1,"-M",NULL);
    break;
   case 2:
    spawnl(P_WAIT,s,"AUTOCON.EXE",s1,"-N",NULL);
    break;
   case 1:
    spawnl(P_WAIT,s,"AUTOCON.EXE",s1,NULL);
    break;
   default:
    break;
   }
  }
 driveto(bbsdir);
 chdir(bbsdir);
 }

/* Duh.  Like you can't figure this out.  */
int is_even(int query)
 {
 div_t x; /*define division structure */

 x = div(query,2);
 if (x.rem==0)    /* if even.. */
  return(YES);
 else
  return(NO);
 }

/***********************************************************
*  Changes drives and directories in one swoop.  Fell, that is.
***********************************************************/
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
 chdir(feed);
 }
/************************************************************************
* GLOBALFLINK routines.  Also used to UNINSTALL globalflink.
*
************************************************************************/
int run_globalflink(void)
 {
 int bbslists[99],match=NO; /* array to hold valid bbslist #s & match int */
 char scratch[256],scratch0[256],small[20],small0[20],small1[20],small2[20],*pointy;
 FILE *gread,*gwrite;
 long crudint,crudint0,crudint1;
 long crudlong;
 /*start with cleanup - each run it'll start afresh.  CONNECT.99 in all
 * networks save primenet will be toasted, the bbslist.0 will have any
 * references to connect.99 removed, if it was originally a bbslist.net
 * network, that'll be restored, CALLOUT.NET will be wiped, and FLINK.INI
 * will have all non-matching entries thrashed - and then it exits.*/
 printf("þ Performing network ");
 if (uninstall==YES)
  printf("uninstall of GFLINK data.\n");
 else
  printf("cleanup of old GFLINK data.\n");
 for (netindex=0;netindex<maxnet;netindex++)
  {
  if (netindex==primenet)  /*we don't touch primenet in cleaning up*/
   continue;
  if (netdata[netindex].fido==1)  /* does not process fido networks - NOT SURE IF THIS WILL WORK HERE */
   continue;
  sprintf(scratch,"%sCONNECT.99",netdata[netindex].dir);
  if (access(scratch,0)==YES)
   unlink(scratch);
/* BBSLIST STUFF
*  HERE IS WHERE WE WILL ADD SUPPORT TO PUT BACK IN THE BBSLIST.NET.
*  I want to enable non-grouped networks to utilize GFLINK.  Currently, I
*  could easily add in bits to help with this.  There are several issues,
*  though...
*
*  Properly updating the whole dataset when a network update comes in
*  (eg - making the bbslist.net and connect.net *.1 and so on)
*  Letting Gflink know that it was originally a non-grouped net
*  Uninstalling to leave a pristine network dataset.
*
*  Possible solutions so far:
*  copying them to a *.bak and looking for the *.bak files
*  looking for node numbers in bbslist.net (only good for incoming updates)
*  possibly being able to comment the bbslist.0?
*/
  sprintf(scratch,"%sBBSLIST.0",netdata[netindex].dir);  /* removing 99 in BBSLIST.0 */
  sprintf(scratch0,"%sBBSLIST0.BAK",netdata[netindex].dir); /* always doing just because */
  if (access(scratch0,0)==YES)
   unlink(scratch0);
  rename(scratch,scratch0);
  gwrite=fsh_open(scratch,"wt");
  gread=fsh_open(scratch0,"rt");
  while (fgets(scratch,254,gread)!=NULL)
   {
   if ((strncmp(scratch,"99",2)!=YES)&&(sscanf(scratch,"%s",small)>0)) /* no match */
    fputs(scratch,gwrite);
   }
  fcloseall(); /* leaving the .BAK file */
  sprintf(scratch,"%sCALLOUT.NET",netdata[netindex].dir);  /* removing additions to CALLOUT.NET */
  sprintf(scratch0,"%sCALLOUT.BAK",netdata[netindex].dir); /* always checking, just in case */
  if (access(scratch0,0)==YES)
   unlink(scratch0);
  rename(scratch,scratch0);
  gwrite=fsh_open(scratch,"wt");
  gread=fsh_open(scratch0,"rt");
  while (fgets(scratch,254,gread)!=NULL)
   {
   if (strnicmp(scratch,"@",1)!=YES) /* not a node entry! */
    continue;
   if (strstr(scratch,"_"))  /* a GFLINK entry! */
    continue;
   fputs(scratch,gwrite);
   }
  fcloseall(); /* again, leaving the *.bak file */
  /* go to next network... */
  }
 sprintf(scratch,"%sFLINK.INI",bbs);
 if (access(scratch,0)==-1)  /* If there's NOT already a FLINK.INI */
  {
  gwrite=fsh_open(scratch,"wt");
  fprintf(gwrite,";Created by NetCommander v4.0\n");
  fclose(gwrite);
  } else {          /* it is there, so we've got to clean it */
  sprintf(scratch0,"%sFLINK.BAK",bbs);
  if (access(scratch0,0)==YES)
   unlink(scratch0);
  rename(scratch,scratch0);
  printf("þ Placing non-automatic entries back in FLINK.INI\n");
  gwrite=fsh_open(scratch,"wt");
  gread=fsh_open(scratch0,"rt");
  while ((fgets(scratch,254,gread))!=NULL)
   {  /* remember, primenet name is now like this ExAmPlE! */
   if (strstr(scratch,netdata[primenet].name)==NULL)
    fputs(scratch,gwrite);
   }
  fcloseall();
  }
 if (uninstall==YES)
  {
  printf("þ Uninstall completed. \n");
  return(YES);
  }
 /* Now to actually start doing this. I NEED to do primenet first, so I
 * don't end up with extra nodes that might overwhelm the system! */
 /* Currently, primenet is assumed to have groups for simplicity */
 /* setting all node numbers to -1 to avoid false matches */
 for (nodeindex=0;nodeindex<=249;nodeindex++)
  {
  for (crudint=0;crudint<maxnet;crudint++)
   {
   node[nodeindex].number[crudint]=-1;
   }
  }
 printf("þ Reading %s nodes and registration numbers.\n",netdata[primenet].name);
 nodeindex=0;maxnode=0;
 sprintf(scratch,"%sBBSLIST.0",netdata[primenet].dir);
 if (access(scratch,0)!=YES)  /* fatal error for primenet */
  return(NO);
 gread=fsh_open(scratch,"rt");
 crudint=0;  /* using this as an index of the bbslists for the array */
 while (fgets(scratch,254,gread)!=NULL)
  {
  if ((scratch[0]=='~')||(scratch[0]==':'))
   continue;  /* don't want any of this stuff here */
  crudint0=atol(scratch);  /* using crudint0 as a double-check - and already defined it anyway. */
  if ((crudint0!=0)&&(crudint0!=99)) /* should NEVER equal zero - either didn't work, or invalid entry */
   {
   bbslists[crudint]=crudint0;  /* forming my array... */
   crudint++;
   }
  }
 fclose(gread);
 /* using crudint0 as the counter, crudint-1 as the maximum! (hence no <= )*/
 for (crudint0=0;crudint0 < crudint;crudint0++)
  {
  sprintf(scratch,"%sBBSLIST.%d",netdata[primenet].dir,bbslists[crudint0]);
  gread=fsh_open(scratch,"rt");
  while (fgets(scratch,254,gread)!=NULL)
   {
   if (scratch[0]!='@')  /* not a node entry for some reason */
    continue;
   pointy=strchr(scratch,'@');  /* node number */
   pointy++;
   sscanf(pointy,"%s",small);
   crudint1=atol(small);
   if (crudint1==netdata[primenet].ournode) /* whoops! Our own node! */
    continue;
   node[nodeindex].marked=NO;
   node[nodeindex].number[primenet]=crudint1;  /* set node number */
   if (strchr(scratch,'`')!=NULL) /* Marked Flinkable */
    node[nodeindex].marked=YES;
   pointy=strchr(scratch,'*');  /* phone number */
   pointy++;
   sscanf(pointy,"%s",node[nodeindex].phonenum);
   pointy=strchr(scratch,'\x22');  /* name - yes, I KNOW it's only the first */
   pointy++; 			    /* two words of the name */
   sscanf(pointy,"%s%s",small,small0);
   sprintf(node[nodeindex].name,"%s %s",small,small0);
   pointy=strchr(scratch,'[');   /* regnum */
   pointy++;
   sscanf(pointy,"%s",small);
   node[nodeindex].regnum=atol(small);
   /* mod for 4.1 */
   if (node[nodeindex].regnum <= 0)
    {
    pointy=strchr(scratch,'\x22'); /* looking for start of name */
    if (pointy!=NULL)
     {
     while ((*pointy!='*')&&(*pointy!='#')) /* gone too far */
      {
      pointy--;        /* now moving BACKWARDS...*/
      if (*pointy=='+') /* is a server - which means may not have reg# */
       {
       node[nodeindex].regnum=12111973;
       pointy=strchr(scratch,'*');
       }
      }
     }
    }
   /* end mod 4.1 - see section below! */
   nodeindex++;maxnode++;  /* successfully read a node! */
   }
  fclose(gread);
  } /* finished reading all primenet nodes after this bracket */

 if (copy_de555()==NO)  /* do they have de555.exe where it needs to be? */
  return(NO);

 /* further bbslist.0 work will be done here once I figure out a good
 * strategy for doing it - search for BBSLIST STUFF above */

 /* bigflink.dat will tell you all the potential flinks - and also allow you
 * to double-check globalflink if you feel like it! */
 sprintf(scratch,"%sbigflink.dat",bbs);
 gwrite=fsh_open(scratch,"wt");
 printf("þ Comparing nodes and registration numbers.\n");
 for (netindex=0;netindex<maxnet;netindex++)
  {
  if (netindex==primenet) /* already read that */
    continue;
  if (netdata[netindex].fido==1)  /* does not process fido networks - NOT SURE IF THIS WILL WORK HERE */
   continue;
  sprintf(scratch,"%sBBSLIST.0",netdata[netindex].dir);
  if (access(scratch,0)!=YES)
   continue;  /* further error handling - and needed for non-bbslist.0 nets from when I clean at the start of the run */
  gread=fsh_open(scratch,"rt");
  crudint=0;  /* using this as an index of the bbslists for the array */
  while (fgets(scratch,254,gread)!=NULL)
   {
   if ((scratch[0]=='~')||(scratch[0]==':'))
    continue;  /* don't want any of this stuff here */
   crudint0=atol(scratch);  /* using crudint0 as a double-check - and already defined it anyway. */
   if ((crudint0!=99)&&(crudint0!=0)) /* should NEVER equal zero - either didn't work, or invalid entry */
    {
    bbslists[crudint]=crudint0;  /* REforming my array... */
    crudint++;			  /* shouldn't matter that I used it earlier,*/
    }				  /* as I reset the index counter */
   }
  fclose(gread);
  /* using crudint0 as the counter, crudint-1 as the maximum! (hence no <= )*/
  for (crudint0=0;crudint0 < crudint;crudint0++)
   {
   sprintf(scratch,"%sBBSLIST.%d",netdata[netindex].dir,bbslists[crudint0]);
   gread=fsh_open(scratch,"rt");
   while (fgets(scratch,254,gread)!=NULL)
    {
    if (scratch[0]!='@')  /* not a node entry for some reason */
     continue;
    pointy=strchr(scratch,'[');   /* regnum first...*/
    pointy++;
    sscanf(pointy,"%s",small);
    crudlong=atol(small);
    /* mod for 4.1 (part 2)*/
    if (crudlong <= 0)
     {
     pointy=strchr(scratch,'\x22'); /* looking for start of name */
     if (pointy!=NULL)
      {
      while ((*pointy!='*')&&(*pointy!='#')) /* gone too far */
       {
       pointy--;        /* now moving BACKWARDS...*/
       if (*pointy=='+') /* is a server - which means may not have reg# */
	{
	crudlong=12111973;
	pointy=strchr(scratch,'*');
	}
       }
      }
     }
    /* end mod 4.1 - part 2 */
    if (crudlong > 0)   /* registered only! */
     {
     pointy=strchr(scratch,'*');  /* phone number */  /* these don't even get read unless unregged */
     pointy++;
     sscanf(pointy,"%s",small1);
     pointy=strchr(scratch,'\x22');  /* name - yes, I KNOW it's only the first */
     pointy++; 			    /* two words of the name */
     sscanf(pointy,"%s%s",small,small0);
     sprintf(scratch0,"%s %s",small,small0);
     match=0;  /* just seems to want it here too.  Too late to argue. */
     for (nodeindex=0;(nodeindex < maxnode)&&(match < 3);nodeindex++) /* search the structure */
      {
      match=0; /* using numerics - greater than 3 is a full match.  +2 for */
	      /* matching regnum, +1 each for name and phone # */
	      /* must be here to reset for each node!!! */
      if ((node[nodeindex].number[netindex]==-1)&&(strnicmp(small1,node[nodeindex].phonenum,(strlen(node[nodeindex].phonenum)))==YES))
       {match=match+1;}
      if ((node[nodeindex].number[netindex]==-1)&&(strnicmp(scratch0,node[nodeindex].name,(strlen(node[nodeindex].name)))==YES))
       {match=match+1;}
      if ((node[nodeindex].number[netindex]==-1)&&(node[nodeindex].regnum==crudlong)) /* checking by regnum */
       {match=match+2;}
      if (match>=3)  /* valid match by my book! */
       {break;}
      }
     if (match>=3)
      {
      pointy=strchr(scratch,'@');  /* node number */
      pointy++;
      sscanf(pointy,"%s",small);
      crudint1=atol(small);
      if (crudint1==netdata[netindex].ournode) /* whoops! Our own node! */
       continue;
      node[nodeindex].number[netindex]=crudint1;  /* set node number */
      fprintf(gwrite,"þ Matched %ld %s to %ld %s, score of %d, ",node[nodeindex].number[netindex],netdata[netindex].name,node[nodeindex].number[primenet],netdata[primenet].name,match);
      if (node[nodeindex].marked==NO)
       fprintf(gwrite,"and is NOT marked.\n");
      else
      fprintf(gwrite,"and IS marked.\n");
      }
     }
    }
   fclose(gread);  /* done with one bbslist...*/
   }
  /* done with one network...*/
  }   /* finished reading all nodes after this bracket */
  fclose(gwrite);
/* We now have all the necessary data.  We have a filled structure, where
*  we can tell you all primenet nodes, and what flinks they have in each
*  of the networks.  We can sort this by network or primenet node number
*  depending on the user's preference in flink.ini, though I'll probably
*  do a separate run by network to write the connect.99 and callout.net */

  printf("þ Go ahead, check my work.  It's in %sBIGFLINK.DAT.\n",bbs);
  if (checkonly==YES)  /*paranoid! */
   return(YES);

/* check all possible flinks to see if they're in flink.ini to avoid dupes
*  I do this by opening FLINK.INI; read a line; find out if it involves
*  primenet (read the network names and compare);  if it does, then we
*  find that node number & see if it's marked (or marking is not cared
*  about).  If yes, then we find out the other net - if we can find it and
*  there is an entry for it in the structure, we *NUKE* that entry in our
*  structure.  */

 sprintf(scratch,"%sFLINK.INI",bbs);
 sprintf(scratch0,"%s",netdata[primenet].name);
 strlwr(scratch0);   /* compare all in lowercase */
	/* using scratch strings to preserve casing of actual data */
 gread=fsh_open(scratch,"rt");
 while ((fgets(scratch,254,gread))!=NULL)
  {
  if ((scratch[0]==';')||(scratch[0]==':')||(scratch[0]==' '))
   continue;
  strlwr(scratch);
  sscanf(scratch,"%s%s%s%s",small,small1,small0,small2);
  crudint=atol(small1);
  crudint0=atol(small2);
  if (strstr(scratch,scratch0)==NULL) /* not involving primenet */
   continue;
  if (strstr(small,scratch0)!=NULL)
   {  		/* make small the other network name, crudint the prime node */
   sprintf(small,"%s",small0);
   } else {
   crudint=crudint0;
   }
    /* now find it in the structure */
  match=NO;
  for (nodeindex=0;(nodeindex < maxnode)&&(match==NO);nodeindex++)
   {
   if (node[nodeindex].number[primenet]==crudint)
    {
    match=YES;   /* found it! */
    if ((node[nodeindex].marked==NO)&&(use_marking==YES))
     {  /* don't need to mess with it - inelegant this way, but hey, it works */
     continue;
     } else {
     for (netindex=0;netindex<maxnet;netindex++)
      {
      sprintf(scratch,"%s",netdata[netindex].name);
      strlwr(scratch);
      if (strstr(small,scratch)!=NULL) /* we already have that node! */
       node[nodeindex].number[netindex]=-1;
      }
     }
    } /* didn't match */
   } /* swept all nodes */
  } /* finished eliminating dupes */
 fclose(gread);
 /* now to write flink.ini the way they want it written... */
 sprintf(scratch,"%sFLINK.INI",bbs);
 fsh_open(scratch,"at");
 if (sort==0)  /* by nodes */
  {
  printf("þ Writing FLINK.INI, sorted by nodes.\n");
  for (nodeindex=0;nodeindex<maxnode;nodeindex++)
   {
   if ((node[nodeindex].marked==NO)&&(use_marking==YES))
    continue;
   for (netindex=0;netindex<maxnet;netindex++)
    {
    if (netindex==primenet)
     continue;
    if (node[nodeindex].number[netindex]!=-1)
     fprintf(gwrite,"%s %ld %s %ld\n",netdata[netindex].name,node[nodeindex].number[netindex],netdata[primenet].name,node[nodeindex].number[primenet]);
    }
   }
  } else { /* sorting by networks */
  printf("þ Writing FLINK.INI, sorted by network.\n");
  for (netindex=0;netindex<maxnet;netindex++)
   {
   if (netindex==primenet)
    continue;
   for (nodeindex=0;nodeindex<maxnode;nodeindex++)
    {
    if ((node[nodeindex].marked==NO)&&(use_marking==YES))
     continue;
    if (node[nodeindex].number[netindex]!=-1)
     fprintf(gwrite,"%s %ld %s %ld\n",netdata[netindex].name,node[nodeindex].number[netindex],netdata[primenet].name,node[nodeindex].number[primenet]);
    }
   }
  }
 fclose(gwrite);  /* written flink.ini - now to connect.99 and callout.net! */
 printf("þ Writing CALLOUT.NET and CONNECT.99.\n");
 for (netindex=0;netindex<maxnet;netindex++)
  {
  match=NO;  /*using as scratch*/
  if (netindex==primenet)
   continue;
  if (netdata[netindex].fido==1)  /* does not process fido networks - NOT SURE IF THIS WILL WORK HERE */
   continue;
  sprintf(scratch0,"%sCALLOUT.NET",netdata[netindex].dir);
  gread=fsh_open(scratch0,"rt");
  /* The logic here is that if there is a non-gflink entry in callout.net,
  * it is one that is defined in another connect.* file.  Therefore, adding
  * it again would be bad.  */
  while ((fgets(scratch,254,gread))!=NULL)
   {
   for (nodeindex=0;nodeindex<maxnode;nodeindex++)
    {
    sprintf(small,"@%ld",node[nodeindex].number[netindex]);
    if (strnicmp(scratch,small,strlen(small))==YES)
     node[nodeindex].number[netindex]=-1;
    }
   }
  fclose(gread);
  gwrite=fsh_open(scratch0,"at");  /* callout.net */
  for (nodeindex=0;nodeindex<maxnode;nodeindex++)
   {
   if ((node[nodeindex].marked==NO)&&(use_marking==YES))
    continue;
   if (node[nodeindex].number[netindex]==-1) /* not in this network */
    continue;
   fprintf(gwrite,"\n@%ld        =_+^&       \x22\x22 ",node[nodeindex].number[netindex]);
   match=YES;  /* there was a flink for this network */
   }
  fclose(gwrite);
  if (match==YES)
   {
   sprintf(scratch0,"%sBBSLIST.0",netdata[netindex].dir);
   gwrite=fsh_open(scratch0,"at");
   fprintf(gwrite,"\n99\n");
   fclose(gwrite);
   sprintf(scratch0,"%sCONNECT.99",netdata[netindex].dir);
   gwrite=fsh_open(scratch0,"wt");
   sprintf(small,"@%u",netdata[netindex].ournode);
   fputs(small,gwrite);  /* using fputs so no newline, null, etc */
   crudint=12-(strlen(small));  /* maximum strlen for initial line */
   crudint1=12;  /* keeping track of where we are on the line */
   for (nodeindex=0;nodeindex<maxnode;nodeindex++) /* going through nodelist */
    {
    if ((node[nodeindex].marked==NO)&&(use_marking==YES))
     continue;
    if (node[nodeindex].number[netindex]==-1) /* not in this network */
     continue;
    sprintf(small,"%ld",node[nodeindex].number[netindex]);
    crudint=crudint-(strlen(small));
    for (crudint0=0;crudint0<=crudint;crudint0++)
     fputs(" ",gwrite);
    fputs(small,gwrite);  /* using fputs so no newline, null, etc */
    crudint1=crudint1+7;
    if (crudint1>69)		/* fill up a line?  If so... */
     {
     fprintf(gwrite,"\n");
     crudint=12;
     crudint1=12;
     } else {
     crudint=7;
     }
    }
   fprintf(gwrite,"\n"); /* get our newline and null */
   for (nodeindex=0;nodeindex<maxnode;nodeindex++) /* going through nodelist */
    {                                        /* for the second time */
    if ((node[nodeindex].marked==NO)&&(use_marking==YES))
     continue;
    if (node[nodeindex].number[netindex]==-1) /* not in this network */
     continue;
    crudint=12;
    sprintf(small,"@%ld",node[nodeindex].number[netindex]);
    crudint=crudint-(strlen(small));
    fputs(small,gwrite);  /* using fputs so no newline, null, etc */
    sprintf(small,"%u\n",netdata[netindex].ournode);
    crudint=crudint-(strlen(small));  /* more formatting crap */
    for (crudint0=0;crudint0<=crudint;crudint0++)
     fputs(" ",gwrite);
    fprintf(gwrite,"%s",small);
    }
   fclose(gwrite);  /* finished with connect.99 */
   }
  }
 printf("þ Returning you to your regularly scheduled processing...\n");
 return(YES);
 }

/************************************************************************
* If DE555 exists, copy it to the main BBS directory.
************************************************************************/

int copy_de555(void)
 {
 char *p,s[MAXPATH],s1[MAXPATH];
 short i;
 FILE *read, *write;

 sprintf(s,"%sDE555.EXE",bbs);
 if (access(s,0))
  {
  sprintf(s1,"%sDE555.EXE",netdata[primenet].dir);
  if (access(s1,0))
   {
   printf("You need to install FLINK8 before using GlobalFlink!\n");
   return(NO);
   }
  read = fsh_open(s1,"rb");
  write = fsh_open(s,"wb");
  while(!feof(read))
   {
   fputc(fgetc(read),write);
   }
  fclose(read);
  fclose(write);
  return(YES);
  }
 return(YES);
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
