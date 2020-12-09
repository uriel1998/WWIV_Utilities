FakeDSZ
==============

##What is FAKEDSZ, and why do I need it?
WWIV 4.30 will only support Omen Technology's DSZ for network transfers.
Not that there's anything /wrong/ with DSZ;  it's just commercial, and I honestly don't have the traffic to justify buying it to myself.  Further, there's a freeware protocol written by Cutting Edge Computing that does nearly the same thing - just with a different command line format.  So while you could use it before... maybe you can't use it now (assuming the rumors are true).
Besides, there are other programs that expect to find DSZ.EXE - and only DSZ.EXE (or GSZ.EXE, which I am told uses the same commandline format).  Same problem.
Further, CEXYZ has explicit support for FOSSIL communication, which DSZ (apparently) does not.  I presume that would be important to those using the "fossil mod".
So what FAKEDSZ.EXE does is simple:  It takes the DSZ/GSZ input, reformats it to something CEXYZ understands, and then spawns CEXYZ.  That's it.  Simple.

##What do I need to run FAKEDSZ?
A PC capable of running WWIV CEXYZ.EXE (readily available;  due to license agreements, I can't 	incorporate it into this archive.)

##How do I set up FAKEDSZ?
First, configure WWIV (if you need to) to use DSZ.  There are docs on how to do that - RTFM.
Secondly, decide what format of communication you want CEXYZ to use - Fossil, Direct, or Fast.  Direct is "normal", while Fast is interrupt driven, and Fossil... well, duh.  FAKEDSZ defaults to DIRECT.  If you want to enable the fossil routines, place a file (0 byte will do) FAKEDSZ.FOS in the same directory. To enable Fast routines, place FAKEDSZ.FAS in the same directory. If both are there, FOSSIL takes precedence.
And that's about it.  As a note - CEXYZ employs a locked baud rate. Honestly, if you're not using one, you need help.  

###A COMPARISON OF COMMAND LINE PARAMETERS
	DSZ.EXE				  				* CEXYZ.EXE

   port <COM port #>                    * /P<number>
   speed <baud rate>                    * /L<locked baud> 				  * /B<true baud>
   pa<timeout value>                    * <null>
   pB<buffer size>                      * <null>
   pv<verbosity>                        * <null>
   pR1 (delete aborted)                 * <null>
   ha[ndshake] <on|off|both|slow>       * <null>
   -y  (overwrite existing)             * FAKEDSZ provides check
   -r (resume)                          * <null>
   -+ (append)                          * <null>
   [D/d] (carrier drop detect)          * <null>
   <s|r>x  (xmodem)                     * </S|/R>x
   <s|r>b  (ymodem)                     * </S|/R>y
   <s|r>z  (zmodem)                     * </S|/R>z
   <s|r>b -g (Y-g)						* </S|/R>yg
   <s|r>x -c (X-crc 1024)				* </S|/R>x1k
   est[imate] <rate>					* <null>
   restrict   (pathnames)               * CEXYZ provides this as default
   provided by FAKEDSZ			  		* /U<fos|dir|fas>

The last argument is always the file or file list, and passed as is.

Default is /SZ /UDIR /P1 /S2400 /L2400
