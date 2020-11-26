/* UrielSoft's Network1 pre-processor (c) 1997 Steven Saus*/

#include <string.h>
#include <dir.h>
#include <io.h>
#include <process.h>

void main (void)
{

char *p;

p=searchpath("FIW.EXE");
if (!access(p,0))
 spawnlp(P_WAIT,p,"FIW.EXE",NULL);
p=searchpath("ICEPT.EXE");
if (!access(p,0))
 spawnlp(P_WAIT,p,"ICEPT.EXE",NULL);
spawnlp(P_WAIT,"NET1.EXE","NET1.EXE",NULL);
exit(0);
}

