/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f r e a d s t a n d . c                            **/
/*******************************************************************/

#include "lpj.h"

Stand *freadstand(FILE *file,const Pftpar pftpar[],const Soilpar *soilpar,
                  Bool swap)
{
#if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
  int j;
#endif
  Stand *stand;
  
  stand=new(Stand);
  stand->pftlist=freadpftlist(file,pftpar,swap);
  initstand(stand);
  freadsoil(file,&stand->soil,soilpar,swap);
  freadpermafrost(file,&stand->permafrost,soilpar,swap);
  
  //read landusetype and stand fractions from restart file
  freadint1(&stand->landusetype,swap,file);
  freadreal1(&stand->frac,swap,file);

  //read ageclass info; if no ageclass then values are 0,1 
  freadint1(&stand->ageclass_standid,swap,file);
  freadint1(&stand->len_frac_transition,swap,file);

  #ifdef AGECLASS_PRIMARY
  if(stand->landusetype==PRIMARY){
    //read vector of within-ageClass fractional transitions
    for(j=0; j < stand->len_frac_transition; j++){
     freadreal1(&stand->frac_transition[j],swap, file);
    }
  }
  #endif
  
  #ifdef AGECLASS_SECFOREST
  if(stand->landusetype==SECFOREST){
    //read vector of within-ageClass fractional transitions
    for(j=0; j < stand->len_frac_transition; j++){
       freadreal1(&stand->frac_transition[j],swap, file);
    }
  }
  #endif

  return stand;
} /* of 'freadstand' */
