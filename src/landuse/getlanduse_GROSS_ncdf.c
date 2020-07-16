/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ g e t l a n d u s e _ G R O S S _ n c d f . c   **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

//------------------------------------
// hyper-functions
// ..called by getlanduse_GROSS_ncdf
//------------------------------------
void get_LUH_V1(Landuse *landuse,int year, int ncell,int ncft, int latMatch[], int lonMatch[]){

  //....................
  // declare vars
  //....................
  int i,j, retval;

  #ifdef NETinGROSS
    Real temp_net, temp_net_remain=0.0;
  #endif

  float *vec_cropFrac;
  float *vec_crop2pasture;
  float *vec_crop2SecForest;
  float *vec_pasture2crop;
  float *vec_pasture2SecForest;
  float *vec_SecForestFrac;
  float *vec_SecForest2crop;
  float *vec_SecForest2pasture;
  float *vec_PriForest2crop;
  float *vec_PriForest2pasture;
  float *vec_PriForestFrac;
  float *vec_pastureFrac;
  float *vec_SummLUFrac;
  float *vec_WHarvest_primary;
  float *vec_WHarvest_primaryNonForest;
  float *vec_WHarvest_secforestOld;
  float *vec_WHarvest_secforestYoung;
  float *vec_WHarvest_secforestNonForest;

  float *vec_WHarvest_biomass_primary;
  float *vec_WHarvest_biomass_primaryNonForest;
  float *vec_WHarvest_biomass_secforestOld;
  float *vec_WHarvest_biomass_secforestYoung;
  float *vec_WHarvest_biomass_secforestNonForest;

  char *varname_cropFrac     = "gcrop";
  char *varname_crop2pasture = "gflcp";
  char *varname_crop2SecForest = "gflcs";
  char *varname_pasture2crop = "gflpc";
  char *varname_pasture2SecForest = "gflps";
  char *varname_SecForestFrac = "gsecd";
  char *varname_SecForest2crop = "gflsc";
  char *varname_SecForest2pasture = "gflsp";
  char *varname_PriForest2crop = "gflvc";
  char *varname_PriForest2pasture = "gflvp";
  char *varname_PriForestFrac = "gothr";
  char *varname_pastureFrac = "gpast";
  char *varname_SummLUFrac = "gsumm";
  char *varname_WHarvest_primary = "gfvh1";
  char *varname_WHarvest_primaryNonForest = "gfvh2";
  char *varname_WHarvest_secforestOld = "gfsh1";
  char *varname_WHarvest_secforestYoung = "gfsh2";
  char *varname_WHarvest_secforestNonForest = "gfsh3";
  char *varname_WHarvest_biomass_primary = "gvbh1";
  char *varname_WHarvest_biomass_primaryNonForest = "gvbh2";
  char *varname_WHarvest_biomass_secforestOld = "gsbh1";
  char *varname_WHarvest_biomass_secforestYoung = "gsbh2";
  char *varname_WHarvest_biomass_secforestNonForest = "gsbh3";

  int varid_cropFrac;
  int varid_crop2pasture;
  int varid_crop2SecForest;
  int varid_pasture2crop;
  int varid_pasture2SecForest;
  int varid_SecForestFrac;
  int varid_SecForest2crop;
  int varid_SecForest2pasture;
  int varid_PriForest2crop;
  int varid_PriForest2pasture;
  int varid_PriForestFrac;
  int varid_pastureFrac;
  int varid_SummLUFrac;
  int varid_WHarvest_primary;
  int varid_WHarvest_primaryNonForest;
  int varid_WHarvest_secforestOld;
  int varid_WHarvest_secforestYoung;
  int varid_WHarvest_secforestNonForest;
  int varid_WHarvest_biomass_primary;
  int varid_WHarvest_biomass_primaryNonForest;
  int varid_WHarvest_biomass_secforestOld;
  int varid_WHarvest_biomass_secforestYoung;
  int varid_WHarvest_biomass_secforestNonForest;

  //Create vector to hold data;
  vec_cropFrac=newvec(float, 1);
  vec_crop2pasture=newvec(float,1);
  vec_crop2SecForest=newvec(float,1);
  vec_pasture2crop=newvec(float,1);
  vec_pasture2SecForest=newvec(float,1);
  vec_SecForestFrac=newvec(float,1);
  vec_SecForest2crop=newvec(float,1);
  vec_SecForest2pasture=newvec(float,1);
  vec_PriForest2crop=newvec(float,1);
  vec_PriForest2pasture=newvec(float,1);
  vec_PriForestFrac=newvec(float,1);
  vec_pastureFrac=newvec(float,1);
  vec_SummLUFrac=newvec(float,1);
  vec_WHarvest_primary=newvec(float,1);
  vec_WHarvest_primaryNonForest=newvec(float,1);
  vec_WHarvest_secforestOld=newvec(float,1);
  vec_WHarvest_secforestYoung=newvec(float,1);
  vec_WHarvest_secforestNonForest=newvec(float,1);
  vec_WHarvest_biomass_primary=newvec(float,1);
  vec_WHarvest_biomass_primaryNonForest=newvec(float,1);
  vec_WHarvest_biomass_secforestOld=newvec(float,1);
  vec_WHarvest_biomass_secforestYoung=newvec(float,1);
  vec_WHarvest_biomass_secforestNonForest=newvec(float,1);

  check(vec_cropFrac);
  check(vec_crop2pasture);
  check(vec_crop2SecForest);
  check(vec_pasture2crop);
  check(vec_pasture2SecForest);
  check(vec_SecForestFrac);
  check(vec_SecForest2crop);
  check(vec_SecForest2pasture);
  check(vec_PriForest2crop);
  check(vec_PriForest2pasture);
  check(vec_PriForestFrac);
  check(vec_pastureFrac);
  check(vec_SummLUFrac);
  check(vec_WHarvest_primary);
  check(vec_WHarvest_primaryNonForest);
  check(vec_WHarvest_secforestOld);
  check(vec_WHarvest_secforestYoung);
  check(vec_WHarvest_secforestNonForest);
  check(vec_WHarvest_biomass_primary);
  check(vec_WHarvest_biomass_primaryNonForest);
  check(vec_WHarvest_biomass_secforestOld);
  check(vec_WHarvest_biomass_secforestYoung);
  check(vec_WHarvest_biomass_secforestNonForest);

  //....................
  // get data
  //....................
  //Set up NCDF read arrays
  //initialize start/count arrays
  size_t start[3], count[3];

  //////////////////////////////////////
  //Loop through cells and read 1 year
  for(j=0; j < ncell; ++j){
     //count identifies to 1 band
     count[0]=1;
     count[1]=1;
     count[2]=1;

     //Skip start
     start[0]=year; //Landuse is read each year; year is passed to getlanduse, updated from index 0
     start[1]=latMatch[j];
     start[2]=lonMatch[j];

     //return the integer ID for the variable name
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_cropFrac, &varid_cropFrac))){
       fprintf(stderr,"Error reading LUC variable from file, crop frac varname should be '%s';\n",varname_cropFrac);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop2pasture, &varid_crop2pasture))){
       fprintf(stderr,"Error reading LUC variable from file, crop2pasture varname should be '%s';\n",varname_crop2pasture);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop2SecForest, &varid_crop2SecForest))){
       fprintf(stderr,"Error reading LUC variable from file, crop2secforest varname should be '%s';\n",varname_crop2SecForest);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture2crop, &varid_pasture2crop))){
       fprintf(stderr,"Error reading LUC variable from file, pasture2crop varname should be '%s';\n",varname_pasture2crop);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture2SecForest, &varid_pasture2SecForest))){
       fprintf(stderr,"Error reading LUC variable from file, pasture2secforest varname should be '%s';\n",varname_pasture2SecForest);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_SecForestFrac, &varid_SecForestFrac))){
       fprintf(stderr,"Error reading LUC variable from file, secforest frac varname should be '%s';\n",varname_SecForestFrac);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_SecForest2crop, &varid_SecForest2crop))){
       fprintf(stderr,"Error reading LUC variable from file, secforest2crop varname should be '%s';\n",varname_SecForest2crop);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_SecForest2pasture, &varid_SecForest2pasture))){
       fprintf(stderr,"Error reading LUC variable from file, secforest2pasture varname should be '%s';\n",varname_SecForest2pasture);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_PriForest2crop, &varid_PriForest2crop))){
       fprintf(stderr,"Error reading LUC variable from file, priforest2crop varname should be '%s';\n",varname_PriForest2crop);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_PriForest2pasture, &varid_PriForest2pasture))){
       fprintf(stderr,"Error reading LUC variable from file, priforest2pasture varname should be '%s';\n",varname_PriForest2pasture);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_PriForestFrac, &varid_PriForestFrac))){
       fprintf(stderr,"Error reading LUC variable from file, priforest frac varname should be '%s';\n",varname_PriForestFrac);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pastureFrac, &varid_pastureFrac))){
       fprintf(stderr,"Error reading LUC variable from file, pasture frac varname should be '%s';\n",varname_pastureFrac);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_SummLUFrac, &varid_SummLUFrac))){
       fprintf(stderr,"Error reading LUC variable from file, sum of all LU fractions varname should be '%s';\n",varname_SummLUFrac);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_primary, &varid_WHarvest_primary))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest frac on primary varname should be '%s';\n",
         varname_WHarvest_primary);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_primaryNonForest, &varid_WHarvest_primaryNonForest))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest frac on primary non-forest varname should be '%s';\n",
         varname_WHarvest_primaryNonForest);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestOld, &varid_WHarvest_secforestOld))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest frac on old secondary varname should be '%s';\n",
         varname_WHarvest_secforestOld);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestYoung, &varid_WHarvest_secforestYoung))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest frac on young secondary varname should be '%s';\n",
         varname_WHarvest_secforestYoung);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestNonForest, &varid_WHarvest_secforestNonForest))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest frac on secondary non-forest varname should be '%s';\n",
         varname_WHarvest_secforestNonForest);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_primary, &varid_WHarvest_biomass_primary))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest biomass on primary varname should be '%s';\n",
         varname_WHarvest_biomass_primary);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_primaryNonForest, &varid_WHarvest_biomass_primaryNonForest))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest biomass on primary non-forest varname should be '%s';\n",
         varname_WHarvest_biomass_primaryNonForest);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestOld, &varid_WHarvest_biomass_secforestOld))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest biomass on old secondary varname should be '%s';\n",
         varname_WHarvest_biomass_secforestOld);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestYoung, &varid_WHarvest_biomass_secforestYoung))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest biomass on young secondary varname should be '%s';\n",
         varname_WHarvest_biomass_secforestYoung);
       ERR(retval);
     }
     if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestNonForest, &varid_WHarvest_biomass_secforestNonForest))){
       fprintf(stderr,"Error reading LUC variable from file, woodharvest biomass on secondary non-forest varname should be '%s';\n",
         varname_WHarvest_biomass_secforestNonForest);
       ERR(retval);
     }

     //retreive the data for the specified variable (int ID, from above)
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_cropFrac, start, count, vec_cropFrac)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop2pasture, start, count, vec_crop2pasture)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop2SecForest, start, count, vec_crop2SecForest)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture2crop, start, count, vec_pasture2crop)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture2SecForest, start, count, vec_pasture2SecForest)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_SecForestFrac, start, count, vec_SecForestFrac)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_SecForest2crop, start, count, vec_SecForest2crop)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_SecForest2pasture, start, count, vec_SecForest2pasture)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_PriForest2crop, start, count, vec_PriForest2crop)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_PriForest2pasture, start, count, vec_PriForest2pasture)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_PriForestFrac, start, count, vec_PriForestFrac)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pastureFrac, start, count, vec_pastureFrac)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_SummLUFrac, start, count, vec_SummLUFrac)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_primary, start, count, vec_WHarvest_primary)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_primaryNonForest, start, count, vec_WHarvest_primaryNonForest)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestOld, start, count, vec_WHarvest_secforestOld)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestYoung, start, count, vec_WHarvest_secforestYoung)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestNonForest, start, count, vec_WHarvest_secforestNonForest)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_primary, start, count, vec_WHarvest_biomass_primary)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_primaryNonForest, start, count, vec_WHarvest_biomass_primaryNonForest)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestOld, start, count, vec_WHarvest_biomass_secforestOld)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestYoung, start, count, vec_WHarvest_biomass_secforestYoung)))
           ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestNonForest, start, count, vec_WHarvest_biomass_secforestNonForest)))
           ERR(retval);

    //check for no data value handling because nodata values (-9999) may be present in data
    for(i=0; i< 1; i++){//loop over 1 band
      if(vec_cropFrac[i] < 0 || vec_cropFrac[i] > 1e19)
        vec_cropFrac[i]=0;
      if(vec_crop2pasture[i] < 0 || vec_crop2pasture[i] > 1e19)
        vec_crop2pasture[i]=0;
      if(vec_crop2SecForest[i] < 0 || vec_crop2SecForest[i] > 1e19)
        vec_crop2SecForest[i]=0;
      if(vec_pasture2crop[i] < 0 || vec_pasture2crop[i] > 1e19)
        vec_pasture2crop[i]=0;
      if(vec_pasture2SecForest[i] < 0 || vec_pasture2SecForest[i] > 1e19)
        vec_pasture2SecForest[i]=0;
      if(vec_SecForestFrac[i] < 0 || vec_SecForestFrac[i] > 1e19)
        vec_SecForestFrac[i]=0;
      if(vec_SecForest2crop[i] < 0 || vec_SecForest2crop[i] > 1e19)
        vec_SecForest2crop[i]=0;
      if(vec_SecForest2pasture[i] < 0 || vec_SecForest2pasture[i] > 1e19)
        vec_SecForest2pasture[i]=0;
      if(vec_PriForest2crop[i] < 0 || vec_PriForest2crop[i] > 1e19)
        vec_PriForest2crop[i]=0;
      if(vec_PriForest2pasture[i] < 0 || vec_PriForest2pasture[i] > 1e19)
        vec_PriForest2pasture[i]=0;
      if(vec_PriForestFrac[i] < 0 || vec_PriForestFrac[i] > 1e19)
        vec_PriForestFrac[i]=0;
      if(vec_pastureFrac[i] < 0 || vec_pastureFrac[i] > 1e19)
        vec_pastureFrac[i]=0;
      if(vec_SummLUFrac[i] < 0 || vec_SummLUFrac[i] > 1e19)
        vec_SummLUFrac[i]=0;
      if(vec_WHarvest_primary[i] < 0 || vec_WHarvest_primary[i] > 1e19)
        vec_WHarvest_primary[i]=0;
      if(vec_WHarvest_primaryNonForest[i] < 0 || vec_WHarvest_primaryNonForest[i] > 1e19)
        vec_WHarvest_primaryNonForest[i]=0;
      if(vec_WHarvest_secforestOld[i] < 0 || vec_WHarvest_secforestOld[i] > 1e19)
        vec_WHarvest_secforestOld[i]=0;
      if(vec_WHarvest_secforestYoung[i] < 0 || vec_WHarvest_secforestYoung[i] > 1e19)
        vec_WHarvest_secforestYoung[i]=0;
      if(vec_WHarvest_secforestNonForest[i] < 0 || vec_WHarvest_secforestNonForest[i] > 1e19)
        vec_WHarvest_secforestNonForest[i]=0;
      if(vec_WHarvest_biomass_primary[i] < 0 || vec_WHarvest_biomass_primary[i] > 1e19)
        vec_WHarvest_biomass_primary[i]=0;
      if(vec_WHarvest_biomass_primaryNonForest[i] < 0 || vec_WHarvest_biomass_primaryNonForest[i] > 1e19)
        vec_WHarvest_biomass_primaryNonForest[i]=0;
      if(vec_WHarvest_biomass_secforestOld[i] < 0 || vec_WHarvest_biomass_secforestOld[i] > 1e19)
        vec_WHarvest_biomass_secforestOld[i]=0;
      if(vec_WHarvest_biomass_secforestYoung[i] < 0 || vec_WHarvest_biomass_secforestYoung[i] > 1e19)
        vec_WHarvest_biomass_secforestYoung[i]=0;
      if(vec_WHarvest_biomass_secforestNonForest[i] < 0 || vec_WHarvest_biomass_secforestNonForest[i] > 1e19)
        vec_WHarvest_biomass_secforestNonForest[i]=0;

      // ..convert to int, round, then convert to float to avoid floating-point errors
      // ..fraction of ag (crop + pasture) converted to secondary forests in cell
      landuse->crop_frac[j]=round(vec_cropFrac[i]*10000000)/10000000;
      landuse->crop2pasture_frac[j]=round(vec_crop2pasture[i]*10000000)/10000000;
      landuse->crop2SecForest_frac[j]=round(vec_crop2SecForest[i]*10000000)/10000000;
      landuse->pasture2crop_frac[j]=round(vec_pasture2crop[i]*10000000)/10000000;
      landuse->pasture2SecForest_frac[j]=round(vec_pasture2SecForest[i]*10000000)/10000000;
      landuse->SecForest_frac[j]=round(vec_SecForestFrac[i]*10000000)/10000000;
      landuse->SecForest2crop_frac[j]=round(vec_SecForest2crop[i]*10000000)/10000000;
      landuse->SecForest2pasture_frac[j]=round(vec_SecForest2pasture[i]*10000000)/10000000;
      landuse->PriForest2crop_frac[j]=round(vec_PriForest2crop[i]*10000000)/10000000;
      landuse->PriForest2pasture_frac[j]=round(vec_PriForest2pasture[i]*10000000)/10000000;
      landuse->PriForest_frac[j]=round(vec_PriForestFrac[i]*10000000)/10000000;
      landuse->pasture_frac[j]=round(vec_pastureFrac[i]*10000000)/10000000;
      landuse->SummLU_frac[j]=round(vec_SummLUFrac[i]*10000000)/10000000;
      landuse->WHarvestPrimary_frac[j]=round(vec_WHarvest_primary[i]*10000000)/10000000;
      landuse->WHarvestPrimaryNonForest_frac[j]=round(vec_WHarvest_primaryNonForest[i]*10000000)/10000000;
      landuse->WHarvestSecForestOld_frac[j]=round(vec_WHarvest_secforestOld[i]*10000000)/10000000;
      landuse->WHarvestSecForestYoung_frac[j]=round(vec_WHarvest_secforestYoung[i]*10000000)/10000000;
      landuse->WHarvestSecForestNonForest_frac[j]=round(vec_WHarvest_secforestNonForest[i]*10000000)/10000000;

      //NOTE: LUH_V1 harvest biomass data is in MgC per grid-cell, convert to grams in ./src/landuse/wood_harvest.c
      landuse->WHarvestPrimary_biomass[j]=round(vec_WHarvest_biomass_primary[i]*10000000)/10000000;
      landuse->WHarvestPrimaryNonForest_biomass[j]=round(vec_WHarvest_biomass_primaryNonForest[i]*10000000)/10000000;
      landuse->WHarvestSecForestOld_biomass[j]=round(vec_WHarvest_biomass_secforestOld[i]*10000000)/10000000;
      landuse->WHarvestSecForestYoung_biomass[j]=round(vec_WHarvest_biomass_secforestYoung[i]*10000000)/10000000;
      landuse->WHarvestSecForestNonForest_biomass[j]=round(vec_WHarvest_biomass_secforestNonForest[i]*10000000)/10000000;

      //get total landuse fraction
      landuse->SummLU_frac[j]=landuse->crop_frac[j]+landuse->pasture_frac[j]+ landuse->PriForest_frac[j]+landuse->SecForest_frac[j];

      #ifdef GROSS_ONESTAND
        landuse->SecForest_frac[j] += landuse->PriForest_frac[j];
        landuse->SecForest2pasture_frac[j] += landuse->PriForest2pasture_frac[j];
        landuse->SecForest2crop_frac[j] += landuse->PriForest2crop_frac[j];

        landuse->WHarvestSecForestOld_frac[j] += landuse->WHarvestPrimary_frac[j];
        landuse->WHarvestSecForestNonForest_frac[j] += landuse->WHarvestPrimaryNonForest_frac[j];
        landuse->WHarvestSecForestOld_biomass[j] += landuse->WHarvestPrimary_biomass[j];
        landuse->WHarvestSecForestNonForest_biomass[j] += landuse->WHarvestPrimaryNonForest_biomass[j];

        //set primary to zero
        landuse->PriForest_frac[j]=landuse->PriForest2pasture_frac[j]=landuse->PriForest2crop_frac[j]=0;
        landuse->WHarvestPrimary_frac[j]=landuse->WHarvestPrimaryNonForest_frac[j]=0;
        landuse->WHarvestPrimary_biomass[j]=landuse->WHarvestPrimaryNonForest_biomass[j]=0;

        #if defined(NETinGROSS)
          //simplify managed lands, crop & pasture are treated the same (Sept 2016)
          landuse->SecForest2crop_frac[j]+=landuse->SecForest2pasture_frac[j];
          landuse->crop2SecForest_frac[j] += landuse->pasture2SecForest_frac[j];
          //zero-out pasture fractions
          landuse->SecForest2pasture_frac[j]=landuse->pasture2SecForest_frac[j]=0;

          //determine if (i) net abandonment or (ii) deforestation or (iii) no change
          temp_net= landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

          if(temp_net > 0){
            //net deforestation
            landuse->SecForest2crop_frac[j]=temp_net;
            landuse->crop2SecForest_frac[j]=0;
          }else if(temp_net < 0){
            //net abandonment
            landuse->SecForest2crop_frac[j]=0;
            landuse->crop2SecForest_frac[j]=temp_net;
          }else{
            //no change
            landuse->SecForest2crop_frac[j]=0;
            landuse->crop2SecForest_frac[j]=0;
          }
        #endif
      #elif defined(NETinGROSS)
        /////////////////////////////////////////////////////////////////
        // // N E T - L U C  in  G R O S S framework
        /////////////////////////////////////////////////////////////////
        //--------------------------------
        //.. two alternate net implmentations
        //.. (1) calculate net first between deforest primary and abandon mgLand
        //.. (2) calculate net first between deforest secondary and abandon mgLand (default)
        //.. key for both is to use primary forest_nonForest harvest Fraction to maintain Pri-Sec fractions
        //--------------------------------
        //update stand fractions so that only crop_frac is used (not pasture_frac)
        landuse->crop_frac[j]+=landuse->pasture_frac[j];
        landuse->pasture_frac[j]=0.0;

        //MgLand inter-conversion
        //Null in this LPJ version bc crop/pasture is grassland(same)
        landuse->crop2pasture_frac[j]=0.0;
        landuse->pasture2crop_frac[j]=0.0;

        //LU conversions, simplify
        landuse->crop2SecForest_frac[j]+=landuse->pasture2SecForest_frac[j];
        landuse->pasture2SecForest_frac[j]=0.0;
        landuse->PriForest2crop_frac[j]+=landuse->PriForest2pasture_frac[j];
        landuse->PriForest2pasture_frac[j]=0.0;
        landuse->SecForest2crop_frac[j]+=landuse->SecForest2pasture_frac[j];
        landuse->SecForest2pasture_frac[j]=0.0;

        #if defined(NOWDHARVEST)
          //Update WoodHarvest Transitions (primary-> secondary)
          landuse->WHarvestPrimary_frac[j]+=landuse->WHarvestPrimaryNonForest_frac[j];
          //no WoodHarvest Transitions for Secondary Forests
          landuse->WHarvestSecForestOld_frac[j]=landuse->WHarvestSecForestYoung_frac[j]=landuse->WHarvestSecForestNonForest_frac[j]=0.0;

          #ifdef ALT_NETLUC_PRIMARY
            //ALT#1: reduce the primary fraction deforest first, then adjust the secondary fraction deforested

            //Determine if net deforestation or abandonment??
            temp_net=landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

            if(temp_net > 0){//net deforestation on primary (but potentially smaller than actual)
              //do nothing to mgland -- it only increases in fractional size
              landuse->crop2SecForest_frac[j]=0.0;
              //maintain fractions of primary and secondary forest
              //separate the net deforest fraction from non-deforested fraction
              landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j]-temp_net;
              //update fraction for deforestation
              landuse->PriForest2crop_frac[j]=temp_net;

            }else if(temp_net < 0){//net abandonment
              landuse->crop2SecForest_frac[j]=temp_net*-1;
              landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j];
            }else{//zero change
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->PriForest2crop_frac[j]=0.0;
            }

            if(landuse->SecForest2crop_frac[j] > 0){
              if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
                temp_net_remain= landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
                if(temp_net_remain >= 0){//net deforestation
                  landuse->SecForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                  landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
                }else{//net abandonment even with newly deforested primary forest
                  landuse->SecForest2crop_frac[j]=0.0;//no deforested fraction
                  landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
                }
              }else{
                //empty call for completeness
                //..net deforest secondary
              }
            }
          #else //ALT#2 (default): reduce the secondary fraction deforest first, then adjust the primary fraction deforested
            //Determine if net deforestation or abandonment??
            temp_net=landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

            if(temp_net > 0){//net deforestation
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->SecForest2crop_frac[j]=temp_net;
            }else if(temp_net < 0){//net abandonment
              landuse->crop2SecForest_frac[j]=temp_net*-1;
              landuse->SecForest2crop_frac[j]=0.0;
            }else{//zero change
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->SecForest2crop_frac[j]=0.0;
            }

            if(landuse->PriForest2crop_frac[j] > 0){
              if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
                temp_net_remain= landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
                if(temp_net_remain >= 0){//net deforestation
                  landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j] - temp_net_remain;//net increase in secforest through mixing
                  landuse->PriForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                  landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
                }else{//net abandonment even with newly deforested primary forest
                  landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j];//net increase in secforest through mixing
                  landuse->PriForest2crop_frac[j]=0.0;//no deforested fraction
                  landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
                }
              }else{//net deforest primary
                //woodHarvest fraction used to merge primary with secforest
                //--without deforestation or change in biomass
                //landuse->WHarvestPrimary_frac[j]=0.0;
              }
            }
          #endif //end ALTERNATE NET IMPLEMENTATION -- default is the SECONDARY NET first, then PRIMARY NET


          //Wood Harvest Fractions
          //landuse->WHarvestPrimary_frac[j]=0.0;
          landuse->WHarvestPrimaryNonForest_frac[j]=0.0;
          landuse->WHarvestSecForestOld_frac[j]=0.0;
          landuse->WHarvestSecForestYoung_frac[j]=0.0;
          landuse->WHarvestSecForestNonForest_frac[j]= 0.0;

          //Wood Harvest Mass
          landuse->WHarvestPrimary_biomass[j]=0.0;
          landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
          landuse->WHarvestSecForestOld_biomass[j]=0.0;
          landuse->WHarvestSecForestYoung_biomass[j]=0.0;
          landuse->WHarvestSecForestNonForest_biomass[j]=0.0;

        #else //end if NOWDHARVEST (in NETinGROSS)

          //--------------------------------
          //key difference in netHybrid...
          //.. use primary forest_nonForest Fraction to maintain Pri-Sec fractions

          //Update WoodHarvest Transitions (primary-> secondary)
          landuse->WHarvestPrimary_frac[j]+=landuse->WHarvestPrimaryNonForest_frac[j];

          //set to zero
          landuse->WHarvestPrimaryNonForest_frac[j]=0.0;

          //WoodHarvest Fractions for Secondary Forests
          landuse->WHarvestSecForestOld_frac[j]+= landuse->WHarvestSecForestYoung_frac[j]+landuse->WHarvestSecForestNonForest_frac[j];
            landuse->WHarvestSecForestYoung_frac[j]=0.0;
            landuse->WHarvestSecForestNonForest_frac[j]=0.0;

          //Determine if net deforestation or abandonment??
          temp_net=landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

          if(temp_net > 0){//net deforestation
            landuse->crop2SecForest_frac[j]=0.0;
            landuse->SecForest2crop_frac[j]=temp_net;
          }else if(temp_net < 0){//net abandonment
            landuse->crop2SecForest_frac[j]=temp_net*-1;
            landuse->SecForest2crop_frac[j]=0.0;
          }else{//zero change
            landuse->crop2SecForest_frac[j]=0.0;
            landuse->SecForest2crop_frac[j]=0.0;
          }

          if(landuse->PriForest2crop_frac[j] > 0){
            if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
              temp_net_remain= landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
              if(temp_net_remain >= 0){//net deforestation
                landuse->WHarvestPrimaryNonForest_frac[j]+=landuse->PriForest2crop_frac[j] - temp_net_remain;//net increase in secforest through mixing
                landuse->PriForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
              }else{//net abandonment even with newly deforested primary forest
                landuse->WHarvestPrimaryNonForest_frac[j]+=landuse->PriForest2crop_frac[j];//net increase in secforest through mixing
                landuse->PriForest2crop_frac[j]=0.0;//no deforested fraction
                landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
              }
            }else{//net deforest primary
              //woodHarvest fraction used to merge primary with secforest
              //--without deforestation or change in biomass
              //landuse->WHarvestPrimary_frac[j]=0.0;
            }
          }

          //Wood Harvest Mass
          landuse->WHarvestPrimary_biomass[j]+=landuse->WHarvestPrimaryNonForest_biomass[j];
            //set to zero; primary nonForest_fraction is used to maintain pri-sec ratio
            landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
          landuse->WHarvestSecForestOld_biomass[j]+=landuse->WHarvestSecForestYoung_biomass[j]+landuse->WHarvestSecForestNonForest_biomass[j];
            landuse->WHarvestSecForestYoung_biomass[j]=0.0;
            landuse->WHarvestSecForestNonForest_biomass[j]=0.0;

        #endif//end WoodHarvest
      #endif
      //---------------------------------------
      //   end GROSS_ONESTAND && NETinGROSS
      //---------------------------------------
      #ifdef NOWDHARVEST
        //NO Wood Harvest, so set biomass values to zero
        //..but keep stand fractions as these maintain the primary/secondary LU fractions in data (primary -> secondary)
        landuse->WHarvestSecForestOld_biomass[j]=0.0;
        landuse->WHarvestSecForestYoung_biomass[j]=0.0;
        landuse->WHarvestSecForestNonForest_biomass[j]=0.0;
        landuse->WHarvestPrimary_biomass[j]=0.0;
        landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
      #endif

    }//end for each band - deprecated for Gross-LUC
  }//end for each cell


  //free memory in vector
  free(vec_cropFrac);
  free(vec_crop2pasture);
  free(vec_crop2SecForest);
  free(vec_pasture2crop);
  free(vec_pasture2SecForest);
  free(vec_SecForestFrac);
  free(vec_SecForest2crop);
  free(vec_SecForest2pasture);
  free(vec_PriForest2crop);
  free(vec_PriForest2pasture);
  free(vec_PriForestFrac);
  free(vec_pastureFrac);
  free(vec_SummLUFrac);
  free(vec_WHarvest_primary);
  free(vec_WHarvest_primaryNonForest);
  free(vec_WHarvest_secforestOld);
  free(vec_WHarvest_secforestYoung);
  free(vec_WHarvest_secforestNonForest);

  free(vec_WHarvest_biomass_primary);
  free(vec_WHarvest_biomass_primaryNonForest);
  free(vec_WHarvest_biomass_secforestOld);
  free(vec_WHarvest_biomass_secforestYoung);
  free(vec_WHarvest_biomass_secforestNonForest);
}//end of get_LUH_V1

