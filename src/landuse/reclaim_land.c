/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ r e c l a i m _ l a n d . c                     **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "permafrost.h"

void copysoil(Stand *stand1, Stand *stand2)
{
  int l;
  /*Soilpar*/
  stand2->soil.par=stand1->soil.par;
  /*Pool*/
  stand2->soil.cpool.slow=stand1->soil.cpool.slow;
  stand2->soil.cpool.fast=stand1->soil.cpool.fast;
  stand2->soil.k_mean.slow=stand1->soil.k_mean.slow;
  stand2->soil.k_mean.fast=stand1->soil.k_mean.fast;
  /*Litter*/
  stand2->soil.litter.ag_tree=stand1->soil.litter.ag_tree;
  stand2->soil.litter.ag_grass=stand1->soil.litter.ag_grass;
  stand2->soil.litter.bg=stand1->soil.litter.bg;
  stand2->soil.litter.ag_tree_inc=stand1->soil.litter.ag_tree_inc;
  stand2->soil.litter.ag_grass_inc=stand1->soil.litter.ag_grass_inc;
  stand2->soil.litter.bg_inc=stand1->soil.litter.bg_inc;
  stand2->soil.litter.ag_tree_pinc=stand1->soil.litter.ag_tree_pinc;
  stand2->soil.litter.ag_grass_pinc=stand1->soil.litter.ag_grass_pinc;
  stand2->soil.litter.bg_pinc=stand1->soil.litter.bg_pinc;
  /*Soil*/
  foreachsoillayer(l)
    stand2->soil.w[l]=stand1->soil.w[l];
  stand2->soil.w_evap=stand1->soil.w_evap;
  stand2->soil.alag=stand1->soil.alag;
  stand2->soil.amp=stand1->soil.amp;
  stand2->soil.meanw1=stand1->soil.meanw1;
  stand2->soil.snowpack=stand1->soil.snowpack;
  stand2->soil.decomp_litter_mean=stand1->soil.decomp_litter_mean;
  /* Soil moisture */
  stand2->soil.soildepth[0]=stand1->soil.soildepth[0];
  stand2->soil.soildepth[1]=stand1->soil.soildepth[1];
  stand2->soil.whcs[0]=stand1->soil.whcs[0];
  stand2->soil.whcs[1]=stand1->soil.whcs[1];

#ifdef PERMAFROST
  stand2->permafrost.all_melted    =stand1->permafrost.all_melted;
  stand2->permafrost.snow_days_prev=stand1->permafrost.snow_days_prev;
  stand2->permafrost.snow_days     =stand1->permafrost.snow_days;
  stand2->permafrost.swe           =stand1->permafrost.swe;
  stand2->permafrost.snowdens      =stand1->permafrost.snowdens;
  stand2->permafrost.snowdepth     =stand1->permafrost.snowdepth;
  for(l=0;l<NLAYERS;l++){
    stand2->permafrost.frozen_day[l]  =stand1->permafrost.frozen_day[l];
    stand2->permafrost.Dz[l]          =stand1->permafrost.Dz[l];
    stand2->permafrost.Di[l]          =stand1->permafrost.Di[l];
    stand2->permafrost.Tsoil_365[l]   =stand1->permafrost.Tsoil_365[l];
    stand2->permafrost.Tsoil[l]       =stand1->permafrost.Tsoil[l];
    stand2->permafrost.T_old[l]       =stand1->permafrost.T_old[l];
    stand2->permafrost.Tsoil_prev[l]  =stand1->permafrost.Tsoil_prev[l];
    stand2->permafrost.Fice[l]        =stand1->permafrost.Fice[l];
    stand2->permafrost.Fice_365[l]    =stand1->permafrost.Fice_365[l];
    stand2->permafrost.Fice_prev[l]   =stand1->permafrost.Fice_prev[l];
    stand2->permafrost.ice_frac[l]    =stand1->permafrost.ice_frac[l];
    stand2->permafrost.Fwater[l]      =stand1->permafrost.Fwater[l];
    stand2->permafrost.Fwater_365[l]  =stand1->permafrost.Fwater_365[l];
    stand2->permafrost.Fwater_prev[l] =stand1->permafrost.Fwater_prev[l];
    stand2->permafrost.Fpwp[l]        =stand1->permafrost.Fpwp[l];
    stand2->permafrost.Fair[l]        =stand1->permafrost.Fair[l];
    stand2->permafrost.wtot[l]        =stand1->permafrost.wtot[l];
  }

  for(l=0;l<PAD_LAYERS;l++){
    stand2->permafrost.pad_dz[l]      =stand1->permafrost.pad_dz[l];
    stand2->permafrost.pad_temp[l]    =stand1->permafrost.pad_temp[l];
  }

  stand2->permafrost.thawdepth        =stand1->permafrost.thawdepth;
  stand2->permafrost.thaw_top         =stand1->permafrost.thaw_top;
  stand2->permafrost.day_count        =stand1->permafrost.day_count;
  stand2->permafrost.perma_count      =stand1->permafrost.perma_count;
  stand2->permafrost.peathydr         =stand1->permafrost.peathydr;
  stand2->permafrost.ice_frac1        =stand1->permafrost.ice_frac1;
  stand2->permafrost.ice_frac2        =stand1->permafrost.ice_frac2;
#endif
  
  return;
} /* of 'copysoil' */

void remove_vegetation_copy(Stand *stand1,Stand *stand2,Real *harvestdef,Real *deforestProduct) 
{
  int p;
  Pft *pft;

  foreachpft(pft,p,stand1->pftlist)
    litter_update_deforest(&stand2->soil.litter,pft,pft->nind,harvestdef,deforestProduct,TRUE); 
  return;

  /* There should be no vegetation on stand2, only the soil carbon was copied to stand2
   * therefore delpft is not necessary here */

}/*of 'remove_vegetation_copy'*/

void reclaim_land(Stand *stand1, Stand *stand2,Real *harvestdef, Real *deforestProduct) 
{
  stand2->fire_sum=stand1->fire_sum;
  copysoil(stand1,stand2);
  remove_vegetation_copy(stand1,stand2,harvestdef,deforestProduct);
  return;
}/* of 'reclaim_land'*/


/*
- called in cultivate(), deforest(), regrowth(), landexpansion(), 
  grasslandreduction()
- copies the values of the variables of one stand to another stand except of the 
  pftlist
- calls local function copysoil()
  -> copysoil() copies the values of all variables in struct Soil (see soil.h)
- calls local function remove_vegetation_crop()
  -> remove_vegetation_crop() removes the pfts from the new stand by updating the 
     litter pools of the new stand
*/
