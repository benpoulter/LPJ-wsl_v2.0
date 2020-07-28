/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/landuse/ l a n d u s e c h a n g e _ G R O S S . c       **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"
#include "grass.h"

void deforest_GROSS(Cell *cell,Real difffrac,const Pftpar pftpar[],Landuse *landuse,int CROP_PASTURE,int PRIMARY_SECFOREST,int npft,int year,Config config)
{
  Stand *primaryStand, *secforestStand, *newMgStand;
  int pos_primaryStand, pos_newMgStand, pos_secforestStand;
  Real harvestdef=0;
  Real current_deforest_product=0;
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // NOTE:  GROSS_LUC does not use the CROP_PASTURE identifier in fn(deforest) for Priority rules in Deforesting       
  //     :  that is, there is only a priority for deforesting oldest stands first               
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(PRIMARY_SECFOREST == PRIMARY && (existlandusetype(cell->standlist,&pos_primaryStand,PRIMARY))){
    //get stands
    pos_newMgStand=addstand(cell->standlist);
    pos_newMgStand--;
    primaryStand=getstand(cell->standlist,pos_primaryStand);
    newMgStand=getstand(cell->standlist,pos_newMgStand);

    //copy carbon pools and send existing biomass to litter
    reclaim_land(primaryStand,newMgStand,&harvestdef,&current_deforest_product);

    #ifdef DEFORESTPRODUCT
      //multiply current woodproducts by difffrac
      cell->woodproducts_temp += current_deforest_product*difffrac;

      //update the product pools
      //..62.5% of deforest wood mass goes into product pool
      //..37.5% of all deforest wood goes to 2yr pool
      //..25% of all wood goes into 25yr pool:
      //..product25yr = 25 year decay pool
      //..product2yr  =  2 year decay pool
      cell->product_25yr += 0.40*current_deforest_product*difffrac;
      cell->product_2yr  += 0.60*current_deforest_product*difffrac;

      #ifdef OUTPUT_BYSTAND
        cell->product_25yr_primary += 0.40*current_deforest_product*difffrac;
        cell->product_2yr_primary  += 0.60*current_deforest_product*difffrac;
      #endif
    #endif
    
    //delete primaryStand if deforesting entire primaryStand fraction
    if(difffrac >= primaryStand->frac){
      difffrac=primaryStand->frac;
      delstand(cell->standlist,pos_primaryStand);
      pos_newMgStand=pos_primaryStand;
    }else{
      primaryStand->frac -= difffrac;
    }

    //set fraction of newMgStand
    newMgStand->frac=difffrac;

    //DEVQ: add option for outputs if transient start year is 1500, 1700, etc
    if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
      cell->output.flux_luc+=harvestdef*difffrac;
      cell->output.flux_luc_primary+=harvestdef*difffrac;
    }

    //send new_cutstand to setaside--temp designation for mixing new stands before regrowth
    setaside(cell,&pos_newMgStand,pftpar,landuse->intercrop,npft,FALSE);

  } else if(PRIMARY_SECFOREST == SECFOREST && (existlandusetype(cell->standlist,&pos_secforestStand,SECFOREST))){
    //get stand
    pos_newMgStand=addstand(cell->standlist);
    pos_newMgStand--;
    secforestStand=getstand(cell->standlist,pos_secforestStand);
    newMgStand=getstand(cell->standlist,pos_newMgStand);

    //copy carbon pools and send existing biomass to litter
    reclaim_land(secforestStand,newMgStand,&harvestdef,&current_deforest_product);

    #ifdef DEFORESTPRODUCT
      //multiply current woodproducts by difffrac
      cell->woodproducts_temp += current_deforest_product*difffrac;

      //update the product pools
      //..62.5% of deforest wood mass goes into product pool
      //..37.5% of all deforest wood goes to 2yr pool
      //..25% of all wood goes into 25yr pool:
      //..product25yr = 25 year decay pool
      //..product2yr  =  2 year decay pool
      cell->product_25yr += 0.40*current_deforest_product*difffrac;
      cell->product_2yr  += 0.60*current_deforest_product*difffrac;

      #ifdef OUTPUT_BYSTAND
        cell->product_25yr_secforest += 0.40*current_deforest_product*difffrac;
        cell->product_2yr_secforest  += 0.60*current_deforest_product*difffrac;
      #endif
    #endif
    
    //delete secforestStand if deforesting entire secforestStand fraction
    if(difffrac >= secforestStand->frac){
      difffrac=secforestStand->frac;
      delstand(cell->standlist,pos_secforestStand);
      pos_newMgStand=pos_secforestStand;
    }else{
      secforestStand->frac -= difffrac;
    }

    //set fraction of newMgStand
    newMgStand->frac=difffrac;

    //DEVQ: add option for outputs if transient start year is 1500, 1700, etc
    if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
      cell->output.flux_luc+=harvestdef*difffrac;
      cell->output.flux_luc_secforest+=harvestdef*difffrac;
    }

    //send new_cutstand to setaside--temp designation for mixing new stands before regrowth
    setaside(cell,&pos_newMgStand,pftpar,landuse->intercrop,npft,FALSE);

   }else {//error trap
     //here: secforestStand = primary || secforest
     foreachstand(secforestStand,pos_secforestStand,cell->standlist){
       printf("\nPROBLEM No PRIMARY || SECFOREST stand to deforest()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, convMg: %d, fracConv: %.10f\n", 
         secforestStand->landusetype, secforestStand->frac, PRIMARY_SECFOREST, CROP_PASTURE, difffrac);
     }
     printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
     fail("\nNo PRIMARY || SECFOREST stand to deforest()\n");
   }
  return;
}

