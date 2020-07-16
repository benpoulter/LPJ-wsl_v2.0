/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ c h e c k _ l u . c                             **/
/*******************************************************************/
#include "lpj.h"
#include "crop.h"

Bool check_lu(Standlist standlist,
              Real *landfrac,
              int id,
              Bool irrigation)
{
  Stand *stand;
  Pft *pft;
  int s;
  
  if(*landfrac>0){
    foreachstand(stand,s,standlist)
      if(stand->landusetype==AGRICULTURE){
        pft=getpft(stand->pftlist,0);
        if(pft->par->id==id && stand->irrigation==irrigation) return FALSE;
      }
    return TRUE;
  }else{
    return FALSE;
  }
}

/*
- called in sowing()
- checks if according to the input landuse data the considered cft (without/with
  irrigation) could be sown in the pixel (landfrac>0)
  -> if TRUE: checks if a stand of such cft (with/without irrigation) still exists
*/