void get_LUH_V2(Landuse *landuse,int year, int ncell,int ncft, int latMatch[], int lonMatch[]){

  //....................
  // declare vars
  //....................
  int i,j, retval;

  #ifdef NETinGROSS
    Real temp_net, temp_net_remain=0.0;
  #endif

  //......................
  // LU fractions (#12)
  //......................
  float *vec_urban_frac;
  float *vec_pri_forest_frac;
  float *vec_pri_nonforest_frac;
  float *vec_sec_forest_frac;
  float *vec_sec_nonforest_frac;
  float *vec_crop_c3ann_frac;
  float *vec_crop_c4ann_frac;
  float *vec_crop_c3per_frac;
  float *vec_crop_c4per_frac;
  float *vec_crop_c3nfx_frac;
  float *vec_pasture_managed_frac;
  float *vec_pasture_rangeland_frac;

  //............................................
  // LU transitions
  // --requires reduction of transitions
  // --DEVQ: reduce by pre-process or in-code
  //         ..current setup is reduce in-code
  //............................................
  float *vec_pri_forest_2urban;
  float *vec_pri_forest_2crop_c3ann;
  float *vec_pri_forest_2crop_c4ann;
  float *vec_pri_forest_2crop_c3per;
  float *vec_pri_forest_2crop_c4per;
  float *vec_pri_forest_2crop_c3nfx;
  float *vec_pri_forest_2pasture_managed;
  float *vec_pri_forest_2pasture_rangeland;
  float *vec_pri_nonforest_2urban;
  float *vec_pri_nonforest_2crop_c3ann;
  float *vec_pri_nonforest_2crop_c4ann;
  float *vec_pri_nonforest_2crop_c3per;
  float *vec_pri_nonforest_2crop_c4per;
  float *vec_pri_nonforest_2crop_c3nfx;
  float *vec_pri_nonforest_2pasture_managed;
  float *vec_pri_nonforest_2pasture_rangeland;

  float *vec_sec_forest_2urban;
  float *vec_sec_forest_2crop_c3ann;
  float *vec_sec_forest_2crop_c4ann;
  float *vec_sec_forest_2crop_c3per;
  float *vec_sec_forest_2crop_c4per;
  float *vec_sec_forest_2crop_c3nfx;
  float *vec_sec_forest_2pasture_managed;
  float *vec_sec_forest_2pasture_rangeland;
  float *vec_sec_nonforest_2urban;
  float *vec_sec_nonforest_2crop_c3ann;
  float *vec_sec_nonforest_2crop_c4ann;
  float *vec_sec_nonforest_2crop_c3per;
  float *vec_sec_nonforest_2crop_c4per;
  float *vec_sec_nonforest_2crop_c3nfx;
  float *vec_sec_nonforest_2pasture_managed;
  float *vec_sec_nonforest_2pasture_rangeland;

  float *vec_urban_2sec_forest;
  float *vec_urban_2sec_nonforest;
  float *vec_urban_2crop_c3ann;
  float *vec_urban_2crop_c4ann;
  float *vec_urban_2crop_c3per;
  float *vec_urban_2crop_c4per;
  float *vec_urban_2crop_c3nfx;
  float *vec_urban_2pasture_managed;
  float *vec_urban_2pasture_rangeland;

  float *vec_crop_c3ann_2urban;
  float *vec_crop_c3ann_2sec_forest;
  float *vec_crop_c3ann_2sec_nonforest;
  float *vec_crop_c3ann_2crop_c4ann;
  float *vec_crop_c3ann_2crop_c3per;
  float *vec_crop_c3ann_2crop_c4per;
  float *vec_crop_c3ann_2crop_c3nfx;
  float *vec_crop_c3ann_2pasture_managed;
  float *vec_crop_c3ann_2pasture_rangeland;

  float *vec_crop_c4ann_2urban;
  float *vec_crop_c4ann_2sec_forest;
  float *vec_crop_c4ann_2sec_nonforest;
  float *vec_crop_c4ann_2crop_c3ann;
  float *vec_crop_c4ann_2crop_c3per;
  float *vec_crop_c4ann_2crop_c4per;
  float *vec_crop_c4ann_2crop_c3nfx;
  float *vec_crop_c4ann_2pasture_managed;
  float *vec_crop_c4ann_2pasture_rangeland;

  float *vec_crop_c3per_2urban;
  float *vec_crop_c3per_2sec_forest;
  float *vec_crop_c3per_2sec_nonforest;
  float *vec_crop_c3per_2crop_c3ann;
  float *vec_crop_c3per_2crop_c4ann;
  float *vec_crop_c3per_2crop_c4per;
  float *vec_crop_c3per_2crop_c3nfx;
  float *vec_crop_c3per_2pasture_managed;
  float *vec_crop_c3per_2pasture_rangeland;

  float *vec_crop_c4per_2urban;
  float *vec_crop_c4per_2sec_forest;
  float *vec_crop_c4per_2sec_nonforest;
  float *vec_crop_c4per_2crop_c3ann;
  float *vec_crop_c4per_2crop_c4ann;
  float *vec_crop_c4per_2crop_c3per;
  float *vec_crop_c4per_2crop_c3nfx;
  float *vec_crop_c4per_2pasture_managed;
  float *vec_crop_c4per_2pasture_rangeland;

  float *vec_crop_c3nfx_2urban;
  float *vec_crop_c3nfx_2sec_forest;
  float *vec_crop_c3nfx_2sec_nonforest;
  float *vec_crop_c3nfx_2crop_c3ann;
  float *vec_crop_c3nfx_2crop_c4ann;
  float *vec_crop_c3nfx_2crop_c3per;
  float *vec_crop_c3nfx_2crop_c4per;
  float *vec_crop_c3nfx_2pasture_managed;
  float *vec_crop_c3nfx_2pasture_rangeland;

  float *vec_pasture_managed_2urban;
  float *vec_pasture_managed_2sec_forest;
  float *vec_pasture_managed_2sec_nonforest;
  float *vec_pasture_managed_2crop_c3ann;
  float *vec_pasture_managed_2crop_c4ann;
  float *vec_pasture_managed_2crop_c3per;
  float *vec_pasture_managed_2crop_c4per;
  float *vec_pasture_managed_2crop_c3nfx;
  float *vec_pasture_managed_2pasture_rangeland;

  float *vec_pasture_rangeland_2urban;
  float *vec_pasture_rangeland_2sec_forest;
  float *vec_pasture_rangeland_2sec_nonforest;
  float *vec_pasture_rangeland_2crop_c3ann;
  float *vec_pasture_rangeland_2crop_c4ann;
  float *vec_pasture_rangeland_2crop_c3per;
  float *vec_pasture_rangeland_2crop_c4per;
  float *vec_pasture_rangeland_2crop_c3nfx;
  float *vec_pasture_rangeland_2pasture_managed;

  float *vec_WHarvest_primary;
  float *vec_WHarvest_primaryNonForest;
  float *vec_WHarvest_secforestOld;
  float *vec_WHarvest_secforestYoung;
  float *vec_WHarvest_secforestNonForest;

  float *vec_WHarvest_biomass_primary;
  float *vec_WHarvest_biomass_primaryNonForest;
  float *vec_WHarvest_biomass_secforestOld;
  float *vec_WHarvest_biomass_secforestYoung;
  float *vec_WHarvest_biomass_secforestNonForest;

  float *vec_WHarvest_roundwood_frac;
  float *vec_WHarvest_fuelwood_frac;
  float *vec_WHarvest_biofuel_frac;

  //......................
  //  var names
  //......................
  char *varname_urban_frac = "urban";
  char *varname_pri_forest_frac = "primf";
  char *varname_pri_nonforest_frac = "primn";
  char *varname_sec_forest_frac = "secdf";
  char *varname_sec_nonforest_frac = "secdn";
  char *varname_crop_c3ann_frac = "c3ann";
  char *varname_crop_c4ann_frac = "c4ann";
  char *varname_crop_c3per_frac = "c3per";
  char *varname_crop_c4per_frac = "c4per";
  char *varname_crop_c3nfx_frac = "c3nfx";
  char *varname_pasture_managed_frac = "pastr";
  char *varname_pasture_rangeland_frac = "range";

  char *varname_pri_forest_2urban = "primf_to_urban";
  char *varname_pri_forest_2crop_c3ann = "primf_to_c3ann";
  char *varname_pri_forest_2crop_c4ann = "primf_to_c4ann";
  char *varname_pri_forest_2crop_c3per = "primf_to_c3per";
  char *varname_pri_forest_2crop_c4per = "primf_to_c4per";
  char *varname_pri_forest_2crop_c3nfx = "primf_to_c3nfx";
  char *varname_pri_forest_2pasture_managed = "primf_to_pastr";
  char *varname_pri_forest_2pasture_rangeland = "primf_to_range";
  char *varname_pri_nonforest_2urban = "primn_to_urban";
  char *varname_pri_nonforest_2crop_c3ann = "primn_to_c3ann";
  char *varname_pri_nonforest_2crop_c4ann = "primn_to_c4ann";
  char *varname_pri_nonforest_2crop_c3per = "primn_to_c3per";
  char *varname_pri_nonforest_2crop_c4per = "primn_to_c4per";
  char *varname_pri_nonforest_2crop_c3nfx = "primn_to_c3nfx";
  char *varname_pri_nonforest_2pasture_managed = "primn_to_pastr";
  char *varname_pri_nonforest_2pasture_rangeland = "primn_to_range";

  char *varname_sec_forest_2urban = "secdf_to_urban";
  char *varname_sec_forest_2crop_c3ann = "secdf_to_c3ann";
  char *varname_sec_forest_2crop_c4ann = "secdf_to_c4ann";
  char *varname_sec_forest_2crop_c3per = "secdf_to_c3per";
  char *varname_sec_forest_2crop_c4per = "secdf_to_c4per";
  char *varname_sec_forest_2crop_c3nfx = "secdf_to_c3nfx";
  char *varname_sec_forest_2pasture_managed = "secdf_to_pastr";
  char *varname_sec_forest_2pasture_rangeland = "secdf_to_range";
  char *varname_sec_nonforest_2urban = "secdn_to_urban";
  char *varname_sec_nonforest_2crop_c3ann = "secdn_to_c3ann";
  char *varname_sec_nonforest_2crop_c4ann = "secdn_to_c4ann";
  char *varname_sec_nonforest_2crop_c3per = "secdn_to_c3per";
  char *varname_sec_nonforest_2crop_c4per = "secdn_to_c4per";
  char *varname_sec_nonforest_2crop_c3nfx = "secdn_to_c3nfx";
  char *varname_sec_nonforest_2pasture_managed = "secdn_to_pastr";
  char *varname_sec_nonforest_2pasture_rangeland = "secdn_to_range";

  char *varname_urban_2sec_forest = "urban_to_secdf";
  char *varname_urban_2sec_nonforest = "urban_to_secdn";
  char *varname_urban_2crop_c3ann = "urban_to_c3ann";
  char *varname_urban_2crop_c4ann = "urban_to_c4ann";
  char *varname_urban_2crop_c3per = "urban_to_c3per";
  char *varname_urban_2crop_c4per = "urban_to_c4per";
  char *varname_urban_2crop_c3nfx = "urban_to_c3nfx";
  char *varname_urban_2pasture_managed = "urban_to_pastr";
  char *varname_urban_2pasture_rangeland = "urban_to_range";

  char *varname_crop_c3ann_2urban = "c3ann_to_urban";
  char *varname_crop_c3ann_2sec_forest = "c3ann_to_secdf";
  char *varname_crop_c3ann_2sec_nonforest = "c3ann_to_secdn";
  char *varname_crop_c3ann_2crop_c4ann = "c3ann_to_c4ann";
  char *varname_crop_c3ann_2crop_c3per = "c3ann_to_c3per";
  char *varname_crop_c3ann_2crop_c4per = "c3ann_to_c4per";
  char *varname_crop_c3ann_2crop_c3nfx = "c3ann_to_c3nfx";
  char *varname_crop_c3ann_2pasture_managed = "c3ann_to_pastr";
  char *varname_crop_c3ann_2pasture_rangeland = "c3ann_to_range";

  char *varname_crop_c4ann_2urban = "c4ann_to_urban";
  char *varname_crop_c4ann_2sec_forest = "c4ann_to_secdf";
  char *varname_crop_c4ann_2sec_nonforest = "c4ann_to_secdn";
  char *varname_crop_c4ann_2crop_c3ann = "c4ann_to_c3ann";
  char *varname_crop_c4ann_2crop_c3per = "c4ann_to_c3per";
  char *varname_crop_c4ann_2crop_c4per = "c4ann_to_c4per";
  char *varname_crop_c4ann_2crop_c3nfx = "c4ann_to_c3nfx";
  char *varname_crop_c4ann_2pasture_managed = "c4ann_to_pastr";
  char *varname_crop_c4ann_2pasture_rangeland = "c4ann_to_range";

  char *varname_crop_c3per_2urban = "c3per_to_urban";
  char *varname_crop_c3per_2sec_forest = "c3per_to_secdf";
  char *varname_crop_c3per_2sec_nonforest = "c3per_to_secdn";
  char *varname_crop_c3per_2crop_c3ann = "c3per_to_c3ann";
  char *varname_crop_c3per_2crop_c4ann = "c3per_to_c4ann";
  char *varname_crop_c3per_2crop_c4per = "c3per_to_c4per";
  char *varname_crop_c3per_2crop_c3nfx = "c3per_to_c3nfx";
  char *varname_crop_c3per_2pasture_managed = "c3per_to_pastr";
  char *varname_crop_c3per_2pasture_rangeland = "c3per_to_range";

  char *varname_crop_c4per_2urban = "c4per_to_urban";
  char *varname_crop_c4per_2sec_forest = "c4per_to_secdf";
  char *varname_crop_c4per_2sec_nonforest = "c4per_to_secdn";
  char *varname_crop_c4per_2crop_c3ann = "c4per_to_c3ann";
  char *varname_crop_c4per_2crop_c4ann = "c4per_to_c4ann";
  char *varname_crop_c4per_2crop_c3per = "c4per_to_c3per";
  char *varname_crop_c4per_2crop_c3nfx = "c4per_to_c3nfx";
  char *varname_crop_c4per_2pasture_managed = "c4per_to_pastr";
  char *varname_crop_c4per_2pasture_rangeland = "c4per_to_range";

  char *varname_crop_c3nfx_2urban = "c3nfx_to_urban";
  char *varname_crop_c3nfx_2sec_forest = "c3nfx_to_secdf";
  char *varname_crop_c3nfx_2sec_nonforest = "c3nfx_to_secdn";
  char *varname_crop_c3nfx_2crop_c3ann = "c3nfx_to_c3ann";
  char *varname_crop_c3nfx_2crop_c4ann = "c3nfx_to_c4ann";
  char *varname_crop_c3nfx_2crop_c3per = "c3nfx_to_c3per";
  char *varname_crop_c3nfx_2crop_c4per = "c3nfx_to_c4per";
  char *varname_crop_c3nfx_2pasture_managed = "c3nfx_to_pastr";
  char *varname_crop_c3nfx_2pasture_rangeland = "c3nfx_to_range";

  char *varname_pasture_managed_2urban = "pastr_to_urban";
  char *varname_pasture_managed_2sec_forest = "pastr_to_secdf";
  char *varname_pasture_managed_2sec_nonforest = "pastr_to_secdn";
  char *varname_pasture_managed_2crop_c3ann = "pastr_to_c3ann";
  char *varname_pasture_managed_2crop_c4ann = "pastr_to_c4ann";
  char *varname_pasture_managed_2crop_c3per = "pastr_to_c3per";
  char *varname_pasture_managed_2crop_c4per = "pastr_to_c4per";
  char *varname_pasture_managed_2crop_c3nfx = "pastr_to_c3nfx";
  char *varname_pasture_managed_2pasture_rangeland = "pastr_to_range";

  char *varname_pasture_rangeland_2urban = "range_to_urban";
  char *varname_pasture_rangeland_2sec_forest = "range_to_secdf";
  char *varname_pasture_rangeland_2sec_nonforest = "range_to_secdn";
  char *varname_pasture_rangeland_2crop_c3ann = "range_to_c3ann";
  char *varname_pasture_rangeland_2crop_c4ann = "range_to_c4ann";
  char *varname_pasture_rangeland_2crop_c3per = "range_to_c3per";
  char *varname_pasture_rangeland_2crop_c4per = "range_to_c4per";
  char *varname_pasture_rangeland_2crop_c3nfx = "range_to_c3nfx";
  char *varname_pasture_rangeland_2pasture_managed = "range_to_pastr";

  char *varname_WHarvest_primary = "primf_harv";
  char *varname_WHarvest_primaryNonForest = "primf_harv";
  char *varname_WHarvest_secforestOld = "secmf_harv";       //mature (old) secondary
  char *varname_WHarvest_secforestYoung = "secyf_harv";     //young secondary
  char *varname_WHarvest_secforestNonForest = "secnf_harv"; //non-forest secondary
  char *varname_WHarvest_biomass_primary = "primf_bioh";
  char *varname_WHarvest_biomass_primaryNonForest = "primn_bioh";
  char *varname_WHarvest_biomass_secforestOld = "secmf_bioh";
  char *varname_WHarvest_biomass_secforestYoung = "secyf_bioh";
  char *varname_WHarvest_biomass_secforestNonForest = "secnf_bioh";
  char *varname_WHarvest_roundwood_frac = "rndwd"; //industrial roundwood
  char *varname_WHarvest_fuelwood_frac = "fulwd";  //fuel wood
  char *varname_WHarvest_biofuel_frac = "combf";   //commercial biofuel

  //......................
  //  var ids for netcdf
  //......................
  int varid_urban_frac;
  int varid_pri_forest_frac;
  int varid_pri_nonforest_frac;
  int varid_sec_forest_frac;
  int varid_sec_nonforest_frac;
  int varid_crop_c3ann_frac;
  int varid_crop_c4ann_frac;
  int varid_crop_c3per_frac;
  int varid_crop_c4per_frac;
  int varid_crop_c3nfx_frac;
  int varid_pasture_managed_frac;
  int varid_pasture_rangeland_frac;

  int varid_pri_forest_2urban;
  int varid_pri_forest_2crop_c3ann;
  int varid_pri_forest_2crop_c4ann;
  int varid_pri_forest_2crop_c3per;
  int varid_pri_forest_2crop_c4per;
  int varid_pri_forest_2crop_c3nfx;
  int varid_pri_forest_2pasture_managed;
  int varid_pri_forest_2pasture_rangeland;
  int varid_pri_nonforest_2urban;
  int varid_pri_nonforest_2crop_c3ann;
  int varid_pri_nonforest_2crop_c4ann;
  int varid_pri_nonforest_2crop_c3per;
  int varid_pri_nonforest_2crop_c4per;
  int varid_pri_nonforest_2crop_c3nfx;
  int varid_pri_nonforest_2pasture_managed;
  int varid_pri_nonforest_2pasture_rangeland;

  int varid_sec_forest_2urban;
  int varid_sec_forest_2crop_c3ann;
  int varid_sec_forest_2crop_c4ann;
  int varid_sec_forest_2crop_c3per;
  int varid_sec_forest_2crop_c4per;
  int varid_sec_forest_2crop_c3nfx;
  int varid_sec_forest_2pasture_managed;
  int varid_sec_forest_2pasture_rangeland;
  int varid_sec_nonforest_2urban;
  int varid_sec_nonforest_2crop_c3ann;
  int varid_sec_nonforest_2crop_c4ann;
  int varid_sec_nonforest_2crop_c3per;
  int varid_sec_nonforest_2crop_c4per;
  int varid_sec_nonforest_2crop_c3nfx;
  int varid_sec_nonforest_2pasture_managed;
  int varid_sec_nonforest_2pasture_rangeland;

  int varid_urban_2sec_forest;
  int varid_urban_2sec_nonforest;
  int varid_urban_2crop_c3ann;
  int varid_urban_2crop_c4ann;
  int varid_urban_2crop_c3per;
  int varid_urban_2crop_c4per;
  int varid_urban_2crop_c3nfx;
  int varid_urban_2pasture_managed;
  int varid_urban_2pasture_rangeland;

  int varid_crop_c3ann_2urban;
  int varid_crop_c3ann_2sec_forest;
  int varid_crop_c3ann_2sec_nonforest;
  int varid_crop_c3ann_2crop_c4ann;
  int varid_crop_c3ann_2crop_c3per;
  int varid_crop_c3ann_2crop_c4per;
  int varid_crop_c3ann_2crop_c3nfx;
  int varid_crop_c3ann_2pasture_managed;
  int varid_crop_c3ann_2pasture_rangeland;

  int varid_crop_c4ann_2urban;
  int varid_crop_c4ann_2sec_forest;
  int varid_crop_c4ann_2sec_nonforest;
  int varid_crop_c4ann_2crop_c3ann;
  int varid_crop_c4ann_2crop_c3per;
  int varid_crop_c4ann_2crop_c4per;
  int varid_crop_c4ann_2crop_c3nfx;
  int varid_crop_c4ann_2pasture_managed;
  int varid_crop_c4ann_2pasture_rangeland;

  int varid_crop_c3per_2urban;
  int varid_crop_c3per_2sec_forest;
  int varid_crop_c3per_2sec_nonforest;
  int varid_crop_c3per_2crop_c3ann;
  int varid_crop_c3per_2crop_c4ann;
  int varid_crop_c3per_2crop_c4per;
  int varid_crop_c3per_2crop_c3nfx;
  int varid_crop_c3per_2pasture_managed;
  int varid_crop_c3per_2pasture_rangeland;

  int varid_crop_c4per_2urban;
  int varid_crop_c4per_2sec_forest;
  int varid_crop_c4per_2sec_nonforest;
  int varid_crop_c4per_2crop_c3ann;
  int varid_crop_c4per_2crop_c4ann;
  int varid_crop_c4per_2crop_c3per;
  int varid_crop_c4per_2crop_c3nfx;
  int varid_crop_c4per_2pasture_managed;
  int varid_crop_c4per_2pasture_rangeland;

  int varid_crop_c3nfx_2urban;
  int varid_crop_c3nfx_2sec_forest;
  int varid_crop_c3nfx_2sec_nonforest;
  int varid_crop_c3nfx_2crop_c3ann;
  int varid_crop_c3nfx_2crop_c4ann;
  int varid_crop_c3nfx_2crop_c3per;
  int varid_crop_c3nfx_2crop_c4per;
  int varid_crop_c3nfx_2pasture_managed;
  int varid_crop_c3nfx_2pasture_rangeland;

  int varid_pasture_managed_2urban;
  int varid_pasture_managed_2sec_forest;
  int varid_pasture_managed_2sec_nonforest;
  int varid_pasture_managed_2crop_c3ann;
  int varid_pasture_managed_2crop_c4ann;
  int varid_pasture_managed_2crop_c3per;
  int varid_pasture_managed_2crop_c4per;
  int varid_pasture_managed_2crop_c3nfx;
  int varid_pasture_managed_2pasture_rangeland;

  int varid_pasture_rangeland_2urban;
  int varid_pasture_rangeland_2sec_forest;
  int varid_pasture_rangeland_2sec_nonforest;
  int varid_pasture_rangeland_2crop_c3ann;
  int varid_pasture_rangeland_2crop_c4ann;
  int varid_pasture_rangeland_2crop_c3per;
  int varid_pasture_rangeland_2crop_c4per;
  int varid_pasture_rangeland_2crop_c3nfx;
  int varid_pasture_rangeland_2pasture_managed;

  int varid_WHarvest_primary;
  int varid_WHarvest_primaryNonForest;
  int varid_WHarvest_secforestOld;
  int varid_WHarvest_secforestYoung;
  int varid_WHarvest_secforestNonForest;

  int varid_WHarvest_biomass_primary;
  int varid_WHarvest_biomass_primaryNonForest;
  int varid_WHarvest_biomass_secforestOld;
  int varid_WHarvest_biomass_secforestYoung;
  int varid_WHarvest_biomass_secforestNonForest;

  int varid_WHarvest_roundwood_frac;
  int varid_WHarvest_fuelwood_frac;
  int varid_WHarvest_biofuel_frac;

  //.................................
  //  allocate memory for vectors
  //.................................
  //Create vector to hold data;
  vec_urban_frac=newvec(float,1);
  vec_pri_forest_frac=newvec(float,1);
  vec_pri_nonforest_frac=newvec(float,1);
  vec_sec_forest_frac=newvec(float,1);
  vec_sec_nonforest_frac=newvec(float,1);
  vec_crop_c3ann_frac=newvec(float,1);
  vec_crop_c4ann_frac=newvec(float,1);
  vec_crop_c3per_frac=newvec(float,1);
  vec_crop_c4per_frac=newvec(float,1);
  vec_crop_c3nfx_frac=newvec(float,1);
  vec_pasture_managed_frac=newvec(float,1);
  vec_pasture_rangeland_frac=newvec(float,1);

  vec_pri_forest_2urban=newvec(float,1);
  vec_pri_forest_2crop_c3ann=newvec(float,1);
  vec_pri_forest_2crop_c4ann=newvec(float,1);
  vec_pri_forest_2crop_c3per=newvec(float,1);
  vec_pri_forest_2crop_c4per=newvec(float,1);
  vec_pri_forest_2crop_c3nfx=newvec(float,1);
  vec_pri_forest_2pasture_managed=newvec(float,1);
  vec_pri_forest_2pasture_rangeland=newvec(float,1);
  vec_pri_nonforest_2urban=newvec(float,1);
  vec_pri_nonforest_2crop_c3ann=newvec(float,1);
  vec_pri_nonforest_2crop_c4ann=newvec(float,1);
  vec_pri_nonforest_2crop_c3per=newvec(float,1);
  vec_pri_nonforest_2crop_c4per=newvec(float,1);
  vec_pri_nonforest_2crop_c3nfx=newvec(float,1);
  vec_pri_nonforest_2pasture_managed=newvec(float,1);
  vec_pri_nonforest_2pasture_rangeland=newvec(float,1);

  vec_sec_forest_2urban=newvec(float,1);
  vec_sec_forest_2crop_c3ann=newvec(float,1);
  vec_sec_forest_2crop_c4ann=newvec(float,1);
  vec_sec_forest_2crop_c3per=newvec(float,1);
  vec_sec_forest_2crop_c4per=newvec(float,1);
  vec_sec_forest_2crop_c3nfx=newvec(float,1);
  vec_sec_forest_2pasture_managed=newvec(float,1);
  vec_sec_forest_2pasture_rangeland=newvec(float,1);
  vec_sec_nonforest_2urban=newvec(float,1);
  vec_sec_nonforest_2crop_c3ann=newvec(float,1);
  vec_sec_nonforest_2crop_c4ann=newvec(float,1);
  vec_sec_nonforest_2crop_c3per=newvec(float,1);
  vec_sec_nonforest_2crop_c4per=newvec(float,1);
  vec_sec_nonforest_2crop_c3nfx=newvec(float,1);
  vec_sec_nonforest_2pasture_managed=newvec(float,1);
  vec_sec_nonforest_2pasture_rangeland=newvec(float,1);

  vec_urban_2sec_forest=newvec(float,1);
  vec_urban_2sec_nonforest=newvec(float,1);
  vec_urban_2crop_c3ann=newvec(float,1);
  vec_urban_2crop_c4ann=newvec(float,1);
  vec_urban_2crop_c3per=newvec(float,1);
  vec_urban_2crop_c4per=newvec(float,1);
  vec_urban_2crop_c3nfx=newvec(float,1);
  vec_urban_2pasture_managed=newvec(float,1);
  vec_urban_2pasture_rangeland=newvec(float,1);

  vec_crop_c3ann_2urban=newvec(float,1);
  vec_crop_c3ann_2sec_forest=newvec(float,1);
  vec_crop_c3ann_2sec_nonforest=newvec(float,1);
  vec_crop_c3ann_2crop_c4ann=newvec(float,1);
  vec_crop_c3ann_2crop_c3per=newvec(float,1);
  vec_crop_c3ann_2crop_c4per=newvec(float,1);
  vec_crop_c3ann_2crop_c3nfx=newvec(float,1);
  vec_crop_c3ann_2pasture_managed=newvec(float,1);
  vec_crop_c3ann_2pasture_rangeland=newvec(float,1);

  vec_crop_c4ann_2urban=newvec(float,1);
  vec_crop_c4ann_2sec_forest=newvec(float,1);
  vec_crop_c4ann_2sec_nonforest=newvec(float,1);
  vec_crop_c4ann_2crop_c3ann=newvec(float,1);
  vec_crop_c4ann_2crop_c3per=newvec(float,1);
  vec_crop_c4ann_2crop_c4per=newvec(float,1);
  vec_crop_c4ann_2crop_c3nfx=newvec(float,1);
  vec_crop_c4ann_2pasture_managed=newvec(float,1);
  vec_crop_c4ann_2pasture_rangeland=newvec(float,1);

  vec_crop_c3per_2urban=newvec(float,1);
  vec_crop_c3per_2sec_forest=newvec(float,1);
  vec_crop_c3per_2sec_nonforest=newvec(float,1);
  vec_crop_c3per_2crop_c3ann=newvec(float,1);
  vec_crop_c3per_2crop_c4ann=newvec(float,1);
  vec_crop_c3per_2crop_c4per=newvec(float,1);
  vec_crop_c3per_2crop_c3nfx=newvec(float,1);
  vec_crop_c3per_2pasture_managed=newvec(float,1);
  vec_crop_c3per_2pasture_rangeland=newvec(float,1);

  vec_crop_c4per_2urban=newvec(float,1);
  vec_crop_c4per_2sec_forest=newvec(float,1);
  vec_crop_c4per_2sec_nonforest=newvec(float,1);
  vec_crop_c4per_2crop_c3ann=newvec(float,1);
  vec_crop_c4per_2crop_c4ann=newvec(float,1);
  vec_crop_c4per_2crop_c3per=newvec(float,1);
  vec_crop_c4per_2crop_c3nfx=newvec(float,1);
  vec_crop_c4per_2pasture_managed=newvec(float,1);
  vec_crop_c4per_2pasture_rangeland=newvec(float,1);

  vec_crop_c3nfx_2urban=newvec(float,1);
  vec_crop_c3nfx_2sec_forest=newvec(float,1);
  vec_crop_c3nfx_2sec_nonforest=newvec(float,1);
  vec_crop_c3nfx_2crop_c3ann=newvec(float,1);
  vec_crop_c3nfx_2crop_c4ann=newvec(float,1);
  vec_crop_c3nfx_2crop_c3per=newvec(float,1);
  vec_crop_c3nfx_2crop_c4per=newvec(float,1);
  vec_crop_c3nfx_2pasture_managed=newvec(float,1);
  vec_crop_c3nfx_2pasture_rangeland=newvec(float,1);

  vec_pasture_managed_2urban=newvec(float,1);
  vec_pasture_managed_2sec_forest=newvec(float,1);
  vec_pasture_managed_2sec_nonforest=newvec(float,1);
  vec_pasture_managed_2crop_c3ann=newvec(float,1);
  vec_pasture_managed_2crop_c4ann=newvec(float,1);
  vec_pasture_managed_2crop_c3per=newvec(float,1);
  vec_pasture_managed_2crop_c4per=newvec(float,1);
  vec_pasture_managed_2crop_c3nfx=newvec(float,1);
  vec_pasture_managed_2pasture_rangeland=newvec(float,1);

  vec_pasture_rangeland_2urban=newvec(float,1);
  vec_pasture_rangeland_2sec_forest=newvec(float,1);
  vec_pasture_rangeland_2sec_nonforest=newvec(float,1);
  vec_pasture_rangeland_2crop_c3ann=newvec(float,1);
  vec_pasture_rangeland_2crop_c4ann=newvec(float,1);
  vec_pasture_rangeland_2crop_c3per=newvec(float,1);
  vec_pasture_rangeland_2crop_c4per=newvec(float,1);
  vec_pasture_rangeland_2crop_c3nfx=newvec(float,1);
  vec_pasture_rangeland_2pasture_managed=newvec(float,1);

  vec_WHarvest_primary=newvec(float,1);
  vec_WHarvest_primaryNonForest=newvec(float,1);
  vec_WHarvest_secforestOld=newvec(float,1);
  vec_WHarvest_secforestYoung=newvec(float,1);
  vec_WHarvest_secforestNonForest=newvec(float,1);

  vec_WHarvest_biomass_primary=newvec(float,1);
  vec_WHarvest_biomass_primaryNonForest=newvec(float,1);
  vec_WHarvest_biomass_secforestOld=newvec(float,1);
  vec_WHarvest_biomass_secforestYoung=newvec(float,1);
  vec_WHarvest_biomass_secforestNonForest=newvec(float,1);

  vec_WHarvest_roundwood_frac=newvec(float,1);
  vec_WHarvest_fuelwood_frac=newvec(float,1);
  vec_WHarvest_biofuel_frac=newvec(float,1);

  //............................
  // check vectors
  //............................
  check(vec_urban_frac);
  check(vec_pri_forest_frac);
  check(vec_pri_nonforest_frac);
  check(vec_sec_forest_frac);
  check(vec_sec_nonforest_frac);
  check(vec_crop_c3ann_frac);
  check(vec_crop_c4ann_frac);
  check(vec_crop_c3per_frac);
  check(vec_crop_c4per_frac);
  check(vec_crop_c3nfx_frac);
  check(vec_pasture_managed_frac);
  check(vec_pasture_rangeland_frac);

  check(vec_pri_forest_2urban);
  check(vec_pri_forest_2crop_c3ann);
  check(vec_pri_forest_2crop_c4ann);
  check(vec_pri_forest_2crop_c3per);
  check(vec_pri_forest_2crop_c4per);
  check(vec_pri_forest_2crop_c3nfx);
  check(vec_pri_forest_2pasture_managed);
  check(vec_pri_forest_2pasture_rangeland);
  check(vec_pri_nonforest_2urban);
  check(vec_pri_nonforest_2crop_c3ann);
  check(vec_pri_nonforest_2crop_c4ann);
  check(vec_pri_nonforest_2crop_c3per);
  check(vec_pri_nonforest_2crop_c4per);
  check(vec_pri_nonforest_2crop_c3nfx);
  check(vec_pri_nonforest_2pasture_managed);
  check(vec_pri_nonforest_2pasture_rangeland);

  check(vec_sec_forest_2urban);
  check(vec_sec_forest_2crop_c3ann);
  check(vec_sec_forest_2crop_c4ann);
  check(vec_sec_forest_2crop_c3per);
  check(vec_sec_forest_2crop_c4per);
  check(vec_sec_forest_2crop_c3nfx);
  check(vec_sec_forest_2pasture_managed);
  check(vec_sec_forest_2pasture_rangeland);
  check(vec_sec_nonforest_2urban);
  check(vec_sec_nonforest_2crop_c3ann);
  check(vec_sec_nonforest_2crop_c4ann);
  check(vec_sec_nonforest_2crop_c3per);
  check(vec_sec_nonforest_2crop_c4per);
  check(vec_sec_nonforest_2crop_c3nfx);
  check(vec_sec_nonforest_2pasture_managed);
  check(vec_sec_nonforest_2pasture_rangeland);

  check(vec_urban_2sec_forest);
  check(vec_urban_2sec_nonforest);
  check(vec_urban_2crop_c3ann);
  check(vec_urban_2crop_c4ann);
  check(vec_urban_2crop_c3per);
  check(vec_urban_2crop_c4per);
  check(vec_urban_2crop_c3nfx);
  check(vec_urban_2pasture_managed);
  check(vec_urban_2pasture_rangeland);

  check(vec_crop_c3ann_2urban);
  check(vec_crop_c3ann_2sec_forest);
  check(vec_crop_c3ann_2sec_nonforest);
  check(vec_crop_c3ann_2crop_c4ann);
  check(vec_crop_c3ann_2crop_c3per);
  check(vec_crop_c3ann_2crop_c4per);
  check(vec_crop_c3ann_2crop_c3nfx);
  check(vec_crop_c3ann_2pasture_managed);
  check(vec_crop_c3ann_2pasture_rangeland);

  check(vec_crop_c4ann_2urban);
  check(vec_crop_c4ann_2sec_forest);
  check(vec_crop_c4ann_2sec_nonforest);
  check(vec_crop_c4ann_2crop_c3ann);
  check(vec_crop_c4ann_2crop_c3per);
  check(vec_crop_c4ann_2crop_c4per);
  check(vec_crop_c4ann_2crop_c3nfx);
  check(vec_crop_c4ann_2pasture_managed);
  check(vec_crop_c4ann_2pasture_rangeland);

  check(vec_crop_c3per_2urban);
  check(vec_crop_c3per_2sec_forest);
  check(vec_crop_c3per_2sec_nonforest);
  check(vec_crop_c3per_2crop_c3ann);
  check(vec_crop_c3per_2crop_c4ann);
  check(vec_crop_c3per_2crop_c4per);
  check(vec_crop_c3per_2crop_c3nfx);
  check(vec_crop_c3per_2pasture_managed);
  check(vec_crop_c3per_2pasture_rangeland);

  check(vec_crop_c4per_2urban);
  check(vec_crop_c4per_2sec_forest);
  check(vec_crop_c4per_2sec_nonforest);
  check(vec_crop_c4per_2crop_c3ann);
  check(vec_crop_c4per_2crop_c4ann);
  check(vec_crop_c4per_2crop_c3per);
  check(vec_crop_c4per_2crop_c3nfx);
  check(vec_crop_c4per_2pasture_managed);
  check(vec_crop_c4per_2pasture_rangeland);

  check(vec_crop_c3nfx_2urban);
  check(vec_crop_c3nfx_2sec_forest);
  check(vec_crop_c3nfx_2sec_nonforest);
  check(vec_crop_c3nfx_2crop_c3ann);
  check(vec_crop_c3nfx_2crop_c4ann);
  check(vec_crop_c3nfx_2crop_c3per);
  check(vec_crop_c3nfx_2crop_c4per);
  check(vec_crop_c3nfx_2pasture_managed);
  check(vec_crop_c3nfx_2pasture_rangeland);

  check(vec_pasture_managed_2urban);
  check(vec_pasture_managed_2sec_forest);
  check(vec_pasture_managed_2sec_nonforest);
  check(vec_pasture_managed_2crop_c3ann);
  check(vec_pasture_managed_2crop_c4ann);
  check(vec_pasture_managed_2crop_c3per);
  check(vec_pasture_managed_2crop_c4per);
  check(vec_pasture_managed_2crop_c3nfx);
  check(vec_pasture_managed_2pasture_rangeland);

  check(vec_pasture_rangeland_2urban);
  check(vec_pasture_rangeland_2sec_forest);
  check(vec_pasture_rangeland_2sec_nonforest);
  check(vec_pasture_rangeland_2crop_c3ann);
  check(vec_pasture_rangeland_2crop_c4ann);
  check(vec_pasture_rangeland_2crop_c3per);
  check(vec_pasture_rangeland_2crop_c4per);
  check(vec_pasture_rangeland_2crop_c3nfx);
  check(vec_pasture_rangeland_2pasture_managed);

  check(vec_WHarvest_primary);
  check(vec_WHarvest_primaryNonForest);
  check(vec_WHarvest_secforestOld);
  check(vec_WHarvest_secforestYoung);
  check(vec_WHarvest_secforestNonForest);

  check(vec_WHarvest_biomass_primary);
  check(vec_WHarvest_biomass_primaryNonForest);
  check(vec_WHarvest_biomass_secforestOld);
  check(vec_WHarvest_biomass_secforestYoung);
  check(vec_WHarvest_biomass_secforestNonForest);

  check(vec_WHarvest_roundwood_frac);
  check(vec_WHarvest_fuelwood_frac);
  check(vec_WHarvest_biofuel_frac);

  //....................
  // get data
  //....................
  //Set up NCDF read arrays
  //initialize start/count arrays
  size_t start[3], count[3];

  //////////////////////////////////////
  //Loop through cells and read 1 year
  for(j=0; j < ncell; ++j){
    //count identifies to 1 band
    count[0]=1;
    count[1]=1;
    count[2]=1;

    //Skip start
    start[0]=year; //Landuse is read each year; year is passed to getlanduse, updated from index 0
    start[1]=latMatch[j];
    start[2]=lonMatch[j];

    //...............................................
    //return the integer ID for the variable name
    //...............................................
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_frac, &varid_urban_frac))){
      fprintf(stderr,"Error reading LUC variable from file, urban frac varname should be '%s';\n",varname_urban_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_frac, &varid_pri_forest_frac))){
      fprintf(stderr,"Error reading LUC variable from file, priforest frac varname should be '%s';\n",varname_pri_forest_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_frac, &varid_pri_nonforest_frac))){
      fprintf(stderr,"Error reading LUC variable from file, priforest non-forest frac varname should be '%s';\n",varname_pri_nonforest_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_frac, &varid_sec_forest_frac))){
      fprintf(stderr,"Error reading LUC variable from file, secforest frac varname should be '%s';\n",varname_sec_forest_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_frac, &varid_sec_nonforest_frac))){
      fprintf(stderr,"Error reading LUC variable from file, secforest non-forest frac varname should be '%s';\n",varname_sec_nonforest_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_frac, &varid_crop_c3ann_frac))){
      fprintf(stderr,"Error reading LUC variable from file, crop c3ann frac varname should be '%s';\n",varname_crop_c3ann_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_frac, &varid_crop_c4ann_frac))){
      fprintf(stderr,"Error reading LUC variable from file, crop c4ann frac varname should be '%s';\n",varname_crop_c4ann_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_frac, &varid_crop_c3per_frac))){
      fprintf(stderr,"Error reading LUC variable from file, crop c3per frac varname should be '%s';\n",varname_crop_c3per_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_frac, &varid_crop_c4per_frac))){
      fprintf(stderr,"Error reading LUC variable from file, crop c4per frac varname should be '%s';\n",varname_crop_c4per_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_frac, &varid_crop_c3nfx_frac))){
      fprintf(stderr,"Error reading LUC variable from file, crop c3nfx frac varname should be '%s';\n",varname_crop_c3nfx_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_frac, &varid_pasture_managed_frac))){
      fprintf(stderr,"Error reading LUC variable from file, pasture managed frac varname should be '%s';\n",varname_pasture_managed_frac);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_frac, &varid_pasture_rangeland_frac))){
      fprintf(stderr,"Error reading LUC variable from file, pasture rangeland frac varname should be '%s';\n",varname_pasture_rangeland_frac);
      ERR(retval);
    }
    //---------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2urban, &varid_pri_forest_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2urban transition varname should be '%s';\n",varname_pri_forest_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2crop_c3ann, &varid_pri_forest_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2crop_c3ann transition varname should be '%s';\n",varname_pri_forest_2crop_c3ann);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2crop_c4ann, &varid_pri_forest_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2crop_c4ann transition varname should be '%s';\n",varname_pri_forest_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2crop_c3per, &varid_pri_forest_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2crop_c3per transition varname should be '%s';\n",varname_pri_forest_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2crop_c4per, &varid_pri_forest_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2crop_c4per transition varname should be '%s';\n",varname_pri_forest_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2crop_c3nfx, &varid_pri_forest_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2crop_c3nfx transition varname should be '%s';\n",varname_pri_forest_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2pasture_managed, &varid_pri_forest_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2pasture_managed transition varname should be '%s';\n",varname_pri_forest_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_forest_2pasture_rangeland, &varid_pri_forest_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, priforest2pasture_rangeland transition varname should be '%s';\n",varname_pri_forest_2pasture_rangeland);
      ERR(retval);
    }
    //----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2urban, &varid_pri_nonforest_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2urban transition varname should be '%s';\n",varname_pri_nonforest_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2crop_c3ann, &varid_pri_nonforest_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2crop_c3ann transition varname should be '%s';\n",varname_pri_nonforest_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2crop_c4ann, &varid_pri_nonforest_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2crop_c4ann transition varname should be '%s';\n",varname_pri_nonforest_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2crop_c3per, &varid_pri_nonforest_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2crop_c3per transition varname should be '%s';\n",varname_pri_nonforest_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2crop_c4per, &varid_pri_nonforest_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2crop_c4per transition varname should be '%s';\n",varname_pri_forest_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2crop_c3nfx, &varid_pri_nonforest_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2crop_c3nfx transition varname should be '%s';\n",varname_pri_nonforest_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2pasture_managed, &varid_pri_nonforest_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2pasture_managed transition varname should be '%s';\n",varname_pri_nonforest_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pri_nonforest_2pasture_rangeland, &varid_pri_nonforest_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, pri nonforest2pasture_rangeland transition varname should be '%s';\n",varname_pri_nonforest_2pasture_rangeland);
      ERR(retval);
    }
    //------------------------------
    //------------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2urban, &varid_sec_forest_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2urban transition varname should be '%s';\n",varname_sec_forest_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2crop_c3ann, &varid_sec_forest_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2crop_c3ann transition varname should be '%s';\n",varname_sec_forest_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2crop_c4ann, &varid_sec_forest_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2crop_c4ann transition varname should be '%s';\n",varname_sec_forest_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2crop_c3per, &varid_sec_forest_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2crop_c3per transition varname should be '%s';\n",varname_sec_forest_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2crop_c4per, &varid_sec_forest_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2crop_c4per transition varname should be '%s';\n",varname_sec_forest_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2crop_c3nfx, &varid_sec_forest_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2crop_c3nfx transition varname should be '%s';\n",varname_sec_forest_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2pasture_managed, &varid_sec_forest_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2pasture_managed transition varname should be '%s';\n",varname_sec_forest_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_forest_2pasture_rangeland, &varid_sec_forest_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, secforest2pasture_rangeland transition varname should be '%s';\n",varname_sec_forest_2pasture_rangeland);
      ERR(retval);
    }
    //----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2urban, &varid_sec_nonforest_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2urban transition varname should be '%s';\n",varname_sec_nonforest_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2crop_c3ann, &varid_sec_nonforest_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2crop_c3ann transition varname should be '%s';\n",varname_sec_nonforest_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2crop_c4ann, &varid_sec_nonforest_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2crop_c4ann transition varname should be '%s';\n",varname_sec_nonforest_2crop_c4ann);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2crop_c3per, &varid_sec_nonforest_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2crop_c3per transition varname should be '%s';\n",varname_sec_nonforest_2crop_c3per);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2crop_c4per, &varid_sec_nonforest_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2crop_c4per transition varname should be '%s';\n",varname_sec_nonforest_2crop_c4per);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2crop_c3nfx, &varid_sec_nonforest_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2crop_c3nfx transition varname should be '%s';\n",varname_sec_nonforest_2crop_c3nfx);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2pasture_managed, &varid_sec_nonforest_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2pasture_managed transition varname should be '%s';\n",varname_sec_nonforest_2pasture_managed);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_sec_nonforest_2pasture_rangeland, &varid_sec_nonforest_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, sec nonforest2pasture_rangeland transition varname should be '%s';\n",varname_sec_nonforest_2pasture_rangeland);
      ERR(retval);    
    }
    //------------------------------
    //------------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2sec_forest, &varid_urban_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, urban2secforest transition varname should be '%s';\n",varname_urban_2sec_forest);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2sec_nonforest, &varid_urban_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, urban2sec nonforest transition varname should be '%s';\n",varname_urban_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2crop_c3ann, &varid_urban_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, urban2crop_c3ann transition varname should be '%s';\n",varname_urban_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2crop_c4ann, &varid_urban_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, urban2crop_c4ann transition varname should be '%s';\n",varname_urban_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2crop_c3per, &varid_urban_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, urban2crop_c3per transition varname should be '%s';\n",varname_urban_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2crop_c4per, &varid_urban_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, urban2crop_c4per transition varname should be '%s';\n",varname_urban_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2crop_c3nfx, &varid_urban_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, urban2crop_c3nfx transition varname should be '%s';\n",varname_urban_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2pasture_managed, &varid_urban_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, urban2pasture_managed transition varname should be '%s';\n",varname_urban_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_urban_2pasture_rangeland, &varid_urban_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, urban2pasture_rangeland transition varname should be '%s';\n",varname_urban_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2urban, &varid_crop_c3ann_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2urban transition varname should be '%s';\n",varname_crop_c3ann_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2sec_forest, &varid_crop_c3ann_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2secforest transition varname should be '%s';\n",varname_crop_c3ann_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2sec_nonforest, &varid_crop_c3ann_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2sec nonforest transition varname should be '%s';\n",varname_crop_c3ann_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2crop_c4ann, &varid_crop_c3ann_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2crop_c4ann transition varname should be '%s';\n",varname_crop_c3ann_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2crop_c3per, &varid_crop_c3ann_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2crop_c3per transition varname should be '%s';\n",varname_crop_c3ann_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2crop_c4per, &varid_crop_c3ann_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2crop_c4per transition varname should be '%s';\n",varname_crop_c3ann_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2crop_c3nfx, &varid_crop_c3ann_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2crop_c3nfx transition varname should be '%s';\n",varname_crop_c3ann_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2pasture_managed, &varid_crop_c3ann_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2pasture_managed transition varname should be '%s';\n",varname_crop_c3ann_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3ann_2pasture_rangeland, &varid_crop_c3ann_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3ann 2pasture_rangeland transition varname should be '%s';\n",varname_crop_c3ann_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2urban, &varid_crop_c4ann_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2urban transition varname should be '%s';\n",varname_crop_c4ann_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2sec_forest, &varid_crop_c4ann_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2secforest transition varname should be '%s';\n",varname_crop_c4ann_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2sec_nonforest, &varid_crop_c4ann_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2sec nonforest transition varname should be '%s';\n",varname_crop_c4ann_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2crop_c3ann, &varid_crop_c4ann_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2crop_c3ann transition varname should be '%s';\n",varname_crop_c4ann_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2crop_c3per, &varid_crop_c4ann_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2crop_c3per transition varname should be '%s';\n",varname_crop_c4ann_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2crop_c4per, &varid_crop_c4ann_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2crop_c4per transition varname should be '%s';\n",varname_crop_c4ann_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2crop_c3nfx, &varid_crop_c4ann_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2crop_c3nfx transition varname should be '%s';\n",varname_crop_c4ann_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2pasture_managed, &varid_crop_c4ann_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2pasture_managed transition varname should be '%s';\n",varname_crop_c4ann_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4ann_2pasture_rangeland, &varid_crop_c4ann_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4ann 2pasture_rangeland transition varname should be '%s';\n",varname_crop_c4ann_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2urban, &varid_crop_c3per_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2urban transition varname should be '%s';\n",varname_crop_c3per_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2sec_forest, &varid_crop_c3per_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2secforest transition varname should be '%s';\n",varname_crop_c3per_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2sec_nonforest, &varid_crop_c3per_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2sec nonforest transition varname should be '%s';\n",varname_crop_c3per_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2crop_c3ann, &varid_crop_c3per_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2crop_c3ann transition varname should be '%s';\n",varname_crop_c3per_2crop_c3ann);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2crop_c4ann, &varid_crop_c3per_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2crop_c4ann transition varname should be '%s';\n",varname_crop_c3per_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2crop_c4per, &varid_crop_c3per_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2crop_c4per transition varname should be '%s';\n",varname_crop_c3per_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2crop_c3nfx, &varid_crop_c3per_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2crop_c3nfx transition varname should be '%s';\n",varname_crop_c3per_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2pasture_managed, &varid_crop_c3per_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2pasture_managed transition varname should be '%s';\n",varname_crop_c3per_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3per_2pasture_rangeland, &varid_crop_c3per_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3per 2pasture_rangeland transition varname should be '%s';\n",varname_crop_c3per_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2urban, &varid_crop_c4per_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2urban transition varname should be '%s';\n",varname_crop_c4per_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2sec_forest, &varid_crop_c4per_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2secforest transition varname should be '%s';\n",varname_crop_c4per_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2sec_nonforest, &varid_crop_c4per_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2sec nonforest transition varname should be '%s';\n",varname_crop_c4per_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2crop_c3ann, &varid_crop_c4per_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2crop_c3ann transition varname should be '%s';\n",varname_crop_c4per_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2crop_c4ann, &varid_crop_c4per_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2crop_c4ann transition varname should be '%s';\n",varname_crop_c4per_2crop_c4ann);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2crop_c3per, &varid_crop_c4per_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2crop_c3per transition varname should be '%s';\n",varname_crop_c4per_2crop_c3per);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2crop_c3nfx, &varid_crop_c4per_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2crop_c3nfx transition varname should be '%s';\n",varname_crop_c4per_2crop_c3nfx);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2pasture_managed, &varid_crop_c4per_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2pasture_managed transition varname should be '%s';\n",varname_crop_c4per_2pasture_managed);
      ERR(retval);    
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c4per_2pasture_rangeland, &varid_crop_c4per_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c4per 2pasture_rangeland transition varname should be '%s';\n",varname_crop_c4per_2pasture_rangeland);
      ERR(retval);    
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2urban, &varid_crop_c3nfx_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2urban transition varname should be '%s';\n",varname_crop_c3nfx_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2sec_forest, &varid_crop_c3nfx_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2secforest transition varname should be '%s';\n",varname_crop_c3nfx_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2sec_nonforest, &varid_crop_c3nfx_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2sec nonforest transition varname should be '%s';\n",varname_crop_c3nfx_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2crop_c3ann, &varid_crop_c3nfx_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2crop_c3ann transition varname should be '%s';\n",varname_crop_c3nfx_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2crop_c4ann, &varid_crop_c3nfx_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2crop_c4ann transition varname should be '%s';\n",varname_crop_c3nfx_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2crop_c3per, &varid_crop_c3nfx_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2crop_c3per transition varname should be '%s';\n",varname_crop_c3nfx_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2crop_c4per, &varid_crop_c3nfx_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2crop_c4per transition varname should be '%s';\n",varname_crop_c3nfx_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2pasture_managed, &varid_crop_c3nfx_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2pasture_managed transition varname should be '%s';\n",varname_crop_c3nfx_2pasture_managed);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_crop_c3nfx_2pasture_rangeland, &varid_crop_c3nfx_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, crop_c3nfx 2pasture_rangeland transition varname should be '%s';\n",varname_crop_c3nfx_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2urban, &varid_pasture_managed_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2urban transition varname should be '%s';\n",varname_pasture_managed_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2sec_forest, &varid_pasture_managed_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2secforest transition varname should be '%s';\n",varname_pasture_managed_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2sec_nonforest, &varid_pasture_managed_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2sec nonforest transition varname should be '%s';\n",varname_pasture_managed_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2crop_c3ann, &varid_pasture_managed_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2crop_c3ann transition varname should be '%s';\n",varname_pasture_managed_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2crop_c4ann, &varid_pasture_managed_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2crop_c4ann transition varname should be '%s';\n",varname_pasture_managed_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2crop_c3per, &varid_pasture_managed_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2crop_c3per transition varname should be '%s';\n",varname_pasture_managed_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2crop_c4per, &varid_pasture_managed_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2crop_c4per transition varname should be '%s';\n",varname_pasture_managed_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2crop_c3nfx, &varid_pasture_managed_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2crop_c3nfx transition varname should be '%s';\n",varname_pasture_managed_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_managed_2pasture_rangeland, &varid_pasture_managed_2pasture_rangeland))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_managed 2pasture_rangeland transition varname should be '%s';\n",varname_pasture_managed_2pasture_rangeland);
      ERR(retval);
    }
    //-----------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2urban, &varid_pasture_rangeland_2urban))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2urban transition varname should be '%s';\n",varname_pasture_rangeland_2urban);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2sec_forest, &varid_pasture_rangeland_2sec_forest))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2secforest transition varname should be '%s';\n",varname_pasture_rangeland_2sec_forest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2sec_nonforest, &varid_pasture_rangeland_2sec_nonforest))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2sec nonforest transition varname should be '%s';\n",varname_pasture_rangeland_2sec_nonforest);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2crop_c3ann, &varid_pasture_rangeland_2crop_c3ann))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2crop_c3ann transition varname should be '%s';\n",varname_pasture_rangeland_2crop_c3ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2crop_c4ann, &varid_pasture_rangeland_2crop_c4ann))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2crop_c4ann transition varname should be '%s';\n",varname_pasture_rangeland_2crop_c4ann);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2crop_c3per, &varid_pasture_rangeland_2crop_c3per))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2crop_c3per transition varname should be '%s';\n",varname_pasture_rangeland_2crop_c3per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2crop_c4per, &varid_pasture_rangeland_2crop_c4per))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2crop_c4per transition varname should be '%s';\n",varname_pasture_rangeland_2crop_c4per);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2crop_c3nfx, &varid_pasture_rangeland_2crop_c3nfx))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2crop_c3nfx transition varname should be '%s';\n",varname_pasture_rangeland_2crop_c3nfx);
      ERR(retval);
    }
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_pasture_rangeland_2pasture_managed, &varid_pasture_rangeland_2pasture_managed))){
      fprintf(stderr,"Error reading LUC variable from file, pasture_rangeland 2pasture_managed transition varname should be '%s';\n",varname_pasture_rangeland_2pasture_managed);
                       ERR(retval);
                }
    //-----------------------------------------
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_primary, &varid_WHarvest_primary)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_primaryNonForest, &varid_WHarvest_primaryNonForest)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestOld, &varid_WHarvest_secforestOld)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestYoung, &varid_WHarvest_secforestYoung)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_secforestNonForest, &varid_WHarvest_secforestNonForest)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_primary, &varid_WHarvest_biomass_primary)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_primaryNonForest, &varid_WHarvest_biomass_primaryNonForest)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestOld, &varid_WHarvest_biomass_secforestOld)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestYoung, &varid_WHarvest_biomass_secforestYoung)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biomass_secforestNonForest, &varid_WHarvest_biomass_secforestNonForest)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_roundwood_frac, &varid_WHarvest_roundwood_frac)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_fuelwood_frac, &varid_WHarvest_fuelwood_frac)))
         ERR(retval);
    if ((retval = nc_inq_varid(landuse->ncid_luc, varname_WHarvest_biofuel_frac, &varid_WHarvest_biofuel_frac)))
         ERR(retval);

    //....................................................................
    //retreive the data for the specified variable (int ID, from above)
    //....................................................................
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_frac, start, count, vec_urban_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_frac, start, count, vec_pri_forest_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_frac, start, count, vec_pri_nonforest_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_frac, start, count, vec_sec_forest_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_frac, start, count, vec_sec_nonforest_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_frac, start, count, vec_crop_c3ann_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_frac, start, count, vec_crop_c4ann_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_frac, start, count, vec_crop_c3per_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_frac, start, count, vec_crop_c4per_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_frac, start, count, vec_crop_c3nfx_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_frac, start, count, vec_pasture_managed_frac)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_frac, start, count, vec_pasture_rangeland_frac)))
         ERR(retval);
    //---------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2urban, start, count, vec_pri_forest_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2crop_c3ann, start, count, vec_pri_forest_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2crop_c4ann, start, count, vec_pri_forest_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2crop_c3per, start, count, vec_pri_forest_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2crop_c4per, start, count, vec_pri_forest_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2crop_c3nfx, start, count, vec_pri_forest_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2pasture_managed, start, count, vec_pri_forest_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_forest_2pasture_rangeland, start, count, vec_pri_forest_2pasture_rangeland)))
         ERR(retval);
    //----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2urban, start, count, vec_pri_nonforest_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2crop_c3ann, start, count, vec_pri_nonforest_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2crop_c4ann, start, count, vec_pri_nonforest_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2crop_c3per, start, count, vec_pri_nonforest_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2crop_c4per, start, count, vec_pri_nonforest_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2crop_c3nfx, start, count, vec_pri_nonforest_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2pasture_managed, start, count, vec_pri_nonforest_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pri_nonforest_2pasture_rangeland, start, count, vec_pri_nonforest_2pasture_rangeland)))
         ERR(retval);
    //------------------------------
    //------------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2urban, start, count, vec_sec_forest_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2crop_c3ann, start, count, vec_sec_forest_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2crop_c4ann, start, count, vec_sec_forest_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2crop_c3per, start, count, vec_sec_forest_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2crop_c4per, start, count, vec_sec_forest_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2crop_c3nfx, start, count, vec_sec_forest_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2pasture_managed, start, count, vec_sec_forest_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_forest_2pasture_rangeland, start, count, vec_sec_forest_2pasture_rangeland)))
         ERR(retval);
    //----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2urban, start, count, vec_sec_nonforest_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2crop_c3ann, start, count, vec_sec_nonforest_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2crop_c4ann, start, count, vec_sec_nonforest_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2crop_c3per, start, count, vec_sec_nonforest_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2crop_c4per, start, count, vec_sec_nonforest_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2crop_c3nfx, start, count, vec_sec_nonforest_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2pasture_managed, start, count, vec_sec_nonforest_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_sec_nonforest_2pasture_rangeland, start, count, vec_sec_nonforest_2pasture_rangeland)))
         ERR(retval);
    //------------------------------
    //------------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2sec_forest, start, count, vec_urban_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2sec_nonforest, start, count, vec_urban_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2crop_c3ann, start, count, vec_urban_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2crop_c4ann, start, count, vec_urban_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2crop_c3per, start, count, vec_urban_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2crop_c4per, start, count, vec_urban_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2crop_c3nfx, start, count, vec_urban_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2pasture_managed, start, count, vec_urban_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_urban_2pasture_rangeland, start, count, vec_urban_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2urban, start, count, vec_crop_c3ann_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2sec_forest, start, count, vec_crop_c3ann_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2sec_nonforest, start, count, vec_crop_c3ann_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2crop_c4ann, start, count, vec_crop_c3ann_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2crop_c3per, start, count, vec_crop_c3ann_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2crop_c4per, start, count, vec_crop_c3ann_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2crop_c3nfx, start, count, vec_crop_c3ann_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2pasture_managed, start, count, vec_crop_c3ann_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3ann_2pasture_rangeland, start, count, vec_crop_c3ann_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2urban, start, count, vec_crop_c4ann_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2sec_forest, start, count, vec_crop_c4ann_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2sec_nonforest, start, count, vec_crop_c4ann_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2crop_c3ann, start, count, vec_crop_c4ann_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2crop_c3per, start, count, vec_crop_c4ann_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2crop_c4per, start, count, vec_crop_c4ann_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2crop_c3nfx, start, count, vec_crop_c4ann_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2pasture_managed, start, count, vec_crop_c4ann_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4ann_2pasture_rangeland, start, count, vec_crop_c4ann_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2urban, start, count, vec_crop_c3per_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2sec_forest, start, count, vec_crop_c3per_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2sec_nonforest, start, count, vec_crop_c3per_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2crop_c3ann, start, count, vec_crop_c3per_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2crop_c4ann, start, count, vec_crop_c3per_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2crop_c4per, start, count, vec_crop_c3per_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2crop_c3nfx, start, count, vec_crop_c3per_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2pasture_managed, start, count, vec_crop_c3per_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3per_2pasture_rangeland, start, count, vec_crop_c3per_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2urban, start, count, vec_crop_c4per_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2sec_forest, start, count, vec_crop_c4per_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2sec_nonforest, start, count, vec_crop_c4per_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2crop_c3ann, start, count, vec_crop_c4per_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2crop_c4ann, start, count, vec_crop_c4per_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2crop_c3per, start, count, vec_crop_c4per_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2crop_c3nfx, start, count, vec_crop_c4per_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2pasture_managed, start, count, vec_crop_c4per_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c4per_2pasture_rangeland, start, count, vec_crop_c4per_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2urban, start, count, vec_crop_c3nfx_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2sec_forest, start, count, vec_crop_c3nfx_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2sec_nonforest, start, count, vec_crop_c3nfx_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2crop_c3ann, start, count, vec_crop_c3nfx_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2crop_c4ann, start, count, vec_crop_c3nfx_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2crop_c3per, start, count, vec_crop_c3nfx_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2crop_c4per, start, count, vec_crop_c3nfx_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2pasture_managed, start, count, vec_crop_c3nfx_2pasture_managed)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop_c3nfx_2pasture_rangeland, start, count, vec_crop_c3nfx_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2urban, start, count, vec_pasture_managed_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2sec_forest, start, count, vec_pasture_managed_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2sec_nonforest, start, count, vec_pasture_managed_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2crop_c3ann, start, count, vec_pasture_managed_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2crop_c4ann, start, count, vec_pasture_managed_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2crop_c3per, start, count, vec_pasture_managed_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2crop_c4per, start, count, vec_pasture_managed_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2crop_c3nfx, start, count, vec_pasture_managed_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_managed_2pasture_rangeland, start, count, vec_pasture_managed_2pasture_rangeland)))
         ERR(retval);
    //-----------------------------
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2urban, start, count, vec_pasture_rangeland_2urban)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2sec_forest, start, count, vec_pasture_rangeland_2sec_forest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2sec_nonforest, start, count, vec_pasture_rangeland_2sec_nonforest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2crop_c3ann, start, count, vec_pasture_rangeland_2crop_c3ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2crop_c4ann, start, count, vec_pasture_rangeland_2crop_c4ann)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2crop_c3per, start, count, vec_pasture_rangeland_2crop_c3per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2crop_c4per, start, count, vec_pasture_rangeland_2crop_c4per)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2crop_c3nfx, start, count, vec_pasture_rangeland_2crop_c3nfx)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_pasture_rangeland_2pasture_managed, start, count, vec_pasture_rangeland_2pasture_managed)))
         ERR(retval);

    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_primary, start, count, vec_WHarvest_primary)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_primaryNonForest, start, count, vec_WHarvest_primaryNonForest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestOld, start, count, vec_WHarvest_secforestOld)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestYoung, start, count, vec_WHarvest_secforestYoung)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_secforestNonForest, start, count, vec_WHarvest_secforestNonForest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_primary, start, count, vec_WHarvest_biomass_primary)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_primaryNonForest, start, count, vec_WHarvest_biomass_primaryNonForest)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestOld, start, count, vec_WHarvest_biomass_secforestOld)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestYoung, start, count, vec_WHarvest_biomass_secforestYoung)))
         ERR(retval);
    if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biomass_secforestNonForest, start, count, vec_WHarvest_biomass_secforestNonForest)))
         ERR(retval);

     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_roundwood_frac, start, count, vec_WHarvest_roundwood_frac)))
          ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_fuelwood_frac, start, count, vec_WHarvest_fuelwood_frac)))
          ERR(retval);
     if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_WHarvest_biofuel_frac, start, count, vec_WHarvest_biofuel_frac)))
          ERR(retval);

    //----------------------------------
    // check for no data values
    //----------------------------------
    //check for no data value handling because nodata values (-9999) may be present in data
    for(i=0; i< 1; i++){//loop over 1 band
      if(vec_urban_frac[i] < 0 || vec_urban_frac[i] > 1e19)
        vec_urban_frac[i]=0;
      if(vec_pri_forest_frac[i] < 0 || vec_pri_forest_frac[i] > 1e19)
        vec_pri_forest_frac[i]=0;
      if(vec_pri_nonforest_frac[i] < 0 || vec_pri_nonforest_frac[i] > 1e19)
        vec_pri_nonforest_frac[i]=0;
      if(vec_sec_forest_frac[i] < 0 || vec_sec_forest_frac[i] > 1e19)
        vec_sec_forest_frac[i]=0;
      if(vec_sec_nonforest_frac[i] < 0 || vec_sec_nonforest_frac[i] > 1e19)
        vec_sec_nonforest_frac[i]=0;
      if(vec_crop_c3ann_frac[i] < 0 || vec_crop_c3ann_frac[i] > 1e19)
        vec_crop_c3ann_frac[i]=0;
      if(vec_crop_c4ann_frac[i] < 0 || vec_crop_c4ann_frac[i] > 1e19)
        vec_crop_c4ann_frac[i]=0;
      if(vec_crop_c3per_frac[i] < 0 || vec_crop_c3per_frac[i] > 1e19)
        vec_crop_c3per_frac[i]=0;
      if(vec_crop_c4per_frac[i] < 0 || vec_crop_c4per_frac[i] > 1e19)
        vec_crop_c4per_frac[i]=0;
      if(vec_crop_c3nfx_frac[i] < 0 || vec_crop_c3nfx_frac[i] > 1e19)
        vec_crop_c3nfx_frac[i]=0;
      if(vec_pasture_managed_frac[i] < 0 || vec_pasture_managed_frac[i] > 1e19)
        vec_pasture_managed_frac[i]=0;
      if(vec_pasture_rangeland_frac[i] < 0 || vec_pasture_rangeland_frac[i] > 1e19)
        vec_pasture_rangeland_frac[i]=0;

      if(vec_pri_forest_2urban[i] < 0 || vec_pri_forest_2urban[i] > 1e19)
        vec_pri_forest_2urban[i]=0;
      if(vec_pri_forest_2crop_c3ann[i] < 0 || vec_pri_forest_2crop_c3ann[i] > 1e19)
        vec_pri_forest_2crop_c3ann[i]=0;
      if(vec_pri_forest_2crop_c4ann[i] < 0 || vec_pri_forest_2crop_c4ann[i] > 1e19)
        vec_pri_forest_2crop_c4ann[i]=0;
      if(vec_pri_forest_2crop_c3per[i] < 0 || vec_pri_forest_2crop_c3per[i] > 1e19)
        vec_pri_forest_2crop_c3per[i]=0;
      if(vec_pri_forest_2crop_c4per[i] < 0 || vec_pri_forest_2crop_c4per[i] > 1e19)
        vec_pri_forest_2crop_c4per[i]=0;
      if(vec_pri_forest_2crop_c3nfx[i] < 0 || vec_pri_forest_2crop_c3nfx[i] > 1e19)
        vec_pri_forest_2crop_c3nfx[i]=0;
      if(vec_pri_forest_2pasture_managed[i] < 0 || vec_pri_forest_2pasture_managed[i] > 1e19)
        vec_pri_forest_2pasture_managed[i]=0;
      if(vec_pri_forest_2pasture_rangeland[i] < 0 || vec_pri_forest_2pasture_rangeland[i] > 1e19)
        vec_pri_forest_2pasture_rangeland[i]=0;

      if(vec_pri_nonforest_2urban[i] < 0 || vec_pri_nonforest_2urban[i] > 1e19)
        vec_pri_nonforest_2urban[i]=0;
      if(vec_pri_nonforest_2crop_c3ann[i] < 0 || vec_pri_nonforest_2crop_c3ann[i] > 1e19)
        vec_pri_nonforest_2crop_c3ann[i]=0;
      if(vec_pri_nonforest_2crop_c4ann[i] < 0 || vec_pri_nonforest_2crop_c4ann[i] > 1e19)
        vec_pri_nonforest_2crop_c4ann[i]=0;
      if(vec_pri_nonforest_2crop_c3per[i] < 0 || vec_pri_nonforest_2crop_c3per[i] > 1e19)
        vec_pri_nonforest_2crop_c3per[i]=0;
      if(vec_pri_nonforest_2crop_c4per[i] < 0 || vec_pri_nonforest_2crop_c4per[i] > 1e19)
        vec_pri_nonforest_2crop_c4per[i]=0;
      if(vec_pri_nonforest_2crop_c3nfx[i] < 0 || vec_pri_nonforest_2crop_c3nfx[i] > 1e19)
        vec_pri_nonforest_2crop_c3nfx[i]=0;
      if(vec_pri_nonforest_2pasture_managed[i] < 0 || vec_pri_nonforest_2pasture_managed[i] > 1e19)
        vec_pri_nonforest_2pasture_managed[i]=0;
      if(vec_pri_nonforest_2pasture_rangeland[i] < 0 || vec_pri_nonforest_2pasture_rangeland[i] > 1e19)
        vec_pri_nonforest_2pasture_rangeland[i]=0;

      if(vec_sec_forest_2urban[i] < 0 || vec_sec_forest_2urban[i] > 1e19)
        vec_sec_forest_2urban[i]=0;
      if(vec_sec_forest_2crop_c3ann[i] < 0 || vec_sec_forest_2crop_c3ann[i] > 1e19)
        vec_sec_forest_2crop_c3ann[i]=0;
      if(vec_sec_forest_2crop_c4ann[i] < 0 || vec_sec_forest_2crop_c4ann[i] > 1e19)
        vec_sec_forest_2crop_c4ann[i]=0;
      if(vec_sec_forest_2crop_c3per[i] < 0 || vec_sec_forest_2crop_c3per[i] > 1e19)
        vec_sec_forest_2crop_c3per[i]=0;
      if(vec_sec_forest_2crop_c4per[i] < 0 || vec_sec_forest_2crop_c4per[i] > 1e19)
        vec_sec_forest_2crop_c4per[i]=0;
      if(vec_sec_forest_2crop_c3nfx[i] < 0 || vec_sec_forest_2crop_c3nfx[i] > 1e19)
        vec_sec_forest_2crop_c3nfx[i]=0;
      if(vec_sec_forest_2pasture_managed[i] < 0 || vec_sec_forest_2pasture_managed[i] > 1e19)
        vec_sec_forest_2pasture_managed[i]=0;
      if(vec_sec_forest_2pasture_rangeland[i] < 0 || vec_sec_forest_2pasture_rangeland[i] > 1e19)
        vec_sec_forest_2pasture_rangeland[i]=0;

      if(vec_sec_nonforest_2urban[i] < 0 || vec_sec_nonforest_2urban[i] > 1e19)
        vec_sec_nonforest_2urban[i]=0;
      if(vec_sec_nonforest_2crop_c3ann[i] < 0 || vec_sec_nonforest_2crop_c3ann[i] > 1e19)
        vec_sec_nonforest_2crop_c3ann[i]=0;
      if(vec_sec_nonforest_2crop_c4ann[i] < 0 || vec_sec_nonforest_2crop_c4ann[i] > 1e19)
        vec_sec_nonforest_2crop_c4ann[i]=0;
      if(vec_sec_nonforest_2crop_c3per[i] < 0 || vec_sec_nonforest_2crop_c3per[i] > 1e19)
        vec_sec_nonforest_2crop_c3per[i]=0;
      if(vec_sec_nonforest_2crop_c4per[i] < 0 || vec_sec_nonforest_2crop_c4per[i] > 1e19)
        vec_sec_nonforest_2crop_c4per[i]=0;
      if(vec_sec_nonforest_2crop_c3nfx[i] < 0 || vec_sec_nonforest_2crop_c3nfx[i] > 1e19)
        vec_sec_nonforest_2crop_c3nfx[i]=0;
      if(vec_sec_nonforest_2pasture_managed[i] < 0 || vec_sec_nonforest_2pasture_managed[i] > 1e19)
        vec_sec_nonforest_2pasture_managed[i]=0;
      if(vec_sec_nonforest_2pasture_rangeland[i] < 0 || vec_sec_nonforest_2pasture_rangeland[i] > 1e19)
        vec_sec_nonforest_2pasture_rangeland[i]=0;

      if(vec_urban_2sec_forest[i] < 0 || vec_urban_2sec_forest[i] > 1e19)
        vec_urban_2sec_forest[i]=0;
      if(vec_urban_2sec_nonforest[i] < 0 || vec_urban_2sec_nonforest[i] > 1e19)
        vec_urban_2sec_nonforest[i]=0;
      if(vec_urban_2crop_c3ann[i] < 0 || vec_urban_2crop_c3ann[i] > 1e19)
        vec_urban_2crop_c3ann[i]=0;
      if(vec_urban_2crop_c4ann[i] < 0 || vec_urban_2crop_c4ann[i] > 1e19)
        vec_urban_2crop_c4ann[i]=0;
      if(vec_urban_2crop_c3per[i] < 0 || vec_urban_2crop_c3per[i] > 1e19)
        vec_urban_2crop_c3per[i]=0;
      if(vec_urban_2crop_c4per[i] < 0 || vec_urban_2crop_c4per[i] > 1e19)
        vec_urban_2crop_c4per[i]=0;
      if(vec_urban_2crop_c3nfx[i] < 0 || vec_urban_2crop_c3nfx[i] > 1e19)
        vec_urban_2crop_c3nfx[i]=0;
      if(vec_urban_2pasture_managed[i] < 0 || vec_urban_2pasture_managed[i] > 1e19)
        vec_urban_2pasture_managed[i]=0;
      if(vec_urban_2pasture_rangeland[i] < 0 || vec_urban_2pasture_rangeland[i] > 1e19)
        vec_urban_2pasture_rangeland[i]=0;

      if(vec_crop_c3ann_2urban[i] < 0 || vec_crop_c3ann_2urban[i] > 1e19)
        vec_crop_c3ann_2urban[i]=0;
      if(vec_crop_c3ann_2sec_forest[i] < 0 || vec_crop_c3ann_2sec_forest[i] > 1e19)
        vec_crop_c3ann_2sec_forest[i]=0;
      if(vec_crop_c3ann_2sec_nonforest[i] < 0 || vec_crop_c3ann_2sec_nonforest[i] > 1e19)
        vec_crop_c3ann_2sec_nonforest[i]=0;
      if(vec_crop_c3ann_2crop_c4ann[i] < 0 || vec_crop_c3ann_2crop_c4ann[i] > 1e19)
        vec_crop_c3ann_2crop_c4ann[i]=0;
      if(vec_crop_c3ann_2crop_c3per[i] < 0 || vec_crop_c3ann_2crop_c3per[i] > 1e19)
        vec_crop_c3ann_2crop_c3per[i]=0;
      if(vec_crop_c3ann_2crop_c4per[i] < 0 || vec_crop_c3ann_2crop_c4per[i] > 1e19)
        vec_crop_c3ann_2crop_c4per[i]=0;
      if(vec_crop_c3ann_2crop_c3nfx[i] < 0 || vec_crop_c3ann_2crop_c3nfx[i] > 1e19)
        vec_crop_c3ann_2crop_c3nfx[i]=0;
      if(vec_crop_c3ann_2pasture_managed[i] < 0 || vec_crop_c3ann_2pasture_managed[i] > 1e19)
        vec_crop_c3ann_2pasture_managed[i]=0;
      if(vec_crop_c3ann_2pasture_rangeland[i] < 0 || vec_crop_c3ann_2pasture_rangeland[i] > 1e19)
        vec_crop_c3ann_2pasture_rangeland[i]=0;

      if(vec_crop_c4ann_2urban[i] < 0 || vec_crop_c4ann_2urban[i] > 1e19)
        vec_crop_c4ann_2urban[i]=0;
      if(vec_crop_c4ann_2sec_forest[i] < 0 || vec_crop_c4ann_2sec_forest[i] > 1e19)
        vec_crop_c4ann_2sec_forest[i]=0;
      if(vec_crop_c4ann_2sec_nonforest[i] < 0 || vec_crop_c4ann_2sec_nonforest[i] > 1e19)
        vec_crop_c4ann_2sec_nonforest[i]=0;
      if(vec_crop_c4ann_2crop_c3ann[i] < 0 || vec_crop_c4ann_2crop_c3ann[i] > 1e19)
        vec_crop_c4ann_2crop_c3ann[i]=0;
      if(vec_crop_c4ann_2crop_c3per[i] < 0 || vec_crop_c4ann_2crop_c3per[i] > 1e19)
        vec_crop_c4ann_2crop_c3per[i]=0;
      if(vec_crop_c4ann_2crop_c4per[i] < 0 || vec_crop_c4ann_2crop_c4per[i] > 1e19)
        vec_crop_c4ann_2crop_c4per[i]=0;
      if(vec_crop_c4ann_2crop_c3nfx[i] < 0 || vec_crop_c4ann_2crop_c3nfx[i] > 1e19)
        vec_crop_c4ann_2crop_c3nfx[i]=0;
      if(vec_crop_c4ann_2pasture_managed[i] < 0 || vec_crop_c4ann_2pasture_managed[i] > 1e19)
        vec_crop_c4ann_2pasture_managed[i]=0;
      if(vec_crop_c4ann_2pasture_rangeland[i] < 0 || vec_crop_c4ann_2pasture_rangeland[i] > 1e19)
        vec_crop_c4ann_2pasture_rangeland[i]=0;

      if(vec_crop_c3per_2urban[i] < 0 || vec_crop_c3per_2urban[i] > 1e19)
        vec_crop_c3per_2urban[i]=0;
      if(vec_crop_c3per_2sec_forest[i] < 0 || vec_crop_c3per_2sec_forest[i] > 1e19)
        vec_crop_c3per_2sec_forest[i]=0;
      if(vec_crop_c3per_2sec_nonforest[i] < 0 || vec_crop_c3per_2sec_nonforest[i] > 1e19)
        vec_crop_c3per_2sec_nonforest[i]=0;
      if(vec_crop_c3per_2crop_c3ann[i] < 0 || vec_crop_c3per_2crop_c3ann[i] > 1e19)
        vec_crop_c3per_2crop_c3ann[i]=0;
      if(vec_crop_c3per_2crop_c4ann[i] < 0 || vec_crop_c3per_2crop_c4ann[i] > 1e19)
        vec_crop_c3per_2crop_c4ann[i]=0;
      if(vec_crop_c3per_2crop_c4per[i] < 0 || vec_crop_c3per_2crop_c4per[i] > 1e19)
        vec_crop_c3per_2crop_c4per[i]=0;
      if(vec_crop_c3per_2crop_c3nfx[i] < 0 || vec_crop_c3per_2crop_c3nfx[i] > 1e19)
        vec_crop_c3per_2crop_c3nfx[i]=0;
      if(vec_crop_c3per_2pasture_managed[i] < 0 || vec_crop_c3per_2pasture_managed[i] > 1e19)
        vec_crop_c3per_2pasture_managed[i]=0;
      if(vec_crop_c3per_2pasture_rangeland[i] < 0 || vec_crop_c3per_2pasture_rangeland[i] > 1e19)
        vec_crop_c3per_2pasture_rangeland[i]=0;

      if(vec_crop_c4per_2urban[i] < 0 || vec_crop_c4per_2urban[i] > 1e19)
        vec_crop_c4per_2urban[i]=0;
      if(vec_crop_c4per_2sec_forest[i] < 0 || vec_crop_c4per_2sec_forest[i] > 1e19)
        vec_crop_c4per_2sec_forest[i]=0;
      if(vec_crop_c4per_2sec_nonforest[i] < 0 || vec_crop_c4per_2sec_nonforest[i] > 1e19)
        vec_crop_c4per_2sec_nonforest[i]=0;
      if(vec_crop_c4per_2crop_c3ann[i] < 0 || vec_crop_c4per_2crop_c3ann[i] > 1e19)
        vec_crop_c4per_2crop_c3ann[i]=0;
      if(vec_crop_c4per_2crop_c4ann[i] < 0 || vec_crop_c4per_2crop_c4ann[i] > 1e19)
        vec_crop_c4per_2crop_c4ann[i]=0;
      if(vec_crop_c4per_2crop_c3per[i] < 0 || vec_crop_c4per_2crop_c3per[i] > 1e19)
        vec_crop_c4per_2crop_c3per[i]=0;
      if(vec_crop_c4per_2crop_c3nfx[i] < 0 || vec_crop_c4per_2crop_c3nfx[i] > 1e19)
        vec_crop_c4per_2crop_c3nfx[i]=0;
      if(vec_crop_c4per_2pasture_managed[i] < 0 || vec_crop_c4per_2pasture_managed[i] > 1e19)
        vec_crop_c4per_2pasture_managed[i]=0;
      if(vec_crop_c4per_2pasture_rangeland[i] < 0 || vec_crop_c4per_2pasture_rangeland[i] > 1e19)
        vec_crop_c4per_2pasture_rangeland[i]=0;

      if(vec_crop_c3nfx_2urban[i] < 0 || vec_crop_c3nfx_2urban[i] > 1e19)
        vec_crop_c3nfx_2urban[i]=0;
      if(vec_crop_c3nfx_2sec_forest[i] < 0 || vec_crop_c3nfx_2sec_forest[i] > 1e19)
        vec_crop_c3nfx_2sec_forest[i]=0;
      if(vec_crop_c3nfx_2sec_nonforest[i] < 0 || vec_crop_c3nfx_2sec_nonforest[i] > 1e19)
        vec_crop_c3nfx_2sec_nonforest[i]=0;
      if(vec_crop_c3nfx_2crop_c3ann[i] < 0 || vec_crop_c3nfx_2crop_c3ann[i] > 1e19)
        vec_crop_c3nfx_2crop_c3ann[i]=0;
      if(vec_crop_c3nfx_2crop_c4ann[i] < 0 || vec_crop_c3nfx_2crop_c4ann[i] > 1e19)
        vec_crop_c3nfx_2crop_c4ann[i]=0;
      if(vec_crop_c3nfx_2crop_c3per[i] < 0 || vec_crop_c3nfx_2crop_c3per[i] > 1e19)
        vec_crop_c3nfx_2crop_c3per[i]=0;
      if(vec_crop_c3nfx_2crop_c4per[i] < 0 || vec_crop_c3nfx_2crop_c4per[i] > 1e19)
        vec_crop_c3nfx_2crop_c4per[i]=0;
      if(vec_crop_c3nfx_2pasture_managed[i] < 0 || vec_crop_c3nfx_2pasture_managed[i] > 1e19)
        vec_crop_c3nfx_2pasture_managed[i]=0;
      if(vec_crop_c3nfx_2pasture_rangeland[i] < 0 || vec_crop_c3nfx_2pasture_rangeland[i] > 1e19)
        vec_crop_c3nfx_2pasture_rangeland[i]=0;

      if(vec_pasture_managed_2urban[i] < 0 || vec_pasture_managed_2urban[i] > 1e19)
        vec_pasture_managed_2urban[i]=0;
      if(vec_pasture_managed_2sec_forest[i] < 0 || vec_pasture_managed_2sec_forest[i] > 1e19)
        vec_pasture_managed_2sec_forest[i]=0;
      if(vec_pasture_managed_2sec_nonforest[i] < 0 || vec_pasture_managed_2sec_nonforest[i] > 1e19)
        vec_pasture_managed_2sec_nonforest[i]=0;
      if(vec_pasture_managed_2crop_c3ann[i] < 0 || vec_pasture_managed_2crop_c3ann[i] > 1e19)
        vec_pasture_managed_2crop_c3ann[i]=0;
      if(vec_pasture_managed_2crop_c4ann[i] < 0 || vec_pasture_managed_2crop_c4ann[i] > 1e19)
        vec_pasture_managed_2crop_c4ann[i]=0;
      if(vec_pasture_managed_2crop_c3per[i] < 0 || vec_pasture_managed_2crop_c3per[i] > 1e19)
        vec_pasture_managed_2crop_c3per[i]=0;
      if(vec_pasture_managed_2crop_c4per[i] < 0 || vec_pasture_managed_2crop_c4per[i] > 1e19)
        vec_pasture_managed_2crop_c4per[i]=0;
      if(vec_pasture_managed_2crop_c3nfx[i] < 0 || vec_pasture_managed_2crop_c3nfx[i] > 1e19)
        vec_pasture_managed_2crop_c3nfx[i]=0;
      if(vec_pasture_managed_2pasture_rangeland[i] < 0 || vec_pasture_managed_2pasture_rangeland[i] > 1e19)
        vec_pasture_managed_2pasture_rangeland[i]=0;

      if(vec_pasture_rangeland_2urban[i] < 0 || vec_pasture_rangeland_2crop_c3ann[i] > 1e19)
        vec_pasture_rangeland_2urban[i]=0;
      if(vec_pasture_rangeland_2sec_forest[i] < 0 || vec_pasture_rangeland_2sec_forest[i] > 1e19)
        vec_pasture_rangeland_2sec_forest[i]=0;
      if(vec_pasture_rangeland_2sec_nonforest[i] < 0 || vec_pasture_rangeland_2sec_nonforest[i] > 1e19)
        vec_pasture_rangeland_2sec_nonforest[i]=0;
      if(vec_pasture_rangeland_2crop_c3ann[i] < 0 || vec_pasture_rangeland_2crop_c3ann[i] > 1e19)
        vec_pasture_rangeland_2crop_c3ann[i]=0;
      if(vec_pasture_rangeland_2crop_c4ann[i] < 0 || vec_pasture_rangeland_2crop_c4ann[i] > 1e19)
        vec_pasture_rangeland_2crop_c4ann[i]=0;
      if(vec_pasture_rangeland_2crop_c3per[i] < 0 || vec_pasture_rangeland_2crop_c3per[i] > 1e19)
        vec_pasture_rangeland_2crop_c3per[i]=0;
      if(vec_pasture_rangeland_2crop_c4per[i] < 0 || vec_pasture_rangeland_2crop_c4per[i] > 1e19)
        vec_pasture_rangeland_2crop_c4per[i]=0;
      if(vec_pasture_rangeland_2crop_c3nfx[i] < 0 || vec_pasture_rangeland_2crop_c3nfx[i] > 1e19)
        vec_pasture_rangeland_2crop_c3nfx[i]=0;
      if(vec_pasture_rangeland_2pasture_managed[i] < 0 || vec_pasture_rangeland_2pasture_managed[i] > 1e19)
        vec_pasture_rangeland_2pasture_managed[i]=0;

      if(vec_WHarvest_primary[i] < 0 || vec_WHarvest_primary[i] > 1e19)
        vec_WHarvest_primary[i]=0;
      if(vec_WHarvest_primaryNonForest[i] < 0 || vec_WHarvest_primaryNonForest[i] > 1e19)
        vec_WHarvest_primaryNonForest[i]=0;
      if(vec_WHarvest_secforestOld[i] < 0 || vec_WHarvest_secforestOld[i] > 1e19)
        vec_WHarvest_secforestOld[i]=0;
      if(vec_WHarvest_secforestYoung[i] < 0 || vec_WHarvest_secforestYoung[i] > 1e19)
        vec_WHarvest_secforestYoung[i]=0;
      if(vec_WHarvest_secforestNonForest[i] < 0 || vec_WHarvest_secforestNonForest[i] > 1e19)
        vec_WHarvest_secforestNonForest[i]=0;
      if(vec_WHarvest_biomass_primary[i] < 0 || vec_WHarvest_biomass_primary[i] > 1e19)
        vec_WHarvest_biomass_primary[i]=0;
      if(vec_WHarvest_biomass_primaryNonForest[i] < 0 || vec_WHarvest_biomass_primaryNonForest[i] > 1e19)
        vec_WHarvest_biomass_primaryNonForest[i]=0;
      if(vec_WHarvest_biomass_secforestOld[i] < 0 || vec_WHarvest_biomass_secforestOld[i] > 1e19)
        vec_WHarvest_biomass_secforestOld[i]=0;
      if(vec_WHarvest_biomass_secforestYoung[i] < 0 || vec_WHarvest_biomass_secforestYoung[i] > 1e19)
        vec_WHarvest_biomass_secforestYoung[i]=0;
      if(vec_WHarvest_biomass_secforestNonForest[i] < 0 || vec_WHarvest_biomass_secforestNonForest[i] > 1e19)
        vec_WHarvest_biomass_secforestNonForest[i]=0;

      if(vec_WHarvest_roundwood_frac[i] < 0 || vec_WHarvest_roundwood_frac[i] > 1e19)
        vec_WHarvest_roundwood_frac[i]=0;
      if(vec_WHarvest_fuelwood_frac[i] < 0 || vec_WHarvest_fuelwood_frac[i] > 1e19)
        vec_WHarvest_fuelwood_frac[i]=0;
      if(vec_WHarvest_biofuel_frac[i] < 0 || vec_WHarvest_biofuel_frac[i] > 1e19)
        vec_WHarvest_biofuel_frac[i]=0;

      // ..convert to int, round, then convert to float to avoid floating-point errors
      // ..fraction of ag (crop + pasture) converted to secondary forests in cell
      vec_urban_frac[i]=roundf(vec_urban_frac[i]*1000000)/1000000;
      vec_pri_forest_frac[i]=roundf(vec_pri_forest_frac[i]*1000000)/1000000;
      vec_pri_nonforest_frac[i]=roundf(vec_pri_nonforest_frac[i]*1000000)/1000000;
      vec_sec_forest_frac[i]=roundf(vec_sec_forest_frac[i]*1000000)/1000000;
      vec_sec_nonforest_frac[i]=roundf(vec_sec_nonforest_frac[i]*1000000)/1000000;
      vec_crop_c3ann_frac[i]=roundf(vec_crop_c3ann_frac[i]*1000000)/1000000;
      vec_crop_c4ann_frac[i]=roundf(vec_crop_c4ann_frac[i]*1000000)/1000000;
      vec_crop_c3per_frac[i]=roundf(vec_crop_c3per_frac[i]*1000000)/1000000;
      vec_crop_c3nfx_frac[i]=roundf(vec_crop_c3nfx_frac[i]*1000000)/1000000;
      vec_pasture_managed_frac[i]=roundf(vec_pasture_managed_frac[i]*1000000)/1000000;
      vec_pasture_rangeland_frac[i]=roundf(vec_pasture_rangeland_frac[i]*1000000)/1000000;

      vec_pri_forest_2urban[i]=roundf(vec_pri_forest_2urban[i]*1000000)/1000000;
      vec_pri_forest_2crop_c3ann[i]=roundf(vec_pri_forest_2crop_c3ann[i]*1000000)/1000000;
      vec_pri_forest_2crop_c4ann[i]=roundf(vec_pri_forest_2crop_c4ann[i]*1000000)/1000000;
      vec_pri_forest_2crop_c3per[i]=roundf(vec_pri_forest_2crop_c3per[i]*1000000)/1000000;
      vec_pri_forest_2crop_c4per[i]=roundf(vec_pri_forest_2crop_c4per[i]*1000000)/1000000;
      vec_pri_forest_2crop_c3nfx[i]=roundf(vec_pri_forest_2crop_c3nfx[i]*1000000)/1000000;
      vec_pri_forest_2pasture_managed[i]=roundf(vec_pri_forest_2pasture_managed[i]*1000000)/1000000;
      vec_pri_forest_2pasture_rangeland[i]=roundf(vec_pri_forest_2pasture_rangeland[i]*1000000)/1000000;

      vec_pri_nonforest_2urban[i]=roundf(vec_pri_nonforest_2urban[i]*1000000)/1000000;
      vec_pri_nonforest_2crop_c3ann[i]=roundf(vec_pri_nonforest_2crop_c3ann[i]*1000000)/1000000;
      vec_pri_nonforest_2crop_c4ann[i]=roundf(vec_pri_nonforest_2crop_c4ann[i]*1000000)/1000000;
      vec_pri_nonforest_2crop_c3per[i]=roundf(vec_pri_nonforest_2crop_c3per[i]*1000000)/1000000;
      vec_pri_nonforest_2crop_c4per[i]=roundf(vec_pri_nonforest_2crop_c4per[i]*1000000)/1000000;
      vec_pri_nonforest_2crop_c3nfx[i]=roundf(vec_pri_nonforest_2crop_c3nfx[i]*1000000)/1000000;
      vec_pri_nonforest_2pasture_managed[i]=roundf(vec_pri_nonforest_2pasture_managed[i]*1000000)/1000000;
      vec_pri_nonforest_2pasture_rangeland[i]=roundf(vec_pri_nonforest_2pasture_rangeland[i]*1000000)/1000000;

      vec_sec_forest_2urban[i]=roundf(vec_sec_forest_2urban[i]*1000000)/1000000;
      vec_sec_forest_2crop_c3ann[i]=roundf(vec_sec_forest_2crop_c3ann[i]*1000000)/1000000;
      vec_sec_forest_2crop_c4ann[i]=roundf(vec_sec_forest_2crop_c4ann[i]*1000000)/1000000;
      vec_sec_forest_2crop_c3per[i]=roundf(vec_sec_forest_2crop_c3per[i]*1000000)/1000000;
      vec_sec_forest_2crop_c4per[i]=roundf(vec_sec_forest_2crop_c4per[i]*1000000)/1000000;
      vec_sec_forest_2crop_c3nfx[i]=roundf(vec_sec_forest_2crop_c3nfx[i]*1000000)/1000000;
      vec_sec_forest_2pasture_managed[i]=roundf(vec_sec_forest_2pasture_managed[i]*1000000)/1000000;
      vec_sec_forest_2pasture_rangeland[i]=roundf(vec_sec_forest_2pasture_rangeland[i]*1000000)/1000000;

      vec_sec_nonforest_2urban[i]=roundf(vec_sec_nonforest_2urban[i]*1000000)/1000000;
      vec_sec_nonforest_2crop_c3ann[i]=roundf(vec_sec_nonforest_2crop_c3ann[i]*1000000)/1000000;
      vec_sec_nonforest_2crop_c4ann[i]=roundf(vec_sec_nonforest_2crop_c4ann[i]*1000000)/1000000;
      vec_sec_nonforest_2crop_c3per[i]=roundf(vec_sec_nonforest_2crop_c3per[i]*1000000)/1000000;
      vec_sec_nonforest_2crop_c4per[i]=roundf(vec_sec_nonforest_2crop_c4per[i]*1000000)/1000000;
      vec_sec_nonforest_2crop_c3nfx[i]=roundf(vec_sec_nonforest_2crop_c3nfx[i]*1000000)/1000000;
      vec_sec_nonforest_2pasture_managed[i]=roundf(vec_sec_nonforest_2pasture_managed[i]*1000000)/1000000;
      vec_sec_nonforest_2pasture_rangeland[i]=roundf(vec_sec_nonforest_2pasture_rangeland[i]*1000000)/1000000;

      vec_urban_2sec_forest[i]=roundf(vec_urban_2sec_forest[i]*1000000)/1000000;
      vec_urban_2sec_nonforest[i]=roundf(vec_urban_2sec_nonforest[i]*1000000)/1000000;
      vec_urban_2crop_c3ann[i]=roundf(vec_urban_2crop_c3ann[i]*1000000)/1000000;
      vec_urban_2crop_c4ann[i]=roundf(vec_urban_2crop_c4ann[i]*1000000)/1000000;
      vec_urban_2crop_c3per[i]=roundf(vec_urban_2crop_c3per[i]*1000000)/1000000;
      vec_urban_2crop_c4per[i]=roundf(vec_urban_2crop_c4per[i]*1000000)/1000000;
      vec_urban_2crop_c3nfx[i]=roundf(vec_urban_2crop_c3nfx[i]*1000000)/1000000;
      vec_urban_2pasture_managed[i]=roundf(vec_urban_2pasture_managed[i]*1000000)/1000000;
      vec_urban_2pasture_rangeland[i]=roundf(vec_urban_2pasture_rangeland[i]*1000000)/1000000;

      vec_crop_c3ann_2urban[i]=roundf(vec_crop_c3ann_2urban[i]*1000000)/1000000;
      vec_crop_c3ann_2sec_forest[i]=roundf(vec_crop_c3ann_2sec_forest[i]*1000000)/1000000;
      vec_crop_c3ann_2sec_nonforest[i]=roundf(vec_crop_c3ann_2sec_nonforest[i]*1000000)/1000000;
      vec_crop_c3ann_2crop_c4ann[i]=roundf(vec_crop_c3ann_2crop_c4ann[i]*1000000)/1000000;
      vec_crop_c3ann_2crop_c3per[i]=roundf(vec_crop_c3ann_2crop_c3per[i]*1000000)/1000000;
      vec_crop_c3ann_2crop_c4per[i]=roundf(vec_crop_c3ann_2crop_c4per[i]*1000000)/1000000;
      vec_crop_c3ann_2crop_c3nfx[i]=roundf(vec_crop_c3ann_2crop_c3nfx[i]*1000000)/1000000;
      vec_crop_c3ann_2pasture_managed[i]=roundf(vec_crop_c3ann_2pasture_managed[i]*1000000)/1000000;
      vec_crop_c3ann_2pasture_rangeland[i]=roundf(vec_crop_c3ann_2pasture_rangeland[i]*1000000)/1000000;

      vec_crop_c4ann_2urban[i]=roundf(vec_crop_c4ann_2urban[i]*1000000)/1000000;
      vec_crop_c4ann_2sec_forest[i]=roundf(vec_crop_c4ann_2sec_forest[i]*1000000)/1000000;
      vec_crop_c4ann_2sec_nonforest[i]=roundf(vec_crop_c4ann_2sec_nonforest[i]*1000000)/1000000;
      vec_crop_c4ann_2crop_c3ann[i]=roundf(vec_crop_c4ann_2crop_c3ann[i]*1000000)/1000000;
      vec_crop_c4ann_2crop_c3per[i]=roundf(vec_crop_c4per_2crop_c3per[i]*1000000)/1000000;
      vec_crop_c4ann_2crop_c4per[i]=roundf(vec_crop_c4ann_2crop_c4per[i]*1000000)/1000000;
      vec_crop_c4ann_2crop_c3nfx[i]=roundf(vec_crop_c4ann_2crop_c3nfx[i]*1000000)/1000000;
      vec_crop_c4ann_2pasture_managed[i]=roundf(vec_crop_c4ann_2pasture_managed[i]*1000000)/1000000;
      vec_crop_c4ann_2pasture_rangeland[i]=roundf(vec_crop_c4ann_2pasture_rangeland[i]*1000000)/1000000;

      vec_crop_c3per_2urban[i]=roundf(vec_crop_c3per_2urban[i]*1000000)/1000000;
      vec_crop_c3per_2sec_forest[i]=roundf(vec_crop_c3per_2sec_forest[i]*1000000)/1000000;
      vec_crop_c3per_2sec_nonforest[i]=roundf(vec_crop_c3per_2sec_nonforest[i]*1000000)/1000000;
      vec_crop_c3per_2crop_c3ann[i]=roundf(vec_crop_c3per_2crop_c3ann[i]*1000000)/1000000;
      vec_crop_c3per_2crop_c4ann[i]=roundf(vec_crop_c3per_2crop_c4ann[i]*1000000)/1000000;
      vec_crop_c3per_2crop_c4per[i]=roundf(vec_crop_c3per_2crop_c4per[i]*1000000)/1000000;
      vec_crop_c3per_2crop_c3nfx[i]=roundf(vec_crop_c3per_2crop_c3nfx[i]*1000000)/1000000;
      vec_crop_c3per_2pasture_managed[i]=roundf(vec_crop_c3per_2pasture_managed[i]*1000000)/1000000;
      vec_crop_c3per_2pasture_rangeland[i]=roundf(vec_crop_c3per_2pasture_rangeland[i]*1000000)/1000000;

      vec_crop_c4per_2urban[i]=roundf(vec_crop_c3per_2urban[i]*1000000)/1000000;
      vec_crop_c4per_2sec_forest[i]=roundf(vec_crop_c4per_2sec_forest[i]*1000000)/1000000;
      vec_crop_c4per_2sec_nonforest[i]=roundf(vec_crop_c4per_2sec_nonforest[i]*1000000)/1000000;
      vec_crop_c4per_2crop_c3ann[i]=roundf(vec_crop_c4per_2crop_c3ann[i]*1000000)/1000000;
      vec_crop_c4per_2crop_c4ann[i]=roundf(vec_crop_c4per_2crop_c4ann[i]*1000000)/1000000;
      vec_crop_c4per_2crop_c3per[i]=roundf(vec_crop_c4per_2crop_c3per[i]*1000000)/1000000;
      vec_crop_c4per_2crop_c3nfx[i]=roundf(vec_crop_c4per_2crop_c3nfx[i]*1000000)/1000000;
      vec_crop_c4per_2pasture_managed[i]=roundf(vec_crop_c4per_2pasture_managed[i]*1000000)/1000000;
      vec_crop_c4per_2pasture_rangeland[i]=roundf(vec_crop_c4per_2pasture_rangeland[i]*1000000)/1000000;

      vec_crop_c3nfx_2urban[i]=roundf(vec_crop_c3nfx_2urban[i]*1000000)/1000000;
      vec_crop_c3nfx_2sec_forest[i]=roundf(vec_crop_c3nfx_2sec_forest[i]*1000000)/1000000;
      vec_crop_c3nfx_2sec_nonforest[i]=roundf(vec_crop_c3nfx_2sec_nonforest[i]*1000000)/1000000;
      vec_crop_c3nfx_2crop_c3ann[i]=roundf(vec_crop_c3nfx_2crop_c3ann[i]*1000000)/1000000;
      vec_crop_c3nfx_2crop_c4ann[i]=roundf(vec_crop_c3nfx_2crop_c4ann[i]*1000000)/1000000;
      vec_crop_c3nfx_2crop_c3per[i]=roundf(vec_crop_c3nfx_2crop_c3per[i]*1000000)/1000000;
      vec_crop_c3nfx_2crop_c4per[i]=roundf(vec_crop_c3nfx_2crop_c4per[i]*1000000)/1000000;
      vec_crop_c3nfx_2pasture_managed[i]=roundf(vec_crop_c3nfx_2pasture_managed[i]*1000000)/1000000;
      vec_crop_c3nfx_2pasture_rangeland[i]=roundf(vec_crop_c3nfx_2pasture_rangeland[i]*1000000)/1000000;

      vec_pasture_managed_2urban[i]=roundf(vec_pasture_managed_2urban[i]*1000000)/1000000;
      vec_pasture_managed_2sec_forest[i]=roundf(vec_pasture_managed_2sec_forest[i]*1000000)/1000000;
      vec_pasture_managed_2sec_nonforest[i]=roundf(vec_pasture_managed_2sec_nonforest[i]*1000000)/1000000;
      vec_pasture_managed_2crop_c3ann[i]=roundf(vec_pasture_managed_2crop_c3ann[i]*1000000)/1000000;
      vec_pasture_managed_2crop_c4ann[i]=roundf(vec_pasture_managed_2crop_c4ann[i]*1000000)/1000000;
      vec_pasture_managed_2crop_c3per[i]=roundf(vec_pasture_managed_2crop_c3per[i]*1000000)/1000000;
      vec_pasture_managed_2crop_c4per[i]=roundf(vec_pasture_managed_2crop_c4per[i]*1000000)/1000000;
      vec_pasture_managed_2crop_c3nfx[i]=roundf(vec_pasture_managed_2crop_c3nfx[i]*1000000)/1000000;
      vec_pasture_managed_2pasture_rangeland[i]=roundf(vec_pasture_managed_2pasture_rangeland[i]*1000000)/1000000;

      vec_pasture_rangeland_2urban[i]=roundf(vec_pasture_rangeland_2urban[i]*1000000)/1000000;
      vec_pasture_rangeland_2sec_forest[i]=roundf(vec_pasture_rangeland_2sec_forest[i]*1000000)/1000000;
      vec_pasture_rangeland_2sec_nonforest[i]=roundf(vec_pasture_rangeland_2sec_nonforest[i]*1000000)/1000000;
      vec_pasture_rangeland_2crop_c3ann[i]=roundf(vec_pasture_rangeland_2crop_c3ann[i]*1000000)/1000000;
      vec_pasture_rangeland_2crop_c4ann[i]=roundf(vec_pasture_rangeland_2crop_c4ann[i]*1000000)/1000000;
      vec_pasture_rangeland_2crop_c3per[i]=roundf(vec_pasture_rangeland_2crop_c3per[i]*1000000)/1000000;
      vec_pasture_rangeland_2crop_c4per[i]=roundf(vec_pasture_rangeland_2crop_c4per[i]*1000000)/1000000;
      vec_pasture_rangeland_2crop_c3nfx[i]=roundf(vec_pasture_rangeland_2crop_c3nfx[i]*1000000)/1000000;
      vec_pasture_rangeland_2pasture_managed[i]=roundf(vec_pasture_rangeland_2pasture_managed[i]*1000000)/1000000;

      //-------------------------------------
      // combine and reduce transitions
      //-------------------------------------
      // ..convert to int, round, then convert to float to avoid floating-point errors
      // ..fraction of ag (crop + pasture) converted to secondary forests in cell
      //.......................
      // LU fractions
      //.......................
      landuse->PriForest_frac[j]=round((vec_pri_forest_frac[i]+vec_pri_nonforest_frac[i])*1000000)/1000000;
      landuse->SecForest_frac[j]=round((vec_urban_frac[i]+vec_sec_forest_frac[i]+vec_sec_nonforest_frac[i])*1000000)/1000000;
      landuse->crop_frac[j]=round((vec_crop_c3ann_frac[i]+
                                   vec_crop_c4ann_frac[i]+
                                   vec_crop_c3per_frac[i]+
                                   vec_crop_c4per_frac[i]+
                                   vec_crop_c3nfx_frac[i])*1000000)/1000000;
      landuse->pasture_frac[j]=round((vec_pasture_managed_frac[i]+vec_pasture_rangeland_frac[i])*1000000)/1000000;

      //.......................
      // deforest transitions
      //.......................
      landuse->PriForest2crop_frac[j]=round((vec_pri_forest_2crop_c3ann[i]+
                                             vec_pri_forest_2crop_c4ann[i]+
                                             vec_pri_forest_2crop_c3per[i]+
                                             vec_pri_forest_2crop_c4per[i]+
                                             vec_pri_forest_2crop_c3nfx[i]+
                                             vec_pri_nonforest_2crop_c3ann[i]+
                                             vec_pri_nonforest_2crop_c4ann[i]+
                                             vec_pri_nonforest_2crop_c3per[i]+
                                             vec_pri_nonforest_2crop_c4per[i]+
                                             vec_pri_nonforest_2crop_c3nfx[i])*1000000)/1000000;
      landuse->PriForest2pasture_frac[j]=round((vec_pri_forest_2pasture_managed[i]+vec_pri_forest_2pasture_rangeland[i])*1000000)/1000000;

      landuse->SecForest2crop_frac[j]=round((vec_sec_forest_2crop_c3ann[i]+
                                             vec_sec_forest_2crop_c4ann[i]+
                                             vec_sec_forest_2crop_c3per[i]+
                                             vec_sec_forest_2crop_c4per[i]+
                                             vec_sec_forest_2crop_c3nfx[i]+
                                             vec_sec_nonforest_2crop_c3ann[i]+
                                             vec_sec_nonforest_2crop_c4ann[i]+
                                             vec_sec_nonforest_2crop_c3per[i]+
                                             vec_sec_nonforest_2crop_c4per[i]+
                                             vec_sec_nonforest_2crop_c3nfx[i])*1000000)/1000000;

      landuse->SecForest2pasture_frac[j]=round((vec_urban_2crop_c3ann[i]+
                                                vec_urban_2crop_c4ann[i]+
                                                vec_urban_2crop_c3per[i]+
                                                vec_urban_2crop_c4per[i]+
                                                vec_urban_2crop_c3nfx[i]+
                                                vec_urban_2pasture_managed[i]+
                                                vec_urban_2pasture_rangeland[i]+
                                                vec_sec_forest_2pasture_managed[i]+
                                                vec_sec_forest_2pasture_rangeland[i])*1000000)/1000000;
      //............................
      // inter-lu transitions
      //............................
      landuse->crop2pasture_frac[j]=round((vec_crop_c3ann_2pasture_managed[i]+
                                           vec_crop_c4ann_2pasture_managed[i]+
                                           vec_crop_c3per_2pasture_managed[i]+
                                           vec_crop_c4per_2pasture_managed[i]+
                                           vec_crop_c3nfx_2pasture_managed[i]+
                                           vec_crop_c3ann_2pasture_managed[i]+
                                           vec_crop_c3ann_2pasture_rangeland[i]+
                                           vec_crop_c4ann_2pasture_rangeland[i]+
                                           vec_crop_c3per_2pasture_rangeland[i]+
                                           vec_crop_c4per_2pasture_rangeland[i]+
                                           vec_crop_c3nfx_2pasture_rangeland[i])*1000000)/1000000;
      landuse->pasture2crop_frac[j]=round((vec_pasture_managed_2crop_c3ann[i]+
                                           vec_pasture_managed_2crop_c4ann[i]+
                                           vec_pasture_managed_2crop_c3per[i]+
                                           vec_pasture_managed_2crop_c4per[i]+
                                           vec_pasture_managed_2crop_c3nfx[i]+
                                           vec_pasture_rangeland_2crop_c3ann[i]+
                                           vec_pasture_rangeland_2crop_c4ann[i]+
                                           vec_pasture_rangeland_2crop_c3per[i]+
                                           vec_pasture_rangeland_2crop_c4per[i]+
                                           vec_pasture_rangeland_2crop_c3nfx[i])*1000000)/1000000;
      //............................
      // abandonment transitions
      //............................
      landuse->crop2SecForest_frac[j]=round((vec_crop_c3ann_2sec_forest[i]+
                                             vec_crop_c4ann_2sec_forest[i]+
                                             vec_crop_c3per_2sec_forest[i]+
                                             vec_crop_c4per_2sec_forest[i]+
                                             vec_crop_c3nfx_2sec_forest[i]+
                                             vec_crop_c3ann_2sec_nonforest[i]+
                                             vec_crop_c4ann_2sec_nonforest[i]+
                                             vec_crop_c3per_2sec_nonforest[i]+
                                             vec_crop_c4per_2sec_nonforest[i]+
                                             vec_crop_c3nfx_2sec_nonforest[i])*1000000)/1000000;
      landuse->pasture2SecForest_frac[j]=round((vec_urban_2sec_forest[i]+
                                                vec_urban_2sec_nonforest[i]+
                                                vec_pasture_managed_2sec_forest[i]+
                                                vec_pasture_rangeland_2sec_forest[i]+
                                                vec_pasture_managed_2sec_nonforest[i]+
                                                vec_pasture_rangeland_2sec_nonforest[i])*1000000)/1000000;
      //............................
      // wood harvest transitions
      //............................
      landuse->WHarvestPrimary_frac[j]=round(vec_WHarvest_primary[i]*1000000)/1000000;
      landuse->WHarvestPrimaryNonForest_frac[j]=round(vec_WHarvest_primaryNonForest[i]*1000000)/1000000;
      landuse->WHarvestSecForestOld_frac[j]=round(vec_WHarvest_secforestOld[i]*1000000)/1000000;
      landuse->WHarvestSecForestYoung_frac[j]=round(vec_WHarvest_secforestYoung[i]*1000000)/1000000;
      landuse->WHarvestSecForestNonForest_frac[j]=round(vec_WHarvest_secforestNonForest[i]*1000000)/1000000;
      landuse->WHarvestPrimary_biomass[j]=round(vec_WHarvest_biomass_primary[i]*1000000)/1000000;
      landuse->WHarvestPrimaryNonForest_biomass[j]=round(vec_WHarvest_biomass_primaryNonForest[i]*1000000)/1000000;
      landuse->WHarvestSecForestOld_biomass[j]=round(vec_WHarvest_biomass_secforestOld[i]*1000000)/1000000;
      landuse->WHarvestSecForestYoung_biomass[j]=round(vec_WHarvest_biomass_secforestYoung[i]*1000000)/1000000;
      landuse->WHarvestSecForestNonForest_biomass[j]=round(vec_WHarvest_biomass_secforestNonForest[i]*1000000)/1000000;

      landuse->WHarvest_roundwood_frac[j]=round(vec_WHarvest_roundwood_frac[i]*1000000)/1000000;
      landuse->WHarvest_fuelwood_frac[j]=round(vec_WHarvest_fuelwood_frac[i]*1000000)/1000000;
      landuse->WHarvest_biofuel_frac[j]=round(vec_WHarvest_biofuel_frac[i]*1000000)/1000000;

      //get total landuse fraction
      landuse->SummLU_frac[j]=landuse->crop_frac[j]+landuse->pasture_frac[j]+ landuse->PriForest_frac[j]+landuse->SecForest_frac[j];

      #ifdef GROSS_ONESTAND
        landuse->SecForest_frac[j] += landuse->PriForest_frac[j];
        landuse->SecForest2pasture_frac[j] += landuse->PriForest2pasture_frac[j];
        landuse->SecForest2crop_frac[j] += landuse->PriForest2crop_frac[j];

        landuse->WHarvestSecForestOld_frac[j] += landuse->WHarvestPrimary_frac[j];
        landuse->WHarvestSecForestNonForest_frac[j] += landuse->WHarvestPrimaryNonForest_frac[j];
        landuse->WHarvestSecForestOld_biomass[j] += landuse->WHarvestPrimary_biomass[j];
        landuse->WHarvestSecForestNonForest_biomass[j] += landuse->WHarvestPrimaryNonForest_biomass[j];

        //set primary to zero
        landuse->PriForest_frac[j]=landuse->PriForest2pasture_frac[j]=landuse->PriForest2crop_frac[j]=0;
        landuse->WHarvestPrimary_frac[j]=landuse->WHarvestPrimaryNonForest_frac[j]=0;
        landuse->WHarvestPrimary_biomass[j]=landuse->WHarvestPrimaryNonForest_biomass[j]=0;

        #if defined(NETinGROSS)
          //simplify managed lands, crop & pasture are treated the same (Sept 2016)
          landuse->SecForest2crop_frac[j]+=landuse->SecForest2pasture_frac[j];
          landuse->crop2SecForest_frac[j] += landuse->pasture2SecForest_frac[j];
          //zero-out pasture fractions
          landuse->SecForest2pasture_frac[j]=landuse->pasture2SecForest_frac[j]=0;

          //determine if (i) net abandonment or (ii) deforestation or (iii) no change
          temp_net= landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

          if(temp_net > 0){
            //net deforestation
            landuse->SecForest2crop_frac[j]=temp_net;
            landuse->crop2SecForest_frac[j]=0;
          }else if(temp_net < 0){
            //net abandonment
            landuse->SecForest2crop_frac[j]=0;
            landuse->crop2SecForest_frac[j]=temp_net;
          }else{
            //no change
            landuse->SecForest2crop_frac[j]=0;
            landuse->crop2SecForest_frac[j]=0;
          }
        #endif
      #elif defined(NETinGROSS)
        /////////////////////////////////////////////////////////////////
        // // N E T - L U C  in  G R O S S framework
        /////////////////////////////////////////////////////////////////
        //--------------------------------
        //.. two alternate net implmentations
        //.. (1) calculate net first between deforest primary and abandon mgLand
        //.. (2) calculate net first between deforest secondary and abandon mgLand (default)
        //.. key for both is to use primary forest_nonForest harvest Fraction to maintain Pri-Sec fractions
        //--------------------------------
        //update stand fractions so that only crop_frac is used (not pasture_frac)
        landuse->crop_frac[j]+=landuse->pasture_frac[j];
        landuse->pasture_frac[j]=0.0;

        //MgLand inter-conversion
        //Null in this LPJ version bc crop/pasture is grassland(same)
        landuse->crop2pasture_frac[j]=0.0;
        landuse->pasture2crop_frac[j]=0.0;

        //LU conversions, simplify
        landuse->crop2SecForest_frac[j]+=landuse->pasture2SecForest_frac[j];
        landuse->pasture2SecForest_frac[j]=0.0;
        landuse->PriForest2crop_frac[j]+=landuse->PriForest2pasture_frac[j];
        landuse->PriForest2pasture_frac[j]=0.0;
        landuse->SecForest2crop_frac[j]+=landuse->SecForest2pasture_frac[j];
        landuse->SecForest2pasture_frac[j]=0.0;

        #if defined(NOWDHARVEST)
          //Update WoodHarvest Transitions (primary-> secondary)
          landuse->WHarvestPrimary_frac[j]+=landuse->WHarvestPrimaryNonForest_frac[j];
          //no WoodHarvest Transitions for Secondary Forests
          landuse->WHarvestSecForestOld_frac[j]=landuse->WHarvestSecForestYoung_frac[j]=landuse->WHarvestSecForestNonForest_frac[j]=0.0;

          #ifdef ALT_NETLUC_PRIMARY
            //ALT#1: reduce the primary fraction deforest first, then adjust the secondary fraction deforested

            //Determine if net deforestation or abandonment??
            temp_net=landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

            if(temp_net > 0){//net deforestation on primary (but potentially smaller than actual)
              //do nothing to mgland -- it only increases in fractional size
              landuse->crop2SecForest_frac[j]=0.0;
              //maintain fractions of primary and secondary forest
              //separate the net deforest fraction from non-deforested fraction
              landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j]-temp_net;
              //update fraction for deforestation
              landuse->PriForest2crop_frac[j]=temp_net;

            }else if(temp_net < 0){//net abandonment
              landuse->crop2SecForest_frac[j]=temp_net*-1;
              landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j];
            }else{//zero change
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->PriForest2crop_frac[j]=0.0;
            }

            if(landuse->SecForest2crop_frac[j] > 0){
              if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
                temp_net_remain= landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
                if(temp_net_remain >= 0){//net deforestation
                  landuse->SecForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                  landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
                }else{//net abandonment even with newly deforested primary forest
                  landuse->SecForest2crop_frac[j]=0.0;//no deforested fraction
                  landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
                }
              }else{
                //net deforest secondary
              }
            }
          #else //ALT#2 (default): reduce the secondary fraction deforest first, then adjust the primary fraction deforested
            //Determine if net deforestation or abandonment??
            temp_net=landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

            if(temp_net > 0){//net deforestation
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->SecForest2crop_frac[j]=temp_net;
            }else if(temp_net < 0){//net abandonment
              landuse->crop2SecForest_frac[j]=temp_net*-1;
              landuse->SecForest2crop_frac[j]=0.0;
            }else{//zero change
              landuse->crop2SecForest_frac[j]=0.0;
              landuse->SecForest2crop_frac[j]=0.0;
            }

            if(landuse->PriForest2crop_frac[j] > 0){
              if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
                temp_net_remain= landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
                if(temp_net_remain >= 0){//net deforestation
                  landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j] - temp_net_remain;//net increase in secforest through mixing
                  landuse->PriForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                  landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
                }else{//net abandonment even with newly deforested primary forest
                  landuse->WHarvestPrimary_frac[j]+=landuse->PriForest2crop_frac[j];//net increase in secforest through mixing
                  landuse->PriForest2crop_frac[j]=0.0;//no deforested fraction
                  landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
                }
              }else{//net deforest primary
                //woodHarvest fraction used to merge primary with secforest
                //--without deforestation or change in biomass
                //landuse->WHarvestPrimary_frac[j]=0.0;
              }
            }
          #endif //end ALTERNATE NET IMPLEMENTATION -- default is the SECONDARY NET first, then PRIMARY NET


          //Wood Harvest Fractions
          //landuse->WHarvestPrimary_frac[j]=0.0;
          landuse->WHarvestPrimaryNonForest_frac[j]=0.0;
          landuse->WHarvestSecForestOld_frac[j]=0.0;
          landuse->WHarvestSecForestYoung_frac[j]=0.0;
          landuse->WHarvestSecForestNonForest_frac[j]= 0.0;

          //Wood Harvest Mass
          landuse->WHarvestPrimary_biomass[j]=0.0;
          landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
          landuse->WHarvestSecForestOld_biomass[j]=0.0;
          landuse->WHarvestSecForestYoung_biomass[j]=0.0;
          landuse->WHarvestSecForestNonForest_biomass[j]=0.0;

        #else //end if NOWDHARVEST (in NETinGROSS)

          //--------------------------------
          //key difference in netHybrid...
          //.. use primary forest_nonForest Fraction to maintain Pri-Sec fractions

          //Update WoodHarvest Transitions (primary-> secondary)
          landuse->WHarvestPrimary_frac[j]+=landuse->WHarvestPrimaryNonForest_frac[j];

          //set to zero
          landuse->WHarvestPrimaryNonForest_frac[j]=0.0;

          //WoodHarvest Fractions for Secondary Forests
          landuse->WHarvestSecForestOld_frac[j]+=landuse->WHarvestSecForestYoung_frac[j]+landuse->WHarvestSecForestNonForest_frac[j];
            landuse->WHarvestSecForestYoung_frac[j]=0.0;
            landuse->WHarvestSecForestNonForest_frac[j]=0.0;

          //Determine if net deforestation or abandonment??
          temp_net=landuse->SecForest2crop_frac[j] - landuse->crop2SecForest_frac[j];

          if(temp_net > 0){//net deforestation
            landuse->crop2SecForest_frac[j]=0.0;
            landuse->SecForest2crop_frac[j]=temp_net;
          }else if(temp_net < 0){//net abandonment
            landuse->crop2SecForest_frac[j]=temp_net*-1;
            landuse->SecForest2crop_frac[j]=0.0;
          }else{//zero change
            landuse->crop2SecForest_frac[j]=0.0;
            landuse->SecForest2crop_frac[j]=0.0;
          }

          if(landuse->PriForest2crop_frac[j] > 0){
            if(landuse->crop2SecForest_frac[j] > 0){//if there is net abandonment && new mgLand is created from primary forest
              temp_net_remain= landuse->PriForest2crop_frac[j] - landuse->crop2SecForest_frac[j];
              if(temp_net_remain >= 0){//net deforestation
                landuse->WHarvestPrimaryNonForest_frac[j]+=landuse->PriForest2crop_frac[j] - temp_net_remain;//net increase in secforest through mixing
                landuse->PriForest2crop_frac[j]=temp_net_remain;//net deforested fraction
                landuse->crop2SecForest_frac[j]=0.0;//no net abandonment
              }else{//net abandonment even with newly deforested primary forest
                landuse->WHarvestPrimaryNonForest_frac[j]+=landuse->PriForest2crop_frac[j];//net increase in secforest through mixing
                landuse->PriForest2crop_frac[j]=0.0;//no deforested fraction
                landuse->crop2SecForest_frac[j]=temp_net_remain*-1;//net abandoned fraction
              }
            }else{//net deforest primary
              //woodHarvest fraction used to merge primary with secforest
              //--without deforestation or change in biomass
              //landuse->WHarvestPrimary_frac[j]=0.0;
            }
          }

          //Wood Harvest Mass
          landuse->WHarvestPrimary_biomass[j]+=landuse->WHarvestPrimaryNonForest_biomass[j];
            //set to zero; primary nonForest_fraction is used to maintain pri-sec ratio
            landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
          landuse->WHarvestSecForestOld_biomass[j]+=landuse->WHarvestSecForestYoung_biomass[j]+landuse->WHarvestSecForestNonForest_biomass[j];
            landuse->WHarvestSecForestYoung_biomass[j]=0.0;
            landuse->WHarvestSecForestNonForest_biomass[j]=0.0;

        #endif//end WoodHarvest
      #endif
      //---------------------------------------
      //   end GROSS_ONESTAND && NETinGROSS
      //---------------------------------------
      #ifdef NOWDHARVEST
        //NO Wood Harvest, so set biomass values to zero
        //..but keep stand fractions as these maintain the primary/secondary LU fractions in data (primary -> secondary)
        landuse->WHarvestSecForestOld_biomass[j]=0.0;
        landuse->WHarvestSecForestYoung_biomass[j]=0.0;
        landuse->WHarvestSecForestNonForest_biomass[j]=0.0;
        landuse->WHarvestPrimary_biomass[j]=0.0;
        landuse->WHarvestPrimaryNonForest_biomass[j]=0.0;
      #endif

    }//end for each band - deprecated for Gross-LUC
  }//end for each cell


  //free memory in vector
  free(vec_urban_frac);
  free(vec_pri_forest_frac);
  free(vec_pri_nonforest_frac);
  free(vec_sec_forest_frac);
  free(vec_sec_nonforest_frac);
  free(vec_crop_c3ann_frac);
  free(vec_crop_c4ann_frac);
  free(vec_crop_c3per_frac);
  free(vec_crop_c4per_frac);
  free(vec_crop_c3nfx_frac);
  free(vec_pasture_managed_frac);
  free(vec_pasture_rangeland_frac);

  free(vec_pri_forest_2urban);
  free(vec_pri_forest_2crop_c3ann);
  free(vec_pri_forest_2crop_c4ann);
  free(vec_pri_forest_2crop_c3per);
  free(vec_pri_forest_2crop_c4per);
  free(vec_pri_forest_2crop_c3nfx);
  free(vec_pri_forest_2pasture_managed);
  free(vec_pri_forest_2pasture_rangeland);
  free(vec_pri_nonforest_2urban);
  free(vec_pri_nonforest_2crop_c3ann);
  free(vec_pri_nonforest_2crop_c4ann);
  free(vec_pri_nonforest_2crop_c3per);
  free(vec_pri_nonforest_2crop_c4per);
  free(vec_pri_nonforest_2crop_c3nfx);
  free(vec_pri_nonforest_2pasture_managed);
  free(vec_pri_nonforest_2pasture_rangeland);

  free(vec_sec_forest_2urban);
  free(vec_sec_forest_2crop_c3ann);
  free(vec_sec_forest_2crop_c4ann);
  free(vec_sec_forest_2crop_c3per);
  free(vec_sec_forest_2crop_c4per);
  free(vec_sec_forest_2crop_c3nfx);
  free(vec_sec_forest_2pasture_managed);
  free(vec_sec_forest_2pasture_rangeland);
  free(vec_sec_nonforest_2urban);
  free(vec_sec_nonforest_2crop_c3ann);
  free(vec_sec_nonforest_2crop_c4ann);
  free(vec_sec_nonforest_2crop_c3per);
  free(vec_sec_nonforest_2crop_c4per);
  free(vec_sec_nonforest_2crop_c3nfx);
  free(vec_sec_nonforest_2pasture_managed);
  free(vec_sec_nonforest_2pasture_rangeland);

  free(vec_urban_2sec_forest);
  free(vec_urban_2sec_nonforest);
  free(vec_urban_2crop_c3ann);
  free(vec_urban_2crop_c4ann);
  free(vec_urban_2crop_c3per);
  free(vec_urban_2crop_c4per);
  free(vec_urban_2crop_c3nfx);
  free(vec_urban_2pasture_managed);
  free(vec_urban_2pasture_rangeland);

  free(vec_crop_c3ann_2urban);
  free(vec_crop_c3ann_2sec_forest);
  free(vec_crop_c3ann_2sec_nonforest);
  free(vec_crop_c3ann_2crop_c4ann);
  free(vec_crop_c3ann_2crop_c3per);
  free(vec_crop_c3ann_2crop_c4per);
  free(vec_crop_c3ann_2crop_c3nfx);
  free(vec_crop_c3ann_2pasture_managed);
  free(vec_crop_c3ann_2pasture_rangeland);

  free(vec_crop_c4ann_2urban);
  free(vec_crop_c4ann_2sec_forest);
  free(vec_crop_c4ann_2sec_nonforest);
  free(vec_crop_c4ann_2crop_c3ann);
  free(vec_crop_c4ann_2crop_c3per);
  free(vec_crop_c4ann_2crop_c4per);
  free(vec_crop_c4ann_2crop_c3nfx);
  free(vec_crop_c4ann_2pasture_managed);
  free(vec_crop_c4ann_2pasture_rangeland);

  free(vec_crop_c3per_2urban);
  free(vec_crop_c3per_2sec_forest);
  free(vec_crop_c3per_2sec_nonforest);
  free(vec_crop_c3per_2crop_c3ann);
  free(vec_crop_c3per_2crop_c4ann);
  free(vec_crop_c3per_2crop_c4per);
  free(vec_crop_c3per_2crop_c3nfx);
  free(vec_crop_c3per_2pasture_managed);
  free(vec_crop_c3per_2pasture_rangeland);

  free(vec_crop_c4per_2urban);
  free(vec_crop_c4per_2sec_forest);
  free(vec_crop_c4per_2sec_nonforest);
  free(vec_crop_c4per_2crop_c3ann);
  free(vec_crop_c4per_2crop_c4ann);
  free(vec_crop_c4per_2crop_c3per);
  free(vec_crop_c4per_2crop_c3nfx);
  free(vec_crop_c4per_2pasture_managed);
  free(vec_crop_c4per_2pasture_rangeland);

  free(vec_crop_c3nfx_2urban);
  free(vec_crop_c3nfx_2sec_forest);
  free(vec_crop_c3nfx_2sec_nonforest);
  free(vec_crop_c3nfx_2crop_c3ann);
  free(vec_crop_c3nfx_2crop_c4ann);
  free(vec_crop_c3nfx_2crop_c3per);
  free(vec_crop_c3nfx_2crop_c4per);
  free(vec_crop_c3nfx_2pasture_managed);
  free(vec_crop_c3nfx_2pasture_rangeland);

  free(vec_pasture_managed_2urban);
  free(vec_pasture_managed_2sec_forest);
  free(vec_pasture_managed_2sec_nonforest);
  free(vec_pasture_managed_2crop_c3ann);
  free(vec_pasture_managed_2crop_c4ann);
  free(vec_pasture_managed_2crop_c3per);
  free(vec_pasture_managed_2crop_c4per);
  free(vec_pasture_managed_2crop_c3nfx);
  free(vec_pasture_managed_2pasture_rangeland);

  free(vec_pasture_rangeland_2urban);
  free(vec_pasture_rangeland_2sec_forest);
  free(vec_pasture_rangeland_2sec_nonforest);
  free(vec_pasture_rangeland_2crop_c3ann);
  free(vec_pasture_rangeland_2crop_c4ann);
  free(vec_pasture_rangeland_2crop_c3per);
  free(vec_pasture_rangeland_2crop_c4per);
  free(vec_pasture_rangeland_2crop_c3nfx);
  free(vec_pasture_rangeland_2pasture_managed);

  free(vec_WHarvest_primary);
  free(vec_WHarvest_primaryNonForest);
  free(vec_WHarvest_secforestOld);
  free(vec_WHarvest_secforestYoung);
  free(vec_WHarvest_secforestNonForest);

  free(vec_WHarvest_biomass_primary);
  free(vec_WHarvest_biomass_primaryNonForest);
  free(vec_WHarvest_biomass_secforestOld);
  free(vec_WHarvest_biomass_secforestYoung);
  free(vec_WHarvest_biomass_secforestNonForest);

  free(vec_WHarvest_roundwood_frac);
  free(vec_WHarvest_fuelwood_frac);
  free(vec_WHarvest_biofuel_frac);
}//end of get_LUH_V2