void harvest_GROSS(Cell *cell,int cellid, Real difffrac, Real harvest_biomass, const Pftpar pftpar[],Landuse *landuse,Bool exist,Stand *secforestTemp, int PRIMARY_SECFOREST,int npft,int year,Config config)
{
  Stand *primaryStand, *newSecforestStand, *secforestStand;
  int pos_primaryStand, pos_newSecforestStand, pos_secforestStand;
  int p;
  Pft *pft;

  if(PRIMARY_SECFOREST == PRIMARY && (existlandusetype(cell->standlist,&pos_primaryStand,PRIMARY))){
    primaryStand=getstand(cell->standlist,pos_primaryStand);

    pos_newSecforestStand=addstand(cell->standlist);
    pos_newSecforestStand--;
    newSecforestStand=getstand(cell->standlist,pos_newSecforestStand);
    newSecforestStand->frac=difffrac;

    //copy fire, soil, veg and harvest wood
    newSecforestStand->fire_sum=primaryStand->fire_sum;
    copysoil(primaryStand, newSecforestStand);
    foreachpft(pft, p, primaryStand->pftlist){
      if( copy_pft(pft, newSecforestStand, pftpar, npft) < 0){
        fail("something went wrong in copy_pft in ./src/landuse/landusechange_GROSS.c");}
    }

    #if !defined(NOWDHARVEST)
      newSecforestStand->landusetype=PRIMARY; //temp landuse designation
      woodharvest_updateStand(cell,newSecforestStand, difffrac, harvest_biomass, landuse, cellid, TRUE);
    #endif

    //update landuse designation
    newSecforestStand->landusetype=SECFOREST_TEMP;

    if(difffrac >= primaryStand->frac){
      foreachpft(pft,p,primaryStand->pftlist){
        delpft(primaryStand->pftlist,p);
        p--;
      }
      delstand(cell->standlist,pos_primaryStand);
      pos_newSecforestStand=pos_primaryStand;
    }else {
      primaryStand->frac -= difffrac; //reduce primary forest fraction by area harvested -> secforest
    }

    //MIX if secforestTemp stand exists
    //.. all secforestTemp stands are mixed together, then mixed with secforest stand at end of wood harvet routines
    if(exist){
      //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
      mix_standComplex(cell, secforestTemp, newSecforestStand, pos_newSecforestStand, pftpar, npft, FALSE);
    }

  }else if( (PRIMARY_SECFOREST == SECFOREST) && (existlandusetype(cell->standlist,&pos_secforestStand,SECFOREST))){
    //stand->frac stays the same w/o tiling
    secforestStand=getstand(cell->standlist,pos_secforestStand);

    #if !defined(NOWDHARVEST)
      woodharvest_updateStand(cell, secforestStand, secforestStand->frac, harvest_biomass, landuse, cellid, TRUE);
    #endif

  }else {//error trap
    //here: secforestStand = primary || secforest
    foreachstand(secforestStand,pos_secforestStand,cell->standlist){
      printf("\nPROBLEM No PRIMARY || SECFOREST stand for wood harvest ()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, fracConv: %.10f\n",
        secforestStand->landusetype, secforestStand->frac, PRIMARY_SECFOREST, difffrac);
    }

    printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
    fail("\nNo PRIMARY || SECFOREST stand for wood harvest()\n");
  }
  return;
}

