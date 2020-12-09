@echo off
REM remove td.lib if not using TD functions
bcc -d -G -O -ms %1.c td.lib
if not exist %1.obj goto no
:yes
del %1.obj
:no
