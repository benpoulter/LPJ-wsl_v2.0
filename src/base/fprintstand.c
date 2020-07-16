/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f p r i n t s t a n d . c                          **/
/*******************************************************************/

#include "lpj.h"

void fprintstand(FILE *file,const Stand *stand)
{
  fprintsoil(file,&stand->soil);
  fprintpftlist(file,stand->pftlist);
} /* of 'fprintstand' */
