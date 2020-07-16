/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ i n i t c l i m a t e _ n c d f . c             **/
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

Climate *initclimate_ncdf(Config *config)
{
  Header header;
  FILE *file_co2;
  int i,yr;
  double val;
  Climate *climate;
  climate=new(Climate);
  int retval;
  #ifndef SITCHCO2
  int j;
  #endif

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
  // update header
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
  // read SW/LW rad
  //.......................
  #ifndef SWLWRAD
    #ifndef SWRAD
      if((climate->ncid_cloud=openinputfile_ncdf(&header,&climate->swap,config->cloud_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
      {
        if ((retval = nc_close(climate->ncid_temp)))
          ERR(retval);
        if ((retval = nc_close(climate->ncid_prec)))
          ERR(retval);
        free(climate);
        return NULL;
      }
    #endif
  
    #ifdef SWRAD
      if((climate->ncid_cloud=openinputfile_ncdf(&header,&climate->swap,config->cloud_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
      {
        if ((retval = nc_close(climate->ncid_temp)))
          ERR(retval);
        if ((retval = nc_close(climate->ncid_prec)))
          ERR(retval);
        free(climate);
        return NULL;
      }
      //ONLY SW RADIATION
      if((climate->ncid_sw=openinputfile_ncdf(&header,&climate->swap,config->sw_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
      {
        if ((retval = nc_close(climate->ncid_temp)))
          ERR(retval);
        if ((retval = nc_close(climate->ncid_prec)))
          ERR(retval);
        if ((retval = nc_close(climate->ncid_cloud)))
          ERR(retval);
        free(climate);
        return NULL;
      }
    #endif
  #endif
  
  #ifdef SWLWRAD
    if((climate->ncid_cloud=openinputfile_ncdf(&header,&climate->swap,config->cloud_filename,
                                     config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      free(climate);
      return NULL;
    }
    //BOTH SW and LW radiation read
    if((climate->ncid_sw=openinputfile_ncdf(&header,&climate->swap,config->sw_filename,
                                     config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_cloud)))
        ERR(retval);
      free(climate);
      return NULL;
    }
    //lwsw
    if((climate->ncid_lw=openinputfile_ncdf(&header,&climate->swap,config->lw_filename,
                                     config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_cloud)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_sw)))
        ERR(retval);
      free(climate);
      return NULL;
    }
  #endif

  //.......................
  // read Ozone
  //.......................
  #ifdef OZONE
    if((climate->ncid_ozone=openinputfile_ncdf(&header,&climate->swap,config->ozone_filename,
                                     config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_cloud)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_sw)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_lw)))
        ERR(retval);
      free(climate);
      return NULL;
    }
  #endif

  //.......................
  // read swet
  //.......................
  #ifdef SWETINPUT
    if((climate->ncid_inund=openinputfile_ncdf(&header,&climate->swap,config->wetfrac_filename,
                                          config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_cloud)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_sw)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_lw)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_ozone)))
        ERR(retval);
      free(climate);
      return NULL;
    }
  #endif

  //.......................
  // read wet days
  //.......................
  if(!israndomprec(config->wet_filename))
    climate->ncid_wet=9999;
  else
  {
    if((climate->ncid_wet=openinputfile_ncdf(&header,&climate->swap,config->wet_filename,
                                   config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
    {
      if ((retval = nc_close(climate->ncid_temp)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_prec)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_cloud)))
        ERR(retval);
      if ((retval = nc_close(climate->ncid_sw))) //lwsw
        ERR(retval);
      if ((retval = nc_close(climate->ncid_lw))) //lwsw
        ERR(retval);
      free(climate);
      return NULL;
    }
  }

  //.......................
  // read co2
  //.......................
  //GET CO2 DATA
  #ifdef CO2GRID
    if((climate->ncid_co2=openinputfile_ncdf(&header,&climate->swap,config->co2_filename,
                                     config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==0)
      return NULL;
  #else
    if((file_co2=fopen(config->co2_filename,"r"))==NULL)
    {
      printfopenerr("initclimate",config->co2_filename);
      free(climate);
      return NULL;
    }
    #ifdef SITCHCO2  //MUST UPDATE END YEAR
      climate->co2=newvec(Real,config->lastyear-climate->firstco2year+1);
      check(climate->co2);
      //Read all values
      for(i=0; i<config->lastyear-climate->firstco2year+1;i++){
        if(fscanf(file_co2,"%d %lg",&yr,&val)==0){fail("error reading co2 file../src/climate/initclimate_ncdf.c");}
        climate->co2[i]=val;
      }
    #else
      climate->co2=newvec(Real,config->lastyear-climate->firstyear+1);
      check(climate->co2);
      
      /* find correct start year in co2-file*/
      j=0;
      if(fscanf(file_co2,"%d %lg",&yr,&val)!=2){
        fprintf(stderr,"Error reading co2 in 'readclimate'.\n");
        free(climate->co2);
        free(climate);
        return NULL;
      }
      if (climate->firstyear<yr){
        fprintf(stderr,"Warning: first year in '%s'=%d greater than climate->firstyear.\nPreindustrial values are used\n",
            config->co2_filename,yr);
        while(climate->firstyear+j<yr && climate->firstyear+j<config->lastyear){
           climate->co2[j]=CO2_P;
           ++j;
        }
        climate->co2[j]= (j < config->lastyear-climate->firstyear) ? val : CO2_P;
      }
      else if (climate->firstyear>yr){
        while(climate->firstyear>yr)
          if(fscanf(file_co2,"%d %lg",&yr,&val)!=2)
             fail("Error reading co2 in initclimate\n");
        climate->co2[j]=val; 
      }
      else
       climate->co2[j]=val;
  
      /* read all other values */
      for(i=j+1; i<config->lastyear-climate->firstyear+1;i++)
        if(fscanf(file_co2,"%d %lg",&yr,&val)!=2)
        {
          fprintf(stderr,"Error reading co2 in 'readclimate'.\n");
          free(climate->co2);
          free(climate);
          return NULL;
        }
        else
          climate->co2[i]=val;
    #endif  /* ..end sitch co2 */
    fclose(file_co2);
  #endif //OF CO2

  //.......................
  // update header
  //.......................
  //Define header information
  climate->size=config->totalgridcell*NMONTH*sizeof(short);
  climate->n=NMONTH*config->ngridcell;
  climate->offset=config->startgrid*NMONTH*sizeof(short)+headersize(LPJ_CLIMATE_HEADER);

  //.......................
  // update ozone
  //.......................
  #ifdef OZONE
    climate->size_ozone=config->totalgridcell*NMONTH*sizeof(short)*3;   
    climate->n_ozone=config->ngridcell*NMONTH*3;             
    climate->offset_ozone=config->startgrid*NMONTH*sizeof(short)*3;     
  #endif

  //.......................
  // read co2 grid
  //.......................
  #ifdef CO2GRID
    if((climate->co2=newvec(Real,climate->n))==NULL)
    {
      printallocerr("initclimate","co2");
      free(climate);
      return NULL;
    }
  #endif

  //.......................
  // free memory
  //.......................
  if((climate->cloud=newvec(Real,climate->n))==NULL)
  {
    printallocerr("initclimate","cloud");
    free(climate);
    return NULL;
  }
  if((climate->prec=newvec(Real,climate->n))==NULL)
  {
    printallocerr("initclimate","prec");
    free(climate->cloud);
    free(climate);
    return NULL;
  }
  if((climate->temp=newvec(Real,climate->n))==NULL)
  {
    printallocerr("initclimate","temp");
    free(climate->cloud);
    free(climate->prec);
    free(climate);
    return NULL;
  }
  //lwsw
  if((climate->swdown=newvec(Real,climate->n))==NULL)
  {
    printallocerr("initclimate","swdown");
    free(climate->cloud);
    free(climate->prec);
    free(climate->temp);
    free(climate);
    return NULL;
  }
  //lwsw
  if((climate->lwdown=newvec(Real,climate->n))==NULL)
  {
    printallocerr("initclimate","lwdown");
    free(climate->cloud);
    free(climate->prec);
    free(climate->temp);
    free(climate->swdown);
    free(climate);
    return NULL;
  }

  #ifdef OZONE
    //ozone
    if((climate->ozone=newvec(Real,climate->n_ozone))==NULL)
    {
      printallocerr("initclimate","ozone");
      free(climate->cloud);
      free(climate->prec);
      free(climate->temp);
      free(climate->swdown);
      free(climate->lwdown);
      free(climate);
      return NULL;
    }
  #endif
  
  #ifdef SWETINPUT
    //inundation
    if((climate->inund=newvec(Real,climate->n))==NULL)
    {
      printallocerr("initclimate","inundation");
      free(climate->cloud);
      free(climate->prec);
      free(climate->temp);
      free(climate->swdown);
      free(climate->lwdown);
      free(climate->ozone);
      free(climate);
      return NULL;
    }
  #endif

  if(israndomprec(config->wet_filename))
  {
    if((climate->wet=newvec(Real,climate->n))==NULL)
    {
      printallocerr("initclimate","wet");
      free(climate->co2);
      free(climate->cloud);
      free(climate->prec);
      free(climate->temp);
      free(climate->swdown); //lwsw
      free(climate->lwdown); //lwsw
      free(climate->ozone); //ozone
      free(climate->inund);
      free(climate);
      return NULL;
    }
  }
  return climate;
} /* of 'initclimate' */
