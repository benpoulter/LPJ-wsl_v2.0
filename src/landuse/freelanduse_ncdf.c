/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ f r e e l a n d u s e _ n c d f . c             **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

void freelanduse_ncdf(Landuse *landuse)
{
  int retval;
  if ((retval = nc_close(landuse->ncid_luc)))
     ERR(retval);
  free(landuse->landfrac);
  #if defined(GROSS_LUC) || defined(NETinGROSS)
    free(landuse->crop_frac);
    free(landuse->crop2pasture_frac);
    free(landuse->crop2SecForest_frac);
    free(landuse->pasture2crop_frac);
    free(landuse->pasture2SecForest_frac);
    free(landuse->SecForest_frac);
    free(landuse->SecForest2crop_frac);
    free(landuse->SecForest2pasture_frac);
    free(landuse->PriForest2crop_frac);
    free(landuse->PriForest2pasture_frac);
    free(landuse->PriForest_frac);
    free(landuse->pasture_frac);
    free(landuse->SummLU_frac);
    free(landuse->WHarvestPrimary_frac);
    free(landuse->WHarvestPrimaryNonForest_frac);
    free(landuse->WHarvestSecForestOld_frac);
    free(landuse->WHarvestSecForestYoung_frac);
    free(landuse->WHarvestSecForestNonForest_frac);
    free(landuse->WHarvestPrimary_biomass);
    free(landuse->WHarvestPrimaryNonForest_biomass);
    free(landuse->WHarvestSecForestOld_biomass);
    free(landuse->WHarvestSecForestYoung_biomass);
    free(landuse->WHarvestSecForestNonForest_biomass);
  #endif
  free(landuse);
} /* of 'freelanduse' */
