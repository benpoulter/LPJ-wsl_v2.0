/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ g e t l a g . c                                    **/
/*******************************************************************/
#include "lpj.h"

#define DIFFUS_CONV 0.0864           /*Convert diffusivity from mm2/s to m2/day*/
#define HALF_OMEGA (M_PI/NDAYYEAR)   /*corresponds to omega/2 = pi/365*/

void getlag(Soil *soil,int month)
{
  Real diffus;
  Real DEPTH;
  DEPTH=soil->soildepth[0]*0.0005; //SOILDEPTH calculate depth dynamically
  soil->meanw1*=ndaymonth1[month];
  diffus=(soil->meanw1<0.15)  ?
            (soil->par->tdiff_15-soil->par->tdiff_0)/0.15*soil->meanw1+soil->par->tdiff_0 :
            (soil->par->tdiff_100-soil->par->tdiff_15)/0.85*(soil->meanw1-0.15)+soil->par->tdiff_15;
  soil->meanw1=0;
  soil->alag= DEPTH/sqrt(diffus*DIFFUS_CONV/HALF_OMEGA);
  soil->amp=exp(-soil->alag);
} /* of 'getlag' */
