/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c l i m a t e _ n c d f . c               **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}


Bool getclimate_ncdf(Climate *climate,int year, Real latIn[], Real lonIn[], int ncell)
{

  char *varTmp, *varCld,*varSw,*varLw, *varWet, *varPpt; //CRU/AR4 variable names, different from CRU
  Real *vec_clm;
  int i, j;
  int varid_tmp, varid_pre, varid_cld, varid_wet, retval;
  int latMatch[ncell], lonMatch[ncell];
  int latMatch_cld[ncell], lonMatch_cld[ncell];
  Real *scale_factor, scale, vec_temp; 
  //Set up NCDF read arrays
  size_t start[3], start_cld[3], count[3];

  #ifdef SWRAD
  int varid_sw;
  #endif

  #ifdef SWLWRAD
  int varid_sw, varid_lw;
  #endif

  #ifdef OZONE
  int varid_ozone;
  size_t start_ozone[3],count_ozone[3];
  Real vec_ozone_temp;
  char *varOzone;
    varOzone="ozone";
  Real *vec_ozone;
  #endif

  #ifdef CLDCLIMATOLOGY
  Real vec_temp_sum_sw[NMONTH],vec_temp_sum_lw[NMONTH];
  #endif

  #if defined(TMPCLIMATOLOGY) || defined(PPTCLIMATOLOGY) || defined(WETCLIMATOLOGY)
  int h;
  int startYearClm, nYearAvg; //calculate mean wet days for 21st century, or mean CLM
  Real vec_temp_sum[NMONTH];
  size_t start_avg[3];
    //startYearWet=104;       //DEVQ: used for recycling AR4 data, start year should be year 2005 (corresponds to CRU baseline) - NEEDS WORK!
    startYearClm=69;          //used to determine averaging year
    nYearAvg = 30;            //constant climate (n years to avg), must be equal to or less than spin up recycling years (n=30)
  #endif
  
  //Allocate memory to the scale_factor
  scale_factor = (double *) malloc(1 * sizeof(double));

  //Variable names for CRU data
  #ifdef CRU
    varTmp="tmp";
    varCld="cld";
    varPpt="pre";
    varWet="wet";
    varLw="clt";
    varSw="clt";
    year-=climate->firstyear;
  #endif
  
  #ifdef MERRA2
    varTmp="TLML";
    varCld="cld";
    varPpt="PRECTOTCORR";
    varWet="wet";
    varLw="LWGAB";
    varSw="SWGDN";
    year-=climate->firstryear;
  #endif
  
  #ifdef ERA
    varTmp="t2m";
    varCld="cld";
    varPpt="tp";
    varWet="wet";
    varLw="strd";
    varSw="ssrd";
    year-=climate->firstryear;
  #endif
  
  #ifdef JRA55
    varTmp="tair";
    varCld="cld";
    varPpt="tp";
    varWet="wet";
    varLw="strd";
    varSw="ssrd";
    year-=climate->firstryear;
  #endif
  
  #ifdef CRUNCEP
    varTmp="tmp";
    varCld="cld";
    varPpt="pre";
    varWet="wet"; //Rain is the attribute name
    varLw="lwdown";
    varSw="swdown";
    year-=climate->firstyear; //first year of input
  #endif
  
  //Variable names for AR4 data
  #ifdef AR4
    varTmp="tas";
    varCld="clt";
    varPpt="pr";
    varWet="wet";
    varLw="clt";
    varSw="clt";
    year-=climate->firstyear;
  #endif
  
  //Variable names for CMIP5 data
  #ifdef CMIP5
    varTmp="tas";
    varCld="clt";
    varPpt="pr";
    varWet="wet";
    varLw="clt";
    varSw="clt";
    year-=climate->firstyear;
  #endif
  
  #ifdef OZONE
    varOzone = "ozone";
    vec_ozone= newvec(Real,climate->n_ozone);
    check(vec_ozone);
  #endif

  //Create vectors for holding data
  vec_clm=newvec(Real,climate->n);
  check(vec_clm);


  //Match lat lon from ingrid to netcdf file ncdf
  //IMPORTANT Assumes all climate files are on the same grid!
  getcellID(climate->ncid_temp , ncell, latIn, lonIn, latMatch, lonMatch);
  getcellID(climate->ncid_cloud , ncell, latIn, lonIn, latMatch_cld, lonMatch_cld);

  //Loop through cells and read in all months for the year
  for(j=0; j < ncell; ++j){
    // Update start / count
    start[0]=year*NMONTH;
    start[1]=latMatch[j];
    start[2]=lonMatch[j];

    //printf("start[0]=%d\t start[1]=%d\t start[2]=%d\n",start[0], start[1], start[2]);

    start_cld[0]=year*NMONTH;
    start_cld[1]=latMatch_cld[j];
    start_cld[2]=lonMatch_cld[j];

    //count identifies to read 12 months (36 months for ozone)
    count[0]=NMONTH;
    count[1]=1;
    count[2]=1;

    #ifdef OZONE
      start_ozone[0]=0;
      start_ozone[1]=latMatch[j];
      start_ozone[2]=lonMatch[j];
    
      count_ozone[0]=NMONTH*3;
      count_ozone[1]=1;
      count_ozone[2]=1;
    #endif

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the data & scaling factor from netcdf file
    //Read TEMPERATURE
    if ((retval = nc_inq_varid(climate->ncid_temp, varTmp, &varid_tmp))){
       fprintf(stderr,"Error reading climate variables from file, temperature varname should be '%s'; check variable names.\n",varTmp); 
       ERR(retval);
    }
    retval = nc_get_att_double(climate->ncid_temp, varid_tmp, "scale_factor", scale_factor);
    if (retval == NC_ENOTATT)
       scale = 1.0;
    else 
       scale = *scale_factor; 

    #ifndef TMPCLIMATOLOGY //transient climate
      if ((retval = nc_get_vara_double(climate->ncid_temp, varid_tmp, start, count, vec_clm)))
          ERR(retval);
      for(i=0;i<NMONTH;i++){
        vec_temp=vec_clm[i];  //Convert from pointer
        climate->temp[i+j*NMONTH]=vec_temp*scale;
      }
    #endif
    
    #ifdef TMPCLIMATOLOGY //constant climate
      //Initiatlize vec_temp_sum
      for(i=0;i<NMONTH;i++)
        vec_temp_sum[i]=0;
    
      for(h = 0; h < nYearAvg; h++){
        start_avg[0]=(startYearClm+h)*NMONTH;
        start_avg[1]=latMatch[j];
        start_avg[2]=lonMatch[j];
        if ((retval = nc_get_vara_double(climate->ncid_temp, varid_tmp, start_avg, count, vec_clm)))
           ERR(retval);
        for(i=0;i<NMONTH;i++)
          vec_temp_sum[i]+=vec_clm[i];  //Convert from pointer
        for(i=0;i<NMONTH;i++)
          climate->temp[i+j*NMONTH] = (vec_temp_sum[i]*scale)/nYearAvg;
      }
    #endif

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Read the data & scaling factor from netcdf file
    //Read PRECIPITATION
    if ((retval = nc_inq_varid(climate->ncid_prec, varPpt, &varid_pre))){
       fprintf(stderr,"Error reading climate variables from file, precipitation varname should be '%s'; check variable names.\n",varPpt);
       ERR(retval);
    }
    retval = nc_get_att_double(climate->ncid_prec, varid_pre, "scale_factor", scale_factor);
    if (retval == NC_ENOTATT)
      scale = 1.0;
    else 
      scale = *scale_factor;

    #ifndef PPTCLIMATOLOGY //transient climate
      if ((retval = nc_get_vara_double(climate->ncid_prec, varid_pre, start, count, vec_clm)))
          ERR(retval);
      for(i=0;i<NMONTH;i++){
        vec_temp=vec_clm[i];  //Convert from pointer
        climate->prec[i+j*NMONTH]=vec_temp*scale;
      }
    #endif

    #ifdef PPTCLIMATOLOGY //constant climate
      //Initiatlize vec_temp_sum
      for(i=0;i<NMONTH;i++)
        vec_temp_sum[i]=0;
     
      for(h=0; h < nYearAvg; h++){
        start_avg[0]=(startYearClm+h)*NMONTH;
        start_avg[1]=latMatch[j];
        start_avg[2]=lonMatch[j];
        if ((retval = nc_get_vara_double(climate->ncid_prec, varid_pre, start_avg, count, vec_clm)))
            ERR(retval);
        for(i=0;i<NMONTH;i++)
          vec_temp_sum[i]+=vec_clm[i];  //Convert from pointer
        for(i=0;i<NMONTH;i++)
          climate->prec[i+j*NMONTH] = (vec_temp_sum[i]*scale)/nYearAvg;
      }
    #endif

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //READ CLOUD COVER

    #ifndef SWRAD
      //satisfy compiler
      if(varSw==0){};
      if(varLw==0){};
      #ifndef SWLWRAD
        // Read the data & scaling factor from netcdf file
        if ((retval = nc_inq_varid(climate->ncid_cloud, varCld, &varid_cld))){
           fprintf(stderr,"Error reading climate variables from file, cloud cover varname should be '%s'; check variable names.\n",varCld);
           ERR(retval);
        }
        retval = nc_get_att_double(climate->ncid_cloud, varid_cld, "scale_factor", scale_factor);
        if (retval == NC_ENOTATT)
           scale = 1.0;
        else 
           scale = *scale_factor; 
    
        #ifndef CLDCLIMATOLOGY
          if ((retval = nc_get_vara_double(climate->ncid_cloud, varid_cld, start_cld, count, vec_clm)))
              ERR(retval);
          //Keep outside to write values to if DSWLWRAD specified
          for(i=0;i<NMONTH;i++){
            vec_temp=vec_clm[i];  //Convert from pointer
            climate->cloud[i+j*NMONTH]=100-(vec_temp*scale);
          }
        #endif
           
        #ifdef CLDCLIMATOLOGY // constant climate
          //Initiatlize vec_temp_sum
          for(i=0;i<NMONTH;i++)
            vec_temp_sum[i]=0;
        
          for(h=0; h < nYearAvg; h++){
            start_avg[0]=(startYearClm+h)*NMONTH;
            start_avg[1]=latMatch[j];
            start_avg[2]=lonMatch[j];
            if ((retval = nc_get_vara_double(climate->ncid_cloud, varid_cld, start_avg, count, vec_clm)))
                ERR(retval);
            for(i=0;i<NMONTH;i++)
              vec_temp_sum[i]+=vec_clm[i];  //Convert from pointer
            for(i=0;i<NMONTH;i++)
              climate->cloud[i+j*NMONTH] = 100-(vec_temp_sum[i]*scale)/nYearAvg;
          }
        #endif
      #endif //Of SWLWRAD
    #endif //of SWRAD
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////Or use SW or LW or both - no constant climate set up for these variables though
    #ifdef SWRAD
      if ((retval = nc_inq_varid(climate->ncid_sw, varSw, &varid_sw))){
         fprintf(stderr,"Error reading climate variables from file, shortwave radiation varname should be '%s'; check variable names.\n",varSw);
         ERR(retval);
      }
    
      //sw
      if ((retval = nc_get_vara_double(climate->ncid_sw, varid_sw, start, count, vec_clm)))
          ERR(retval);
      for(i=0;i<NMONTH;i++){
        vec_temp=vec_clm[i];  //Convert from pointer
        climate->swdown[i+j*NMONTH]=vec_temp;
      }
    #endif

    #ifdef SWLWRAD
      if ((retval = nc_inq_varid(climate->ncid_sw, varSw, &varid_sw))){
         fprintf(stderr,"Error reading climate variables from file, shortwave radiation varname should be '%s'; check variable names.\n",varSw);
         ERR(retval);
      }
      if ((retval = nc_inq_varid(climate->ncid_lw, varLw, &varid_lw))){
         fprintf(stderr,"Error reading climate variables from file, longwave radiation varname should be '%s'; check variable names.\n",varLw);
         ERR(retval);
      }
    
      #ifndef CLDCLIMATOLOGY
        //sw
        if ((retval = nc_get_vara_double(climate->ncid_sw, varid_sw, start, count, vec_clm)))
            ERR(retval);
        for(i=0;i<NMONTH;i++){
          vec_temp=vec_clm[i];  //Convert from pointer
          climate->swdown[i+j*NMONTH]=vec_temp;
        }
        
        //lwsw
        if ((retval = nc_get_vara_double(climate->ncid_lw, varid_lw, start, count, vec_clm)))
            ERR(retval);
        for(i=0;i<NMONTH;i++){
          vec_temp=vec_clm[i];  //Convert from pointer
          climate->lwdown[i+j*NMONTH]=vec_temp;
        }
      #endif //Of reading a year of radiation data, not mean
    
      #ifdef CLDCLIMATOLOGY //constant climate
        //Initiatlize vec_temp_sum
        for(i=0;i<NMONTH;i++){
           vec_temp_sum_sw[i]=0;
           vec_temp_sum_lw[i]=0;
        }
    
        for(h=0; h < nYearAvg; h++){
          start_avg[0]=(startYearClm+h)*NMONTH;
          start_avg[1]=latMatch[j];
          start_avg[2]=lonMatch[j];
          //SW
          if ((retval = nc_get_vara_double(climate->ncid_sw, varid_sw, start_avg, count, vec_clm)))
              ERR(retval);
          for(i=0;i<NMONTH;i++)
            vec_temp_sum_sw[i]+=vec_clm[i];  //Convert from
          for(i=0;i<NMONTH;i++)
            climate->swdown[i+j*NMONTH] = vec_temp_sum_sw[i]/nYearAvg;
          //LW
          if ((retval = nc_get_vara_double(climate->ncid_lw, varid_lw, start_avg, count, vec_clm)))
              ERR(retval);
          for(i=0;i<NMONTH;i++)
            vec_temp_sum_lw[i]+=vec_clm[i];  //Convert from
          for(i=0;i<NMONTH;i++)
            climate->lwdown[i+j*NMONTH] = vec_temp_sum_lw[i]/nYearAvg;
        }
      #endif //Of radiation mean
    #endif //Of radiation

    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //READ WET DAYS
    //Flag 9999 is defined for non random wetdays in initclimate_ncdf.c
    if(climate->ncid_wet!=9999){
      if ((retval = nc_inq_varid(climate->ncid_wet, varWet, &varid_wet))){
        fprintf(stderr,"Error reading climate variables from file, wet days varname should be '%s'; check variable names.\n",varWet);
        ERR(retval);
      }
      retval = nc_get_att_double(climate->ncid_wet, varid_wet, "scale_factor", scale_factor);
      if (retval == NC_ENOTATT)
        scale = 1.0;
      else
        scale = *scale_factor;

      #ifndef WETCLIMATOLOGY
        // Read the data & scaling factor from netcdf file
        //if(year < startYearWet){
          if ((retval = nc_get_vara_double(climate->ncid_wet, varid_wet, start, count, vec_clm)))
            ERR(retval);
          for(i=0;i<NMONTH;i++){
            vec_temp=vec_clm[i];  //Convert from pointer
            climate->wet[i+j*NMONTH]=vec_temp*scale;
          }
        //}
        //DEVQ: This conditional recycles mean WET days and is used only in AR4 simulations
        //else{
        //  for(h=nYearAvg;h==0;h--){
          //  start_avg[0]=0+((year-h)*NMONTH);
          //  start_avg[1]=latMatch[j];
          //  start_avg[2]=lonMatch[j];
          //  if ((retval = nc_get_vara_double(climate->ncid_wet, varid_wet, start_avg, count, vec_clm)))
          //    ERR(retval);
          //  for(i=0;i<NMONTH;i++)
          //    vec_temp_sum[i]+=vec_clm[i];  //Convert from pointer
          //  for(i=0;i<NMONTH;i++)
          //    climate->wet[i+j*NMONTH] = (vec_temp_sum[i]*scale)/nYearAvg;
          //}
        //}
      #endif

      #ifdef WETCLIMATOLOGY //constant climate
        //Initiatlize vec_temp_sum
        for(i=0;i<NMONTH;i++)
          vec_temp_sum[i]=0;

        for(h=0; h < nYearAvg; h++){
          start_avg[0]=(startYearClm+h)*NMONTH;
          start_avg[1]=latMatch[j];
          start_avg[2]=lonMatch[j];
          if ((retval = nc_get_vara_double(climate->ncid_wet, varid_wet, start_avg, count, vec_clm)))
            ERR(retval);
          for(i=0;i<NMONTH;i++)
            vec_temp_sum[i]+=vec_clm[i];  //Convert from pointer
          for(i=0;i<NMONTH;i++)
            climate->wet[i+j*NMONTH] = (vec_temp_sum[i]*scale)/nYearAvg;
        }
      #endif
    } //..end of wet days

    ///////////////////////////////////////
    //Read ozone
    #ifdef OZONE
       if ((retval = nc_inq_varid(climate->ncid_ozone, varOzone, &varid_ozone))){
          fprintf(stderr,"Error reading Ozone variable from file, ozone varname should be '%s'; check variable names.\n",varOzone);
          ERR(retval);
       }
       if ((retval = nc_get_vara_double(climate->ncid_ozone, varid_ozone, start_ozone, count_ozone, vec_ozone)))
          ERR(retval);
       for(i=0;i<NMONTH*3;i++){
         vec_ozone_temp=vec_ozone[i];  //Convert from pointer     
         climate->ozone[i+j*NMONTH*3]=vec_ozone_temp*0.1;
       }
    #endif
  }//..end cell loop

  #ifdef OZONE
  free(vec_ozone);
  #endif
  free(scale_factor);
  free(vec_clm);

  return FALSE;
} /* of 'getclimate' */
