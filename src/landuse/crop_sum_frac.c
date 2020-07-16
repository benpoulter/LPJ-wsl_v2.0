/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ c r o p _ s u m _ f r a c . c                   **/
/*******************************************************************/
#include "lpj.h"

Real crop_sum_frac(int cellid, Landuse *landuse)
{
   int i;
   Real sumfrac=0.0;
  
   for(i=0;i<landuse->nbands;i++) sumfrac+=(getcelllanduse(landuse,cellid,landuse->nbands))[i];
   
   return sumfrac;

} /* of 'crop_sum_frac' */
