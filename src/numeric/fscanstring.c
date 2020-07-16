/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ f s c a n s t r i n g . c                       **/
/*******************************************************************/
#include <stdio.h>
#include "types.h"

Bool fscanstring(FILE *file,char *s)
{
  char c;
  while(fscanf(file,"%c",&c)==1 && c!='\"');
  if(c!='\"')
    return TRUE; 
  for(; fscanf(file,"%c",s)==1 && (*s!='\"');s++);
  if(*s!='\"')
    return TRUE;
  *s='\0';
  return FALSE;
} /* of 'fscanstring' */
