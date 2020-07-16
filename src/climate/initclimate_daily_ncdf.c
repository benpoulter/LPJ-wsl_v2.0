/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ i n i t c l i m a t e _ d a i l y _ n c d f . c **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

/*
 *     Function initclimate
 *
 *     Initializes climate datatype 
 *
 */

Climate *initclimate_daily_ncdf(Config *config)
{
  Header header;
  FILE *file_co2;
  int i,yr;
  double val;
  Climate *climate;
  climate=new(Climate);

  int retval;


  //.......................
  // read temp
  //.......................
  if((climate->ncid_temp=openinputfile_ncdf(&header,&climate->swap,config->temp_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
  {
    free(climate);
    return NULL;
  }

  //.......................
  // update climate header 
  //.......................
  if (header.order==YEARCELL)
    fail("Order YEARCELL of climate data is not supported in this LPJ-Version\nPlease reorganize your input data!\n");

  climate->firstyear=header.firstyear;
  climate->firstco2year=config->firstco2year;
  climate->firstryear=config->firstryear;

  //.......................
  // read precip
  //.......................
  if((climate->ncid_prec=openinputfile_ncdf(&header,&climate->swap,config->prec_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
  {
    if ((retval = nc_close(climate->ncid_temp)))
      ERR(retval);
    free(climate);
      return NULL;
  }

  //.......................
  // read SWrad
  //.......................
  if((climate->ncid_sw=openinputfile_ncdf(&header,&climate->swap,config->sw_filename,
                                   config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
  {
    if ((retval = nc_close(climate->ncid_temp)))
      ERR(retval);
    if ((retval = nc_close(climate->ncid_prec)))
      ERR(retval);
    free(climate);
    return NULL;
  }
  //.......................
  // read LWrad
  //.......................
  if((climate->ncid_lw=openinputfile_ncdf(&header,&climate->swap,config->lw_filename,
                                   config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
  {
    if ((retval = nc_close(climate->ncid_temp)))
      ERR(retval);
    if ((retval = nc_close(climate->ncid_prec)))
      ERR(retval);
    if ((retval = nc_close(climate->ncid_sw)))
      ERR(retval);
    free(climate);
    return NULL;
  }

  //.......................
  // read co2 
  //.......................
  //MODIFIED because this approach assumes climate data begins with co2 date - we do a transient run starting in 1901
  //Force climate first year to be equal to 1901 to read in the entire co2 vector
  if((file_co2=fopen(config->co2_filename,"r"))==NULL)
  {
    printfopenerr("initclimate",config->co2_filename);
    free(climate);
    return NULL;
  }

  climate->co2=newvec(Real,config->lastyear-climate->firstco2year+1);
  check(climate->co2);
  for(i=0; i<config->lastyear-climate->firstco2year+1;i++){
    if(fscanf(file_co2,"%d %lg",&yr,&val)==0){fail("soemthing went wrong reading co2 data from file..see /src/climate/initclimate_daily_ncdf.c");}
    climate->co2[i]=val;
  }

  //.......................
  // update climate header
  //.......................
  //Define header information
  climate->size_day=config->totalgridcell*NDAYYEAR*sizeof(short);
  climate->n_day=NDAYYEAR*config->ngridcell;
  climate->offset_day=config->startgrid*NDAYYEAR*sizeof(short)+headersize(LPJ_CLIMATE_HEADER);


  //.......................
  // free memory
  //.......................
  //Check daily climate
  if((climate->sw_day=newvec(Real,climate->n_day))==NULL)
  {
    printallocerr("initclimate","sw");
    free(climate->co2);
    free(climate);
    return NULL;
  }
  if((climate->lw_day=newvec(Real,climate->n_day))==NULL)
  {
    printallocerr("initclimate","lw");
    free(climate->co2);
    free(climate->sw_day);
    free(climate);
    return NULL;
  }
  if((climate->prec_day=newvec(Real,climate->n_day))==NULL)
  {
    printallocerr("initclimate","prec");
    free(climate->co2);
    free(climate->sw_day);
    free(climate->lw_day);
    free(climate);
    return NULL;
  }
  if((climate->temp_day=newvec(Real,climate->n_day))==NULL)
  {
    printallocerr("initclimate","temp");
    free(climate->co2);
    free(climate->lw_day);
    free(climate->sw_day);
    free(climate->prec_day);
    free(climate);
    return NULL;
  }
  return climate;
} /* of 'initclimate' */
