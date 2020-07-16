/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ i r r i g a t i o n . c                            **/
/*******************************************************************/
#include "lpj.h"

#define GM 3.26      /* empirical parameter in demand function */
#define ALPHAM 1.391 /* Priestley-Taylor coefficient (Demand,maximum-value)*/ 

void irrigation(Pft *pft,
                Real w[NSOILLAYER], /* water in soil layers */
                Real gp_stand,
                Real *wet,
                Real pet,
                const Real whcs[NSOILLAYER],
                Real *irrig_amount
               ) 
{
  int l; 
  Real supply,demand,wr;
  Real wr_0,w_new_0,dummy; 
   
  wr=0;
  for(l=0;l<NSOILLAYER;l++) 
    wr+=pft->par->rootdist[l]*w[l];
  
  if(*wet>0.9999) *wet=0.9999;
  
  supply=pft->par->emax*wr*pft->phen; 
  /*demand=(pft->phen>0 && pft->fpc>0) ? (1.0-*wet)*pet*ALPHAM/(1+GM/((gp_stand+pft->par->gmin*pft->fpc)*pft->phen)) : 0;*/
  demand=(gp_stand>0) ? (1.0-*wet)*pet*ALPHAM/(1+GM/gp_stand) : 0;

  if(supply<demand && pft->phen>0.0  /* && wr<=IRRIGATION_THRESHOLD */){ 
    /* version 1 */
    /* *irrig_amount=(1.0-w[0])*whcs[0]; */

    /* managed grassland could be contain C3 & C4 grass at one stand */
    dummy=*irrig_amount;
    wr_0=pft->par->rootdist[0]*w[0];
    w_new_0=(demand/(pft->par->emax*pft->phen)-(wr-wr_0))/pft->par->rootdist[0];
    if(w_new_0>1.0) w_new_0=1.0;
    *irrig_amount=(w_new_0-w[0])*whcs[0];
    /* printf("diff:%f irrig:%f\n",w_new_0-w[0],*irrig_amount); */
    if(dummy>*irrig_amount) *irrig_amount=dummy;
  }
}
