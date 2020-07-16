/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ f r e e l a n d u s e . c                       **/
/*******************************************************************/
#include "lpj.h"

void freelanduse(Landuse *landuse)
{
  fclose(landuse->file_landuse);
  free(landuse->landfrac);
  free(landuse);
} /* of 'freelanduse' */