void landexpansion_GROSS(
               Cell *cell,
               Real difffrac,
               const Pftpar *pftpar,
               int npft,
               int ntypes,
               Bool exist,
               Stand *grassStand,
               Bool irrigation,
               int cultivate_type,
               int year,Config config)
{
  int p,t, pos_setaside, pos_newMgStand;
  int *n_est;
  Real rharvest;
  Real dummy=-9;
  Real current_deforest_product=0;
  Stand *setasideStand, *newMgStand;
  Pft *pft;
  
  n_est=newvec(int,ntypes);
  check(n_est);
  for(t=0;t<ntypes;t++)
    n_est[t]=0;
    
  if(existlandusetype(cell->standlist,&pos_setaside,SETASIDE)){
    //DEVQ: For PASTURE: the SETASIDE stand with maximum Grass-pft needs to be passed here as Priority Rule
    //    : For CROP   : the SETASIDE stand with oldest Forest needs to passed here as Priority Rule (soil C as priority?)
    //    : DEVQ: condisider for both (above) pass into the landexpansion(fn) with a TAG (CROP / PASTURE)
    setasideStand=getstand(cell->standlist,pos_setaside);

    if(difffrac >= setasideStand->frac){ /*setaside stand has not enough space for grassland expansion*/
      newMgStand=getstand(cell->standlist,pos_setaside);
      foreachpft(pft,p,newMgStand->pftlist){
        litter_update(&newMgStand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
        delpft(newMgStand->pftlist,p);
        p--;
      }
      difffrac=newMgStand->frac;
      pos_newMgStand=pos_setaside;
    }else{
      pos_newMgStand=addstand(cell->standlist); /*setaside big enough for grassland expansion*/
      pos_newMgStand--;
      newMgStand=getstand(cell->standlist,pos_newMgStand);
      newMgStand->frac=difffrac;

      #ifdef DEFORESTPRODUCT
      dummy=0;
      #endif
      
      //now update stand for grassland expansion..
      reclaim_land(setasideStand,newMgStand,&dummy,&current_deforest_product);
      
      #ifdef DEFORESTPRODUCT
        //multiply current woodproducts by difffrac
        cell->woodproducts_temp += current_deforest_product*difffrac;

        //update the product pools
        //..62.5% of deforest wood mass goes into product pool
        //..37.5% of all deforest wood goes to 2yr pool
        //..25% of all wood goes into 25yr pool:
        //..product25yr = 25 year decay pool
        //..product2yr  =  2 year decay pool
        cell->product_25yr += 0.40*current_deforest_product*difffrac;
        cell->product_2yr  += 0.60*current_deforest_product*difffrac;

        if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
          //update the harvest flux
          cell->output.flux_luc+=dummy*difffrac;
        }
      #endif
      
      setasideStand->frac -= difffrac;
    }

    if(exist){//mix with GRASSLAND_TEMP
      //simple complex: copy soil, mix pft->nind, increase frac, and delete stand#2
      mix_standComplex(cell, grassStand, newMgStand, pos_newMgStand, pftpar, npft, FALSE);
    }else{
      switch(cultivate_type)
      {
       case 1:
         newMgStand->landusetype=GRASSLAND_TEMP;
         break;
       default:
         fail ("WRONG CULTIVATION_TYPE in landusechange.c");
         break;
      }
      newMgStand->irrigation=irrigation;

    }
  }
  return;
}/* of 'landexpansion' */

void grasslandreduction_GROSS(
            Cell *cell,
            Real difffrac,
            const Pftpar pftpar[],
            Landuse *landuse,
            int npft,
            int *s,
            Stand *grassStand, int year, Config config)
{
  int pos_newSecforestStand,p;
  Stand *newSecforestStand;
  Pft *pft;
  Real rharvest;
  Real dummy=-9;
  Real current_deforest_product=0;
  
  //if existing GRASSLAND is less than difffrac, delete the GRASSLAND stand
  if(difffrac >= grassStand->frac){
    foreachpft(pft,p,grassStand->pftlist){
      litter_update(&grassStand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
      delpft(grassStand->pftlist,p);
      p--;  
    }
    setaside(cell,s,pftpar,landuse->intercrop,npft,FALSE);
  }else{//else just reduce the fraction of GRASSLAND by amount=difffrac
    pos_newSecforestStand=addstand(cell->standlist);
    pos_newSecforestStand--;//required decrement bc ListLength index is NULL
    newSecforestStand=getstand(cell->standlist,pos_newSecforestStand);
    #ifdef DEFORESTPRODUCT
      dummy=0;
    #endif

    //now update stand for grassland reduction..
    reclaim_land(grassStand,newSecforestStand,&dummy,&current_deforest_product);

    #ifdef DEFORESTPRODUCT
      //multiply current woodproducts by difffrac
      cell->woodproducts_temp += current_deforest_product*difffrac;

      //update the product pools
      //..62.5% of deforest wood mass goes into product pool
      //..37.5% of all deforest wood goes to 2yr pool
      //..25% of all wood goes into 25yr pool
      //..product25yr = 25 year decay pool
      //..product2yr  =  2 year decay pool
      cell->product_25yr += 0.40*current_deforest_product*difffrac;
      cell->product_2yr  += 0.60*current_deforest_product*difffrac;

      if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
        //update the harvest flux
        cell->output.flux_luc+=dummy*difffrac;
      }
    #endif
    
    newSecforestStand->frac=difffrac;
    grassStand->frac -= difffrac;

    setaside(cell,&pos_newSecforestStand,pftpar,landuse->intercrop,npft,FALSE);
  }

  return;
}/* of 'grasslandreduction */