//------------------------------------
// main function
// ..calls hyper-functions
//------------------------------------
Bool getlanduse_GROSS_ncdf(Config config, Landuse *landuse,int year, int ncell,int ncft, Real latIn[], Real lonIn[])
{
  //////////////////////////////////////////////////////////////////////////////////
  //  reads in the Land Use Harmonization Dataset
  //  fractions for multiple transitions stored as vectors in landuse structrue
  /////////////////////////////////////////////////////////////////////////////////
  #ifdef NAT_VEG
    int cell;

    for(cell=0;cell<ncell;cell++){
      //sum=0;

      //workaround to calculate exclusively natural vegetation

      //stand fractions
      landuse->crop_frac[cell]=0;
      landuse->SecForest_frac[cell]=0;
      landuse->PriForest_frac[cell]=0;
      landuse->pasture_frac[cell]=0;
      landuse->SummLU_frac[cell]=0;

      //MgLand inter-conversion
      landuse->crop2pasture_frac[cell]=0;
      landuse->pasture2crop_frac[cell]=0;

      //LU conversions
      landuse->crop2SecForest_frac[cell]=0;
      landuse->pasture2SecForest_frac[cell]=0;
      landuse->SecForest2crop_frac[cell]=0;
      landuse->SecForest2pasture_frac[cell]=0;
      landuse->PriForest2crop_frac[cell]=0;
      landuse->PriForest2pasture_frac[cell]=0;

      //Wood Harvest
      landuse->WHarvestPrimary_frac[cell]=0;
      landuse->WHarvestPrimaryNonForest_frac[cell]=0;
      landuse->WHarvestSecForestOld_frac[cell]=0;
      landuse->WHarvestSecForestYoung_frac[cell]=0;
      landuse->WHarvestSecForestNonForest_frac[cell]=0;
    }//end of NAT_VEG

  #else
    //int i, j, retval, latMatch[ncell], lonMatch[ncell];//as in getlanduse_ncdf.c
    int latMatch[ncell], lonMatch[ncell];//as in getlanduse_ncdf.c
    #if defined(CONST_LU1860) || defined(CONST_LU1700) || defined(CONST_LU1500)
    int cell;
    #endif

    //-----------------------------------
    // get cell ids by matching lon/lat
    //-----------------------------------
    //Set up NCDF read arrays
    //initialize start/count arrays
    //TEMPsize_t start[3], count[3];

    //Match lat lon from ingrid to netcdf file ncdf
    //IMPORTANT Assumes all climate files are on the same grid!
    getcellID(landuse->ncid_luc, ncell, latIn, lonIn, latMatch, lonMatch);

    //-----------------------------------------------------
    // CONSTLU: read-in data only to set LU fractions
    //        ..after LU fractions are set
    //      ..in years 0,1 of landuse_spinup
    //      ..then set LU transitions to zero
    //      ..and skip reading data (slow for LUH_V2)
    //  ..runtime is reduced from 8 to <1 hr for LU spinup
    //-----------------------------------------------------
    #if defined(CONST_LU1860) || defined(CONST_LU1700) || defined(CONST_LU1500)
    if( (config.firstyear==config.lastyear) && 
        ((year == config.firstyear-config.nspinup) || (year == config.firstyear-config.nspinup+1)))
    {//..if landuse_spinup and first or second year of run..
    #endif

    //-----------------------------------
    //set year to index of LUC file
    //-----------------------------------
    //To match the first year of simulation with the land use year, i.e.,1901-1850 = 51 to start reading
    //For land use initialization runs, set year manually for land use initialization and for TRENDY transient runs
    //year=2005-1500; //For Hurtt, 1500 is constant, change to 1860 or 2005 or ignore

    //year is brought in from iterate.c as (year=config.firstyear-config.nspinup year++)
 
    //use CONST_LUXXXX for constant landuse, either for spinup or with transient NAT_VEG (LU fixed)
    #ifdef CONST_LU1860
      year =1860-landuse->firstyear;
    #elif CONST_LU1700
      year =1700-landuse->firstyear;        
    #elif CONST_LU1500
      year =1500-landuse->firstyear;
    #elif LU2015 
      //version future
      if(year < 2015){
        year =2015-landuse->firstyear;
      }else{
        //NOTE: if year is past last year in data, then LPJ will fail
        year-=landuse->firstyear;
      }
    #elif LU1860
      if(year < 1860){
        //spin_luc allows equilibrium state at first year before transient runs occur
        //don't start LUC (secondary forest) until year is YYYY (all years in spin-up are < YYYY)
        year =1860-landuse->firstyear;
      }else{
        //NOTE: if year is past last year in data, then LPJ will fail
        year-=landuse->firstyear;
        //year-=landuse->firstyear;//LUC netcdf starts at 0, therefore year passed to LUC is relative to firstyear
      }
    #elif LU1700
      if(year < 1700){
        year =1700-landuse->firstyear;
      }else{
        //NOTE: if year is past last year in data, then LPJ will fail
        year-=landuse->firstyear;
      }
    #elif LU1500
      if(year < 1500){
        year =1500-landuse->firstyear;
      }else{
        //NOTE: if year is past last year in data, then LPJ will fail
        year-=landuse->firstyear;
      }
    #else
      fail("ERROR..no flag specified for landuse start year, use LU1500 OR LU1860");
    #endif
    //year set to index


    #if defined(LUH_V2) || defined(LUH_V2_F)
      //------------------------------------------------
      // LUH v2 Dataset:  Gross only (for now May 2017)
      // ..version future has same format as version2
      //------------------------------------------------
      get_LUH_V2(landuse,year,ncell,ncft,latMatch,lonMatch);
    #elif LUH_V1
      //------------------------------------------------
      // LUH v1 Dataset:  Gross and Net-Hybrid
      //------------------------------------------------
      get_LUH_V1(landuse,year,ncell,ncft,latMatch,lonMatch);
    #else //compiler check
      fail("ERROR specify flag for the landusechange driver data {LUH_V1,LUH_V2}");
    #endif


    #if defined(CONST_LU1860) || defined(CONST_LU1700) || defined(CONST_LU1500) 
    }else{
      //if not first year or second year in landuse_spinup
      //..then set transitions to zero
      //..and don't read the LUC data; fast landuse_spinup solution
      for(cell=0;cell<ncell;cell++){
        //stand fractions
        landuse->crop_frac[cell]=0;
        landuse->SecForest_frac[cell]=0;
        landuse->PriForest_frac[cell]=0;
        landuse->pasture_frac[cell]=0;
        landuse->SummLU_frac[cell]=0;

        //MgLand inter-conversion
        landuse->crop2pasture_frac[cell]=0;
        landuse->pasture2crop_frac[cell]=0;

        //LU conversions
        landuse->crop2SecForest_frac[cell]=0;
        landuse->pasture2SecForest_frac[cell]=0;
        landuse->SecForest2crop_frac[cell]=0;
        landuse->SecForest2pasture_frac[cell]=0;
        landuse->PriForest2crop_frac[cell]=0;
        landuse->PriForest2pasture_frac[cell]=0;

        //Wood Harvest
        landuse->WHarvestPrimary_frac[cell]=0;
        landuse->WHarvestPrimaryNonForest_frac[cell]=0;
        landuse->WHarvestSecForestOld_frac[cell]=0;
        landuse->WHarvestSecForestYoung_frac[cell]=0;
        landuse->WHarvestSecForestNonForest_frac[cell]=0;

        landuse->WHarvestPrimary_biomass[cell]=0;
        landuse->WHarvestPrimaryNonForest_biomass[cell]=0;
        landuse->WHarvestSecForestOld_biomass[cell]=0;
        landuse->WHarvestSecForestYoung_biomass[cell]=0;
        landuse->WHarvestSecForestNonForest_biomass[cell]=0;
      }//..end cell loop
    }
    #endif  //..end constant landuse

  #endif//end TRANSIENT getlanduse

  return FALSE;
} /* of 'getlanduse' */

/*
- order of landuse input data: 
  0-10       11        12-22            23     
  CFT   PASTURE      CFT_irr   PASTURE_irr

- called in iterate()
- reads every year the fractions of the bands for all cells from the input file
- checks if sum of fraction is not greater 1
  -> if sum of fraction is greater 1: subtraction from fraction of managed grass if possible
  -> else fail incorrect input file
*/
