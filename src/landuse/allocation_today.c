/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ a l l o c a t i o n _ t o d a y . c             **/
/*******************************************************************/
#include "lpj.h"
#include "stand.h"

void allocation_today(Stand *setasidestand, Bool *alloc_today, int ntypes)
{

  int p;
  Real rharvest;
  Real *fpc_inc=NULL;
  Pft *pft;
  #ifdef DEBUGEFENCE
  Bool test=FALSE;
  #endif
  
  #ifdef DEBUGEFENCE
  if(getnpft(setasidestand->pftlist)>0){
    test=TRUE;
  #endif

  fpc_inc=newvec(Real,getnpft(setasidestand->pftlist));
  if(getnpft(setasidestand->pftlist)!=0) check(fpc_inc);

  #ifdef DEBUGEFENCE
  }
  #endif

  if(!*alloc_today){
    foreachpft(pft,p,setasidestand->pftlist){
      allocation(&setasidestand->soil.litter,pft,fpc_inc+p,TRUE);
      pft->bm_inc=0;
      fpc_inc[p]=fpc_inc[getnpft(setasidestand->pftlist)-1];
      if(isneg(pft)){ 
        litter_update(&setasidestand->soil.litter,pft,pft->nind,&rharvest,FALSE, TRUE);
        delpft(setasidestand->pftlist,p);
        p--;  
        continue;
      }        
    }
    light(&setasidestand->soil.litter,setasidestand->pftlist,ntypes,fpc_inc,TRUE);
    *alloc_today=TRUE;
  }
  #ifdef DEBUGEFENCE
  if(test==TRUE)
  #endif
  free(fpc_inc);
}

/*
- called in functions sowing.c and update_daily.c
- calls specific function allocation() for each pft
- sets bm_inc for each pft to 0
- deletes pft if it could not survive
- calls specific function light()
*/
