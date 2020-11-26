/* mime64 */
/* MIME base64 encoder/decoder by Karl Hahn  hahn@lds.loral.com  3-Aug-94 */
/* modified 30-Sep-94 by Karl Hahn hahn@lds.loral.com: handle multiple
   content */
/* modified 12-Jan-95 by Karl Hahn hahn@lds.loral.com: handle file names
   that are encased in quotes */
/* modified 18-Jan-95 by Karl Hahn hahn@lds.loral.com: prevent complete
   failure if filename in name field matches name of input file */
/* modified 19-Jan-95 by Karl Hahn hahn@lds.loral.com: prevent early exit
   if last decoded character falls on a multiple of 3 -- would cause error
   message and failure to rename output file if rename was necessary */
/* modified 19-Jan-95 by Karl Hahn hahn@lds.loral.com: prevent complete
   failure if a line of text preceding the MIME64 stuff contains no
   non-base64 characters */
/* modified 19-Jan-95 by Karl Hahn hahn@lds.loral.com: fixed command
   line parser to prevent missing a name field preceded by another
   name field */
/* modified 19-Jan-95 by Karl Hahn hahn@lds.loral.com: prevent error
   message at the end of decoding each section.  Terminates output
   file now on a blank line as well as the conditions that did so
   previously */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                  "0123456789+/";

enum TOKENTYPE { NONE, BLANKS, PUNCT, TAG, NAME, CONTENT };

struct TOKEN {
                char *text;
                int  length;
                int  index;
                enum TOKENTYPE type;
             };

int compare_token( struct TOKEN *token, char *text )
{
   int index=0;
   int count;
   int result;
   char blivit1, blivit2;

   count = token->length;

   if ( count > 0 )
   {
      result = 1;
   }
   else
   {
      result = 0;
   }

   while ( (count > 0) && ( result != 0 ) )
   {
      blivit1 = token->text[index++];
      if ( (blivit1 >= 'a' ) && (blivit1 <= 'z') )
      {
         blivit1 -= ' ';
      }

      blivit2 = *text++;
      if ( (blivit2 >= 'a' ) && (blivit2 <= 'z') )
      {
         blivit2 -= ' ';
      }

      if ( blivit1 != blivit2 )
      {
         result = 0;
      }

      count--;
   }

   return result;
}

