/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ f s c a n r e a l . c                           **/
/*******************************************************************/
#include <stdio.h>
#include "types.h"

Bool fscanreal(FILE *file,Real *val)
{
  double x;
  if(fscanf(file,"%lg",&x)!=1)
    return TRUE;
  *val=x;
  return FALSE;
} /* of 'fscanreal' */
