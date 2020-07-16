/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / b u f f e r . h                             **/
/*******************************************************************/

#ifndef BUFFER_H
#define BUFFER_H

/* Definitions of datatypes */

typedef struct
{
  int index,n,size;
  Real avg;
  Real *data;
} Buffer;

/* Declaration of functions */

extern Buffer *newbuffer(int);
extern Real updatebuffer(Buffer *,Real); 
extern Bool fwritebuffer(FILE *,const Buffer *);
extern Buffer *freadbuffer(FILE *,Bool);
extern void freebuffer(Buffer *);

#endif
