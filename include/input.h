/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / i n p u t . h                               **/
/*******************************************************************/

#ifndef INPUT_H
#define INPUT_H

/* Definition of datatypes */

/* Declaration of functions */

extern FILE *openinputfile(Header *, Bool *, const char *, Config *, const char *, int);
extern int openinputfile_ncdf(Header *, Bool *, const char *, Config *, const char *, int);  

#endif
