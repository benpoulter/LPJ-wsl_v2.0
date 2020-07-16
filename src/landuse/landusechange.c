/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ l a n d u s e c h a n g e . c                   **/
/*******************************************************************/
#include "lpj.h"
#include "stand.h"
#include "tree.h"
#include "grass.h"

void deforest(Cell *cell,Real difffrac,const Pftpar pftpar[],Landuse *landuse,int npft,int year,Config config) 
{
  int s,pos;
  Stand *natstand,*cutstand;
  Real harvestdef=0; 

  if(existlandusetype(cell->standlist,&s,PRIMARY)){
    pos=addstand(cell->standlist);
    pos--;
    natstand=getstand(cell->standlist,s);
    cutstand=getstand(cell->standlist,pos);
    reclaim_land(natstand,cutstand,&harvestdef,&harvestdef);
    if(difffrac+epsilon>=natstand->frac) {
      difffrac=natstand->frac;
      delstand(cell->standlist,s);
      pos=s;
    }
    else natstand->frac-=difffrac;

    cutstand->frac=difffrac;
    if(year>=config.firstyear)
      cell->output.flux_luc=harvestdef*difffrac; 
    setaside(cell,&pos,pftpar,landuse->intercrop,npft,TRUE); //true to allow establishment
  }
  else fail("No natural stand for deforest()");
  
  return;
}

