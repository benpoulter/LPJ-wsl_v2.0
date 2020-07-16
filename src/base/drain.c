/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ d r a i n . c                                      **/
/*******************************************************************/

#include "lpj.h"

void lakebalance(Real *dmass,
                 Real *dmass_lake,
                 Real *dfout_lake,
                 Real dmass_lake_max)
{
  *dmass_lake+=*dmass+*dfout_lake;
  *dmass=*dfout_lake=0.0;
  if(*dmass_lake>dmass_lake_max){
    *dmass=*dmass_lake-dmass_lake_max;
    *dmass_lake=dmass_lake_max;
  }
}

void drain(Cell *grid,
           Config config)
{
  Real dmass,dfout;
  int c,cell,i,value;

  /* outflow of lakes */
  for(cell=0;cell<config.ngridcell;cell++){
    (grid+cell)->discharge.fin=0.0;
    (grid+cell)->discharge.dfout_lake=0.0; /* dfout_lake = outflow of lake [dm] */

    if((grid+cell)->discharge.dmass_lake>0.0 && (grid+cell)->discharge.l>0){
      (grid+cell)->discharge.dfout_lake=kr*(grid+cell)->discharge.dmass_lake*
        pow((grid+cell)->discharge.dmass_lake/(grid+cell)->discharge.dmass_lake_max,1.5);
    }

    (grid+cell)->discharge.dmass_lake-=(grid+cell)->discharge.dfout_lake; /* dmass_lake = water content in lake [dm] */

    if(!(grid+cell)->skip){
      (grid+cell)->discharge.dmass_lake+=(grid+cell)->discharge.drunoff*(grid+cell)->coord.area;
      (grid+cell)->discharge.mfin+=(grid+cell)->discharge.drunoff*(grid+cell)->coord.area;
      if((grid+cell)->discharge.dmass_lake>(grid+cell)->discharge.dmass_lake_max){
        (grid+cell)->discharge.dmass_new+=(grid+cell)->discharge.dmass_lake-(grid+cell)->discharge.dmass_lake_max;
        (grid+cell)->discharge.dmass_lake=(grid+cell)->discharge.dmass_lake_max;
      }
    }
  }//..end grid

  for(cell=0;cell<config.ngridcell;cell++){
    c=(int)((grid+cell)->discharge.count);
    if((grid+cell)->discharge.dmass_new<-0.1)
      fail("NEGATIV DISCHARGE cell:%d %f\n",cell,(grid+cell)->discharge.dmass_new);
    if((grid+cell)->discharge.dmass_new<-0.00001) (grid+cell)->discharge.dmass_new=0.0;
    dmass=(grid+cell)->discharge.dmass_new; /* dmass = mass of water in time t [dm3] */
 
    (grid+cell)->discharge.dmass_new=0; /* dmass_new = mass of water in time t+1 [dm3] */

    if((grid+cell)->discharge.l>0){
      if(v<=(grid+cell)->discharge.l){
        /**************************************************************************************/
        /* case: l>0 && v<=l, i.e. d[c+1]>=0 || d[c+1]==-1                                    */
        /* distribution of the water between two cells, or one cell and the ocean/inland sink */
        /**************************************************************************************/
        if(c==0){
          (grid+cell)->output.mdischarge+=(grid+cell)->discharge.dfout_lake;
          (grid+cell)->discharge.mfout+=(grid+cell)->discharge.dfout_lake;
        }

        /* discharge between original cell and the last but two cells */
        value=cell;
        for(i=0;i<c;i++){
          (grid+value)->output.mdischarge+=dmass+(grid+cell)->discharge.dfout_lake;
          (grid+value)->discharge.mfout+=dmass+(grid+cell)->discharge.dfout_lake;

          value=(int)((grid+cell)->discharge.d[i+1])-config.startgrid;
          (grid+value)->discharge.mfin+=dmass+(grid+cell)->discharge.dfout_lake;

          lakebalance(&dmass,&(grid+value)->discharge.dmass_lake,
                      &(grid+cell)->discharge.dfout_lake,(grid+value)->discharge.dmass_lake_max);
        }

        /* inflow and discharge to the cell before last */
        dfout=dmass*v/(grid+cell)->discharge.l;
        value=(int)((grid+cell)->discharge.d[c])-config.startgrid; 
        (grid+value)->discharge.fin+=dmass-dfout;

        (grid+value)->output.mdischarge+=dfout;
        (grid+value)->discharge.mfout+=dfout;

        /* outflow to the last cell */
        if((grid+cell)->discharge.d[c+1]>=0){
          value=(int)((grid+cell)->discharge.d[c+1])-config.startgrid;
          (grid+value)->discharge.mfin+=dfout+(grid+cell)->discharge.dfout_lake;

          lakebalance(&dfout,&(grid+value)->discharge.dmass_lake,
                      &(grid+cell)->discharge.dfout_lake,(grid+value)->discharge.dmass_lake_max);
          (grid+value)->discharge.fin+=dfout;
        } 
      //..end if v less than discharge.l
      }else{ 
        /*********************************************************************/
        /* case: l>0 && v>l, i.e. d[c+1]==-1 ocean or inland sink            */
        /* whole water of considered cell drains to the ocean or inland sink */
        /*********************************************************************/

        value=cell;
        for(i=0;i<c;i++){
          (grid+value)->output.mdischarge+=dmass+(grid+cell)->discharge.dfout_lake;
          (grid+value)->discharge.mfout+=dmass+(grid+cell)->discharge.dfout_lake;

          value=(int)((grid+cell)->discharge.d[i+1])-config.startgrid;
          (grid+value)->discharge.mfin+=dmass+(grid+cell)->discharge.dfout_lake;

          lakebalance(&dmass,&(grid+value)->discharge.dmass_lake,
                      &(grid+cell)->discharge.dfout_lake,(grid+value)->discharge.dmass_lake_max);
        }

        (grid+value)->output.mdischarge+=dmass+(grid+cell)->discharge.dfout_lake;
        (grid+value)->discharge.mfout+=dmass+(grid+cell)->discharge.dfout_lake;        
      }
    //..end if(l>0)
    }else{ 
      /**********************************************/
      /* case: l==0, i.e. c==0 && d[1]==-9 no value */
      /**********************************************/

      (grid+cell)->discharge.mfout+=dmass+(grid+cell)->discharge.dmass_lake;
      (grid+cell)->output.mdischarge+=dmass+(grid+cell)->discharge.dmass_lake;
      (grid+cell)->discharge.dmass_lake=0;
    }
  }  /* of 'for(cell=...)' */

  for(cell=0;cell<config.ngridcell;cell++)
    (grid+cell)->discharge.dmass_new+=(grid+cell)->discharge.fin;
}//..end fn 
