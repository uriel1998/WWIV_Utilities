/* to make a file_id.diz for Gutenberg etexts */

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

#define WORKING 0
#define FAIL 1
#define SUCCEED 2

void main (void)
{
short a,doing,lread,counter;
char s[255],s1[255],reads[255],name[20],*p;
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT];
FILE *read,*diz;
struct ffblk ffblk;

sprintf(s,"*.TXT");
a=findfirst(s,&ffblk,0);
while (!a)
 {
 sprintf(name,"%s",ffblk.ff_name);
 read=fopen(ffblk.ff_name,"rt");
 doing=WORKING;lread=0;
 do {
  fgets(reads,255,read);
  lread++;
  printf(".");
  if ((reads==NULL)||(lread > 200))
   {
   doing=FAIL;
   continue;
   }
  sprintf(s,"%s",reads);
  strlwr(s);
  if (strstr(s,"this file should be named"))
   {
   doing=SUCCEED;
   continue;
   } else {
   sprintf(s1,"%s",reads); //this will end up holding the first line
   }
  } while (doing==WORKING);
 fclose(read);
 if (doing==SUCCEED)
  {
  printf("%s\n",s1);
  if (strstr(s1,"*"))
   {
   sprintf(s,"%s",s1);
   p=NULL;
   p=s1;
   for (lread=0;(lread <= strlen(s))&&s[lread]!='\0';lread++)
    {
    switch(s[lread]) {
     case '\n':
     case '\r':
     case '*':
      break;
     default:
      *p++=s[lread];
      break;
     }
    }
   *p='\0';
   }
  printf("%s\n",s1);
   if (strstr(s1,"Etext of"))
    {
    sprintf(s,"%s",s1);
    p=strpbrk(s,"f");
    p++;p++;
    }
   if (strstr(s1,"utenberg's"))
    {
    sprintf(s,"%s",s1);
    p=strpbrk(s,"\'");
    p++;p++;p++;
    }
   sprintf(s1,"%s",p);
  printf("%s\n",s1);
  if (doing==SUCCEED)
   {
   diz=fopen("FILE_ID.DIZ","wt");
   fprintf(diz,"%s\n",s1);
   fprintf(diz,"A Project Gutenberg Etext\n");
   fclose(diz);
   fnsplit(name,drive,dir,file,ext);
   printf("Zipping!\n");
   spawnlp(P_WAIT,"C:\\TOOLS\\PKZIP.EXE","PKZIP.EXE",file,name,"*.diz",NULL);
   sprintf(dir,"%s.ZIP",file);
   if (!access(dir,0))
    {
    unlink("FILE_ID.DIZ");
    unlink(name);
    }
   }
  }
 a=findnext(&ffblk);
 }
exit(0);
}