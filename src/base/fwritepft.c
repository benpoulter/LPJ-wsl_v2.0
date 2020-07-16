/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f w r i t e p f t . c                              **/
/*******************************************************************/

#include "lpj.h"


Bool fwritepft(FILE *file,const Pft *pft,Bool full)
{
  fwrite1(&pft->par->id,sizeof(int),file);
  fwrite1(&pft->wscal,sizeof(Real),file);
  fwrite1(&pft->wscal_mean,sizeof(Real),file);
  fwrite1(&pft->aphen,sizeof(Real),file);
  fwrite1(&pft->phen,sizeof(Real),file);

  #ifdef FORKEL_PHENOLOGY
  fwrite1(&pft->fn_cold_prev,sizeof(Real),file);
  fwrite1(&pft->fn_heat_prev,sizeof(Real),file);
  fwrite1(&pft->fn_light_prev,sizeof(Real),file);
  fwrite1(&pft->fn_water_prev,sizeof(Real),file);
  #endif

  pft->par->fwrite(file,pft);
  if(full){
    fwrite1(&pft->bm_inc,sizeof(Real),file);
    fwrite1(&pft->nind,sizeof(Real),file);
    fwrite1(&pft->gdd,sizeof(Real),file);
    fwrite1(&pft->fpc,sizeof(Real),file);
    if(fwritebuffer(file,pft->gddbuf))
      return TRUE;
  }
  return FALSE;
} /* of 'fwritepft' */
