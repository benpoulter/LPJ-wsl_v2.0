/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ i n t e r c e p t i o n . c                        **/
/*******************************************************************/

#include "lpj.h"

#define PT 1.32   /* Priestley-Taylor coefficient */

Real interception(Real *wet,const Pft *pft,Real pet,Real rain)   
{
  Real int_store;
  if(pet<0.0001)
  {
    *wet=0;
    return 0; 
  }
  int_store=pft->par->intc*lai(pft)*pft->phen;
  if(int_store>0.9999)
    int_store=0.9999;
  *wet=int_store*rain/(pet*PT);
  if(*wet>0.9999)
    *wet=0.9999;
  if(rain>0 && rain<=pet*PT*(*wet)*pft->fpc)
  printf("par_intc: %f, lai: %f, phen: %f pet: %f rain: %f interc: %f pft: %s\n",
          pft->par->intc,lai(pft),pft->phen,pet,rain,pet*PT*(*wet)*pft->fpc,pft->par->name);
  
  return pet*PT*(*wet)*pft->fpc;
} /* of interception */
