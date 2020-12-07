# Networked-BBS-Doors

BASH scripts to connect to networked BBS door games, providing a large userbase and 
many already-installed doors.  Designed for WWIV 5+ with CHAIN.TXT, but 
will work with anything that can pass a username and/or usernumber. 

## Contents
 1. [About](#1-about)
 2. [License](#2-license)
 3. [Prerequisites](#3-prerequisites)
 4. [Installation](#4-installation)
 5. [TODO](#5-todo)

***

## 1. About

Both BBSLink and Door Party provide networked door options, but are optimized for
other BBSes. These scripts provide the same functionality, with a bit of an upgrade.

The crypto/auth portions of the BBSLink script are largely lifted from [Dennis Ayala's 
work on an earlier BBSLink script](http://www.bbslink.net/sysop/linux.php).

## 2. License

This project is licensed under the MIT. For the full license, see `LICENSE`.

## 3. Prerequisites

**Apply and be accepted to DoorParty and/or BBSLink before beginning.**

Most of the program requirements are installed by default on linux systems, but
for DoorParty you will possibly need `rlogin-client`; install it on Debian/Ubuntu
with `sudo apt install rlogin-client`.

Please note that [RLOGIN is a security risk](https://www.ssh.com/ssh/rlogin) if 
exposed to the internet.  Ensure that you have both your router and system firewalls 
so that they will not accept connections from outside the LAN, and you should be 
fine. It is for this reason that it is best practice to have the `doorparty-connector` 
daemon running on the same machine as the BBS to minimize risk.

Optional:

* [throttle](https://linux.die.net/man/1/throttle)
* ANSI art loading screen for each script


## 4. Installation

Place the files where the BBS can find them. While they could technically all 
exist in the same folder, just... just don't.  

For BBSLink, edit the file `BBSLink.ini` to include your information as given 
to you when your application was accepted.  Do not put anything else in that 
file.

If you do not wish to use the ANSI art files, delete or rename them. (They are 
loaded by default if they exist).

If you wish to use `throttle` to emulate connection speeds, just ensure that 
it is present in your `$PATH`. If it is not present, it will simply be skipped.

### Door Party

The installation instructions were used on Debian 10 with systemd.

1. Install Doorparty Connector
* Choose where you want the connector daemon to live, for example `/opt/doorparty-connector`
* `sudo mkdir -p /opt/doorparty-connector`
* `cd /opt/doorparty-connector`
* `sudo wget https://raw.githubusercontent.com/echicken/dpc2/master/doorparty-connector.ini`
* `sudo wget https://github.com/echicken/dpc2/blob/master/build/linux_x64/doorparty-connector`
* `sudo wget https://raw.githubusercontent.com/echicken/dpc2/master/doorparty-connector.service`
* If you did not use /opt/doorparty-connector as your install path, edit the paths in `doorparty-connector.service`
* `sudo ln -s $PWD/doorparty-connector.service /etc/systemd/system/doorparty-connector.service`
* `sudo systemctl daemon-reload`
* `sudo systemctl enable doorparty-connector`
* `sudo systemctl start doorparty-connector`
    
2. Place the `doorparty` directory alongside your other doors. These instructions 
assume you have placed it in `/home/wwiv/chains/doorparty`.

3. Set up the door. For WWIV, the entries to change are:

`A) Description  : Door Party`
`B) Filename     : bash -c "./chains/doorparty/doorparty-link.sh -f %1 -s AA"`
`F) Exec Mode:     STDIO`

**IMPORTANT** - the variable after `-s` is your system tag, as assigned by DoorParty.

Additional command-line options for `doorparty-link.sh` are:

* -h = show this help
* -u = manually specify an user **name**, overrides any in CHAIN.TXT
* -s = Your SystemTag from DoorParty. Do NOT use brackets.
* -f = The path to CHAIN.TXT
* -a = The path to optional ANSI file splash screen

If `doorparty.ans` is in the script directory and no other file is specified with 
the `-a` switch, it will be shown automatically. You can specify another filename 
by calling it like so: 

`bash -c "./chains/doorparty/doorparty-link.sh -f %1 -s AA -a /path/to/file.ANS"`

If you are using another BBS system that can pass the username, use the `-u` 
switch followed by the username variable.  You can also use this from the 
command-line yourself, e.g. `doorparty-link.sh -u USERNAME -s AA`

At present there is not a way to specify a specific door at runtime from Door Party.

### BBSLink

The installation instructions were used on Debian 10 with systemd.

1. Place the `bbslink` directory alongside your other doors. These instructions 
assume you have placed it in `/home/wwiv/chains/bbslink`.

2. Edit `bbslink.ini` and put in the syscode, authcode, and schemecode where 
appropriate.

3. Set up the door. For WWIV, the entries to change are:

`A) Description  : BBSLink`
`B) Filename     : bash -c "./chains/bbslink/bbslink.sh -f %1"`
`F) Exec Mode:     STDIO`

Additional command-line options for `bbslink.sh` are:

* -h = show this help
* -u = manually specify an user **number**, overrides any in CHAIN.TXT
* -f = The path to CHAIN.TXT
* -a = The path to optional ANSI file splash screen
* -d = The door code to use (defaults to the menu)

If you are using another BBS system that can pass the user **number**, use the `-u` 
switch followed by the username variable.  You can also use this from the 
command-line yourself, e.g. `bbslink.sh -u USERNUMBER`

If `bbslink.ans` is in the script directory and no other file is specified with 
the `-a` switch, it will be shown automatically. You can specify another filename 
by calling it like so: 

`bash -c "./chains/bbslink/bbslink.sh -f %1 -a /path/to/file.ANS"`

This feature may be of particular use if you use BBSLink as a "direct door" so 
that there are different "loading" screens for each door.

If you wish to use the script as a "direct door" link instead of going to the 
menu, set up a new door entry, but modify the filename like so:

`B) Filename     : bash -c "./chains/bbslink/bbslink.sh -f %1 -d DOORCODE"`

A list of valid door codes is found on the [BBSLink website](http://www.bbslink.net/sysop/).

## 5. TODO

* Determine if there's a way to use direct door links with Door Party
* Standardize how I capitalize/write "Door Party".  
* If there are direct door links possible with Door Party, create script to let 
users enter the door directly 
