SplitMail
==============

The 32k limitation of WWIV is usually no big deal, but sometimes you've got incoming e-mail (and text at that!) that is huge.
If you're not paying attention, that mail can sit there for a long time waiting for you.
Hence this program.  It will go through your \SPOOL directory and split any large NON-MIME/NON-UUE e-mail, append headers to each split part, and then let the PPP project handle it from there.

From the commandline (or batch file):

    SPLTMAIL <path:\to\spool>

The pathname should NOT have a trailing backslash.	There isn't extensive error checking, so if you manage to mess up this simplistic interface, it's your problem. 

You can run this as often - or rarely - as you like.