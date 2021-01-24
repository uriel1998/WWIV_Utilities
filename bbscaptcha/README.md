# BBS Autovalidator

Example of use, showing CAPTCHA, SOLVA, and what happens if you get it wrong.

![Example of use](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/bbscaptcha/bbscaptcha.gif "Example")

## Contents
 1. [About](#1-about)
 2. [License](#2-license)
 3. [Prerequisites](#3-prerequisites)
 4. [Installation](#4-installation)
 5. [TODO](#5-todo)

***

## 1. About

I saw a simple auto-validation script on another BBS system, and thought it would 
be cool to have for WWIV as well.  Many kudos to **rushfan** for implementing 
the required portions in `wwivutil` to make this happen!

Basically the flow as concieved at the moment is :
* If they have ANSI, use `toilet/figlet` and `boxes` to make an ASCII image that
would be *annoying* to parse programatically (by randomizing the box and
figlet font).

* If they *don't* have ANSI/color, present five different codes, with a text
bit saying which one to enter (using words, not numbers).  Easier to parse,
but still a PITA. I'm calling this a "SOLVA"; yes, it's a silly name.

This script does **NOT** tie a user account to identifiable information, and 
therefore is not a defense against sockpuppetting.  Doing so could require more 
regulatory/privacy safeguards, and is a bit overkill at the moment. Such would be 
better implemented as an IP based solution.  

What it *does* is gives us a mechanism so that users can gain access to the 
normal capabilities of the BBS without having to wait for validation and without 
having to nerf security levels to the point of irrelevance.

All codes are reset after ten minutes, so if a user somehow forgets the code, 
they can wait for that short time and restart the process.

This is designed for WWIV, but can be adapted for other BBSes **IF** there 
is a way for you to externally modify the security values of the user.  You will 
need to either generate CHAIN.TXT or modify the code so that another dropfile is 
usable.

## 2. License

This project is licensed under the MIT license. For the full license, see `LICENSE`.

## 3. Prerequisites

* toilet
* toilet-fonts
* figlet
* boxes

On Debian, these can be installed by `sudo apt install toilet toilet-fonts figlet boxes`

### Optional

* ANSI art loading screen for each script

If you do not wish to use the ANSI art files, delete or rename them. (They are 
loaded by default for users with color/ANSI enabled if they exist).  They will 
NOT be loaded if the user has ANSI off (e.g. for screen readers).

The files are:

* captcha-error.ans
* captcha-success.ans
* captcha-code-enter.ans
* captcha-welcome.ans

Samples are included; they are included under the MIT license of this project. 

## 4. Installation

* Set up `Security Level Configuration` and `Auto-Validation Level Configuration`. 
Instructions on doing this are in the [WWIV documentation](http://docs.wwivbbs.org). 
Pay particular attention to the auto-validation level that you want regular 
users to have.

* Extract/clone the repository in the directory of your choice. It is simplest 
to configure it like any other door/chain, and if you wish to be fancy, add 
instructions or a menu item to another menu for your new users.  The instructions 
below assume you have placed it in `$WWIVDIR/chains/bbscaptcha`. If you do not 
create `$WWIVDIR/chains/bbscaptcha/data`, it will be created by the script. Your 
WWIV user *must* be able to write to this directory.

* Edit `bbscaptcha.ini`. There are only three lines in this file:

```
newuserSL=10
valuserSL=20
ASVlevel=1
WWIVutil_bin=/home/wwiv/wwiv/wwivutil
```

The first two ensure that the user is appropriate for autovalidation; the third 
line defines the auto-validation level to apply upon successful validation. 
The fourth line should reflect the full pathname for `WWIVutil`.

The assumption in these instructions are that new users have a SL of 10 and 
validated users have an SL of 20, and that you wish to use the first Auto Validation 
level for validated users (hence the "1").

* Set up the door/chain in WWIV. %1 is the path to CHAIN.TXT supplied by the BBS.

```
A) Description  : Auto Validation
B) Filename     : bash -c "./chains/bbscaptcha/bbscaptcha.sh -f %1"
C) ACS          : user.sl <= 19
D) ANSI         : Optional
E) Exec Mode    : STDIO
F) Launch From  : BBS Root Directory
G) Local only   : No
H) Multi user   : No
I) Usage        : 0
J) Registered by: AVAILABLE
```

### Optional: Catch exit codes emitted by the script.  They are:

* 0 - normal
* 99 - error reading CHAIN.TXT
* 98 - went into creating verify when verify exists
* 88 - Missing validation code
* 77 - WRONG validation code

## 5. TODO

* Redisplay captcha upon request
