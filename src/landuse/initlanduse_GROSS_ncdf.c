/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ i n i t l a n d u s e _ G R O S S _ n c d f . c **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/*
 *     Function initlanduse
 *
 *     Initializes landuse datatype 
 *
 */


//------------------------------------
// hyper-functions
// ..called by initlanduse_GROSS_ncdf
//------------------------------------
int init_luh_v1(Landuse *landuse, Config *config){
  //LU fractions
  //transition fractions
  //wood harvest fraction and biomass harvested
  //total LU fraction (as a check)

  //-------------------------------------
  //   L A N D U S E  F R A C T I O N S
  //  ..detailed crop fractions (not used)
  //    --primary
  //    --secondary
  //    --crop
  //    --pasture
  //-------------------------------------
  if((landuse->PriForest_frac=newvec(Real,config->ngridcell))==NULL){
      printallocerr("initlanduse","SecForest_frac");
      free(landuse);
      return 0;
  }
  if((landuse->SecForest_frac=newvec(Real,config->ngridcell))==NULL){
      printallocerr("initlanduse","SecForest_frac");
      free(landuse);
      return 0;
  }
  if((landuse->crop_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","crop_frac");
    free(landuse);
    return 0;
  }
  if((landuse->pasture_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","pasture_frac");
    free(landuse);
    return 0;
  }

  //-------------------------------------
  //   T R A N S I T I O N S  (frac)
  //  ..detailed crop fractions (not used)
  //  ..detailed urban transitions (not used)
  //  DEVQ: is there a way to not use part of grid-cell (urban)?
  //    : ,or will this affect carbon/water balance
  //    --primary
  //    --secondary
  //    --crop
  //    --pasture
  //-------------------------------------
  //...........................
  // deforest transitions
  //...........................
  if((landuse->PriForest2crop_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","PriForest2crop_frac");
    free(landuse);
    return 0;
  }
  if((landuse->PriForest2pasture_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","PriForest2pasture_frac");
    free(landuse);
    return 0;
  }
  if((landuse->SecForest2crop_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","SecForest2crop_frac");
    free(landuse);
    return 0;
  }
  if((landuse->SecForest2pasture_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","SecForest2pasture_frac");
    free(landuse);
    return 0;
  }
  //.......................................
  // crop/pasture abandonment transitions
  //   (create secondary)
  //.......................................
  if((landuse->crop2SecForest_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","crop2SecForest_frac");
    free(landuse);
    return 0;
  }
  if((landuse->pasture2SecForest_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","pasture2SecForest_frac");
    free(landuse);
    return 0;
  }
  //.......................................
  // crop/pasture inter-conversions
  //   (not used)
  //.......................................
  if((landuse->crop2pasture_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","crop2pasture_frac");
    free(landuse);
    return 0;
  }
  if((landuse->pasture2crop_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","pasture2crop_frac");
    free(landuse);
    return 0;
  }

  //.......................................
  // wood harvest details
  // ..harvest fractional transitions
  //   ,goes to young secondary
  // ..biomass harvested (kg)
  //.......................................
  if((landuse->WHarvestPrimary_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestPrimary_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestPrimaryNonForest_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestPrimaryNonForest_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestOld_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestOld_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestYoung_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestYoung_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestNonForest_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestNonForest_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestPrimary_biomass=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestPrimary_biomass");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestPrimaryNonForest_biomass=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestPrimaryNonForest_biomass");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestOld_biomass=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestOld_biomass");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestYoung_biomass=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestYoung_biomass");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvestSecForestNonForest_biomass=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvestSecForestNonForest_biomass");
    free(landuse);
    return 0;
  }

  //................................
  // total grid cell fraction
  // ..check for unity
  // ..eq sum of all LU fractions
  //................................
  if((landuse->SummLU_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","SummLU_frac");
    free(landuse);
    return 0;
  }
  ////////////////////////////////

  return 1; //successful return
}//end init_luh_v1

int init_luh_v2(Landuse *landuse, Config *config){

  int check_return;
  //-------------------------------------
  //  L A N D    M A N A G E M E N T
  //  ..detailed crop data (not used)
  //    -- wood harvest product fractions
  //      (fraction of biomass)
  //       ..annual by country
  //     ..biofuel fraction is zero
  //       ,until > yr 2015 (RCPs)
  //-------------------------------------

  //initialize variables for LUHv1
  check_return=init_luh_v1(landuse,config);

  //add variables to use from LUHv2
  if((landuse->WHarvest_biofuel_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvest_biofuel_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvest_fuelwood_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvest_fuelwood_frac");
    free(landuse);
    return 0;
  }
  if((landuse->WHarvest_roundwood_frac=newvec(Real,config->ngridcell))==NULL){
    printallocerr("initlanduse","WHarvest_roundwood_frac");
    free(landuse);
    return 0;
  }
  ////////////////////////////////

  return check_return; //successful return
}//end init_luh_v2


//------------------------------------
// main function
// ..calls hyper-functions
//------------------------------------
Landuse *initlanduse_GROSS_ncdf(Config *config)
{
  int check_return=0;
  int i;
  Header header;
  Landuse *landuse;
  landuse=new(Landuse);

  if((landuse->ncid_luc=openinputfile_ncdf(&header,&landuse->swap,config->landuse_filename,config,
                 LPJ_LANDUSE_HEADER,LPJ_LANDUSE_VERSION))==0)
  {
    free(landuse);
    return NULL;
  }

  /* GROSS_LUC not setup for use with SYNMAP or HYDE
  #ifdef SYNMAP
    landuse->firstyear=1700; //For SYNMAP
  #endif

  #ifdef HYDE
    landuse->firstyear=1850; //For Hydev3.1 ncdf input, where crop and pasture and summed
  #endif
  */

  /////////////////////////////////////////////////////////////////////////////////
  //  GROSS_LUC module only works with the Land Use Harmonization (LUH) dataset  //
  /////////////////////////////////////////////////////////////////////////////////
  #ifdef LUH_V1
  landuse->firstyear=1500; //For LUH input, with primary, crop, pasture, secondary
  #elif LUH_V2
  landuse->firstyear=850;
  #elif LUH_V2_F
  landuse->firstyear=2015;
  #else
    printf("\n ERROR:  GROSS_LUC module only works with the LUH dataset \n");
  #endif


  //DEVQ: consider change nbands to include CROPgeneral/PASTURE LUtypes for deforest routine in SecForest module
  landuse->nbands=24;  //ADDED THIS to fix to keep to full 24 bands - importnat in get land use and land use module
  landuse->size=config->totalgridcell*landuse->nbands*sizeof(short);
  landuse->n=config->ngridcell*landuse->nbands;
  landuse->offset=0;
  landuse->intercrop=TRUE;

  if((landuse->landfrac=newvec(Real,landuse->n))==NULL)
  {
    printallocerr("initlanduse","nfrac");
    free(landuse);
    return NULL;
  }
  //initialize vector to zero
  for(i=0; i < landuse->n; i++){landuse->landfrac[i]=0.0;}

  //------------------------------------------------------
  //  initialize vectors to hold
  //    the LUH transition data
  //
  // -- use the LUHv1 data holders
  // -- add WH product fractions if LUHv2
  // -- DEVQ: consider separate vars for LUHv1 and LUHv2
  //------------------------------------------------------
  #if defined(LUH_V2) || defined(LUH_V2_F)
    check_return=init_luh_v2(landuse, config);
  #elif LUH_V1
    check_return=init_luh_v1(landuse, config);
  #else //compiler check
    fail("..ERROR specify flag for the landusechange driver data {LUHv1,LUHv2}");
  #endif
  //check for null data
  if(check_return==0){return NULL;}

  return landuse;
} /* of 'initlanduse' */

/*
- called in lpj()
- opens the landuse input file (see also cfts26_lu2clm.c)
- sets the landuse variables (see also landuse.h)
*/
