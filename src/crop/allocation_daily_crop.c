/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ a l l o c a t i o n _ d a i l y _ c r o p . c      **/
/*******************************************************************/

#include "lpj.h"
#include "crop.h"

#define FROOTMAX 0.4
#define FROOTMIN 0.2

void allocation_daily_crop(Pft *pft,Real npp, Real wdf){
  Pftcrop *crop;
  Pftcroppar *par;
  Real froot;
  Real fhiopt,himind,hi;

  crop=pft->data;
  par=pft->par->data;

  /* vegetation growth */
  pft->bm_inc+=npp;    

  /* root growth */
  froot=FROOTMAX-FROOTMIN*crop->fphu;
  crop->ind.root=froot*pft->bm_inc;
  
  /* leaf growth */  
  /* Calculation of leaf biomass (g C/m2), prescribed LAI development, constrained by NPP */  
  if(!crop->senescence){
    if(pft->bm_inc-crop->ind.root>=crop->lai/pft->par->sla){
      crop->ind.leaf=crop->lai/pft->par->sla;
      crop->lai_nppdeficit=0;
    }
    else{
      crop->ind.leaf=pft->bm_inc-crop->ind.root;
      crop->lai_nppdeficit=crop->lai-crop->ind.leaf*pft->par->sla;
      /* today's lai_nppdeficit is substracted from tomorrow's LAI in phenology_crop() to impact fpar */
      /*crop->lai-=crop->lai_nppdeficit;*/
    }
  }

  /* storage organs growth */
  fhiopt=100*crop->fphu/(100*crop->fphu+exp(11.1-10.0*crop->fphu));    
  hi=(par->hiopt>1.0) ? fhiopt*(par->hiopt-1.0)+1.0 : fhiopt*par->hiopt;
  himind=(par->himin>1.0) ? fhiopt*(par->himin-1.0)+1.0 : fhiopt*par->himin;
  
  if(wdf>=0)
    hi=(hi-himind)*wdf/(wdf+exp(6.13-0.0883*wdf))+himind;
    
  if((crop->ind.leaf+crop->ind.root)<pft->bm_inc)
    crop->ind.so=(par->hiopt>1.0) ? (1.0-1.0/hi)*(1.0-froot)*pft->bm_inc : hi*(1.0-froot)*pft->bm_inc;
  else
    crop->ind.so=0;
  
  /* additional pool */
  crop->ind.pool=pft->bm_inc-crop->ind.root-crop->ind.leaf-crop->ind.so;
}

/*
- called in npp_crop()
- calculation of the vegetation growth:
  -> calculation of carbon biomass (total plant growth) 
  -> calculation of carbon mass root (root growth)
  -> calculation of carbon mass leaf (leaf growth)
  -> calculation of carbon mass so (storage organs growth)
  -> calculation of carbon mass pool (additional pool stem + reserve)
*/
