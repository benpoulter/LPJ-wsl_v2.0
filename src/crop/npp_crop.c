/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ n p p _ c r o p . c                                **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

#define k 0.0548 

Real npp_crop(Pft *pft,Real gtemp_air,Real gtemp_soil,Real assim,Bool *negbm,Real hi, Real *bgaresp)
{
  Pftcrop *crop;
  const Pftcroppar *par;
  Real npp;
  Real roresp,soresp,presp,gresp;
  crop=pft->data;
  par=pft->par->data;

  roresp=pft->par->respcoeff*k*crop->ind.root/par->cn_ratio.root*gtemp_soil;
  soresp=pft->par->respcoeff*k*crop->ind.so/par->cn_ratio.so*gtemp_air;
  presp=pft->par->respcoeff*k*crop->ind.pool/par->cn_ratio.pool*gtemp_air;
  /* pools can't be negative any more as LAI growth and SO allocation is limited by NPP now */
  /*presp=(crop->ind.pool<0.0) ? 0.0 : pft->par->respcoeff*k*crop->ind.pool
    /par->cn_ratio.pool*gtemp_air;*/
  gresp=(assim-roresp-soresp-presp)*0.25;
  *bgaresp=roresp;
  if(gresp<0.0) gresp=0.0;
  npp=assim-roresp-soresp-presp-gresp;
  if(npp<0 && (crop->ind.pool+npp)<0){
    *negbm=TRUE;
    crop->ind.pool+=npp;
    pft->bm_inc+=npp; 
  } 
  else allocation_daily_crop(pft,npp,hi);
  
  return npp;
} /* of 'npp_crop' */

/*
- called in update_daily():
  -> cell->output.mnpp[month]+=npp(pft,pft->phen,gtemp_air,gtemp_soil,(gpp-rd),
            &negbm);
- this function returns the computed daily npp
  -> calculation of roresp (root respiration)
  -> calculation of soresp (storage organ respiration)
  -> calculation of presp (pool (stem + reserve) respiration)
  -> calculation of gresp (growth respiration)
  -> calculation of daily npp (net primary production)

- calls the function allocation_daily_crop() if npp>0 und carbon mass pool+npp>0
  -> allocation_daily_crop() needs the determined npp of npp_crop() 
- sets negbm to TRUE if the biomass gets negative
  -> cropstand will be deleted in update_daily()
*/
