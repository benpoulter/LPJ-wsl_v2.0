/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ e s t a b l i s h . c                              **/
/*******************************************************************/

#include "lpj.h"

Bool establish(Real gdd,const Pftpar *pftpar,const Climbuf *climbuf, int pftid)
{
  Real temp_min20;
  temp_min20=climbuf->min->avg/climbuf->min->n;

  if(pftid==0){
    return (temp_min20>=pftpar->temp.low) && 
           (temp_min20<=pftpar->temp.high) && 
           (gdd>=pftpar->gdd5min);
  }else if((pftid-1) == pftpar->id || pftpar->type == GRASS){
    //establish the PFT if equal to fixed PFT or if it is a grass
    return TRUE;
  }else{
    return FALSE;
  }

} /* of 'establish' */
