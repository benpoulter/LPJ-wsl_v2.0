/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f r e a d p f t . c                                **/
/*******************************************************************/

#include "lpj.h"

Pft *freadpft(FILE *file,const Pftpar pftpar[],Bool swap)
{
  Pft *pft;
  int id;
  if(freadint1(&id,swap,file)!=1)
    return NULL;
  pft=new(Pft);
  pft->par=pftpar+id;
  freadreal1(&pft->wscal,swap,file);
  freadreal1(&pft->wscal_mean,swap,file);
  freadreal1(&pft->aphen,swap,file);
  freadreal1(&pft->phen,swap,file);

  #ifdef FORKEL_PHENOLOGY
  freadreal1(&pft->fn_cold_prev,swap,file);
  freadreal1(&pft->fn_heat_prev,swap,file);
  freadreal1(&pft->fn_light_prev,swap,file);
  freadreal1(&pft->fn_water_prev,swap,file);
  #endif

  pft->par->fread(file,pft,swap);
  freadreal1(&pft->bm_inc,swap,file);
  freadreal1(&pft->nind,swap,file);
  freadreal1(&pft->gdd,swap,file);
  freadreal1(&pft->fpc,swap,file);
  pft->gddbuf=freadbuffer(file,swap);
  return pft;
} /* of 'freadpft' */
