/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c o 2 _ n c d f . c                       **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Bool getco2_ncdf(const Climate *climate,int year, Real latIn[], Real lonIn[], int ncell)
{
  int latMatch[ncell], lonMatch[ncell], i, j, varid_co2, retval;
  char *varCo2;
  Real *vec_co2, vec_temp;

  //Set up NCDF read arrays
  size_t start[3], count[3];

  //Match lat lon from ingrid to netcdf file ncdf
  //IMPORTANT Assumes all climate files are on the same grid!
  getcellID(climate->ncid_co2 , ncell, latIn, lonIn, latMatch, lonMatch);

  //Create vectors for holding data
  //CO2 grid is always monthly
  vec_co2=newvec(Real,ncell*12);
  check(vec_co2);

  //---------------------------------
  // Gridded CO2 Files:
  //
  // (1) Calle, Patra and Poulter:
  //     ..Fluxes: LPJ (s2) + FossilFuel (CDIAC) + Ocean (Takahashi et al. 2009)
  //     ..ACTM forward transport
  //     .. 1859-2016, matched annual global mean to gapfilled icecore data (NOAA)
  //     .. < http://scrippsco2.ucsd.edu/data/atmospheric_co2/icecore_merged_products >
  //     .. ..(a) monthly average taken from 2001-2010
  //                ..assumes similar spatial distribution for FF between 1860-2016; 
  //                ..alternate model might be an improvement
  //                ..FF magnitude increases naturally according to icecore global mean co2 growth rate
  //     .. ..(b) annual global mean adjusted via offset to match gap-filled icecore data (NOAA)
  //     .. ..(c) growth rate from icecore data first subtracted from FF co2 field,
  //                      ..then remaining offset applied to total
  //     .. ..(d) first year in data (1859) is only Land + Ocean spatio-temporal fields, adjusted to 1859 icecore data
  // (2) MsTMIP, use 1801 year for spinup (& start year for CO2 ncdf for MsTMIP is year 1700)
  // (3) CMIP6, use 1860 year for spinup; data is latitude by month; same data for all longitudes
  //--------------------------------
  #ifdef CO2GRID_SPINUP
    year= 1859 - 1859;
  #else
    year= year - 1859;
  #endif

  //Read the ID for CO2
  varCo2="CO2";
  if ((retval = nc_inq_varid(climate->ncid_co2, varCo2, &varid_co2))){
     fprintf(stderr,"Error reading CO2 variable from gridded file, carbon dioxide varname should be '%s'; check variable names.\n",varCo2);
     ERR(retval);
  }

  //Loop through cells and read in all months for the year
  for(j=0; j < ncell; ++j){
    // Update start / count
    start[0]=year*NMONTH;
    start[1]=latMatch[j];
    start[2]=lonMatch[j];

    //count identifies to read 12 months
    count[0]=NMONTH;
    count[1]=1;
    count[2]=1;

    if ((retval = nc_get_vara_double(climate->ncid_co2, varid_co2, start, count, vec_co2)))
        ERR(retval);
    for(i=0;i<NMONTH;i++){
      //----------------------------------------------------------
      // CO2MEAN is now specified as CO2GRID_SPINUP 
      // ..auto-fill in distribute script if simulation_type == spinup or landuse_spinup && CO2GRID
      // ..spatio-temporal  distribution is based on inversion results from Patra-ACTM 2016
      // ..recycles spatio-temporal distribution for Land +Ocean
      // ..matched to annual global mean co2 for 1859 from gap-filled icecore datia
      //----------------------------------------------------------
      vec_temp=vec_co2[i];  //Convert from pointer
      climate->co2[i+j*NMONTH]=vec_temp;
      //printf("\n \t\t CO2 grid...climate->co2[month: %d]: %f",i, climate->co2[i+j*NMONTH]);
    }
  }
  
  free(vec_co2);
  return FALSE;
} /* of 'getco2' */
