/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ i r r i g _ a m o u n t .c                         **/
/*******************************************************************/
#include "lpj.h"
#define IRRIGATION_THRESHOLD 0.9
#define CONVEYANCE_LOSS 0.5

Real wateruse(Discharge *discharge,
              Real *dmass_new,
              Real *dmass_lake,
              Real area)
{
  Real dfout=0.0;

  /* wateruse for irrigation */
  if(*dmass_new+*dmass_lake>discharge->gir*area){
    dfout+=discharge->gir*area;
    discharge->act_irrig_amount+=discharge->gir;
    *dmass_lake-=discharge->gir*area;
    if(*dmass_lake<0.0){
      *dmass_new+=*dmass_lake;
      *dmass_lake=0.0;
    } 
    discharge->gir=0.0;
  }
  else{
    dfout+=*dmass_new+*dmass_lake;
    discharge->act_irrig_amount+=(*dmass_new+*dmass_lake)/area;
    discharge->gir-=(*dmass_new+*dmass_lake)/area;
    *dmass_new=*dmass_lake=0.0;
  }
  if(*dmass_new<0.0) *dmass_new=0.0;
  if(*dmass_lake<0.0) *dmass_lake=0.0;

  return dfout;
}

void irrig_amount(Cell *grid,
                  Config config)
{
  int cell,s,l,p;
  Pft *pft;
  Stand *stand;
  Real frac_irrig_amount;
  Real gir_stand;
  Real conv_loss;
  Real sum;
  Real wr;
  #ifdef RIVER_ROUTING
  int c;
  Real dfout;
  #endif

  for(cell=0;cell<config.ngridcell;cell++){
    if(!(grid+cell)->skip){
      (grid+cell)->discharge.gir=(grid+cell)->discharge.act_irrig_amount=0.0;
      foreachstand(stand,s,(grid+cell)->standlist)
        (grid+cell)->discharge.gir+=stand->pot_irrig_amount/(grid+cell)->manage.par->ep*stand->frac;
    }
  } /* end of cell-loop */

  for(cell=0;cell<config.ngridcell;cell++){
    if(!(grid+cell)->skip){
      #ifdef RIVER_ROUTING
      /* if not enough water, check if there is water in the neighbour cell with the most input */ 
      c=(int)((grid+cell)->discharge.neighb_irrig)-config.startgrid;
      dfout=wateruse(&(grid+cell)->discharge,
                     &(grid+c)->discharge.dmass_new,
                     &(grid+c)->discharge.dmass_lake,
                     (grid+cell)->coord.area);

      (grid+c)->discharge.mfout+=dfout;
      #endif

      foreachstand(stand,s,(grid+cell)->standlist)
        (grid+cell)->discharge.gir+=stand->pot_irrig_amount/(grid+cell)->manage.par->ep*stand->frac;


      /* determine if today irrigation dependent on threshold */   
      foreachstand(stand,s,(grid+cell)->standlist)
        if((stand->landusetype==AGRICULTURE || stand->landusetype==GRASSLAND) && stand->irrigation)
          foreachpft(pft,p,stand->pftlist){
            wr=0;
            for(l=0;l<NSOILLAYER;l++) 
              wr+=pft->par->rootdist[l]*stand->soil.w[l];
            stand->count=1;
            if(wr<=IRRIGATION_THRESHOLD) stand->count=0;
          }

      #ifdef RIVER_ROUTING
      /* actual irrigation requirement */
      frac_irrig_amount=((grid+cell)->discharge.gir>0) ? (grid+cell)->discharge.act_irrig_amount/(grid+cell)->discharge.gir : 0.0;
      #else
      /* potential irrigation requirement */
      frac_irrig_amount=((grid+cell)->discharge.gir>0) ? 1.0 : 0.0;
      #endif

      sum=0.0;
      foreachstand(stand,s,(grid+cell)->standlist){
        conv_loss=0.0;
        gir_stand=stand->pot_irrig_amount/(grid+cell)->manage.par->ep;
        if(stand->count==0){
          sum+=stand->pot_irrig_amount/(grid+cell)->manage.par->ep*stand->frac;

          stand->irrig_amount=stand->irrig_sum+gir_stand*frac_irrig_amount;

          stand->count=IRRIGATION_PAUSE;
          stand->irrig_sum=0.0;
          if(stand->irrig_amount>gir_stand){
            (grid+cell)->discharge.dmass_new+=(stand->irrig_amount-gir_stand)*(grid+cell)->coord.area*stand->frac;
            (grid+cell)->discharge.mfin+=(stand->irrig_amount-gir_stand)*(grid+cell)->coord.area*stand->frac;
            stand->irrig_amount=gir_stand;
          }
          /* irrigation withdrawal */
          (grid+cell)->output.mirrig_wd+=stand->irrig_amount*stand->frac;
          /* conveyance losses */
          conv_loss=stand->irrig_amount*(1-(grid+cell)->manage.par->ec);
          stand->irrig_amount-=conv_loss;
          /* irrigation water reaching the field */
          (grid+cell)->output.mirrig+=stand->irrig_amount*stand->frac;
        }
        else{ 
          stand->irrig_sum+=gir_stand*frac_irrig_amount;
          stand->irrig_amount=0.0;
          stand->count-=1;
          if(stand->irrig_sum>gir_stand){
            (grid+cell)->discharge.dmass_new+=(stand->irrig_sum-gir_stand)*(grid+cell)->coord.area*stand->frac;
            (grid+cell)->discharge.mfin+=(stand->irrig_sum-gir_stand)*(grid+cell)->coord.area*stand->frac;
            stand->irrig_sum=gir_stand;
          }  
        }

        stand->pot_irrig_amount=0.0;
        (grid+cell)->discharge.dmass_new+=conv_loss*(grid+cell)->coord.area*(1-CONVEYANCE_LOSS)*stand->frac;
        (grid+cell)->discharge.mfin+=conv_loss*(grid+cell)->coord.area*(1-CONVEYANCE_LOSS)*stand->frac;
      } /* end of stand-loop */
    }
  } /* end of cell-loop */
}
