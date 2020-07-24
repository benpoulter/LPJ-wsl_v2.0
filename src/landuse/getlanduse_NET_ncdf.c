/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ g e t l a n d u s e _ N E T _ n c d f . c       **/
/*******************************************************************/
#include "lpj.h"
#include <netcdf.h>

// Handle errors by printing an error message and exiting with a non-zero status
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

Bool getlanduse_NET_ncdf(Landuse *landuse,int year, int ncell,int ncft, Real latIn[], Real lonIn[])
{
#ifndef GROSS_LUC
   int i,cell;
   Real sum;
 #ifndef NAT_VEG //to avoid reading LUC file if only using natural vegetation
   float *vec_crop, *vec_past;
   char *varCrop, *varPast;
   int j, varid_past, varid_crop, retval, latMatch[ncell], lonMatch[ncell];
   Real vec_temp=0;
   //satisfy comiler and intialize
   sum=vec_temp;

   #ifdef SYNMAP
     int varid_cropSynMap;
     char *varCropSynMap;
   #endif
   #ifdef BONDEAU
     char *varBondeau;
     int  varid_bondeau;
     Real *valueBondeau;
   #endif

   //Set up for synmap
   //Set arrays for each crop type
   double *value28, *value29, *value30, *value31, *value32, *value33, *value34, *value35, *value36, *value39, *value42, *value44;
   value28=newvec(double,landuse->n);
   check(value28);
   value29=newvec(double,landuse->n);
   check(value29);
   value30=newvec(double,landuse->n);
   check(value30);
   value31=newvec(double,landuse->n);
   check(value31);
   value32=newvec(double,landuse->n);
   check(value32);
   value33=newvec(double,landuse->n);
   check(value33);
   value34=newvec(double,landuse->n);
   check(value34);
   value35=newvec(double,landuse->n);
   check(value35);
   value36=newvec(double,landuse->n);
   check(value36);
   value39=newvec(double,landuse->n);
   check(value39);
   value42=newvec(double,landuse->n);
   check(value42);
   value44=newvec(double,landuse->n);
   check(value44);

  //Create vector to hold data (number of cells * number of land uses (24)
  vec_crop=newvec(float,landuse->n);
  vec_past=newvec(float,landuse->n);

  check(vec_crop);
  check(vec_past);
  #ifdef BONDEAU
    valueBondeau=newvec(Real,landuse->nbands);
    check(valueBondeau);
  #endif

  //TRENDY
  //To match the first year of simulation with the land use year, i.e.,1901-1850 = 51 to start reading
  
  //transient .. also for constant landuse
  //..catch BONDEAU condition, no data before 1901
  #ifdef BONDEAU
    //transient .. also for constant landuse
    //..catch BONDEAU condition, no data before 1901
    if(year < 1901){
      year =1901-landuse->firstyear;//(use year 1901= 0 index) for spinup
    }else{
      year-=landuse->firstyear;
    }
  #elif CONST_LU1860
    year =1860-landuse->firstyear;
  #elif CONST_LU1700
    year =1700-landuse->firstyear;
  #elif CONST_LU1500
    year =1500-landuse->firstyear;
  #elif LU1860
    if(year < 1860){
      //spin_luc allows equilibrium state at first year before transient runs occur
      //don't start LUC (secondary forest) until year is YYYY (all years in spin-up are < YYYY)
      year =1860-landuse->firstyear; //(use year 1901= 0 index) for spinup
    }else{
      year-=landuse->firstyear;
    }
  #elif LU1700
    if(year < 1700){
      year =1700-landuse->firstyear;
    }else{
      year-=landuse->firstyear;
    }
  #elif LU1500
    if(year < 1500){
      year =1500-landuse->firstyear;
    }else{
      year-=landuse->firstyear;
    }
  #else 
    fail("..ERROR..no flag specified for landuse start year, use LU1500 OR LU1860"); 
  #endif

  //Set up NCDF read arrays
  #if defined(HURTT) || defined(HYDE) || defined(LUH_V1) || defined(LUH_V2) || defined(LUH_V2_F)
    size_t start[3], count[3];
  #endif
  #if defined(SYNMAP) || defined(BONDEAU)
    size_t start[4], count[4];
  #endif

  //Match lat lon from ingrid to netcdf file ncdf
  //IMPORTANT Assumes all climate files are on the same grid!
  getcellID(landuse->ncid_luc, ncell, latIn, lonIn, latMatch, lonMatch);
  varCrop="gcrop";
  varPast="gpast";
  #ifdef SYNMAP
  varCropSynMap="biome_frac";
  #endif

  //Loop through cells and read 1 year
  for(j=0; j < ncell; ++j){

    #if defined(HURTT) || defined(HYDE) || defined(LUH_V1) || defined(LUH_V2) || defined(LUH_V2_F)
      //count identifies to 1 band
      count[0]=1;
      count[1]=1;
      count[2]=1;

      //Skip start
      start[1]=latMatch[j]; //Landuse is read each year; year is passed to function
      start[2]=lonMatch[j];
      start[0]=year;
      //return the integer ID for the variable name
      if ((retval = nc_inq_varid(landuse->ncid_luc, varCrop, &varid_crop))){
        fprintf(stderr,"Error reading LUC variable from file, crop frac varname should be '%s'; check variable names.\n",varCrop);
        ERR(retval);
      }
      if ((retval = nc_inq_varid(landuse->ncid_luc, varPast, &varid_past))){
        fprintf(stderr,"Error reading LUC variable from file, pasture frac varname should be '%s'; check variable names.\n",varPast);
        ERR(retval);
      }

      //retreive the data for the specified variable (int ID, from above)
      if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_crop, start, count, vec_crop)))
        ERR(retval);
      if ((retval = nc_get_vara_float(landuse->ncid_luc, varid_past, start, count, vec_past)))
        ERR(retval);

      for(i=0;i<1;i++){  //Loop through 1 band
        //July 10 2013, added new check for no data value handling because Beni Stocker has -9999 mixed with zeros for pasture layer
        if(vec_crop[i] < 0) vec_crop[i]=0;
        if(vec_past[i] < 0) vec_past[i]=0;  
        //end checks
              
        vec_temp=(double)(vec_crop[i]+vec_past[i]);  //Convert from pointer
        if(vec_temp > 1.) vec_temp=1.;
        
        landuse->landfrac[11+j*(landuse->nbands)]=vec_temp;    //Fix to PASTURE == 11
      }
    #elif defined(SYNMAP)
      //count identifies to 1 band
      count[0]=1;
      count[1]=1;
      count[2]=1;
      count[3]=1;

      //Skip start
      start[0]=year;
      start[2]=latMatch[j];
      start[3]=lonMatch[j];
     
      //Get crop fraction dimension
      if ((retval = nc_inq_varid(landuse->ncid_luc, varCropSynMap, &varid_cropSynMap)))
        ERR(retval);
     
      //Read in each crop type (the NCDF id is ID minus 1)
      //printf("%d\t %d\t %d\n", year, latMatch[j], lonMatch[j]);
      start[1]=27;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value28)))
        ERR(retval);

      start[1]=28;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value29)))
        ERR(retval)

      start[1]=29;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value30)))
        ERR(retval);

      start[1]=30;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value31)))
        ERR(retval);

      start[1]=31;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value32)))
        ERR(retval);

      start[1]=32;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value33)))
        ERR(retval);

      start[1]=33;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value34)))
        ERR(retval);

      start[1]=34;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value35)))
        ERR(retval);

      start[1]=35;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value36)))
        ERR(retval);

      start[1]=38;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value39)))
        ERR(retval);

      start[1]=41;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value42)))
        ERR(retval);

      start[1]=43;
      if ((retval = nc_get_vara_double(landuse->ncid_luc, varid_cropSynMap, start, count, value44)))
        ERR(retval);
        
      for(i=0;i<1;i++){  //Loop through 1 band
          //Must divide by 2 for the mix class categories (i.e., forest-crop)
          vec_temp= value28[i]/2.+value29[i]/2.+value30[i]/2.+value31[i]/2.+value32[i]/2.+value33[i]/2.+
                    value34[i]/2.+value35[i]/2.+value36[i]/2.+value39[i]/2.+value42[i]/2.+value44[i];
          if(vec_temp > 1.)
            vec_temp=1.; //convert from pointer
          landuse->landfrac[11+j*(landuse->nbands)]=vec_temp;     //Fix to PASTURE == 11
          //printf("cropFrac=%f\n",landuse->landfrac[11+j*24]);
      }
    #elif defined(BONDEAU)
      //set var to get
      varBondeau="CropFractions";

      //count identifies to 1 band
      count[0]=1;
      count[1]=landuse->nbands;
      count[2]=1;
      count[3]=1;

      //Skip start
      start[0]=year;
      start[1]=0;
      start[2]=latMatch[j];
      start[3]=lonMatch[j];

      //Get crop fraction dimension
      if((retval = nc_inq_varid(landuse->ncid_luc, varBondeau, &varid_bondeau)))
        ERR(retval);

      if((retval = nc_get_vara_double(landuse->ncid_luc, varid_bondeau, start, count,valueBondeau)))
        ERR(retval);

      //Read in each crop type (bands)
      for(i=0;i<landuse->nbands;i++){
        if(valueBondeau[i] < 0) valueBondeau[i]=0; //catch negative values
        landuse->landfrac[i+j*(landuse->nbands)]=valueBondeau[i];
      }
    #endif
  } //..end cell loop for get landuse data

  //Free arrays
  free(vec_crop);
  free(vec_past);

  //Free synmap arrays
  free(value28);
  free(value29);
  free(value30);
  free(value31);
  free(value32);
  free(value33);
  free(value34);
  free(value35);
  free(value36);
  free(value39);
  free(value42);
  free(value44);

