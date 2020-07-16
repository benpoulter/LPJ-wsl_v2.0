/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/climate/ g e t c l i m a t e . c                         **/
/*******************************************************************/
#include "lpj.h"

Bool getclimate(Climate *climate,int year)
{
  short *vec, *tmpvec; 
  int i;  
  #if defined(TMPCLIMATOLOGY) || defined(PPTCLIMATOLOGY) || defined(PPTCLIMATOLOGY)
  int j,k;
  int nYearRead,ngridCell,startYear=0; //constant climate (year (1901==0) in simulation to begin averaging after
  nYearRead = 30;                      //constant climate (n years to avg)
  ngridCell = climate->n/NMONTH;
  #endif

  tmpvec=newvec(short,climate->n);             
  vec=newvec(short,climate->n);
  check(tmpvec);
  check(vec);

  //Update year to start on 0 thru nyears instead of real year (ie 1901)
  #ifndef CLIMMODULUS
    year-=climate->firstyear;
  #else
    year=year%20;
    printf("climmodulus%d\t",year);
  #endif
  
  //===================================
  //       T e m p e r a t u r e 
  //===================================
  #ifndef TMPCLIMATOLOGY //constant climate
    fseek(climate->file_temp,year*climate->size+climate->offset,SEEK_SET);
    if(fread(vec,sizeof(short),climate->n,climate->file_temp)!=climate->n)
    {
      fprintf(stderr,"Error reading temperature of year %d in 'readclimate'.\n",year);
      free(vec);
      return TRUE;
    }
  #else 
    //-----------------------------------
    //  TMPCLIMATOLOGY constant temp
    //-----------------------------------
    //Clean up vector - initialize with zero
    for(i=0;i<climate->n;i++) vec[i] = 0;

    //After a specified simulation year, start using the mean/constant monthly climate
    if(year > startYear){
      //Loop through n years to calculate monthly means
      for(i=0;i<nYearRead;i++){
        fseek(climate->file_temp,(startYear+i)*climate->size+climate->offset,SEEK_SET);
        if(fread(tmpvec,sizeof(short),climate->n,climate->file_temp)!=climate->n)
        {
          fprintf(stderr,"Error reading temperature of year %d in 'readclimate'.\n",year);
          free(tmpvec);
          return TRUE;
        }

        //Add each year / grid cell and divide by number of years
        for(j=0;j<ngridCell;j++){
          for(k=0;k<NMONTH;k++){
             vec[k+j*NMONTH] += tmpvec[k + j*NMONTH]  / nYearRead;
          }
        }
      }//..end nYearRead

      //use transient climate until startYear is reached
    }else{
      fseek(climate->file_temp,year*climate->size+climate->offset,SEEK_SET);
      if(fread(vec,sizeof(short),climate->n,climate->file_temp)!=climate->n)
      {
        fprintf(stderr,"Error reading temperature of year %d in 'readclimate'.\n",year);
        free(vec);
        return TRUE;
      }
    }
  #endif  //..end temperature

  if(climate->swap)
    for(i=0;i<climate->n;i++) climate->temp[i]=swapshort(vec[i])*0.1;
  else
    for(i=0;i<climate->n;i++) climate->temp[i]=vec[i]*0.1;
  //////////////////END TEMP
      
  //===================================
  //     P r e c i p i t a t i o n
  //===================================
  #ifndef PPTCLIMATOLOGY 
    fseek(climate->file_prec,year*climate->size+climate->offset,SEEK_SET);
    if(fread(vec,sizeof(short),climate->n,climate->file_prec)!=climate->n)
    {
      fprintf(stderr,"Error reading precipitation of year %d in 'readclimate'.\n",year);
      free(vec);
      return TRUE;
    }
  #else
    //-----------------------------------
    //  PPTCLIMATOLOGY constant precip
    //-----------------------------------
    //Clean up vector - initialize with zero
    for(i=0;i<climate->n;i++) vec[i] = 0;
  
    //After a specified simulation year, start using the mean/constant monthly climate
    if(year > startYear){
      //Loop through n years to calculate monthly means
      for(i=0;i<nYearRead;i++){
        fseek(climate->file_prec,(startYear+i)*climate->size+climate->offset,SEEK_SET);
        if(fread(tmpvec,sizeof(short),climate->n,climate->file_prec)!=climate->n)
        {
          fprintf(stderr,"Error reading precipitation of year %d in 'readclimate'.\n",year);
          free(tmpvec);
          return TRUE;
        }
  
        for(j=0;j<ngridCell;j++){
          for(k=0;k<NMONTH;k++){
             vec[k+j*NMONTH] += tmpvec[k + j*NMONTH] / nYearRead;
          }
        }
      }//..end nYearRead
  
      //Or, use transient climate until startYear is reached
    }else{
      fseek(climate->file_prec,year*climate->size+climate->offset,SEEK_SET);
      if(fread(vec,sizeof(short),climate->n,climate->file_prec)!=climate->n)
      {
        fprintf(stderr,"Error reading precipitation of year %d in 'readclimate'.\n",year);
        free(vec);
        return TRUE;
      }
    }
  #endif  //..end of precip

  if(climate->swap)
    for(i=0;i<climate->n;i++) climate->prec[i]=swapshort(vec[i]);
  else
    for(i=0;i<climate->n;i++) climate->prec[i]=vec[i];
  //..END PRECIP


  //===================================
  //     C l o u d    C o v e r 
  //===================================
  #ifndef CLDCLIMATOLOGY 
    fseek(climate->file_cloud,year*climate->size+climate->offset,SEEK_SET);
    if(fread(vec,sizeof(short),climate->n,climate->file_cloud)!=climate->n)
    {
      fprintf(stderr,"Error reading cloudiness of year %d in 'readclimate'.\n",year);
      free(vec);
      return TRUE;
    }
  #else
    //-----------------------------------
    //  CLDCLIMATOLOGY constant cloud
    //-----------------------------------
    //Clean up vector - initialize with zero
    for(i=0;i<climate->n;i++) vec[i] = 0;
  
    //After a specified simulation year, start using the mean/constant monthly climate
    if(year > startYear){
      //Loop through n years to calculate monthly means
      for(i=0;i<nYearRead;i++){
        fseek(climate->file_cloud,(startYear+i)*climate->size+climate->offset,SEEK_SET);
        if(fread(tmpvec,sizeof(short),climate->n,climate->file_cloud)!=climate->n)
        {
          fprintf(stderr,"Error reading cloudiness of year %d in 'readclimate'.\n",year);
          free(tmpvec);
          return TRUE;
        }
  
        for(j=0;j<ngridCell;j++){
          for(k=0;k<NMONTH;k++){
             vec[k+j*NMONTH] += tmpvec[k + j*NMONTH] / nYearRead;
          }
        }
      }//..end nYearRead
  
      //Or, use 'regular' approach until startYear is reached
    }else{
      fseek(climate->file_cloud,year*climate->size+climate->offset,SEEK_SET);
      if(fread(vec,sizeof(short),climate->n,climate->file_cloud)!=climate->n)
      {
        fprintf(stderr,"Error reading cloudiness of year %d in 'readclimate'.\n",year);
        free(vec);
        return TRUE;
      }
    }
  #endif  //..end of wet days

  if(climate->swap)
    for(i=0;i<climate->n;i++) climate->cloud[i]=100-swapshort(vec[i]);
  else
    for(i=0;i<climate->n;i++) climate->cloud[i]=100-vec[i];
  //..END CLOUD COVER

  //===================================
  //         W e t   D a y s
  //===================================
  //Ozone - read in wet days data modify for post-2006 mean
    if(climate->file_wet!=NULL)
    {
  
      #ifndef WETCLIMATOLOGY
        fseek(climate->file_wet,year*climate->size+climate->offset,SEEK_SET);
        if(fread(vec,sizeof(short),climate->n,climate->file_wet)!=climate->n)
        {
          fprintf(stderr,"Error reading wetdays of year %d in 'readclimate'.\n",year);
          free(vec);
          return TRUE;
        }
      #else
        //-------------------------------------
        // WETCLIMATOLOGY constant wet days
        //-------------------------------------
        //Clean up vector - initialize with zero
        for(i=0;i<climate->n;i++) vec[i] = 0;
      
        //After a specified simulation year, start using the mean/constant monthly climate
        if(year > startYear){
          //Loop through n years to calculate monthly means
          for(i=0;i<nYearRead;i++){
            fseek(climate->file_wet,(startYear+i)*climate->size+climate->offset,SEEK_SET);
            if(fread(tmpvec,sizeof(short),climate->n,climate->file_wet)!=climate->n){
              fprintf(stderr,"Error reading wetdays of year %d in 'readclimate'.\n",year);
              free(tmpvec);
              return TRUE;
            }
      
            for(j=0;j<ngridCell;j++){
              for(k=0;k<NMONTH;k++){
                 vec[k+j*NMONTH] += tmpvec[k + j*NMONTH] / nYearRead;
              }
            }
          }//..end for nYearRead
      
          //Or, use 'regular' approach until startYear is reached
        }else{
          fseek(climate->file_wet,year*climate->size+climate->offset,SEEK_SET);
          if(fread(vec,sizeof(short),climate->n,climate->file_wet)!=climate->n){
            fprintf(stderr,"Error reading wetdays of year %d in 'readclimate'.\n",year);
            free(vec);
            return TRUE;
          }
        }
      #endif
      
      if(climate->swap)
        for(i=0;i<climate->n;i++) climate->wet[i]=swapshort(vec[i]);
      else  
        for(i=0;i<climate->n;i++) climate->wet[i]=vec[i];
      
  }
  //..END WET DAYS

  free(tmpvec); 
  free(vec);
  return FALSE;
} /* of 'getclimate' */