void regrowth_GROSS(Cell *cell,
               Real difffrac,
               const Pftpar *pftpar,
               int npft,
               int ntypes, int year,Config config)
{
  int p;
  Real rharvest;
  Real dummy=-9; 
  Real current_deforest_product=0;
  Pft *pft;
  Stand *setasideStand;
  
  int pos_setaside, pos_newSecforestStand, pos_existSecforestTemp;
  Stand *newSecforestStand, *existSecforestTemp;

  if(existlandusetype(cell->standlist,&pos_setaside,SETASIDE)){
    setasideStand=getstand(cell->standlist,pos_setaside);

    //-----------------------------------
    //FIRST: litter update or reclaim land
    //   if: fraction is smaller-equal to difffrac, then whole setasidestand is converted to Natural
    // else: new stand (setaside..SECFOREST_TEMP) is added and merged with the youngest secondary stand at end of landusechange routine
    //-----------------------------------

    //setaside stand has not enough space for regrowth
    if(setasideStand->frac <= difffrac) {
      foreachpft(pft,p,setasideStand->pftlist){
        litter_update(&setasideStand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
        delpft(setasideStand->pftlist,p);
        p--;
      }
      difffrac=setasideStand->frac;
      newSecforestStand=getstand(cell->standlist,pos_setaside);
      pos_newSecforestStand=pos_setaside;

    }else{
      pos_newSecforestStand=addstand(cell->standlist); //setaside big enough for regrowth
      pos_newSecforestStand--;
      newSecforestStand=getstand(cell->standlist,pos_newSecforestStand);
      newSecforestStand->frac=difffrac;
      newSecforestStand->landusetype=SETASIDE;//temp designation, updated few lines down

      //inPlace of reclaim_land().c
      newSecforestStand->fire_sum=setasideStand->fire_sum;
      copysoil(setasideStand, newSecforestStand);
      foreachpft(pft,p,setasideStand->pftlist){
        #ifdef DEFORESTPRODUCT
          dummy=0;
          current_deforest_product=0;
        #endif
        
        litter_update_deforest(&newSecforestStand->soil.litter,pft,pft->nind,&dummy,&current_deforest_product,TRUE);
        
        #ifdef DEFORESTPRODUCT
          //multiply current woodproducts by difffrac
          cell->woodproducts_temp += current_deforest_product*difffrac;

          //update the product pools
          //..62.5% of deforest wood mass goes into product pool
          //..37.5% of all deforest wood goes to 2yr pool
          //..25% of all wood goes into 25yr pool
          //..product25yr = 25 year decay pool
          //..product2yr  =  2 year decay pool
          cell->product_25yr += 0.40*current_deforest_product*difffrac;
          cell->product_2yr  += 0.60*current_deforest_product*difffrac;

          if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
            //update the harvest flux
            cell->output.flux_luc+=dummy*difffrac;
          }
        #endif
      }//..end pft loop
      setasideStand->frac -= difffrac;
    }

    //----------------------------------
    //SECOND: if SecforestTemp exists, then mix with the temp stand (youngest stand)
    //       .. merge only with youngest Forest if similar
    //----------------------------------
    if(existlandusetype(cell->standlist,&pos_existSecforestTemp,SECFOREST_TEMP)){//mixing of natural vegetation with regrowth//
      existSecforestTemp=getstand(cell->standlist,pos_existSecforestTemp);

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
      mix_standComplex(cell, existSecforestTemp, newSecforestStand, pos_newSecforestStand, pftpar, npft, FALSE);
    }else{
      newSecforestStand->landusetype=SECFOREST_TEMP;
      existSecforestTemp=newSecforestStand;
    }
  }
  return;
}/* of 'regrowth' */

