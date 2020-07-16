/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ p o p e n . c                                   **/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "popen.h"

FILE *pt_popen(const char *cmd, const char *mode)
{
#if defined(USE_CPP) && defined(_WIN32)
  char *cmd2;
  cmd2=(char *)malloc(strlen(cmd)+strlen(" >tmp.conf")+1);
  strcpy(cmd2,cmd);
  strcat(cmd2," >tmp.conf");
  system(cmd2);
  return fopen("tmp.conf","rb");
#elif defined(USE_CPP)  /* !WIN32  */
  return popen(cmd,mode);
#else
  return fopen(cmd,mode);
#endif /* !WIN32  */
} /* of 'pt_open' */

void pt_pclose(FILE *file)
{
#if defined(_WIN32) || !defined(USE_CPP)
  fclose(file);
#else
  pclose(file);
#endif
} /* of 'pt_close* */
