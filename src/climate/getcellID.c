/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c e l l I D . c                           **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>
#include <math.h>

/* Handle errors by printing an error message and exiting with a non-zero status */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Real getcellID(int ncfile, int ncell, Real latgridIn[], Real longridIn[], int latMatch[ncell], int lonMatch[ncell]){
  int retval, lon_varid, lat_varid, lon_dimid, lat_dimid, i, j;
  float tmp;
  size_t lenLon, lenLat;

  // Get the dimids of lat, lon
  if ((retval = nc_inq_dimid(ncfile, "lon", &lon_dimid)))
     ERR(retval);
  if ((retval = nc_inq_dimid(ncfile, "lat", &lat_dimid)))
     ERR(retval);
  
  // Get the varids of lat, lon
  if ((retval = nc_inq_varid(ncfile, "lon", &lon_varid)))
     ERR(retval);
  if ((retval = nc_inq_varid(ncfile, "lat", &lat_varid)))
     ERR(retval);

  // Get the dimension lengths of lon, lat
  if ((retval = nc_inq_dimlen(ncfile, lon_dimid, &lenLon)))
     ERR(retval);
  if ((retval = nc_inq_dimlen(ncfile, lat_dimid, &lenLat)))
     ERR(retval);

  // Define ncdf variables
  float lats_in[lenLat], lons_in[lenLon];

  // Read all the coordinate variable data
  if ((retval = nc_get_var_float(ncfile, lon_varid, &lons_in[0])))
     ERR(retval);
  if ((retval = nc_get_var_float(ncfile, lat_varid, &lats_in[0])))
     ERR(retval);

  // Match site lon with ncdf lon
  for(i=0; i<ncell; ++i){
     tmp = 10000.;
     lonMatch[i] = 0;
     for(j=0; j<lenLon; ++j)
        if(fabs(longridIn[i]-lons_in[j]) < tmp){
          tmp = fabs(longridIn[i]-lons_in[j]);
          lonMatch[i]=j;
        }
  }

  // Match site lat with ncdf lat
  for(i=0; i<ncell; ++i){
     tmp = 10000.;
     latMatch[i] = 0;
     for(j=0; j<lenLat; ++j)
        if(fabs(latgridIn[i]-lats_in[j]) < tmp){
          tmp = fabs(latgridIn[i]-lats_in[j]);
          latMatch[i]=j;
        }
  }

  return 0;
}