void regrowth (Cell *cell,
               Real difffrac,
               const Pftpar *pftpar,
               int npft,
               int ntypes, int year,Config config)
{
  int s,p,pos;
  Real rharvest;
  Real dummy=-9;
  Pft *pft;
  Stand *setasidestand,*natstand,*mixstand;

  if(existlandusetype(cell->standlist,&s,SETASIDE)){
    setasidestand=getstand(cell->standlist,s);
    if(setasidestand->frac<=difffrac*-1+epsilon) {          /*setaside stand has not enough space for regrowth*/
      mixstand=getstand(cell->standlist,s);
      foreachpft(pft,p,mixstand->pftlist){
        litter_update(&mixstand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
        delpft(mixstand->pftlist,p);
        p--;
      }
      difffrac=mixstand->frac*-1;
      pos=s;
    }else{
      pos=addstand(cell->standlist);                /*setaside big enough for regrowth*/
      pos--;
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac=difffrac*-1;
      mixstand->landusetype=SETASIDE;
      reclaim_land(setasidestand,mixstand,&dummy,&dummy); 
      setasidestand->frac+=difffrac;
    }
    
    if(existlandusetype(cell->standlist,&s,PRIMARY)){           /*mixing of natural vegetation with regrowth*/
      natstand=getstand(cell->standlist,s);
      mixsoil(natstand,mixstand);
      foreachpft(pft,p,natstand->pftlist)
        mix_veg(pft,natstand->frac/(natstand->frac-difffrac));
      natstand->frac+=mixstand->frac;
      delstand(cell->standlist,pos);
    }else{
      mixstand->landusetype=PRIMARY;
      natstand=mixstand; 
    }
    if(year>=config.firstyear)
      cell->output.flux_estab+=establishmentpft(&natstand->soil.litter,natstand->pftlist,
                                                pftpar,npft,ntypes,cell->gdd,&cell->climbuf,0,TRUE)*natstand->frac;
  }
  return;
}/* of 'regrowth' */

void landexpansion (
               Cell *cell,
               Real difffrac,
               const Pftpar *pftpar,
               int npft,
               int ntypes,
               Bool exist,
               Stand *grassstand,
               Bool irrigation,
               int cultivate_type,
               int year,Config config) 
{
  int s,p,pos,q,t;
  Real rharvest,flux_estab=0;
  int *n_est;
  Real dummy=-9; 
  Pft *pft;
  Stand *setasidestand,*mixstand;

  n_est=newvec(int,ntypes);
  check(n_est);
  for(t=0;t<ntypes;t++)
    n_est[t]=0;
    
  if(existlandusetype(cell->standlist,&s,SETASIDE)){
    setasidestand=getstand(cell->standlist,s);
    if(setasidestand->frac<=difffrac*-1+epsilon) {          /*setaside stand has not enough space for grassland expansion*/
      mixstand=getstand(cell->standlist,s);
      foreachpft(pft,p,mixstand->pftlist){
        litter_update(&mixstand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
        delpft(mixstand->pftlist,p);
        p--;
      }
      difffrac=mixstand->frac*-1;
      pos=s;
    }else{
      pos=addstand(cell->standlist);                /*setaside big enough for grassland expansion*/
      pos--;
      mixstand=getstand(cell->standlist,pos);
      mixstand->frac=difffrac*-1;
      reclaim_land(setasidestand,mixstand,&dummy,&dummy); 
      setasidestand->frac+=difffrac;
    }
    
    if(exist){
      mixsoil(grassstand,mixstand);
      foreachpft(pft,p,grassstand->pftlist)
        mix_veg(pft,grassstand->frac/(grassstand->frac-difffrac));
      grassstand->frac+=mixstand->frac;
      delstand(cell->standlist,pos); /* deleting temporary mixstand copy */   
    }else{
      switch(cultivate_type)
      {
       case 1:
         for(p=0;p<npft;p++)  
           if(establish(cell->gdd[p],pftpar+p,&cell->climbuf,0) && pftpar[p].type==GRASS){ 
             addpft(mixstand->pftlist,pftpar+p);
             n_est[pftpar[p].type]++;
           }
       mixstand->landusetype=GRASSLAND;
         break;
       default:
         fail ("WRONG CULTIVATION_TYPE in landusechange.c");
         break;
      }
      foreachpft(pft,q,mixstand->pftlist){
        flux_estab=establishment(pft,0,0,n_est[pft->par->type]);
      
        if(year>=config.firstyear)
          cell->output.flux_estab+=flux_estab*mixstand->frac;
      }
      mixstand->irrigation=irrigation;
    }
  }
  return;
}/* of 'landexpansion' */

void grasslandreduction (
            Cell *cell,
            Real difffrac,
            const Pftpar pftpar[],
            Landuse *landuse,
            int npft,
            int *s,
            Stand *grassstand)
{
  int pos,p;
  Stand *cutstand;
  Pft *pft;
  Real rharvest;
  Real dummy=-9; 
  
  if(grassstand->frac<=difffrac+epsilon){
    foreachpft(pft,p,grassstand->pftlist){
      litter_update(&grassstand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
      delpft(grassstand->pftlist,p);
      p--;  
    }
    setaside(cell,s,pftpar,landuse->intercrop,npft,TRUE); //true to allow establishment
  }else{
    pos=addstand(cell->standlist);
    pos--;
    cutstand=getstand(cell->standlist,pos);
    reclaim_land(grassstand,cutstand,&dummy,&dummy);
    cutstand->frac=difffrac;
    grassstand->frac-=difffrac;
    setaside(cell,&pos,pftpar,landuse->intercrop,npft,TRUE); //true to allow establishment
  }
  
  return;
}/* of 'grasslandreduction */

void landusechange(Cell *cell,
       const Pftpar *pftpar,
       int npft,
       int ncft,
       int ntypes,
       int cellid, 
       Landuse *landuse,
       int year,Config config)
{
  Real difffrac,newcropfrac;
  Stand *stand,*dummy;
  Bool exist=FALSE,irrigation;
  int cultivation_type;
  Real grassfrac[NGRASS*2]; /*pasture; including irrigation*/
  Real sum;
  int s;

  dummy=getstand(cell->standlist,0); /* is used as dummy for landexpansion if grassland[x] don't exist*/
  grassfrac[0]=(getcelllanduse(landuse,cellid,landuse->nbands))[landuse->nbands-ncft-NGRASS*2]; /*pasture non-irrigated*/
  grassfrac[1]=(getcelllanduse(landuse,cellid,landuse->nbands))[landuse->nbands-1];

  newcropfrac=crop_sum_frac(cellid,landuse);

  difffrac=newcropfrac-cell->cropfrac;

  if(difffrac>0.001 && cell->lakefrac+cell->cropfrac<0.99999) deforest(cell,difffrac,pftpar,landuse,npft,year,config);  // Deforestation
  else if(difffrac<-0.001) regrowth(cell,difffrac,pftpar,npft,ntypes,year,config); // Regrowth

  if (grassfrac[0]>0.0001){
    cultivation_type=1;                                          /*PASTURE*/
    irrigation=FALSE;   
    if (findstand(cell->standlist,&s,GRASSLAND,FALSE)){
      exist=TRUE;
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-grassfrac[0];
      if(difffrac>0.000001)
        grasslandreduction(cell,difffrac,pftpar,landuse,npft,&s,stand);
      else if(difffrac<-0.000001)
        landexpansion(cell,difffrac,pftpar,npft,ntypes,exist,stand,irrigation,cultivation_type,year,config); 
    }else {
      exist=FALSE;
      difffrac=grassfrac[0]*-1;
      landexpansion(cell,difffrac,pftpar,npft,ntypes,exist,dummy,irrigation,cultivation_type,year,config); 
    }
  }


  if (grassfrac[1]>0.0001){
    cultivation_type=1;
    irrigation=TRUE;
    if (findstand(cell->standlist,&s,GRASSLAND,TRUE)){
      exist=TRUE;
      stand=getstand(cell->standlist,s);
      difffrac=stand->frac-grassfrac[1];
      if(difffrac>0.000001)
        grasslandreduction(cell,difffrac,pftpar,landuse,npft,&s,stand);
      else if(difffrac<-0.000001)
        landexpansion(cell,difffrac,pftpar,npft,ntypes,exist,stand,irrigation,cultivation_type,year,config);
    }else{
      exist=FALSE;
      difffrac=grassfrac[1]*-1;
      landexpansion(cell,difffrac,pftpar,npft,ntypes,exist,dummy,irrigation,cultivation_type,year,config); 
    }
  }
  check_stand_fracs(cell->standlist,cell->lakefrac);
  sum=0.0;
  foreachstand(stand,s,cell->standlist)
    if(stand->landusetype!=PRIMARY)
      sum+=stand->frac;
  cell->cropfrac=sum;    /* could be different from landusefraction input, due to not harvested winter cereals*/
  return;
} /* of 'landusechange' */


/*
- called in iterateyear.c
- calls the function crop_sum_frac()
  -> crop_sum_frac() sums the fractions of each crop in the considered cell
     -> share of agricultural land in the cell
  -> calls getcelllanduse() (see landuse.h) 
- compares actual share with new share
- if the new crop share is greater than the actual share then calls local function 
  deforest()
  -> deforest() checks if a stand with natural vegetation still exist 
     (see existlandusetype.c in tools)
  -> if no natural stand exists deforest is not possible
     -> error in input file
  -> if natural stand exists: 
     - add new stand to the standlist and save the position of the new stand in
       the standlist (see addstand() in standlist.c)
     - brings new stand to set-aside stand as follows:
       -> calls function reclaim_land() which copies the values of the natural 
          stand to the new stand (except the pftlist) and updates the litter pools
          of the new stand
  -> updates the fraction of the natural stand
     -> deletes the natural stand if fraction is zero
  -> calls the function setaside() which adds the new stand to the set-aside stand 
     if exist, or sets the new stand to the set-aside stand if no set-aside stand 
     still there
    
- if the new crop share is smaller than actual share then calls local function 
  regrowth()
  -> regrowth() checks if set-aside stand exist (see existlandusetype.c in 
     tools)
  -> if no set-aside stand exists regrowth is still not possible
     -> regrowth will be next year
  -> if set-aside stand exists:
     -> distinguish if fraction of set-aside stand is smaller equal or greater 
  than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole set-aside stand gets natural
  -> update of litter pools for the pfts on the set-aside stand
     -> if the fraction is greater a new stand is added to the standlist
  -> call of function reclaim_land()
  -> update of the fraction of the set-aside stand

     -> distinguish if a natural stand exists
  -> if natural stand exist, mix of soil and vegetation (see mixsoil() in 
     setaside.c and specific functions mix_veg_tree/grass.c)
  -> call of function establishmentpft()

- landusechange of managed grassland
  -> distinguish between irrigated and not irrigated managed grassland stand
  -> distinguish if grassland stand exists:
  -> if the new grassland share is smaller than the actual share then calls the
     local function grasslandreduction()
     -> distinguish if fraction of grassland stand is smaller equal or greater 
  than the determined fraction for reforestation
     -> if the fraction is smaller equal the whole grassland stand goes to the 
        set-aside stand
  -> update of litter pools for the pfts on the grassland stand
        -> call of function setaside()
     -> if the fraction is greater a new stand is added to the standlist
  -> call of function reclaim_land()
  -> update of the fraction of the grassland stand
        -> call of function setaside()

  -> if the new grassland share is greater than the actual share then calls the
     local function landexpansion()
     -> if no set-aside stand exists grassland expansion is still not possible
        -> grassland expansion will be next year
     -> if set-aside stand exists:
        -> distinguish if fraction of set-aside stand is smaller equal or greater 
     than the determined fraction for grassland expansion
        -> if the fraction is smaller equal the whole set-aside stand gets 
           grassland
     -> update of litter pools for the pfts on the set-aside stand
        -> if the fraction is greater a new stand is added to the standlist
     -> call of function reclaim_land()
     -> update of the fraction of the set-aside stand

     -> distinguish if a grassland stand exists
     -> if grassland stand exist, mix of soil and vegetation (see mixsoil() in 
  setaside.c and specific function mix_veg_grass.c)
  -> update of the fraction of the grassland stand
     -> if grassland stand does not exist, the temporary mixstand gets grassland 
        stand
        -> call of function establish() and add pft to the pftlist if possible
        -> call of function establishment()
        -> set the variable irrigation

  -> if the grassland stand does not exist and the grassland stand fraction is 
     greater 0 then calls the local function landexpansion()

- sets the actual crop share to the new crop share 
*/
