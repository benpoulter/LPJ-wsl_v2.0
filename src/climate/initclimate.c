/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ i n i t c l i m a t e . c                       **/
/*******************************************************************/
#include "lpj.h"

/*
 *     Function initclimate
 *
 *     Initializes climate datatype 
 *
 */

Climate *initclimate(Config *config)
{
  Header header;
  FILE *file_co2;
  int i,yr;
  double val;
  Climate *climate;
  climate=new(Climate);
  #ifndef CO2TRENDY2
  int j;
  #endif
  if((climate->file_temp=openinputfile(&header,&climate->swap,config->temp_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==NULL)
  {
    free(climate);
    return NULL;
  }
  if (header.order==YEARCELL)
    fail("Order YEARCELL of climate data is not supported in this LPJ-Version\nPlease reorganize your input data!\n");
  climate->firstyear=header.firstyear;
  climate->firstco2year=config->firstco2year;
  climate->firstryear=config->firstryear;

  if((climate->file_prec=openinputfile(&header,&climate->swap,config->prec_filename,
                                       config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==NULL)
  {
    fclose(climate->file_temp);
    free(climate);
    return NULL;
  }

  if((climate->file_cloud=openinputfile(&header,&climate->swap,config->cloud_filename,
                                   config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==NULL)
  {
    fclose(climate->file_temp);
    fclose(climate->file_prec);
    free(climate);
    return NULL;
  }

  if(!israndomprec(config->wet_filename))
    climate->file_wet=NULL;
  else 
  {
    if((climate->file_wet=openinputfile(&header,&climate->swap,config->wet_filename,
                                   config,LPJ_CLIMATE_HEADER,LPJ_CLIMATE_VERSION))==NULL)
    {
      fclose(climate->file_temp);
      fclose(climate->file_prec);
      fclose(climate->file_cloud);
      free(climate);
      return NULL;
    }
  }

  //Open the CO2 text file for reading and return and error if no file is found
  if((file_co2=fopen(config->co2_filename,"r"))==NULL)
  {
    printfopenerr("initclimate",config->co2_filename);
    free(climate);
    return NULL;
  }
  
#ifndef CO2TRENDY2
  //Create an annual CO2 vector that is the length of the simulation
  //..check that the vector memory allocation is ok
  climate->co2=newvec(Real,config->lastyear-climate->firstyear+1);
  check(climate->co2);
  
  //Read the first line of the CO2 text file (year and [CO2]) and quit if there is an error
  //j is initialized to index the number of years in the CO2 vector
  j=0;
  if(fscanf(file_co2,"%d %lg",&yr,&val)!=2){
    fprintf(stderr,"Error reading co2 in 'readclimate'.\n");
    free(climate->co2);
    free(climate);
    return NULL;
  }

  //FIRST GET TO THE CORRECT STARTING POINT IN THE CO2 TEXT FILE
  //If the first year of the climate simulation is less than what is found in the CO2 file, 
  //..then use preindustrial concentrations
  //..for example, if the simulation starts in 1901 and the CO2 file starts in 1910
  if (climate->firstyear<yr){
    fprintf(stderr,"Warning: first year in '%s'=%d greater than climate->firstyear.\nPreindustrial values are used\n",
        config->co2_filename,yr);
    //Continue to populate the CO2 vector with preindustrial values until the simulation year matches the CO2 file...
    //..Or when the simulation year reaches the end of the simulation
    while(climate->firstyear+j<yr && climate->firstyear+j<config->lastyear){
       climate->co2[j]=CO2_P; //preindustrial co2 
       ++j;
    }
    
    //If you are at the end of the simulation and there is no CO2 year, then use preindustrial, or use the first year of the obs CO2
    climate->co2[j]= (j < config->lastyear-climate->firstyear) ? val : CO2_P; 
  }else if (climate->firstyear>yr){
    //If the simulation year is greater than the CO2 file year
    //..then scan all the file until you get to the first simulation year
    //..and put this value in the vector
    while(climate->firstyear>yr)
      if(fscanf(file_co2,"%d %lg",&yr,&val)!=2)
         fail("Error reading co2 in initclimate\n");
    climate->co2[j]=val; 
  }else{
   //If the simulation year matches the first year in the CO2 file, then use this value
   climate->co2[j]=val;
  }

  //SECOND now read in all the years of CO2 data
  //..return error is bad
  for(i=j+1; i<config->lastyear-climate->firstyear+1;i++)
    if(fscanf(file_co2,"%d %lg",&yr,&val)!=2)
    {
      fprintf(stderr,"Error reading co2 in 'readclimate'.\n");
      free(climate->co2);
      free(climate);
      return NULL;
    }else{
      //If the value is good, then put in the CO2 vector
      climate->co2[i]=val;
    }
  //Now close the CO2 file
  fclose(file_co2);
#endif  /*..end reading co2 vector*/

#ifdef CO2TRENDY2
  //For TRENDY2, just read in the entire value chain of CO2 from 1860-2012
  //In getco2.c, the row of the vector is selected by prescribing 1860 as the start year
  climate->co2=newvec(Real,config->lastyear-climate->firstco2year+1);
  check(climate->co2);

  //If the value is good, then put in the CO2 vector
  for(i=0; i<(config->lastyear-climate->firstco2year+1);i++){ 
  fscanf(file_co2,"%d %lg",&yr,&val);
    climate->co2[i]=val;
  }

  //Now close the CO2 file
  fclose(file_co2);
#endif

  //............................
  // update climate header
  //............................  
  if(header.order==CELLYEAR)
  {
    climate->size=config->totalgridcell*NMONTH*sizeof(short);
    climate->n=NMONTH*config->ngridcell;
    climate->offset=config->startgrid*NMONTH*sizeof(short)+headersize(LPJ_CLIMATE_HEADER);
  }
  else 
  {
    climate->size=NMONTH*sizeof(short);
    climate->n=header.nyear*NMONTH*config->ngridcell;
    climate->offset=config->startgrid*NMONTH*header.nyear*sizeof(short)+
                    headersize(LPJ_CLIMATE_HEADER);
  }

  //............................
  // free memory
  //............................
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
  if(israndomprec(config->wet_filename))
  {
    if((climate->wet=newvec(Real,climate->n))==NULL)
    {
      printallocerr("initclimate","wet");
      free(climate->co2);
      free(climate->cloud);
      free(climate->prec);
      free(climate->temp);
      free(climate);
      return NULL;
    }
  }
  return climate;
} /* of 'initclimate' */
