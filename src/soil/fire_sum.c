/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f i r e _ s u m . c                                **/
/*******************************************************************/
#include "lpj.h"

#define flam_tree 0.3
#define flam_grass 0.3

Real fire_sum(const Litter *litter,Real w_surf)
{
  Real moistfactor,litter_sum;
  moistfactor=litter_sum=0.0;

  litter_sum=litter->ag_tree+litter->ag_grass;
  if(litter_sum==0)
    return 0;
  moistfactor=(flam_tree*litter->ag_tree+flam_grass*litter->ag_grass)/
              litter_sum;
  return (moistfactor>0) ? 
            exp(-M_PI*(w_surf/moistfactor)*(w_surf/moistfactor)) : 0;
} /* of 'fire_sum' */
