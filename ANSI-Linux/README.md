# ANSI-Linux


These are some utilities to preview or convert ANSI/ASCII art on a linux system. 
They are also *rough*, and due to the many possible variations of how these 
files may be encoded or what line endings may have, manual tweaking 
of output is practically *guaranteed*.  Works in progress.  You may wish to check out
[showdocs-wombat](http://uriel1998.github.io/showdocs-wombat/) for another (and 
cleaner) implementation.

### Requires

* [ansilove](https://www.ansilove.org/)
* fzf
* iconv (node-iconv works)
* feh

* ansi-preview-creator.sh : Creates PNG images of all ANSI/ASCII art in a directory using ansilove.  Usage: `ansi-preview-creator.sh /path/to/directory`

* browse-ansi and browse-ansi-preview: Uses fzf to let you browse the ANSI art in a directory and get a preview. Usage: `browse-ansi /path/to/directory`

* Converters: Each indicates the direction of conversion in the filename, e.g. `escape-to-ascii-and-pipe.sh` converts ANSI art (with ANSI escape codes) to ASCII characters with "pipe codes" for coloration.  Please note that the conversion table for characters is optimized for the ones in the menuset I'm putting together, so not all codes are currently implemented.  Usage: `escape-to-ascii-and-pipe.sh < ANSIFILE.ANS > OUTFILE.ASC` or `escape-to-ascii-and-pipe.sh < ANSIFILE.ANS` for output to STDOUT

For example, this main menu went from 
![ANSI example](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/main-ans.png "Example output")

to 

![ASCII PIPE example](https://raw.githubusercontent.com/uriel1998/WWIV_Utilities/master/docs/main-pipe.png "Example output")
