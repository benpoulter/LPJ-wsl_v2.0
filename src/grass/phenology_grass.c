/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ p h e n o l o g y _ g r a s s . c                 **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

Bool phenology_grass(
            Pft *pft,
            int npft,
            Real temp,
            Real lat,
            Real country_laimax[],
            int day, 
            Real daylength,
            Bool irrigation,
            int landusetype,
            Real mtemp, 
            Real swrad  //default assume swrad=2*par
            )
{
  Real dtemp;
  #ifdef FORKEL_PHENOLOGY
  //declare variables
  Real exp_val,fn_cold,fn_light,fn_water,fn_heat;
  #endif

  dtemp=temp - getpftpar(pft,gddbase);
  if(dtemp>0.0)
    pft->gdd+=dtemp;

  #ifdef FORKEL_PHENOLOGY
    //----------------------------------------------------
    // Growing Season Index (GSI)
    // ..Forkel et al. 2014
    // ..phenology status (phen) modified by limiting fns
    // ..phen [0,1] = f(cold)*f(light)*f(water)*f(heat)
    //----------------------------------------------------
    exp_val = 1/(1+exp(-1*pft->par->slope_cold*(temp-pft->par->base_cold)));
    fn_cold = pft->fn_cold_prev + (exp_val-pft->fn_cold_prev)*pft->par->tau_cold;
  
    exp_val  = 1/(1+exp(-1*pft->par->slope_light*(swrad-pft->par->base_light)));
    fn_light = pft->fn_light_prev + (exp_val-pft->fn_light_prev)*pft->par->tau_light;
  
    exp_val  = 1/(1+exp(-1*pft->par->slope_water*(pft->wscal*100-pft->par->base_water)));
    fn_water = pft->fn_water_prev + (exp_val-pft->fn_water_prev)*pft->par->tau_water;
  
    exp_val  = 1/(1+exp(pft->par->slope_heat*(temp-pft->par->base_heat)));
    fn_heat  = pft->fn_heat_prev + (exp_val-pft->fn_heat_prev)*pft->par->tau_heat;
  
    //update values for next time step
    pft->fn_cold_prev  = fn_cold;
    pft->fn_heat_prev  = fn_heat;
    pft->fn_light_prev = fn_light;
    pft->fn_water_prev = fn_water;
  
    //Daily phenological status is product of the limiting functions (Forkel 2014)
    pft->phen = fn_cold*fn_light*fn_water*fn_heat;
  #else
    //-------------------------------
    // Default Phenology
    // ..Sitch et al. 2003
    //-------------------------------
    pft->phen=pft->gdd*pft->par->ramp;
  #endif //end of phenology modules

  if(pft->phen>1)
    pft->phen=1;
  if ((lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) || (lat<0.0 && day==COLDEST_DAY_SHEMISPHERE)) 
    pft->aphen=pft->gdd=0.0;
  pft->aphen+=pft->phen;
  if(pft->phen==1 && pft->bm_inc>=100 && landusetype==GRASSLAND)
    return FALSE;
  
  return TRUE;
} /* of 'phenology_grass' */
