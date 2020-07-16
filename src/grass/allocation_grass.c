/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ a l l o c a t i o n _ g r a s s . c               **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"


void allocation_grass(Litter *litter,Pft *pft,Real *fpc_inc,Bool subannual)
{
  Real bm_inc_ind,lmtorm;
  Grassphys inc_ind;
  Pftgrass *grass;
  bm_inc_ind=pft->bm_inc/pft->nind;
  lmtorm=getpftpar(pft,lmro_ratio)*(pft->wscal_mean/365); 

  grass=pft->data;
  if (lmtorm<1.0e-10){
    inc_ind.leaf=0.0;
    inc_ind.root=bm_inc_ind;
  }else{
    inc_ind.leaf=(bm_inc_ind+grass->ind.root-grass->ind.leaf/lmtorm)/
                (1.0+1.0/lmtorm);
    if(inc_ind.leaf<0.0){
      /*NEGATIVE ALLOCATION TO LEAF MASS */
      inc_ind.root=bm_inc_ind;
      inc_ind.leaf=(grass->ind.root+inc_ind.root)*lmtorm-grass->ind.leaf;
      
      if(subannual){
        litter->ag_grass+=-inc_ind.leaf*pft->nind; 
      }else{
        litter->ag_grass_inc+=-inc_ind.leaf*pft->nind; 
      }
    }else{
      inc_ind.root=bm_inc_ind-inc_ind.leaf;
    }
  }
  grass->ind.leaf+=inc_ind.leaf;
  grass->ind.root+=inc_ind.root;
  *fpc_inc=fpc_grass(pft);
} /* of 'allocation_grass' */
