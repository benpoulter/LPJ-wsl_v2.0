/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ f a i l . c                                     **/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "types.h"

void fail(const char *msg,...)
{
  va_list ap;
  va_start(ap,msg);
  vfprintf(stderr,msg,ap);
  fprintf(stderr,", program terminated unsuccessfully.\n");
  va_end(ap);
  exit(EXIT_FAILURE);
} /* of 'fail' */
