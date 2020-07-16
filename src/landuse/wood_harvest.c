/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ w o o d _ h a r v e s t . c                     **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"
#include "grass.h"

//NOTE: product pools set at 60% of harvest, rest (40%) to litter
Real woodharvest_updateStand(Cell *cell,Stand *stand, Real harvest_frac, Real harvest_biomass1, Landuse *landuse,int cellid, Bool subannual)
{
  int p;
  int pos_maxBio;
  Real area, overall_harvest, actual_harvest, potential_harvest, product_pool, nind_demand, PERC2LITTER, PERC2HARVEST;
  Real biomass_sum, temp_Bsum;
  Real harvest_biomass;
  Litter *litter;
  Pft *pft;
  Pfttree *tree;

  PERC2LITTER=0.4;
  PERC2HARVEST=0.6;
  overall_harvest=actual_harvest=product_pool=0.0;
  litter=&stand->soil.litter;
  area=cell->coord.area; //m2
  harvest_biomass=(Real)harvest_biomass1; //redundant from prior data type casting

  //DEVQ: harvest the largest biomass PFTs as first (?) an assumption
  //DEVQ: set priority rules for pft harvesting E.G. max tree-pft gets harvest first, etc.

  //////////////////////////////////////////////////////////////////////////////////////
  //
  // H A R V E S T :  L A R G E S T   T R E E  B I O M A S S  1ST, 2ND...
  //
  //////////////////////////////////////////////////////////////////////////////////////
  //------------------------------------------------------------------------------------
  //  'while' statement important for looping structure on secondary ageClasses  
  //  ..results in harvest_biomass remaining if tile is too small          
  //  ..after which, harvest will occur on next tile to satisfy harvest demand   
  //------------------------------------------------------------------------------------

  #if defined(LUH_V2) || defined(LUH_V2_F)
    //biomass in kg for gridcell, conver to g
    harvest_biomass=harvest_biomass*1000;
  #elif LUH_V1
    //biomass in Mg for gridcell, conver to g
    harvest_biomass=harvest_biomass*1000000;
  #endif

  while(harvest_biomass > epsilon){
    biomass_sum=temp_Bsum=0.0;
    pos_maxBio=-1;

    //find and return treePFT with largest biomass
    foreachpft(pft,p,stand->pftlist){
      if(istree(pft)){
        tree=pft->data;
        temp_Bsum=pft->nind*(tree->ind.heartwood + tree->ind.sapwood);
        if(temp_Bsum > biomass_sum){biomass_sum=temp_Bsum; pos_maxBio=p;}
      }
    }

    //fail-safe, if no trees, then break (no more harvest possible)
    if(pos_maxBio== -1){break;}

    //get pft with largest biomass
    pft=getpft(stand->pftlist,pos_maxBio);
    tree=pft->data;

    //reset potential_harvest for each pft tree
    potential_harvest=0.0;

    //all heartwood goes to satisfy woodHarvest quotas
    potential_harvest=(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*pft->nind*harvest_frac; 

    if(harvest_biomass > (potential_harvest*area)){

      actual_harvest   = potential_harvest;
      product_pool    += PERC2HARVEST*potential_harvest;
      overall_harvest += potential_harvest;

      //update harvest remain
      harvest_biomass-=actual_harvest*area; //decrease harvest demand

      if(subannual){
        //leaf litter (all)
        litter->ag_tree += tree->ind.leaf*pft->nind;
        //sapwood+heartwood as fraction to litter (slash)
        litter->ag_tree += PERC2LITTER*(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*pft->nind;//half to litter, half to harvest
        //roots to litter (all)
        litter->bg      += tree->ind.root*pft->nind;
      }else{
        //leaf litter (all)
        litter->ag_tree_inc += tree->ind.leaf*pft->nind;
        //sapwood+heartwood as fraction to litter (slash)
        litter->ag_tree_inc += PERC2LITTER*(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*pft->nind;//half to litter, half to harvest
        //roots to litter (all)
        litter->bg_inc      += tree->ind.root*pft->nind;
      }

      //delete pft bc all biomsass is either harvested or sent to litter
      delpft(stand->pftlist,pos_maxBio);

    }else{// harvest_biomass <= (potential_harvest)

      //get nind fraction required to satisfy wood harvest demand
      //harvest_biomass is in gC for entire fraction (harvest_frac)
      nind_demand=harvest_biomass/((tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*harvest_frac*area);

      //safety
      if(nind_demand > pft->nind){
        nind_demand=pft->nind;
      }

      actual_harvest   =(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*nind_demand*harvest_frac;
      product_pool    +=PERC2HARVEST*(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*nind_demand*harvest_frac;
      overall_harvest +=actual_harvest;

      //update harvest remain
      harvest_biomass -= actual_harvest*area;

      if(subannual){
        //leaf litter (based on new density)
        litter->ag_tree += tree->ind.leaf*nind_demand;
        //sapwood+heartwood as fraction to litter (slash)
        litter->ag_tree += PERC2LITTER*(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*nind_demand;//half to litter, half to harvest
        //roots to litter (all)
        litter->bg    += tree->ind.root*nind_demand;
      }else{
        //leaf litter (based on new density)
        litter->ag_tree_inc += tree->ind.leaf*nind_demand;
        //sapwood+heartwood as fraction to litter (slash)
        litter->ag_tree_inc += PERC2LITTER*(tree->ind.heartwood+tree->ind.sapwood-tree->ind.debt)*nind_demand;//half to litter, half to harvest
        //roots to litter (all)
        litter->bg_inc    += tree->ind.root*nind_demand;
      }

      //reduce pft fractional cover (nind); biomass is same for remaining fraction
      if(nind_demand >= pft->nind){
        delpft(stand->pftlist,pos_maxBio);
      }else{
        pft->nind -= nind_demand;

        //fpc after pft->nind is updated
        pft->fpc=tree->crownarea*pft->nind*(1.0-exp(-1*pft->par->k_beer*lai_tree(pft)));
        //pft->fpc=fpc_tree(pft);
      }
    }
  }
  //////////// end harvest largest (tree) biomass
  ///////////////////////////////////////////////////////////////
  #if defined(LUH_V2) || defined(LUH_V2_F)
    //product pools = 60% of harvest, rest (40%) to litter
    //assume 25% of roundwood fraction goes to 100yr, 25% of roundwood fraction goes to 10yr pool, and 100% of fuelwood fraction goes to 1yr product pools
    cell->product_100yr +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;//send to product pools as (g C ^m-2)
    cell->product_10yr  +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;
    cell->product_1yr   +=product_pool*landuse->WHarvest_fuelwood_frac[cellid]/PERC2HARVEST;
    cell->product_1yr   +=product_pool*landuse->WHarvest_biofuel_frac[cellid]/PERC2HARVEST;

    #ifdef OUTPUT_BYSTAND
      if(stand->landusetype==PRIMARY){
        //product pools = 60% of harvest, rest (40%) to litter
        //assume 50% of roundwood fraction goes to 100yr and 1yr product pools
        cell->product_100yr_primary +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;//send to product pools as (g C ^m-2)
        cell->product_10yr_primary  +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;
        cell->product_1yr_primary   +=product_pool*landuse->WHarvest_fuelwood_frac[cellid]/PERC2HARVEST;
        cell->product_1yr_primary   +=product_pool*landuse->WHarvest_biofuel_frac[cellid]/PERC2HARVEST;
      }else if(stand->landusetype==SECFOREST){
        //product pools = 60% of harvest, rest (40%) to litter
        //assume 50% of roundwood fraction goes to 100yr and 1yr product pools
        cell->product_100yr_secforest +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;//send to product pools as (g C ^m-2)
        cell->product_10yr_secforest  +=product_pool*landuse->WHarvest_roundwood_frac[cellid]*0.50/PERC2HARVEST;
        cell->product_1yr_secforest   +=product_pool*landuse->WHarvest_fuelwood_frac[cellid]/PERC2HARVEST;
        cell->product_1yr_secforest   +=product_pool*landuse->WHarvest_biofuel_frac[cellid]/PERC2HARVEST;
      }
    #endif
  #elif LUH_V1
    //product pools = 60% of harvest, rest (40%) to litter
    cell->product_100yr +=product_pool*0.07/PERC2HARVEST;//send to product pools as (g C ^m-2)
    cell->product_10yr  +=product_pool*0.20/PERC2HARVEST;
    cell->product_1yr   +=product_pool*0.33/PERC2HARVEST;

    #ifdef OUTPUT_BYSTAND
      if(stand->landusetype==PRIMARY){
        //product pools = 60% of harvest, rest (40%) to litter
        cell->product_100yr_primary +=product_pool*0.07/PERC2HARVEST;//send to product pools as (g C ^m-2)
        cell->product_10yr_primary  +=product_pool*0.20/PERC2HARVEST;
        cell->product_1yr_primary   +=product_pool*0.33/PERC2HARVEST;
      }else if(stand->landusetype==SECFOREST){
        //product pools = 60% of harvest, rest (40%) to litter
        cell->product_100yr_secforest +=product_pool*0.07/PERC2HARVEST;//send to product pools as (g C ^m-2)
        cell->product_10yr_secforest  +=product_pool*0.20/PERC2HARVEST;
        cell->product_1yr_secforest   +=product_pool*0.33/PERC2HARVEST;
      }
    #endif
  #endif

  //track products for C-balance
  cell->woodproducts_temp += product_pool;

  return(harvest_biomass);
}

/*
- called in landusechange_GROSS.c, landusechange_AGECLASS_GROSS.c

- harvests all sapwood and hardwood to meet the harvest products demands from the LUH dataset
- sends harvest products to product pools in fixed fraction (product pools: 1/10/100 yr)
- other fraction gets sent to litter
-   all root sent to bg_litter (includes bg_sapwood and bg_heartwood as fixed fractions of total)
-  leaves sent to 1 year product pool
*/