///////////////////////////////////////////////////////////////////////////////////////////
/// hyper-functions that call deforest, harvest, landexpansion, and regrowth      
/// catches special cases                                
/// ...where climate precludes pfts from occurring, yet LU transitions are prescribed  
///////////////////////////////////////////////////////////////////////////////////////////
void deforest_expandMgLand(//calls deforest_GROSS and landexpansion GROSS
    int PRIMARY_SECFOREST,
    Real difffrac,
    int CROP_PASTURE,
    const Pftpar *pftpar,
    int npft,
    int ntypes,
    int year,
    Config config,
    Cell* cell,
    Landuse* landuse, int cultivation_type, int irrigation){

  ///////////////////////////////////////////////////
  //  D e f o r e s t  &  E x p a n d  M g L a n d //
  ///////////////////////////////////////////////////

  //   primary -> pasture
  // secForest -> pasture
  //
  //   primary -> crop
  // secForest -> crop
  //
  //      crop -> pasture (inter-conversion not implemented)
  //   pasture -> crop  (inter-conversion not implemented)

  int s, pos_grassTemp, pos_newStand;
  Bool exist;
  Stand  *stand, *grassTemp, *newStand, *dummy;

  if (difffrac > 0) {

    dummy=getstand(cell->standlist,0);
    if (existlandusetype(cell->standlist, &s, PRIMARY_SECFOREST)){
      stand = getstand(cell->standlist, s);

      //catch floating-point errors
      if(difffrac+epsilon >= stand->frac){
        difffrac = stand->frac;
      }

      if (getlistlen(stand->pftlist) == 0) {//no pfts, simply mix and reduce stand fractions

        pos_newStand = addstand(cell->standlist);
        pos_newStand--;
        newStand=getstand(cell->standlist,pos_newStand);
        copysoil(stand, newStand);
        //litter_copy(stand, newStand);
        newStand->fire_sum=stand->fire_sum;
        newStand->frac=difffrac;
        newStand->landusetype = SETASIDE;//temp designation
        if(existlandusetype(cell->standlist, &pos_grassTemp,GRASSLAND_TEMP)){
          grassTemp=getstand(cell->standlist,pos_grassTemp);

          //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
          mix_standComplex(cell, grassTemp, newStand, pos_newStand, pftpar, npft, FALSE);

        }else{
          newStand->landusetype = GRASSLAND_TEMP;
          newStand->TimeSinceDist=0;
          newStand->CropYrs=stand->CropYrs;
        }

        ////////////////////////////////
        if (difffrac >= stand->frac) {
          delstand(cell->standlist, s);
        }else {
          //simple reduction of stand fraction
          stand->frac -= difffrac;
        }

      } else {
        deforest_GROSS(cell, difffrac, pftpar, landuse, CROP_PASTURE, PRIMARY_SECFOREST, npft, year, config);
        if (existlandusetype(cell->standlist, &pos_grassTemp,GRASSLAND_TEMP)) {
          exist = TRUE;          //mix with GRASSLAND_TEMP
          grassTemp = getstand(cell->standlist, pos_grassTemp);
          landexpansion_GROSS(cell,difffrac, pftpar,npft, ntypes, exist, grassTemp, irrigation,cultivation_type, year, config);
        } else {
          exist = FALSE;          //create GRASSLAND_TEMP
          landexpansion_GROSS(cell,difffrac, pftpar,npft, ntypes, exist, dummy, irrigation, cultivation_type, year, config);
        }
      }
    }
  }

  return;
}

void abandonMgLand_regrow(
             Real difffrac,
             const Pftpar *pftpar,
             int npft,
             int ntypes,
             int year,
             Config config,
             Cell* cell,
             Landuse* landuse) {

  int pos_secforest, pos_grassStand, pos_newStand;
  Stand *secforest, *grassStand, *newStand;

  ////////////////////////////////////////////////////////////////
  //  A b a n d o n  L a n d  &  C r e a t e  S e c F o r e s t //
  ////////////////////////////////////////////////////////////////
  //crop -> secForest
  //pasture -> secForest
  if (difffrac > 0) {

    if (existlandusetype(cell->standlist, &pos_grassStand, GRASSLAND)){
      grassStand = getstand(cell->standlist, pos_grassStand);

      //catch floating-point errors
      if(difffrac+epsilon >= grassStand->frac){
        difffrac = grassStand->frac;
      }

      //if no pfts, simply mix and reduce fractions
      if (getlistlen(grassStand->pftlist) == 0) {
        pos_newStand = addstand(cell->standlist);
        pos_newStand--;
        newStand=getstand(cell->standlist,pos_newStand);
        copysoil(grassStand, newStand);
        newStand->fire_sum=grassStand->fire_sum;
        newStand->frac=difffrac;
        newStand->landusetype = SETASIDE;//temp designation
        if(existlandusetype(cell->standlist, &pos_secforest,SECFOREST_TEMP)){
          secforest=getstand(cell->standlist,pos_secforest);

          //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
          mix_standComplex(cell, secforest, newStand, pos_newStand, pftpar, npft, FALSE);
        }else{
          //DEVQ: TimeSinceDist & CropYrs should be taken as a mean
          newStand->landusetype = SECFOREST_TEMP;
          newStand->TimeSinceDist=0;
          newStand->CropYrs=grassStand->CropYrs;
        }
        ////////////////////////////////
        ////////////////////////////////
        if (difffrac >= grassStand->frac) {
          delstand(cell->standlist, pos_grassStand);
        }else {
          //simple reduction of stand fraction
          grassStand->frac -= difffrac;
        }
      } else {
        grasslandreduction_GROSS(cell,difffrac, pftpar,landuse, npft, &pos_grassStand, grassStand, year, config);
        regrowth_GROSS(cell, difffrac,pftpar, npft, ntypes, year, config);
      }
    }
  }

  return;
}

