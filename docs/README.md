WWIV_Utilities
==============

A collection of utilities I wrote for the WWIV BBS system.  These are old, and designed for Windows systems (3.1 - XP).  Largely presented for historical and educational reasons.

FAKEDSZ.ZIP - Creates CEXYZ commandlines from DSZ commandlines
SPLTMAIL.ZIP - Splits > 32k text e-mail for PPP project
HTMLLIST.ZIP - Create HTML bbslist
FIW292BG.ZIP - FIW for the PPP project; FTN network e-mail transfer
NETCDR41.ZIP - Network3 preprocessor and GlobalFlink.  VERY useful.
FTSER11.ZIP - Filenet Filelist maintainer/requester

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.

There are several mod files for WWIV 4.24 in /WWIV-424-MODS. These have NOT been tested against the 5.x codebase!

# ANSI Art Utilities

These are new.  :) 

* ansilove 
* fzf
* iconv
* feh

* ansi-preview-creator.sh : Creates PNG images of all ANSI/ASCII art in a directory using ansilove.  Usage: `ansi-preview-creator.sh /path/to/directory`

* browse-ansi and browse-ansi-preview: Uses fzf to let you browse the ANSI art in a directory and get a preview. Usage: `browse-ansi /path/to/directory`

* Converters: Each indicates the direction of conversion in the filename, e.g. `escape-to-ascii-and-pipe.sh` converts ANSI art (with ANSI escape codes) to ASCII characters with "pipe codes" for coloration.  Please note that the conversion table for characters is optimized for the ones in the menuset I'm putting together, so not all codes are currently implemented.  Usage: `escape-to-ascii-and-pipe.sh < ANSIFILE.ANS > OUTFILE.ASC` or `escape-to-ascii-and-pipe.sh < ANSIFILE.ANS` for output to STDOUT

For example, this main menu went from 
![ANSI example](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/main-ans.png "Example output")

to 

![ASCII PIPE example](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/main-pipe.png "Example output")
