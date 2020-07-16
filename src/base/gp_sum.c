/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ g p _ s u m . c                                    **/
/*******************************************************************/

#include "lpj.h"

#define lambda_opt 0.8

Real gp_sum(const Pftlist pftlist, /* Pft list */
            Real co2,              /* atmospheric CO2 concentration (ppm) */
            Real temp,             /* temperature (deg C) */
            Real par,              /* photosynthetic active radiation flux */
            Real daylength,        /* daylength (h) */
            Real *gp_stand_leafon, /* pot. canopy conduct.at full leaf cover */
            Real *gp_pft,          /* pot. canopy conductance for PFTs & CFTs*/
            Real *fpc              /* total fpc of all Pfts */
           )
{
  int p;
  const Pft *pft;
  Real agd,adtmm,gp,gp_stand,rd,fpc_total;
  *gp_stand_leafon=gp=*fpc=gp_stand=0;
  if(daylength<1e-20){
    foreachpft(pft,p,pftlist){
      gp_pft[getpftpar(pft,id)]=0;
    }
    return 0;
  }
  foreachpft(pft,p,pftlist){
    //OZONE - added notes to track units and processes related to conductance
    //Calculate population-wide photosynthesis with pft->fpc as FPAR (equivalent to maximum annual FPAR)
    adtmm=photosynthesis(&agd,&rd,pft->par->path,lambda_opt,
                         temp_stress(pft->par,temp,daylength),ppm2Pa(co2),
                         temp,par,pft->fpc,daylength,pft->par->alphaa);

    //Calculate population-wide PFT gp (include gmin scaled to grid cell for PFT)
    gp=(1.6*adtmm/(ppm2bar(co2)*(1.0-lambda_opt)*hour2sec(daylength)))+
                    pft->par->gmin*pft->fpc;  //ozone gp is in units of mm s-1

    //Scale gp for the PFT population based on phenological status for that day
    gp_pft[getpftpar(pft,id)]=gp*pft->phen;

    //Calculate gp stand by summing phenology scaled gp
    gp_stand+=gp*pft->phen;

    //Calculate population-wide gp stand from max FPAR (FPC)
    *gp_stand_leafon+=gp;
                        
    *fpc+=pft->fpc;
  }//..end pfts
  fpc_total=*fpc;

  //Scale gp_stand to be mean stand gp for PFTs by dividing by fpc_total
  *gp_stand_leafon= (gp_stand<1e-20 || fpc_total<1e-20) ? 0 : *gp_stand_leafon/fpc_total;
  return (gp_stand<1e-20 || fpc_total<1e-20) ? 0 : gp_stand/fpc_total;
} /* of 'gp_sum' */

