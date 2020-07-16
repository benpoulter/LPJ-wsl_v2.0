/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ i n i t _ c r o p d a t e s . c                    **/
/*******************************************************************/

#include "lpj.h"

void init_cropdates(const Pftpar *par,Cropdates *cropdates,int npft,int ncft,Real lat)
{
  Pftcroppar *croppar;
  int p;
  
  for(p=0;p<ncft;p++){ 

    croppar=par[npft+p].data;
  
    cropdates->last_update_fall[p]=cropdates->last_update_spring[p]=cropdates->last_update_vern[p]=0;  
    cropdates->fall_sdate20[p]=cropdates->spring_sdate20[p]=cropdates->vern_date20[p]=0;
    cropdates->fallow[p]=cropdates->fallow[p+ncft]=0;

    if(croppar->calcmethod_sdate==NO_CALC_SDATE || croppar->calcmethod_sdate==MULTICROP){
      cropdates->last_update_fall[p]=cropdates->last_update_spring[p]=-999;
      if(lat>=0) cropdates->fall_sdate20[p]=croppar->initdate.sdatenh;
      else cropdates->fall_sdate20[p]=croppar->initdate.sdatesh;
    }
  }
}

/*
- called in newgrid()
- initialisation of the 3 cropdates + related counters and fallow to zero
- for group NO_CALC_SDATE & MULTICROP the counters last_update_fall &
  last_update_spring are set to -999 and the cropdate fall_sdate20 is set to the 
  initialised sowing date
*/