int ispunct( char blivit )
{
   if ( ( blivit >= 'a' ) && (blivit <= 'z' ) )
   {
      blivit -= ' ';
   }

   if ( ( ( blivit < '0' ) ||
          ( ( blivit > '9' ) && (blivit < 'A') ) ||
          ( blivit > 'Z' ) ) &&
        ( blivit != '-') && (blivit != '/') && (blivit != '.') )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

void fixname( char *name )
{
   while ( *name != '\0' )
   {

     if (ispunct( *name ) )
     {
        *name = '\0';
     }

     name++;
   }
}

void acquire_token( char *line, enum TOKENTYPE type, struct TOKEN *token )
{
   int doneflag=0, startflag=1;
   int index;
   enum TOKENTYPE nextstate=NONE;
   char blivit;

   if (token->type == NONE)
   {
      token->index = 0;
      token->length = 0;
   }

   index = token->index + token->length;

   token->text = 0;

   while ( doneflag == 0 )
   {
      blivit = line[index];
      if ( (blivit >= 'a') && (blivit <= 'z') )
      {
         blivit -= ' ';
      }

      switch (token->type)
      {
         case NONE:
         if ( blivit == ' ')
         {
            index++;
            token->index++;
         }
         else
         {
            token->type = TAG;
            nextstate = TAG;
         }
         break;

         case BLANKS:
         if      ( blivit == ' ')
         {
            index++;
         }
         else if ( ispunct( blivit ) )
         {
            token->type = PUNCT;
            token->index = index;
         }
         else
         {
            token->type = nextstate;
            token->index = index;
         }
         break;

         case PUNCT:
         if      ( blivit <  ' ')
         {
            doneflag = 1;
            token->type = NONE;
            token->index = index;
            token->text = line + index;
            token->length = 0;
         }
         else if ( blivit == ' ' )
         {
            token->type = BLANKS;
            token->index = index;
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

         }
         else if ( ispunct( blivit ) )
         {
            index++;
         }
         else
         {
            if      ( line[ token->index ] == ';' )
            {
               nextstate = NAME;
            }
            else if ( line[ token->index ] == '=' )
            {
               nextstate = CONTENT;
            }

            token->type = nextstate;
            token->index = index;
         }
         break;

         case TAG:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == TAG ) || ( type == NONE ) ) && !startflag)
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case NAME:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;

            if ( blivit != ';' )
            {
               nextstate = CONTENT;
            }
            else
            {
               nextstate = NAME;
            }

            if ( ( ( type == NAME ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;

         case CONTENT:
         if ( ispunct( blivit ) )
         {
            token->length = index - token->index;
            token->text = line + token->index;
            nextstate = NAME;

            if ( ( ( type == CONTENT ) || ( type == NONE ) ) && !startflag )
            {
               doneflag = 1;
            }
            else if (blivit == ' ')
            {
               token->type = BLANKS;
               token->index = index;
            }
            else
            {
               token->type = PUNCT;
               token->index = index;
            }
         }
         else
         {
            index++;
         }
         break;
      }
      startflag = 0;
   }
}

void fputch( char blivit, FILE *f )
{
/*   if (blivit == '\n') fputc( '\r', f );*/
   fputc( blivit, f );
}

int classify_args( int narg,
                   char *rawargs[], char *fileargs[], char *optargs[] )
{
   int index, jndex, kndex;
   char *argptr;

   for ( index = 0, jndex = 0, kndex = 0; index < narg; index++ )
   {
      argptr = rawargs[index];
      if (*argptr == '-')
      {
         argptr++;
         optargs[kndex++] = argptr;
      }
      else
      {
         fileargs[jndex++] = argptr;
      }
   }

   return kndex;
}

int cvt_ascii( unsigned char alpha )
{
   if      ( (alpha >= 'A') && (alpha <= 'Z') ) return (int)(alpha - 'A');
   else if ( (alpha >= 'a') && (alpha <= 'z') )
        return 26 + (int)(alpha - 'a');
   else if ( (alpha >= '0') && (alpha <= '9' ) )
        return 52 + (int)(alpha - '0');
   else if ( alpha == '+' ) return 62;
   else if ( alpha == '/' ) return 63;
   else if ( alpha == '=' ) return -2;
   else                     return -1;
}

char *fileargs[64], *optargs[64];

struct STATE64 {
                  unsigned long int accum;
                  int               shift;
               };


int main( int nargs, char *cargs[] )
{
   int n_options, n_files, index, jndex, shift, save_shift;
   enum { ENCODE, DECODE } whattodo = DECODE;
   int help_flag = 0, replace_flag = 0, perm_replace_flag = 0, quit = 0;
   int cycle_flag = 0;
   FILE *fin, *fout, *dummy;
   unsigned char blivit;
   unsigned long accum, value;
   char buf[80], dumname[80];
   char *cptr, *altptr;
   int decode_state;
   struct TOKEN token;
   int firsttime = 1;
   int skipflag = 0;
   int printmsg = 1;

   n_options = classify_args( nargs, cargs, fileargs, optargs );

   n_files = nargs - n_options;

   if ( n_files < 2 ) help_flag = 1;

   for ( index = 0; index < n_options; index++ )
   {
      if ( ( optargs[index][0] == 'e' ) ||
           ( optargs[index][0] == 'E' ) ) whattodo = ENCODE;
      if ( optargs[index][0] == '?' ) help_flag = 1;
   }

   if ( help_flag )
   {
      printf( "mime64 infile [outfile] [-option] [-option] etc.\n\n"
              "convert between binary and MIME BASE64 format\n\n"
              "        -e       MIME base64 encode (default is decode)\n"
              "        -?       display help message\n\n"
              "if no outfile given, output file replaces infile\n" );
   }

   if ( n_files < 2 ) exit(0);

   if ( whattodo == DECODE )
   {
      fin = fopen( fileargs[1], "r" );
   }
   else
   {
      fin = fopen( fileargs[1], "rb" );
   }

   if ( fin == 0 )
   {
      printf( "%s file not found\n", fileargs[1] );
      exit(-1);
   }

   if ( n_files > 2 )
   {
      if ( whattodo == DECODE )
      {
         sprintf( dumname, "%s", fileargs[2] );
      }
      else
      {
         fout = fopen( fileargs[2], "w" );
      }

      if ( fout == 0 )
      {
         printf( "Couldn't open %s for output\n", fileargs[2] );
      }
   }
   else
   {
      if ( whattodo == DECODE )
      {
         sprintf( dumname, "%s", fileargs[1] );
      }
      else
      {
         fout = fopen( "$$$$$$$$.$$$", "w" );
      }

      replace_flag = 1;
   }


do {
   quit = 0;
   printmsg = 1;

   if ( whattodo == DECODE )
   {
      shift = 0;
      accum = 0;
      decode_state = 0;

      while ( ( !feof( fin ) ) && (quit == 0) )
      {
         fgets( buf, 80, fin );
         if ( feof( fin ) )
         {
            if ( ( dumname[0] != '\0' ) && ( shift != 0 ) )
            {
               printf( "Unexpected end of file encountered in %s\n"
                       "last few bytes may have been lost\n", dumname );
               quit = 1;
               decode_state = 1;
               continue;
            }
            else if ( cycle_flag == 0 )
            {
               quit = 1;
               decode_state = 1;
               continue;
            }
         }
         else
         {
            cycle_flag = 1;

            if ( (decode_state == 1) &&
                 ( (buf[0] == '\n') || (buf[0] < '+') ) )
            {
               quit = 1;

               if ( shift != 0 )
               {
                  printf( "Unexpected end of section in %s\n"
                          "last few bytes may have been lost\n", dumname );
               }

               continue;
            }
         }


         if ( decode_state == 0 )
         {
            for ( index = 0;
                  (buf[index] != '\n') && (buf[index] != '\0') &&
                  (decode_state >= 0);
                  index++ )
            {
               if ( ( (buf[index] >= 'A') && (buf[index] <= 'Z') ) ||
                    ( (buf[index] >= 'a') && (buf[index] <= 'z') ) ||
                    ( (buf[index] >= '0') && (buf[index] <= '9') ) ||
                    (buf[index] == '+') ||
                    (buf[index] == '/') ||
                    (buf[index] == '=') )
               {
                  decode_state = 1;
               }
               else
               {
                  decode_state = -2;
               }
            }

            if ( decode_state <= 0 )
            {

               decode_state = 0;
               token.type = NONE;

               acquire_token( buf, TAG, &token );
               if      ( compare_token( &token, "Content-Type") )
               {
                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "name" ) )
                     {
                        acquire_token( buf, CONTENT, &token );

                        if ( ( replace_flag ) ||
                             ( firsttime == 0 ) )
                        {
                           sscanf( token.text, "%s", dumname );
                           fixname( dumname );

                           if ( strcmpi( dumname, fileargs[1] ) != 0 )
                           {
                              replace_flag = 0;
                           }
                           else
                           {
                              if ( perm_replace_flag )
                              {
                                 printf( 
                                 "More than one output file named %s\n",
                                 dumname );

                                 exit(-1);
                              }
                           }
                        }
                     }
                  } while ( token.type != NONE );
               }
               else if ( compare_token( &token, "Content-transfer-encoding" ) )
               {
                  skipflag = 1;

                  do
                  {
                     acquire_token( buf, NAME, &token );
                     if ( compare_token( &token, "base64" ) )
                     {
                        skipflag = 0;
                     }
                  } while ( token.type != NONE );
               }
               continue;
            }
            else if ( skipflag != 0 )
            {
               continue;
            }
         }

         if ( printmsg )
         {
            if ( skipflag )
            {
               printf( "Section %s not MIME base64\n", dumname );
            }
            else
            {
               printf( "Creating %s\n", dumname );
               if ( strcmpi( dumname, fileargs[1] ) == 0 )
               {
                  replace_flag = 1;
               }

               if ( replace_flag )
               {
                  fout = fopen( "$$$$$$$$.$$$", "wb" );
               }
               else
               {
                  fout = fopen( dumname, "wb" );
               }

               if ( fout == 0 )
               {
                  printf( "Couldn't open %s for output\n", dumname );
               }
            }

            printmsg = 0;
         }

         if ( fout == 0 )
         {
            printf( "No filename given for subsequent section\n" );
            exit(-1);
         }

         if ( feof(fin) )
         {
            quit = 1;
         }

         if ( quit != 0 )
         {
            buf[0] = '\0';
         }

         for ( index = 0; (buf[index] != '\n') && (buf[index] != '\0'); index++)
         {
            value = cvt_ascii( buf[index] );

            if ( value < 64 )
            {
               accum <<= 6;
               shift += 6;
               accum |= value;
               if ( shift >= 8 )
               {
                  shift -= 8;
                  value = accum >> shift;
                  blivit = (unsigned char)value & 0xFFl;
                  fputc( blivit, fout );
               }
            }
            else
            {
               quit = 1;
               break;
            }
         }
      }
   }
   else
   {
      fprintf ( fout,
       "Content-Type: text/plain; charset=US-ASCII; name=%s\n"
       "Content-transfer-encoding: base64\n\n", fileargs[1] );

      shift = 0;
      accum = 0;
      index = 0;
      while ( ( !feof( fin ) ) || (shift != 0) )
      {
         if ( ( !feof( fin ) ) && ( quit == 0 ) )
         {
            blivit = fgetc( fin );

            if ( feof( fin ) )
            {
               quit = 1;
               save_shift = shift;
               blivit = 0;
            }
         }
         else
         {
            quit = 1;
            save_shift = shift;
            blivit = 0;
         }

         if ( (quit == 0) || (shift != 0) )
         {
            value = (unsigned long)blivit;
            accum <<= 8;
            shift += 8;
            accum |= value;
         } /* ENDIF */

         while ( shift >= 6 )
         {
            shift -= 6;
            value = (accum >> shift) & 0x3Fl;
            blivit = alphabet[value];

            buf[index++] = blivit;
            if ( index >= 60 )
            {
               buf[index] = '\0';
               fprintf( fout, "%s\n", buf );
               index = 0;
            }

            if ( quit != 0 )
            {
               shift = 0;
            }
         }
      }

      if      ( save_shift == 2 )
      {
         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }

         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }
      }
      else if ( save_shift == 4 )
      {
         buf[index++] = '=';
         if ( index >= 60 )
         {
            buf[index] = '\0';
            fprintf( fout, "%s\n", buf );
            index = 0;
         }
      }

      if ( index != 0 )
      {
         buf[index] = '\0';
         fprintf( fout, "%s\n", buf );
      }
   }

   fclose( fout );

   if ( replace_flag )
   {
      perm_replace_flag = 1;

      if ( ( whattodo == DECODE ) && ( decode_state <= 0 ) )
      {
         remove( "$$$$$$$$.$$$" );
         printf( "No MIME base64 lines found in %s\n", fileargs[1] );
      }
   }
   else
   {
      if ( ( whattodo == DECODE ) && ( decode_state <= 0 ) )
      {
         remove( fileargs[2] );
         printf( "No MIME base64 lines found in %s\n", fileargs[1] );
      }
   }

   fout = 0;
   firsttime = 0;
   dumname[0] = '\0';
   cycle_flag = 0;

} while ( !feof( fin ) );


if ( perm_replace_flag )
{
   remove( fileargs[1] );
   rename( "$$$$$$$$.$$$", fileargs[1] );
}

fclose( fin );
}
