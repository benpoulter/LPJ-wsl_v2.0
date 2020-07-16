/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ l a i m a x _ m a n a g e . c                      **/
/*******************************************************************/
#include "lpj.h"
#include "manage.h"


void laimax_manage(Real laimax[],Real laimax_tempcer,Real laimax_maize,const Pftpar *pftpar,int npft,int ncft,int year)
{
 int i;
 Pftcroppar *croppar;

 for(i=npft;i<npft+ncft;i++){ 

   croppar=pftpar[i].data;

   if(pftpar[i].id==TROPICAL_CEREALS){
     laimax[i-npft]=croppar->laimax;
   }else{
     if(year<=1949){
       laimax[i-npft]=croppar->laimin;
     }else if(year>2003){
        if(pftpar[i].id==TEMPERATE_CEREALS){
          laimax[i-npft]=laimax_tempcer;
        }else if(pftpar[i].id==MAIZE){
          laimax[i-npft]=laimax_maize;
        }else{ 
          laimax[i-npft]=croppar->laimax;
        }
     }else{
        /*calculate linear trend of country-specific laimax from 1950 onwards*/
        /*function y=ax+b   (x,y): (1950,laimin)....(2000,laimax)*/
        if(pftpar[i].id==TEMPERATE_CEREALS) laimax[i-npft]=(laimax_tempcer-croppar->laimin)/50*year+40*croppar->laimin-39*laimax_tempcer;
        else if(pftpar[i].id==MAIZE) laimax[i-npft]=(laimax_maize-croppar->laimin)/50*year+40*croppar->laimin-39*laimax_maize;
        else laimax[i-npft]=(croppar->laimax-croppar->laimin)/50*year+40*croppar->laimin-39*croppar->laimax;
     } 
   }
 }
} /* of 'laimax_manage' */

/* note: the wintercrop can be still at the field when a new laimax is calculated at the beginning of the year */
