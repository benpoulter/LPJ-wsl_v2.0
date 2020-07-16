/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ c h e c k _ s t a n d _ f r a c s . c             **/
/*******************************************************************/
#include "lpj.h"

void check_stand_fracs(Standlist standlist,Real lakefrac)
{
  int s;
  Real frac_sum=0;
  Stand *stand;
  
  frac_sum=lakefrac;
  foreachstand(stand,s,standlist){
    if(stand->frac>0){
      frac_sum+=stand->frac;
    }else fail("negative STAND_FRAC :%.10f, lakefrac: %.10f landusetype: %d",stand->frac, lakefrac, stand->landusetype);
  }
  
  if(frac_sum>=1.00000001 || frac_sum<=0.999999) {
    printf("\nWARNING STAND FRAC SUM ERROR %.10f\n", frac_sum);  
    foreachstand(stand,s,standlist){
      printf("CHECKSTANDFRACS: frac_sum: %.10f; standLUtype: %d, standFrac: %.10f, lakefrac: %.10f\n", frac_sum, stand->landusetype, stand->frac, lakefrac);
    }
    fail("STAND_FRAC_SUM-error %.2f lakefrac:%.2f\n",frac_sum,lakefrac);
  }
}
