# WWIV_Utilities

Please note that since there are multiple scripts, you will want to point your 
RSS reader at [recent commits](https://github.com/uriel1998/WWIV_Utilities/commits/master.atom) 
for this repository so you are aware of additions, changes, and updates

If you're looking for the BASH scripts for networked BBS doors (BBSList and DoorParty),
that's the folder `networked-bbs-doors`, with instructions inside that folder.


## Contents
 1. [Historical](#1-historical)
 2. [ANSI-Linux](#2-ansi-linux)
 3. [networked-bbs-doors](#3-networked-bbs-doors)
 4. [bbs-weather](#4-bbs-weather)
 5. [bbscaptcha](#5-bbscaptcha)


## 1. Historical

![WWIV v4 logo](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/wwiv-4.jpg "WWIV v4 logo")

All "historical" utitilies (e.g. for v4.2.4) are in the `WWIV-v4x` directory and 
described in the README in that directory.  Please note that they may be under 
different licenses than the "modern" (v5+) utilities.
 
--- 

## Modern WWIV (v5+) Utilities

![WWIV v5 logo](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/wwiv-5.jpg "WWIV v5 logo") 
 
## 2. ANSI-Linux

These are some utilities to preview or convert ANSI/ASCII art on a linux system. 
They are also *rough*, and due to the many possible variations of how these 
files may be encoded or what line endings may have, manual tweaking 
of output is practically *guaranteed*.  Works in progress.  You may wish to check out
[showdocs-wombat](http://uriel1998.github.io/showdocs-wombat/) for another (and 
cleaner) implementation. Detailed instructions in the README in the directory.

## 3. networked-bbs-doors

BASH scripts to connect to networked BBS door games - specifically BBSLink and 
Door Party - providing a large userbase and many already-installed doors. Designed
for WWIV 5+ with CHAIN.TXT, but will work with anything that can pass a 
username and/or usernumber.  Detailed instructions in the README in the directory.

## 4. bbs-weather

BASH scripts to operate as a door providing weather and forecast for the 
BBS *and* optionally for any location entered by a user.  Relies on a modified 
version of [weather.sh](https://uriel1998.github.io/weather.sh/).  Detailed 
instructions in the README in the directory.  **coming soon**

## 5. bbscaptcha

BASH & Python3 scripts to provide a comprehensive CAPTCHA autovalidation system 
as a door for WWIV 5+. Can provide ANSI/ASCII captcha, audio captcha, and 
email code validation. Currently awaiting additional code in WWIV codebase 
and/or modification. Detailed 
instructions in the README in the directory. **coming soon**
