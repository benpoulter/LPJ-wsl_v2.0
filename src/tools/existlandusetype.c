/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ e x i s t l a n d u s e t y p e . c               **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"
#include "tree.h"
#include "pft.h"


Bool existlandusetype(Standlist standlist,int *s,Bool landusetype)
{
  int t;
  Stand *stand;
  foreachstand(stand,t,standlist)
    if(stand->landusetype==landusetype){
      *s=t;
      return TRUE;
    }
  return FALSE;
}