void woodharvest_createSecforest(
    int PRIMARY_SECFOREST,
    Real difffrac,
    Real biomass2harvest,
    const Pftpar *pftpar,
    int npft,
    int year,
    Config config,
    Cell* cell,
    int cellid,
    Landuse* landuse){

  int s, pos_secforest, pos_newStand;
  Bool exist;
  Stand *stand, *secforest, *newStand, *dummy;

  ////////////////////////////////////////////////////////
  //  W o o d  H a r v e s t                
  ////////////////////////////////////////////////////////
  //harvest...
  //      secforest Old   -> secforest
  //      secforest Young -> secforest
  //  secforest nonForest -> secforest
  //        primary -> secforest
  //    primary nonForest -> secforest

  if (difffrac > 0) {
    dummy=getstand(cell->standlist,0);

    if (existlandusetype(cell->standlist, &s, PRIMARY_SECFOREST)){
      stand = getstand(cell->standlist, s);

      //catch floating-point errors
      if(difffrac+epsilon >= stand->frac){
        difffrac = stand->frac;
      }

      //if no pfts, simply mix and reduce fractions
      if (getlistlen(stand->pftlist) == 0){
        if(PRIMARY_SECFOREST == PRIMARY){
          pos_newStand = addstand(cell->standlist);
          pos_newStand--;
          newStand=getstand(cell->standlist,pos_newStand);
          copysoil(stand, newStand);
          newStand->fire_sum=stand->fire_sum;
          newStand->frac=difffrac;
          newStand->landusetype = SETASIDE;//temp designation
          if(existlandusetype(cell->standlist, &pos_secforest,SECFOREST_TEMP)){
            secforest=getstand(cell->standlist,pos_secforest);

            //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
            mix_standComplex(cell, secforest, newStand, pos_newStand, pftpar, npft, FALSE);
          }else{
            newStand->landusetype = SECFOREST_TEMP;
            newStand->TimeSinceDist=0;
            newStand->CropYrs=stand->CropYrs;
          }

          /////////////////////////////////
          if (difffrac >= stand->frac) {
            delstand(cell->standlist, s);
          }else {
            stand->frac -= difffrac;
          }
        } else {//SECFOREST harvest...
            //do nothing because if no pfts, then secondary stand stays the same
        }
      }else{
        //if there are pfts...
        if (existlandusetype(cell->standlist, &pos_secforest,SECFOREST_TEMP)) {
          exist = TRUE; //mix with SECFOREST_TEMP
          secforest = getstand(cell->standlist, pos_secforest);
          harvest_GROSS(cell,cellid, difffrac, biomass2harvest, pftpar, landuse,exist, secforest, PRIMARY_SECFOREST, npft, year,config);
        } else {
          exist = FALSE;
          harvest_GROSS(cell,cellid, difffrac, biomass2harvest, pftpar, landuse,exist, dummy, PRIMARY_SECFOREST, npft, year, config);
        }
      }
    }
  }

  return;
}

