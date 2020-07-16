/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f w r i t e s t a n d . c                          **/
/*******************************************************************/

#include "lpj.h"

Bool fwritestand(FILE *file,const Stand *stand,Bool full)
{
  if(fwritepftlist(file,stand->pftlist,full)!=getnpft(stand->pftlist))
    return TRUE;
  if(fwritesoil(file,&stand->soil,full))
    return TRUE;
  if(fwritepermafrost(file,&stand->permafrost,full))
      return TRUE;
  fwrite1(&stand->landusetype,sizeof(int),file);
  fwrite1(&stand->frac,sizeof(Real),file);

  //if no ageclass, these values are 0,1
  //..otherwise keep track of within-ageClass transitions
  fwrite1(&stand->ageclass_standid,sizeof(int),file);
  fwrite1(&stand->len_frac_transition,sizeof(int),file);

  #ifdef AGECLASS_PRIMARY
  if(stand->landusetype==PRIMARY){
    //store each within-stand fractional transition
    for(int j=0; j < stand->len_frac_transition; j++){
        fwrite1(&stand->frac_transition[j],sizeof(Real), file);
    }
  }
  #endif

  #ifdef AGECLASS_SECFOREST
  if(stand->landusetype==SECFOREST){
    //store each within-stand fractional transition
    for(int j=0; j < stand->len_frac_transition; j++){
          fwrite1(&stand->frac_transition[j],sizeof(Real), file);
    }
  }
  #endif

  return FALSE;
} /* of *fwritestand' */
