/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c l i m a t e _ d a i l y _ n c d f . c   **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Bool getclimate_daily_ncdf(Climate *climate,int year, Real latIn[], Real lonIn[], int ncell)
{
  char  *varTmp, *varPre, *varSw, *varLw;
  Real *vec_clm;  //Globcarbon daily input
  Real vec_temp;
  int i, j;
  int varid_tmp, varid_pre, varid_sw, varid_lw, retval;
  int latMatch[ncell], lonMatch[ncell];
  size_t start[3], count[3];

  //initialize for compiler
  varTmp=varPre=varSw=varLw="dummy";

  vec_clm=newvec(Real,NDAYYEAR);
  check(vec_clm);

  #ifdef CRUNCEP
    //Variable names for CRU-NCEP data
    varTmp="tair";
    varPre="rain";
    varSw="swdown";
    varLw="lwdown";
    
    year-=climate->firstyear;
  #endif
  
  #ifdef ERA
    varTmp="t2m";
    varPre="tp";
    varLw="strd";
    varSw="ssrd";
    
    year-=climate->firstryear;
  #endif
  
  #ifdef MERRA2
    varTmp="TLML";
    varPre="PRECTOTCORR";
    varLw="LWGAB";
    varSw="SWGDN";
    
    year-=climate->firstryear;
  #endif
  
  #ifdef JRA55
    varTmp="tair";
    varPre="tp";
    varLw="strd";
    varSw="ssrd";
    
    year-=climate->firstryear;
  #endif

  //Match lat lon from ingrid to netcdf file ncdf
  //Assumes all climate files are on the same grid!
  getcellID(climate->ncid_temp, ncell, latIn, lonIn, latMatch, lonMatch);

  for(j=0;j < ncell;++j){
    // Update start / count
    start[0]=year*NDAYYEAR;
    start[1]=latMatch[j];
    start[2]=lonMatch[j];
    count[0]=NDAYYEAR;
    count[1]=1;
    count[2]=1;
   
    if ((retval = nc_inq_varid(climate->ncid_temp, varTmp, &varid_tmp))){
        fprintf(stderr,"Error reading climate variable from file, temperature varname should be '%s'; check variable names.\n",varTmp);
        ERR(retval);
    }
    if ((retval = nc_get_vara_double(climate->ncid_temp, varid_tmp, start, count, vec_clm)))
        ERR(retval);
    for(i=0; i<NDAYYEAR; i++){
        vec_temp=vec_clm[i];
        climate->temp_day[i+j*NDAYYEAR]=vec_temp;
    }

    if ((retval = nc_inq_varid(climate->ncid_prec, varPre, &varid_pre))){  
       fprintf(stderr,"Error reading climate variable from file, precipitation varname should be '%s'; check variable names.\n",varPre);
       ERR(retval);
    }
    if ((retval = nc_get_vara_double(climate->ncid_prec, varid_pre, start, count, vec_clm)))
        ERR(retval);
    for(i=0; i<NDAYYEAR; i++){
        vec_temp=vec_clm[i];
        climate->prec_day[i+j*NDAYYEAR]=vec_temp;
    }
    
    if ((retval = nc_inq_varid(climate->ncid_sw, varSw, &varid_sw))){
       fprintf(stderr,"Error reading climate variable from file, shortwave radiation varname should be '%s'; check variable names.\n",varSw);
       ERR(retval);
    }
    if ((retval = nc_get_vara_double(climate->ncid_sw, varid_sw, start, count, vec_clm)))
        ERR(retval);
    for(i=0; i<NDAYYEAR; i++){
        vec_temp=vec_clm[i];
        climate->sw_day[i+j*NDAYYEAR]=vec_temp;
    }

    if ((retval = nc_inq_varid(climate->ncid_lw, varLw, &varid_lw))){
       fprintf(stderr,"Error reading climate variable from file, longwave radiation varname should be '%s'; check variable names.\n",varLw);
       ERR(retval);
    }
    if ((retval = nc_get_vara_double(climate->ncid_lw, varid_lw, start, count, vec_clm)))
        ERR(retval);
    for(i=0; i<NDAYYEAR; i++){
        vec_temp=vec_clm[i];
        climate->lw_day[i+j*NDAYYEAR]=vec_temp;
    }

  }

  free(vec_clm);
  return FALSE;
} /* of 'getclimate_daily' */