///////////////////////////////////////////////////////////////////////
///                                  
///    M A I N   F N :  L A N D U S E C H A N G E _ G R O S S    
///                                 
///////////////////////////////////////////////////////////////////////
void landusechange_GROSS(Cell *cell,
       const Pftpar *pftpar,
       int npft,
       int ncft,
       int ntypes,
       int cellid, 
       Landuse *landuse,
       int year,
       Config config)
{
#if !defined(NAT_VEG) && (defined(GROSS_LUC) || defined(NETinGROSS))

  Real sum;
  int p,s,t, pos_grassStand, pos_grassTemp, pos_secforest, pos_secforestTemp, cultivation_type, FORESTTYPE;
  Stand *stand, *grassStand, *grassTemp, *secforestStand, *secforestTemp;
  Bool irrigation, *present;
  int *n_est; 
  Pft *pft;
  Real flux_estab;

  //--------------------------------------------------------------------------------------
  // for landuse spin-up:
  //  ...modify the deforest/abandonment fractions to ensure proper LU conversion
  //  ...first spin-up is nat_veg (100% natural fraction
  //  ...landuse spin-up requires matching the existing stand fractions (pri/sec/mgland)
  //  (1) in first year of the landuse_spin, all deforestation (cropland creation)
  //  (2) in second year of the landuse spin, creation of secondary forest
  //---------------------------------------------------------------------------------------
  if(config.firstyear == config.lastyear){ //..landuse_spinup 

    if(year == config.firstyear-config.nspinup){
      //deforest the fraction required for secondary and mgland fractions
      landuse->PriForest2crop_frac[cellid]= landuse->SecForest_frac[cellid] + landuse->crop_frac[cellid] + landuse->pasture_frac[cellid];

      //landuse->PriForest2crop_frac[cellid]=0;
      landuse->crop2SecForest_frac[cellid]=0;

    }else if(year == config.firstyear-config.nspinup+1){
      //abandon mgLand required for secondary fraction
      landuse->crop2SecForest_frac[cellid]= landuse->SecForest_frac[cellid];

      landuse->PriForest2crop_frac[cellid]=0;
      //landuse->crop2SecForest_frac[cellid]=0;
    }else{
      landuse->PriForest2crop_frac[cellid]=0;
      landuse->crop2SecForest_frac[cellid]=0;
    }

    //set all other states and transitions to zero
    landuse->crop_frac[cellid]=0;
    landuse->crop2pasture_frac[cellid]=0;
    //landuse->crop2SecForest_frac[cellid]=0;
    landuse->pasture2crop_frac[cellid]=0;
    landuse->pasture2SecForest_frac[cellid]=0;
    landuse->SecForest_frac[cellid]=0;
    landuse->SecForest2crop_frac[cellid]=0;
    landuse->SecForest2pasture_frac[cellid]=0;
    //landuse->PriForest2crop_frac[cellid]=0;
    landuse->PriForest2pasture_frac[cellid]=0;
    landuse->PriForest_frac[cellid]=0;
    landuse->pasture_frac[cellid]=0;
    landuse->SummLU_frac[cellid]=0;
    landuse->WHarvestPrimary_frac[cellid]=0;
    landuse->WHarvestPrimaryNonForest_frac[cellid]=0;
    landuse->WHarvestSecForestOld_frac[cellid]=0;
    landuse->WHarvestSecForestYoung_frac[cellid]=0;
    landuse->WHarvestSecForestNonForest_frac[cellid]=0;
    landuse->WHarvestPrimary_biomass[cellid]=0;
    landuse->WHarvestPrimaryNonForest_biomass[cellid]=0;
    landuse->WHarvestSecForestOld_biomass[cellid]=0;
    landuse->WHarvestSecForestYoung_biomass[cellid]=0;
    landuse->WHarvestSecForestNonForest_biomass[cellid]=0;
  } //..end of transient landusechange

  ////////////////////////
  //
  // Gross transition between MgLand and Natural
  //
  ////////////////////////
  //DEVQ: mix all crop 1st go
  //    : BUT, need to keep track of CropYrs, therefore, crop [GRASSLAND] also requires multiple stands
  //DEVQ: separate pastures for irrigated vs non-irrigated
  cultivation_type=1;  //PASTURE
  irrigation=FALSE;

  /////////////////////////////////////////////////////
  //  D e f o r e s t   &   E x p a n d  M g L a n d 
  /////////////////////////////////////////////////////
  //for PriForest
  deforest_expandMgLand(PRIMARY,landuse->PriForest2pasture_frac[cellid], PASTURE, pftpar, npft, ntypes, 
                        year, config,  cell, landuse, cultivation_type, irrigation);
  deforest_expandMgLand(PRIMARY,landuse->PriForest2crop_frac[cellid], CROP, pftpar, npft, ntypes, 
                        year, config,  cell, landuse, cultivation_type, irrigation);
  //for SecForest
  deforest_expandMgLand(SECFOREST,landuse->SecForest2pasture_frac[cellid], PASTURE, pftpar, npft, ntypes, 
                        year, config,  cell, landuse, cultivation_type, irrigation);
  deforest_expandMgLand(SECFOREST,landuse->SecForest2crop_frac[cellid], CROP, pftpar, npft, ntypes, 
                        year, config,  cell, landuse, cultivation_type, irrigation);

  //////////////////////////////////////////////////////////////////
  //  A b a n d o n  L a n d   &   C r e a t e  S e c F o r e s t 
  //////////////////////////////////////////////////////////////////
  //always creates secforest
  abandonMgLand_regrow(landuse->pasture2SecForest_frac[cellid], pftpar, npft, ntypes, year, config, cell, landuse);
  abandonMgLand_regrow(landuse->crop2SecForest_frac[cellid], pftpar, npft, ntypes, year, config, cell, landuse);

  ////////////////////////////////////////////////////////
  //  W o o d  H a r v e s t                 
  ////////////////////////////////////////////////////////
  //harvest from primary forests
  woodharvest_createSecforest(PRIMARY, landuse->WHarvestPrimary_frac[cellid], landuse->WHarvestPrimary_biomass[cellid], 
                              pftpar, npft, year, config, cell,cellid, landuse);
  woodharvest_createSecforest(PRIMARY, landuse->WHarvestPrimaryNonForest_frac[cellid], landuse->WHarvestPrimaryNonForest_biomass[cellid], 
                              pftpar, npft, year, config, cell, cellid, landuse);

  //harvest from secondary forests
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestOld_frac[cellid], landuse->WHarvestSecForestOld_biomass[cellid], 
                              pftpar, npft, year, config, cell, cellid, landuse);
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestYoung_frac[cellid], landuse->WHarvestSecForestYoung_biomass[cellid], 
                              pftpar, npft, year, config, cell, cellid, landuse);
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestNonForest_frac[cellid], landuse->WHarvestSecForestNonForest_biomass[cellid], 
                              pftpar, npft, year, config, cell, cellid, landuse);

  /////////////////////////////////////////////////////////////////
  //      M I X :  GRASSLAND_TEMP && GRASSLAND         
  //                                 
  //  grassland_temp = current year area of grassland expansion  
  /////////////////////////////////////////////////////////////////
  //initialize vector to hold data on number of establishing pft types
  n_est=newvec(int,ntypes);
  check(n_est);

  //initialize vector to check for presence of pfts
  present=newvec(Bool,npft);
  check(present);

  //mixing at end (here) avoids converting recently created MgLand to SecForest
  if (existlandusetype(cell->standlist, &pos_grassTemp, GRASSLAND_TEMP)) { //if there was a grasslandExpansion
    grassTemp = getstand(cell->standlist, pos_grassTemp);

    //------------------------
    // DEVQ: grassland stands should be kept separate if using multiple crops and pasture types (irrigated vs. non-irrgated)
    //     : ..then establishment should be here and the stand should only mix with similar LU types
    //-------------------------
    // NOTE: keep switch(jump) table
    //     :  .. more efficient for many cultivars
    //-------------------------

    //allow establishment on the new fraction (there shouldn't be any pfts on new grasslands)
    //..then mix
    //..but check for existing pfts
    for(t=0;t<ntypes;t++) n_est[t]=0;//establishing pft types
    for(p=0;p<npft;p++)
      present[p]=FALSE;
    foreachpft(pft,p,grassTemp->pftlist)
      present[pft->par->id]=TRUE;

    switch(cultivation_type)
    {
      case 1:
        for(p=0;p<npft;p++)
           if(establish(cell->gdd[p],pftpar+p,&cell->climbuf,0) && pftpar[p].type==GRASS){
           if(!present[p]) addpft(grassTemp->pftlist,pftpar+p);
           n_est[pftpar[p].type]++;
           }
        break;
      default:
        fail ("WRONG CULTIVATION_TYPE in landusechange_GROSS.c");
        break;
    }

    //establish
    foreachpft(pft,p,grassTemp->pftlist){
      flux_estab=establishment(pft,0,0,n_est[pft->par->type]);
      if(year>=config.firstyear)
        cell->output.flux_estab+=flux_estab*grassTemp->frac;
    }

    //----------------------
    // mix after establish
    //----------------------
    if (existlandusetype(cell->standlist, &pos_grassStand, GRASSLAND)) {
      grassStand = getstand(cell->standlist, pos_grassStand);

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
      mix_standComplex(cell, grassStand, grassTemp, pos_grassTemp, pftpar, npft, FALSE);
    }else {
      //if no grassland exists, set the temp stand to GRASSLAND
      grassTemp->landusetype = GRASSLAND;
    }
  }

  ///////////////////end mix grassland_temp & grassland
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////
  //      M I X :  SECFOREST_TEMP && SECFOREST        
  //                                
  // secforestTemp = current year area of new secondary stand   
  ////////////////////////////////////////////////////////////////

  //mixing at end (here) avoids converting recently created MgLand to SecForest
  if(existlandusetype(cell->standlist,&pos_secforestTemp,SECFOREST_TEMP)){//if there was a grasslandExpansion
    secforestTemp=getstand(cell->standlist,pos_secforestTemp);

    FORESTTYPE = SECFOREST;

    //if number of secondary forests is at maximum, then return the youngest secforest stand, else set temp stand to youngest stand
    if(existlandusetype(cell->standlist,&pos_secforest,FORESTTYPE)){
      secforestStand=getstand(cell->standlist,pos_secforest);

      //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
      mix_standComplex(cell, secforestStand, secforestTemp, pos_secforestTemp, pftpar, npft, FALSE);

      //DEVQ: should establishment on new secondary stands be moved to before mixing?
      //try to establish in youngest stand
      //limit tree establishment where annual precip is greater than 100 mm (APREC_MIN)
      if(cell->aprec>=APREC_MIN){
        //moved establishment to AFTER mixing with secforest stand
        flux_estab=establishmentpft(&secforestStand->soil.litter,secforestStand->pftlist,
                                    pftpar,npft,ntypes,cell->gdd,&cell->climbuf,0,TRUE)*secforestStand->frac;

        if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
          //update output
          cell->output.flux_estab+=flux_estab;
          #ifdef OUTPUT_BYSTAND
            cell->output.flux_estab_secforest+=flux_estab;
          #endif
        }
      }
    }else{//new secforest stand
      secforestTemp->landusetype=FORESTTYPE;

      //standid 12 is default for oldest secforest stand, see stand.h for codes
      secforestTemp->ageclass_standid=12;

      //try to establish in youngest stand
      //limit tree establishment where annual precip is greater than 100 mm (APREC_MIN)
      if(cell->aprec>=APREC_MIN){
        flux_estab=establishmentpft(&secforestTemp->soil.litter,secforestTemp->pftlist,
                                    pftpar,npft,ntypes,cell->gdd,&cell->climbuf,0,TRUE)*secforestTemp->frac;

        if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
          //update output
          cell->output.flux_estab+=flux_estab;
          #ifdef OUTPUT_BYSTAND
            cell->output.flux_estab_secforest+=flux_estab;
          #endif
        }
      }
    }
  }
  ///////////////////end mix secforest_temp & secforest
  ////////////////////////////////////////////////////////

  //free memory in temp vectors
  free(present);
  free(n_est);

  ////////////////////////////////////////////
  //      C h e c k  D a t a         
  ////////////////////////////////////////////
  //check stand fractions after Gross LUC
  check_stand_fracs(cell->standlist,cell->lakefrac);

  //update total fraction of managed land
  sum=0.0;
  foreachstand(stand,s,cell->standlist){
    if(stand->landusetype!=PRIMARY && stand->landusetype!=SECFOREST){
      sum+=stand->frac;
    }
  }
  cell->cropfrac=sum;
  ///////////////////end Data Checks
  ////////////////////////////////////////////////////////
#endif// nat_veg exclusion
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
