/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ g e t l a n d u s e . c                         **/
/*******************************************************************/
#include "lpj.h"

Bool getlanduse(Landuse *landuse, int year, int ngridcell,int ncft)
{
  short *vec;
  int i,cell;
  Real sum;

  vec=newvec(short,landuse->n);
  check(vec);
  year-=landuse->firstyear;

  #ifndef NAT_VEG //to avoid reading LUC file if running natural veg only
    fseek(landuse->file_landuse,year*landuse->size+landuse->offset,SEEK_SET);
    if(fread(vec,sizeof(short),landuse->n,landuse->file_landuse)!=landuse->n){
      fprintf(stderr,"Error reading landuse of year %d in 'getlanduse'.\n",year);
      free(vec);
      return TRUE;
    }
    
    if(landuse->swap){
      for(i=0;i<landuse->n;i++)
        landuse->landfrac[i]=swapshort(vec[i])*0.001;
    }else{
      for(i=0;i<landuse->n;i++)
        landuse->landfrac[i]=vec[i]*0.001;
    }
  #endif

  for(cell=0;cell<ngridcell;cell++){
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

  free(vec);
  
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
