/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f i r e _ p r o b . c                              **/
/*******************************************************************/
#include "lpj.h"

//#define minfuel 200.0 /* fuel threshold to carry fire (gC/m2) */
#define minfuel 100.0 //100 is carbon

Real fire_prob(const Litter *litter,Real fire_sum)
{
  Real fire_index,sm,fire_frac;
  fire_index=fire_sum/NDAYYEAR;
  sm=fire_index-1;
  fire_frac=fire_index*exp(sm/(0.45*sm*sm*sm+2.83*sm*sm+2.96*sm+1.04));
  #ifdef SAFE
    if (fire_frac>1.0)
      fail("fire: probability of fire=%g >1.0",fire_frac);
  #endif

  //new litter scheme
  return (fire_frac<0.001 ||(litter->ag_tree+litter->ag_grass+litter->ag_tree_inc+litter->ag_grass_inc<minfuel)) ?
            0.001 : fire_frac;
} /* of 'fire_prob' */
