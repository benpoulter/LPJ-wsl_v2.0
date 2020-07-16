/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ i n i t _ d r a i n . c                            **/
/*******************************************************************/

#include "lpj.h"

Bool initdrain(Cell *grid,
         Config *config)
{
  FILE *neighb_irrig,*drainage;
  Header header;
  int cell;
  int c;
  short rbuf1,rbuf2;
  int swap;

  if((neighb_irrig=openinputfile(&header,&swap,config->neighb_irrig_filename,
                     config,LPJNEIGHB_IRRIG_HEADER,LPJNEIGHB_IRRIG_VERSION))==NULL){
    return TRUE;
  }

  for(cell=0;cell<config->ngridcell;cell++){
    fseek(neighb_irrig,headersize(LPJNEIGHB_IRRIG_HEADER)+sizeof(short)*(cell+config->startgrid),SEEK_SET);
    if(fread(&rbuf1,sizeof(short),1,neighb_irrig)==0){fail("failed to read neighbb irrigation info..");}
    if(swap){ 
      (grid+cell)->discharge.neighb_irrig=swapshort(rbuf1);
    }else{
      (grid+cell)->discharge.neighb_irrig=rbuf1;
    }

    if((grid+cell)->discharge.neighb_irrig<config->startgrid || 
       (grid+cell)->discharge.neighb_irrig>=config->startgrid+config->ngridcell){

      fprintf(stderr,"neighbour cell for additional irrigation water not there:%d\n",
              (grid+cell)->discharge.neighb_irrig);
      exit(99);
    }
  }

  if((drainage=openinputfile(&header,&swap,config->drainage_filename,
                             config,LPJDRAIN_HEADER,LPJDRAIN_VERSION))==NULL){
    return TRUE;
  }

  for(cell=0;cell<config->ngridcell;cell++){
    c=0;
    (grid+cell)->discharge.d[0]=cell+config->startgrid;

    fseek(drainage,headersize(LPJDRAIN_HEADER)+2*sizeof(short)*(cell+config->startgrid),SEEK_SET);
    if(fread(&rbuf1,sizeof(short),1,drainage)==0){fail("failed to read drainage1 info..");}
    if(fread(&rbuf2,sizeof(short),1,drainage)==0){fail("failed to read drainage2 info..");}
    if(swap){
      (grid+cell)->discharge.d[1]=swapshort(rbuf1);
      (grid+cell)->discharge.l=swapshort(rbuf2)*1000;  /* unit from km -> m */
    }else{
      (grid+cell)->discharge.d[1]=rbuf1;
      (grid+cell)->discharge.l=rbuf2*1000;  /* unit from km -> m */
    }

    if((grid+cell)->discharge.d[1]>=0 && ((grid+cell)->discharge.d[1]<config->startgrid || 
       (grid+cell)->discharge.d[1]>=config->startgrid+config->ngridcell)){

      fprintf(stderr,"cell to rout the river not there:%d\n",(grid+cell)->discharge.d[1]);
      exit(99);
    }
  
    while(v>(grid+cell)->discharge.l && (grid+cell)->discharge.d[c+1]>=0){
      fseek(drainage,headersize(LPJDRAIN_HEADER)+2*sizeof(short)*(int)((grid+cell)->discharge.d[c+1]),SEEK_SET);
      c+=1;
      if(fread(&rbuf1,sizeof(short),1,drainage)==0){fail("failed to read drainage3 info..");}
      if(fread(&rbuf2,sizeof(short),1,drainage)==0){fail("failed to read drainage4 info..");}
      if(swap){
        (grid+cell)->discharge.d[c+1]=swapshort(rbuf1);
        (grid+cell)->discharge.l+=swapshort(rbuf2)*1000;
      }else{
        (grid+cell)->discharge.d[c+1]=rbuf1;
        (grid+cell)->discharge.l+=rbuf2*1000;
      }
    }//..end while
    (grid+cell)->discharge.count=c;
    /* initialisation */
    (grid+cell)->discharge.gir=(grid+cell)->discharge.act_irrig_amount=0.0;
  } /* of for '(cell=0;..)' */
  return FALSE;
}

/* 
case1: l==0, i.e. c==0 && d[1]==-9 no value
case2: l>0 && v>l, i.e. d[c+1]==-1 ocean or inland sink, whole water of considered cell drains to the ocean or inland sink
case3: l>0 && v<=l, i.e. d[c+1]>=0 || d[c+1]==-1 distribution of the water between two cells, or one cell and the ocean/inland sink

*/
