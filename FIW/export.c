/***********************************************************************
* Deals with inbound acks
***********************************************************************/

void inbound_ack(void)
{
char s[255],s1[MAXPATH],s2[255];
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

    p=strpbrk(s,':');
    *p++;*p++;  /* get past the : and space */
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
    p=strpbrk(s,'*');     /* fileid line is started by *, so can look for */
    do {                  /* that and then work from there, eh? */
     s2[walk]=*p++;
     walk++;
     } while ((*p!=' ')&&(*p!='\0')&&(*p!='\n')&&(*p!='\r'));
    s2[walk++]='\0';  /* s is now the fileid */
    sscanf(small,"%s",p);
    if (strnicmp(small,"n",1)==0)
     ack=FALSE;
    p=strpbrk(s,':');
    *p++;*p++;
    if (*p!='*')  /* is a seg-id */
     {
     segment=TRUE;
     for (walk=0;walk<=3;walk++) /* we always have 3 chars in seg-id */
      small[walk]=*p++;
     small[walk+1]='\0';
     segnum=(atoi(small)-1);
     *p++;
     for (walk=0;walk<=3;walk++) /* we always have 3 chars in seg-id */
      small[walk]=*p++;
     small[walk+1]='\0';
     total=atoi(small);
     }
    if (start!=NULL)
     {
     point=start;
     do {
      prior=point->next;
      if ((strnicmp(point->fileid,s2,(strlen(s2)))==0) && (point->finish==FALSE))
       break;    /* point is pointing to applicable structure */
      point=point->next;
      } while (prior!=NULL);     /* quit when next is NULL */
     if ((strnicmp(point->fileid,s2,(strlen(s2)))==0) && (point->finish==FALSE))
      {  /* double check neccessary w/o using additional variable */
      FILE *read;  /* declaration */

      if (ack==FALSE)
       {
       if (segment==TRUE)
	{
	sprintf(s,"%s\\fiw\\%8ud.%3ud",filedir,point->store,segnum);
	if (!access(s,0))
	 {
	 read=fsh_open(s,"rb");
	 sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,segnum);
	 ctrl=fsh_open(s,"wb");
	 while (!feof(ctrl))
	  fputc(fgetc(read),ctrl);
	 fclose(write);
	 fclose(ctrl);
	 time(&point->time);
	 } else {  /* for some reason the file isn't there!  Cancel the whole */
	 for(walk=0;walk<=point->total;walk++);
	  {
	  sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,walk);
	  unlink(s);          /* if we can't resend the whole file, kill it */
	  }
	 point->finish=TRUE;
	 seatin=(struct seat *)malloc(sizeof(struct seat));
	 sprintf(seatin->from,"%s",to);
	  sprintf(seatin->segid,"%3ud-%3ud%s",segnum,total,s2);
	 send_ack(OTHER);
	 free(seatin);
	 }
	} else {  /* resend entire file */
	for (walk=0;walk<=point->total;walk++)
	 {
	 sprintf(s,"%s\\fiw\\%8ud.%3ud",filedir,point->store,walk);
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
	  sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,walk);
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
	  sprintf(s,"%s\\fiw\\%8ud.%3ud",filedir,point->store,walk);
	  read=fsh_open(s,"rb");
	  sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,walk);
	  ctrl=fsh_open(s,"wb");
	  while (!feof(ctrl))
	   fputc(fgetc(read),ctrl);
	  fclose(write);
	  fclose(ctrl);
	  time(&point->time);
	  }
	 }
	}
       } else {  /* it's an ack */
       if (segment==TRUE)
	{
	sprintf(s,"%s\\fiw\\%8ud.%3ud",filedir,point->store,segnum);
	unlink(s);  /* it got there */
	point->finish=TRUE;
	for(walk=0;walk<=point->total;walk++);
	 {
	 sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,walk);
	 if (!access(s,0))
	  {
	  point->finish=FALSE;
	  break;
	  }
	 }
	} else { /* ack of entire file */
	for(walk=0;walk<=point->total;walk++);
	 {
	 sprintf(s,"%s\\mqueue\\%8ud.%3ud",filedir,point->store,walk);
	 unlink(s);          /* if we can't resend the whole file, kill it */
	 }
	point->finish=TRUE;
	}
       }
      } else {  /* it doesn't exist */
      seatin=(struct seat *)malloc(sizeof(struct seat));
      sprintf(seatin->from,"%s",to);
      if (segment==TRUE)
       sprintf(seatin->segid,"%3ud-%3ud%s",segnum,total,s2);
      else
       sprintf(seatin->segid,"%s",s2);
      send_ack(OTHER);
      free(seatin);
      }
     } else { /* no control structure! */
     seatin=(struct seat *)malloc(sizeof(struct seat));
     sprintf(seatin->from,"%s",to);
     if (segment==TRUE)
      sprintf(seatin->segid,"%3ud-%3ud%s",segnum,total,s2);
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
 write_control();
 point=start;
 do {
  prior=point->next;
  free(point);
  point=prior;
  } while (prior != NULL);
 }