#endif //end of NAT VEG exclusion

  for(cell=0;cell<ncell;cell++){
    sum=0;
    for(i=0;i<landuse->nbands;i++){
      #ifdef NAT_VEG
      landuse->landfrac[i+(cell*landuse->nbands)]=0; /*workaround to calculate exclusively natural vegetation*/
      #endif
      sum+=landuse->landfrac[i+(cell*landuse->nbands)];
    }

    if(sum>1.00001){
      if(year>0) printf("WARNING sum of cropfraction greater 1: %f\n",sum);

      sum-=1.0;

      if(landuse->landfrac[ncft+(cell*landuse->nbands)]>(0.0+sum)){
        landuse->landfrac[ncft+(cell*landuse->nbands)]-=sum;
      }else if(landuse->landfrac[2*ncft+1+(cell*landuse->nbands)]>(0.0+sum)){
        landuse->landfrac[2*ncft+1+(cell*landuse->nbands)]-=sum;
      }else{
        for(i=0;i<landuse->nbands;i++)
          if(landuse->landfrac[i+(cell*landuse->nbands)]>(0.0+sum)){
            landuse->landfrac[i+(cell*landuse->nbands)]-=sum;
            sum=0.0;
            break;
          }else{
            fail("cropfraction greater 1: %f cell: %d, managed grass is 0\n",sum+1,cell);
          }
      }
    }
  }

#endif //..end check Not GROSS_LUC  
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
