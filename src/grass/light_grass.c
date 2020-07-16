/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/grass/ l i g h t _ g r a s s . h                         **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"

void light_grass(Litter *litter,Pft *pft,Real excess,Bool subannual)
{
  Grassphys  m_kill;    /*reduction in grass PFT mass to reduce grass cover to permitted maximum (gC)*/
  Real lm_old;
  Pftgrass *grass;
  grass=pft->data; 
  lm_old=grass->ind.leaf;
  grass->ind.leaf=-2.0*log(1.0-(pft->fpc-excess))/getpftpar(pft,sla);
  m_kill.leaf=lm_old-grass->ind.leaf;
  m_kill.root=grass->ind.root*(m_kill.leaf/lm_old);
  grass->ind.root-=m_kill.root;
  
  if(subannual){
    litter->ag_grass+=m_kill.leaf;
    litter->bg+=m_kill.root;
  }else{
    litter->ag_grass_inc+=m_kill.leaf;
    litter->bg_inc+=m_kill.root;
  }
  fpc_grass(pft);
} /* of 'light_grass' */
