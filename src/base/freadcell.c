/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f r e a d c e l l . c                              **/
/*******************************************************************/

#include "lpj.h"

void reduce_frac(Real *standfrac, Real *remainfrac)
{
  if(*standfrac>=*remainfrac){ 
     *standfrac-=*remainfrac;
     *remainfrac=0;
  }
  else{
    *remainfrac-=*standfrac;
    *standfrac=0;
  }
}

Bool freadcell(FILE *file,Cell *cell,const Pftpar pftpar[],int npft,int ncft,
               const Soilpar *soilpar,Bool swap,Landuse *landuse,int cellid)
{
  Stand *stand,*standg;
  int s,pos,i;
  Real grassfrac[NGRASS*2];
  Real cropfrac=0.0;
  Real naturalfrac=0.0;
  Real frac;
  Real dummy=-9; 
  int separate=NGRASS*2;
  Bool exist[2];
  exist[0]=exist[1]=0;
  #ifdef RIVER_ROUTING
  Real remain_lakefrac;
  #endif
  
  freadint(&cell->skip,1,swap,file);

  if(!cell->skip){
    /* Max 4 stands in restart file: PRIMARY, SECFOREST, SETASIDE, GRASSLAND */
    cell->standlist=freadstandlist(file,pftpar,soilpar,swap);

    if(cell==NULL)
      return TRUE;
    freadclimbuf(file,&cell->climbuf,swap);
    freadreal(cell->gdd,npft+ncft,swap,file);
    freadreal1(&cell->discharge.dmass_new,swap,file);
    freadreal1(&cell->discharge.dmass_lake,swap,file);
    freadcropdates(file,&cell->cropdates,ncft,swap); 
    
    //woodharvest product pools remaining
    freadreal1(&cell->product_100yr,swap,file);
    freadreal1(&cell->product_10yr,swap,file);
    freadreal1(&cell->product_1yr,swap,file);
    //deforestation product pools remaining
    freadreal1(&cell->product_25yr,swap,file);
    freadreal1(&cell->product_2yr,swap,file);
    
    frac=1.0-cell->lakefrac;

    #ifdef RIVER_ROUTING
    /* if a lake fraction exist, 
    * reduce fractions of the stands in the order 
    * natural vegetation, setaside, managed grassland 
    * */
    remain_lakefrac=cell->lakefrac;
    if(remain_lakefrac>0) reduce_frac(&naturalfrac,&remain_lakefrac);
    if(remain_lakefrac>0) reduce_frac(&cropfrac,&remain_lakefrac);
    if(remain_lakefrac>0) reduce_frac(&grassfrac[0],&remain_lakefrac);
    if(remain_lakefrac>0) reduce_frac(&grassfrac[1],&remain_lakefrac);
    #endif

    foreachstand(stand,s,cell->standlist){
      if(stand->landusetype==GRASSLAND){
        if(stand->frac < epsilon){
          delstand(cell->standlist,s);
          s--;
        }else{cropfrac+=stand->frac;}
      }else if(stand->landusetype==SETASIDE){
        if(stand->frac < epsilon){
          delstand(cell->standlist,s);
          s--;
        }else{cropfrac+=stand->frac;}
      }else if(stand->landusetype==PRIMARY){
        naturalfrac+=stand->frac;
      }else if(stand->landusetype==SECFOREST){
        naturalfrac+=stand->frac;
      }else{
         fail("\nONLY GRASSLAND, SETASIDE, PRIMARY, SECFOREST\
                 stands allowed in Restart File\n");
      }
    }//..end standlist

    //update cropfrac
    cell->cropfrac=cropfrac;
  
    /* no NATURAL stand in restart */
    //-------------------------------
    // check for natural fraction
    // if none, take from grassland or setaside
    // (code block effectively not used)
    // .. because ..
    //    .. stand fractions read-in from restart file
    //    .. therefore code is always okay
    //-------------------------------
    if (frac-cell->cropfrac-naturalfrac>epsilon){
      if(existlandusetype(cell->standlist,&pos,SETASIDE) || existlandusetype(cell->standlist,&pos,GRASSLAND)){
        standg=getstand(cell->standlist,pos);
        s=addstand(cell->standlist);
        stand=getstand(cell->standlist,s-1);
        reclaim_land(standg,stand,&dummy,&dummy);
        stand->frac=1.0-frac;
      }else fail("fraction error in freadcell cropfrac %.2f natural_frac= %.5f\n",cell->cropfrac,naturalfrac);
    }
    
    //-----------------------------------------------------
    // Irrigated vs non-irrigated pasture 
    // (code block effectively not used)
    // .. because ..
    //   .. stand fraction read from restart file
    //   .. LUC NOT set up for irrigation
    //   .. alternately, read-in irrigated boolean from restart
    //-----------------------------------------------------
    if (separate<NGRASS*2){
      if (existlandusetype(cell->standlist,&s,GRASSLAND)){
        for (i=separate;i<NGRASS*2;i++)
           if (grassfrac[i]>0) {
             pos=addstand(cell->standlist);
             pos--;
             standg=getstand(cell->standlist,pos);
             stand=getstand(cell->standlist,s);
             reclaim_land(stand,standg,&dummy,&dummy);
             initstand(standg);
             standg->landusetype=GRASSLAND;
             if (i==1)standg->irrigation=TRUE;
             standg->frac=grassfrac[i];
           }
      }else fail("NO GRASSLAND FOUND IN RESTART");
    }//..end if separate grasses    
  }//..end if skip gridcell
  return FALSE;
} /* of 'freadcell' */
