/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / l a n d u s e . h                           **/
/*******************************************************************/

#ifndef LANDUSE_H /* Already included? */
#define LANDUSE_H

#include "soil.h"
#include "pft.h"

/* Definitions of datatypes */

typedef struct
{
  int n,nbands,offset,size,firstyear, ncid_luc;
  Bool swap,intercrop;
  FILE *file_landuse;
  Real *landfrac;
  Real *cropfrac;
  
  Real *crop_frac;  //this differs from 'cropfrac' (crop+pasture), whereas 'crop_frac' is only crop
  Real *crop2pasture_frac;
  Real *crop2SecForest_frac;
  Real *pasture2crop_frac;
  Real *pasture2SecForest_frac;
  Real *SecForest_frac;
  Real *SecForest2crop_frac;
  Real *SecForest2pasture_frac;
  Real *PriForest2crop_frac;
  Real *PriForest2pasture_frac;
  Real *PriForest_frac;
  Real *pasture_frac;
  Real *SummLU_frac;
  Real *WHarvestPrimary_frac;
  Real *WHarvestPrimaryNonForest_frac;
  Real *WHarvestSecForestOld_frac;
  Real *WHarvestSecForestYoung_frac;
  Real *WHarvestSecForestNonForest_frac;

  Real *WHarvestPrimary_biomass;
  Real *WHarvestPrimaryNonForest_biomass;
  Real *WHarvestSecForestOld_biomass;
  Real *WHarvestSecForestYoung_biomass;
  Real *WHarvestSecForestNonForest_biomass;

  //for LUHv2
  Real *WHarvest_roundwood_frac;
  Real *WHarvest_fuelwood_frac;
  Real *WHarvest_biofuel_frac;
} Landuse;

/* Definitions of macros */
#define mixpool(pool1,pool2,frac1,frac2) pool1=(pool1*frac1+pool2*frac2)/(frac1+frac2)
#define getcelllanduse(landuse,cell,nbands) landuse->landfrac+(cell)*nbands

/* Declaration of functions */
extern Real litter_update_deforest(Litter *, Pft *,Real,Real *,Real *,Bool);
extern Landuse *initlanduse_GROSS_ncdf(Config *);
extern Landuse *initlanduse_NET_ncdf(Config *);
extern Landuse *initlanduse(Config *);

extern Bool getlanduse(Landuse *,int,int,int);
extern Bool getlanduse_NET_ncdf(Landuse *,int,int,int, Real[], Real[]); 
extern Bool getlanduse_GROSS_ncdf(Config, Landuse *,int,int,int, Real[], Real[]);

extern Real crop_sum_frac(int, Landuse *);
extern void freelanduse(Landuse *);
extern void freelanduse_ncdf(Landuse *);
#endif
