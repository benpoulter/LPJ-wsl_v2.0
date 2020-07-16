/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ f i n d s t a n d . c                             **/
/*******************************************************************/
#include "lpj.h"

Bool findstand(Standlist standlist, int *t, Bool landusetype, Bool irrigation)
{
  int s;
  Stand *stand;
  
  foreachstand(stand,s,standlist)
    if(stand->landusetype==landusetype && stand->irrigation==irrigation){
      *t=s;
      return TRUE;
    }
  return FALSE;
}/* of findstand*/
