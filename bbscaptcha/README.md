# BBS Autovalidator

This is designed for WWIV, but can be adapted for other BBSes **IF** there 
is a way for you to externally modify the security values of the user.  

That portion is currently being worked on via either modification or incorporation 
in the WWIV source code.  THIS IS ROUGH AND UNTESTED CODE.  

**UNTESTED CODE**  

**NO GUARANTEES AT ALL**


## Contents
 1. [About](#1-about)
 2. [License](#2-license)
 3. [Prerequisites](#3-prerequisites)
 4. [Installation](#4-installation)
 5. [TODO](#5-todo)

***

## 1. About


## 2. License

This project is licensed under the MIT license. For the full license, see `LICENSE`.

## 3. Prerequisites

* chafa
* python3
* pip
* https://github.com/lepture/captcha

Optional:

* mutt for email verification

* [throttle](https://linux.die.net/man/1/throttle)

If you wish to use `throttle` to emulate connection speeds, just ensure that 
it is present in your `$PATH`. If it is not present, it will simply be skipped.

* ANSI art loading screen for each script

If you do not wish to use the ANSI art files, delete or rename them. (They are 
loaded by default if they exist).

## 4. Installation

install chafa, python, pip, throttle, mutt

exit codes 

0 - normal
99 - error reading CHAIN.TXT
98 - went into creating verify when verify exists
88 - Missing validation code
77 - WRONG validation code

newuserSL=10
valuserSL=20
webaudiopath=/var/www/bbs/validate  <--- should be under main dir
webaudiourl=https://bbs.faithcollapsing.com/validate  <---- URL to report (notice rewrite)
from-email=email-address-verification@faithcollapsing.com <-------- your sysop email (or noreply, I guess?)

if webaudiopath= then no audio will be produced
if from-email= then no option for email will be used

example website mockup using https://strapdownjs.com/ strapdown library with 
nginx rewrite block and apache htaccess rewrite block

## 5. TODO

