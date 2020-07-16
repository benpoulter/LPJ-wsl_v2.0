/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / s w a p . h                                 **/
/*******************************************************************/

#ifndef SWAP_H /* Already included? */
#define SWAP_H

/* Declaration of functions */

extern short int swapshort(short int);
extern int swapint(int);
extern int freadint(int *,int,Bool,FILE *);
extern int freadreal(Real *,int,Bool,FILE *);

/* Definitions of macros */

#define freadint1(data,swap,file) freadint(data,1,swap,file)
#define freadreal1(data,swap,file) freadreal(data,1,swap,file)

#endif
