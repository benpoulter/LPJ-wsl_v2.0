/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ p h e n o l o g y _ t r e e . c                    **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

#define APHEN_MAX 210
#define APHEN_MIN 10    /*minimum aphen for cold-induced senescence */

Bool phenology_tree(
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
               Real swrad //assume daily swrad=2*par
               )
{
  Pfttree *tree;
  Real dtemp,gddtw;
  #ifdef DAILY
    Real mdtemp;
  #endif

  #ifdef FORKEL_PHENOLOGY
    //declare variables
    Real exp_val,fn_cold,fn_light,fn_water,fn_heat;
  #endif

  tree=pft->data;
  dtemp=temp - getpftpar(pft,gddbase);
  #ifdef DAILY
    mdtemp=temp - getpftpar(pft,gddbase);
  #endif

  gddtw=temp - getpftpar(pft,twmax);
  if(dtemp>0.0)
    pft->gdd+=dtemp;
  tree->gddtw+= (gddtw>0.0) ? gddtw : 0.0;  

  #ifdef FORKEL_PHENOLOGY
    //----------------------------------------------------
    // Growing Season Index (GSI)
    // ..Forkel et al. 2014
    // ..phenology status (phen) modified by limiting fns
    // ..phen [0,1] = f(cold)*f(light)*f(water)*f(heat)
    //----------------------------------------------------
    exp_val = 1/(1+exp(-1*pft->par->slope_cold*(temp-pft->par->base_cold)));
    fn_cold = pft->fn_cold_prev + (exp_val-pft->fn_cold_prev)*pft->par->tau_cold;
  
    exp_val = 1/(1+exp(pft->par->slope_heat*(temp-pft->par->base_heat)));
    fn_heat = pft->fn_heat_prev + (exp_val-pft->fn_heat_prev)*pft->par->tau_heat;
  
    exp_val  = 1/(1+exp(-1*pft->par->slope_light*(swrad-pft->par->base_light)));
    fn_light = pft->fn_light_prev + (exp_val-pft->fn_light_prev)*pft->par->tau_light;
  
    exp_val  = 1/(1+exp(-1*pft->par->slope_water*(pft->wscal*100-pft->par->base_water)));
    fn_water = pft->fn_water_prev + (exp_val-pft->fn_water_prev)*pft->par->tau_water;
  
    //update values for next time step
    pft->fn_cold_prev  = fn_cold;
    pft->fn_heat_prev  = fn_heat;
    pft->fn_light_prev = fn_light;
    pft->fn_water_prev = fn_water;
  
    //Daily phenological status is product of the limiting functions (Forkel 2014)
    pft->phen = fn_cold*fn_heat*fn_light*fn_water;
  #else
    //-------------------------------
    // Default Phenology
    // ..Sitch et al. 2003
    //-------------------------------
    switch(getpftpar(pft,phenology))
    {
      case SUMMERGREEN: 
        if(pft->aphen<APHEN_MAX)
        {  
          pft->phen=pft->gdd*getpftpar(pft,ramp);
         if(pft->phen>1)
           pft->phen=1;
        }
        else
          pft->phen=0.0;
        /* cold-induced senescence;
           works only for linearly interpolated, monthly temperature data! */
    #ifdef MONTHLY
      if(dtemp<0 && pft->aphen>APHEN_MIN)
        pft->phen=pft->gdd=0.0;  
    #elif DAILY
      if(mdtemp<0 && pft->aphen>APHEN_MIN)
        pft->phen=pft->gdd=0.0;
    #endif

        break;
      case RAINGREEN: 
        if(pft->wscal<getpftpar(pft,minwscal)) 
          pft->phen=0.0;
        else
        {
          pft->phen=1.0;
          tree->aphen_raingreen++;
        }
        break;
      case EVERGREEN:
        pft->phen=1;
        break;
    } /* of 'switch' */
  #endif //end of phenology modules

  if ((lat>=0.0 && day==COLDEST_DAY_NHEMISPHERE) ||
      (lat<0.0 && day==COLDEST_DAY_SHEMISPHERE)) 
    pft->aphen=pft->gdd=0.0;

  //update accumulated phenological status
  //aphen only used in default-Sitch phenology module
  pft->aphen+=pft->phen;

  return TRUE;
} /* of 'phenology_tree' */
