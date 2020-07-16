/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ u p d a t e g d d . c                              **/
/*******************************************************************/

#include "lpj.h"

void updategdd(Real gdd[],
               const Pftpar pftpar[], /* PFT parameter vector of length npft */
               int npft,              /* Number of PFTs */
               int ncft,
               Real temp              /* temperature (deg C) */
              )
{
  int p;
  Real dtemp;
  for(p=0;p<npft+ncft;p++)
  {
    dtemp=temp-pftpar[p].gddbase;
    if(dtemp>0)
      gdd[p]+=dtemp;
  }
} /* of 'updategdd' */
