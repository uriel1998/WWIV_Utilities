@echo off
:top
REM remove spawns.lib if not using Ralf Brown's SPAWNO
bcc -ms -d -G -O %1.c spawns.lib
if not exist %1.obj goto no
:yes
del %1.obj
:no
