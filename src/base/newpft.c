/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ n e w p f t . c                                    **/
/*******************************************************************/

#include "lpj.h"

Pft *newpft(const Pftpar *pftpar /* Parameter of pft */
           )                     /* returns allocated pointer to pft */
{
  Pft *pft;
  pft=new(Pft);
  pft->par=pftpar;
  pft->gddbuf=newbuffer(CLIMBUFSIZE);
  pft->fpc=pft->nind=pft->wscal=pft->aphen=pft->bm_inc=pft->wscal_mean=pft->gdd=pft->phen=0.0;
  pft->par->newpft(pft);
  #ifdef FORKEL_PHENOLOGY
  pft->fn_cold_prev=pft->fn_heat_prev=pft->fn_light_prev=pft->fn_water_prev=0.0;
  #endif
  return pft;
} /* of 'newpft' */
