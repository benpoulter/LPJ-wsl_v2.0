/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t i n u n d _ n c d f . c                   **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Bool getinund_ncdf(const Climate *climate,int year, Real latIn[], Real lonIn[], int ncell)
{
  char *varinund;
  Real *vec_inund, vec_temp;
  int i,j;
  int latMatch[ncell], lonMatch[ncell];
  int  varid_inund, retval;

  //Set up NCDF read arrays
  size_t start[3], count[3];

  varinund="Fw";

  //Match lat lon from ingrid to netcdf file ncdf
  //IMPORTANT Assumes all climate files are on the same grid!
  getcellID(climate->ncid_inund , ncell, latIn, lonIn, latMatch, lonMatch);

  //Create vectors for holding data
  //inund grid is always monthly
  vec_inund=newvec(Real,climate->n);
  check(vec_inund);

  //Read the ID for inund
  if ((retval = nc_inq_varid(climate->ncid_inund, varinund, &varid_inund))){
    fprintf(stderr,"Error reading inundation variable from file, inundation varname should be '%s'; check variable names.\n",varinund);
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

    if ((retval = nc_get_vara_double(climate->ncid_inund, varid_inund, start, count, vec_inund)))
        ERR(retval);
    for(i=0;i<NMONTH;i++){
        vec_temp=vec_inund[i];  //Convert from pointer
        climate->inund[i+j*NMONTH]=vec_temp;
    }
  }
  
  free(vec_inund);
  return FALSE;
} /* of 'getinund' */
