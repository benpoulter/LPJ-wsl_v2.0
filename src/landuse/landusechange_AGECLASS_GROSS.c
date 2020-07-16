/********************************************************************************/
/**                         +-+-+-+-+-+-+-+-+-+-+                              **/
/**                         |  L P J  -  w s l  |                              **/
/**                         +-+-+-+-+-+-+-+-+-+-+                              **/
/**                                                                            **/
/**  src/landuse/ l a n d u s e c h a n g e _ A G E C L A S S _ G R O S S . c  **/ 
/********************************************************************************/
#include "lpj.h"
#include "tree.h"
#include "grass.h"

void deforest_GROSS(Cell *cell,Real difffrac,const Pftpar pftpar[],Landuse *landuse,int CROP_PASTURE,int PRIMARY_SECFOREST,int npft,int year,Config config)
{
    int j,pos,new_pos;
    Stand *old_natstand,*new_cutstand,*stand;
    Real harvestdef=0;
    Real current_deforest_product=0;
  
    //------------------------------------------------------
    // Deforest stands until difffrac=0 (balance met)
    //  OR until no forest left to deforest
    // Priority Rules for Deforesting:
    //  if Primary or Secondary, then deforest oldest in descending order until (prescribed) deforestation area is zero
    //    ..Forest (Age X)
    //  ..Forest (Age X-1)
    //  ..Forest (Age X-n)
    //
    // There may be a situation where Primary forest is conserved (land mgmt?) and only Secondary Forest is chosen for harvest (but maybe not land conversion)
    //   DEVQ: consider LandMgmt (conservation) as an alternative hypothesis (past, modern day & future scenarios)
    //-------------------------------------------------------

    /* by definition, ONE or BOTH of the flags are defined if landusechange_AGECLASS gets called {AGECLASS_PRIMARY, AGECLASS_SECFOREST} */

    //------------------------------------------------------------
    //CASE #1: NO ageclass structure on primary OR secondary stand
    //     ..return statement added by section
    //------------------------------------------------------------
    #if !defined(AGECLASS_PRIMARY) || !defined(AGECLASS_SECFOREST)
      //---------------------------------------
      // (simple) deforest stand if..
      // ..no ageclass structure, then..
      // ..diff approach
      // ..set fn to return
      //---------------------------------------

      //one of the two flags are on if code gets this far
      //..skip to CASE #2 if stand in loop has ageclass
      //..bracket structure is resolved on compilation
    #if defined(AGECLASS_PRIMARY)
    if(PRIMARY_SECFOREST == PRIMARY){
      //do nothing, and don't return yet
      //..continue to CASE #2
    #else
      //by definition, ONE or BOTH of the flags are defined{AGECLASS_PRIMARY, AGECLASS_SECFOREST}
      //..this section is for condition where only AGECLASS_SECFOREST is defined
      //..if NONE are defined, then landusechange_GROSS.c gets called and not landusechange_AGECLASS_GROSS.c
    if(PRIMARY_SECFOREST == SECFOREST){
      //do nothing, and don't return yet
      //..continue to CASE #2
    #endif

    }else if(existlandusetype(cell->standlist,&pos,PRIMARY_SECFOREST)){
      //add stand and store position of new stand (@ end of standlist)
      new_pos=addstand(cell->standlist);
      new_pos--;

      old_natstand=getstand(cell->standlist,pos);
      new_cutstand=getstand(cell->standlist,new_pos);
      new_cutstand->frac=0.0;
      new_cutstand->landusetype=GRASSLAND_TEMP;

      //reset harvest_deforest=0.0, because output.flux_luc is updated in secforest loop
      harvestdef=0.0;
      //copy soils, send carbon to litter pools via reclaim_land...litter_update_deforest.c
      reclaim_land(old_natstand,new_cutstand,&harvestdef,&current_deforest_product);

      //delete STAND if deforesting entire stand fraction
      if(difffrac >= old_natstand->frac){
        //update difffrac
        difffrac=old_natstand->frac;

        //delete stand and preserve order (start of list==1, position of stand to delete==0)
        delstand_preserveOrder(cell, old_natstand, 1, pos);

        //store position of new stand (@ end of standlist)
        new_pos=cell->standlist->n;
        new_pos--;

        new_cutstand=getstand(cell->standlist,new_pos);
      }else {old_natstand->frac -= difffrac;}

      //set fraction of newMgStand
      new_cutstand->frac=difffrac;

      #ifdef DEFORESTPRODUCT
        //multiply current woodproducts by deforest fraction
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
          if(old_natstand->landusetype==PRIMARY){
            cell->product_25yr_primary   += 0.40*current_deforest_product*difffrac;
            cell->product_2yr_primary    += 0.60*current_deforest_product*difffrac;
          }else if(old_natstand->landusetype==SECFOREST){
            cell->product_25yr_secforest += 0.40*current_deforest_product*difffrac;
            cell->product_2yr_secforest  += 0.60*current_deforest_product*difffrac;
          }
        #endif
      #endif

      if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
        cell->output.flux_luc+=harvestdef*difffrac;
        if(PRIMARY_SECFOREST == PRIMARY){
          cell->output.flux_luc_primary  +=harvestdef*difffrac;
        }else if(PRIMARY_SECFOREST == SECFOREST){
          cell->output.flux_luc_secforest+=harvestdef*difffrac;
        }
      }

      //send new_cutstand to setaside--temp designation for mixing new stands before regrowth
      //set to FALSE to skip establishment until end of LUC routine so that establishment only occurs once
      setaside(cell,&new_pos,pftpar,landuse->intercrop,npft,FALSE);

      //...................................
      //successful deforest, return okay
      return;
    }else{
      //fail if no primary or secforest to deforest
      foreachstand(stand,pos,cell->standlist){
        printf("\nPROBLEM No PRIMARY || SECFOREST stand to deforest()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, convMg: %d, fracConv: %.10f\n",
            stand->landusetype, stand->frac, PRIMARY_SECFOREST, CROP_PASTURE, difffrac);
      }
      printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
      fail("\nNO PRIMARY || SECFOREST stand to deforest()\n");
    }//..end if exists forest to deforest

    //.......................................................
    //continues code below if either stand has ageclass
    //.......................................................
    #endif //end of CASE #1

    //------------------------------------------------------------
    //CASE #2: ageclass structure on secondary and/or primary
    //------------------------------------------------------------
    if(existlandusetype(cell->standlist,&pos,PRIMARY_SECFOREST)){

      //initial sort
      //sort tiles by LUtype, then by age (oldest->youngest) or dbh [for harvest] (greatest to least dbh)
      sort_ForestTile(cell->standlist,AGESTAND,FOREST,PRIMARY_SECFOREST);

      while(difffrac > 0.0000009){//small value less than smallest transition

        //sort in while-loop
        //sort tiles by age (oldest->youngest) or dbh [for harvest] (greatest to least dbh)
        //for safety in case setaside exists
        sort_ForestTile(cell->standlist,AGESTAND,FOREST,PRIMARY_SECFOREST);

        //get first stand in list, which should be stand of interest
        old_natstand=(Stand *)(cell->standlist->data[0]);

        //safety loop with error trap
        if(old_natstand->landusetype != PRIMARY_SECFOREST){
          //Sorted list should result in LUtype of interest First position, if it exists
          for(j=0; j < cell->standlist->n && (stand=getstand(cell->standlist,j)); j++){
            printf("\n stand pos: %d, stand->lutype: %d, stand->frac: %f", j, stand->landusetype, stand->frac);
          }
          printf("\n deforest secforest NO MORE...difffrac: %0.7f, old_natstand->lutype: %d, old_natstand->frac: %0.7f",
                        difffrac, old_natstand->landusetype, old_natstand->frac);

          fail("\n\n ERROR: NO FOREST left for deforest()");
        }

        //add and store position of new stand (@ end of standlist)
        new_pos=addstand(cell->standlist);
        new_pos--;
        new_cutstand=(Stand *)(cell->standlist->data[new_pos]);
        new_cutstand->frac=0.0;
        new_cutstand->landusetype=GRASSLAND_TEMP;

        #ifdef DEFORESTPRODUCT
          //update deforested product
          current_deforest_product=0;
        #endif
        
        //reset harvest_deforest=0.0, because output.flux_luc is updated in secforest loop
        harvestdef=0.0;
        //copy soil, litter, and veg(send to litter) from old_natstand to new_cutstand
        reclaim_land(old_natstand,new_cutstand,&harvestdef,&current_deforest_product);

        ////////////////////////////////////////////////////////
        /////////  continue deforest or reduce old->frac
        /////////  .. until demand met
        ////////////////////////////////////////////////////////

        //if deforested area is greater/equal to stand area
        //..then delete secforest stand and search for next oldest secforest stand
        if(difffrac >= old_natstand->frac){

          //update the difffrac
          difffrac-=old_natstand->frac;

          #ifdef DEFORESTPRODUCT
            //multiply current woodproducts by deforest fraction            
            cell->woodproducts_temp += current_deforest_product*old_natstand->frac;
            //update the product pools
            //..62.5% of deforest wood mass goes into product pool
                //..37.5% of all deforest wood goes to 2yr pool
              //..25% of all wood goes into 25yr pool:
              //..product25yr = 25 year decay pool
              //..product2yr  =  2 year decay pool
            cell->product_25yr += 0.40*current_deforest_product*old_natstand->frac;
            cell->product_2yr  += 0.60*current_deforest_product*old_natstand->frac;

            #ifdef OUTPUT_BYSTAND
              if(old_natstand->landusetype==PRIMARY){
                cell->product_25yr_primary   += 0.40*current_deforest_product*old_natstand->frac;
                cell->product_2yr_primary    += 0.60*current_deforest_product*old_natstand->frac;
              }else if(old_natstand->landusetype==SECFOREST){
                cell->product_25yr_secforest += 0.40*current_deforest_product*old_natstand->frac;
                cell->product_2yr_secforest  += 0.60*current_deforest_product*old_natstand->frac;
              }
            #endif
          #endif
            
          //set frac of new stand equal to old_natstand
          new_cutstand->frac=old_natstand->frac;

          if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
            cell->output.flux_luc+=harvestdef*old_natstand->frac;

            if(old_natstand->landusetype==PRIMARY){
              cell->output.flux_luc_primary  +=harvestdef*old_natstand->frac;
            }else if(old_natstand->landusetype==SECFOREST){
              cell->output.flux_luc_secforest+=harvestdef*old_natstand->frac;
            }
          }

          //copy years under agriculture to new stand
          //DEVQ: cropyears should be averaged, not simple replace
          new_cutstand->CropYrs=old_natstand->CropYrs;

          //delete stand and preserve order (start of list==1, position of stand to delete==0)
          delstand_preserveOrder(cell, old_natstand, 1, 0);

          //update position of new_stand
          new_pos=cell->standlist->n;
          new_pos--;

          //mix carbon pools
          //if old stand is deleted, then pass cutstand; else pass old stand frac left
          //set to FALSE to skip establishment until end of LUC routine so that establishment only occurs once
          setaside(cell,&new_pos,pftpar,landuse->intercrop,npft,FALSE);

        }else{//then difffrac is less than or equal to old_natstand->frac

          //update new stand fracs
          new_cutstand->frac=difffrac;

          #ifdef DEFORESTPRODUCT
            //multiply current woodproducts by deforest fraction             
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
              if(old_natstand->landusetype==PRIMARY){
                cell->product_25yr_primary   += 0.40*current_deforest_product*difffrac;
                cell->product_2yr_primary    += 0.60*current_deforest_product*difffrac;
              }else if(old_natstand->landusetype==SECFOREST){
                cell->product_25yr_secforest += 0.40*current_deforest_product*difffrac;
                cell->product_2yr_secforest  += 0.60*current_deforest_product*difffrac;
              }
            #endif
          #endif
            
          //mix carbon pools
          //if old stand is deleted, then pass cutstand; else pass old stand frac left
          setaside(cell,&new_pos,pftpar,landuse->intercrop,npft,FALSE);

          //track the deforest flux, based on the diffrac
          if((config.firstyear!=config.lastyear) && (year >= (config.firstyear-config.nspinup))){
            cell->output.flux_luc+=harvestdef*difffrac;
    
            if(old_natstand->landusetype==PRIMARY){
              cell->output.flux_luc_primary  +=harvestdef*old_natstand->frac;
            }else if(old_natstand->landusetype==SECFOREST){
              cell->output.flux_luc_secforest+=harvestdef*old_natstand->frac;
            }
          }

          //------------------------------------
          // update old stand fracs
          //------------------------------------
          if(old_natstand->ageclass_standid==12){ //standid 12 is the oldest ageclass, see ./include/stand.h
            //simple reduction of stand fraction
            old_natstand->frac_transition[0]-=difffrac;
            old_natstand->frac-=difffrac;
          }else{
            old_natstand->frac-=difffrac;

            //update within-stand frac_transitions//complex reduction of stand fraction
            for(j=0; j < (old_natstand->len_frac_transition); j++){
              if(difffrac > old_natstand->frac_transition[j]){
                //reduce difffrac
                difffrac -= old_natstand->frac_transition[j];
                //set fractional transition to zero
                old_natstand->frac_transition[j]=0.0;
              }else{
                //difffrac is less than the fractional transition
                old_natstand->frac_transition[j]-=difffrac;
                difffrac=0.0;
                break;
              }
            }
          }//..end update old stand fracs
          //------------------------------------

          //difffrac is zero, as defined above
          //here as check for floating-point errors
          difffrac = 0.0;//end of while loop
        }//..end if difffrac < old_natstand_frac
      }//..end while loop

    }else{
      //here: secforestStand = primary || secforest
      foreachstand(stand,pos,cell->standlist){
          printf("\nPROBLEM No PRIMARY || SECFOREST stand to deforest()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, convMg: %d, fracConv: %.10f\n",
              stand->landusetype, stand->frac, PRIMARY_SECFOREST, CROP_PASTURE, difffrac);
      }
      printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
      fail("\nNO PRIMARY || SECFOREST stand to deforest()\n");
     }//..end if exists forest to deforest
  return;
}//..end of main deforest routine

void harvest_GROSS(Cell *cell, int cellid, Real difffrac, Real harvest_biomass, const Pftpar pftpar[],Landuse *landuse, int PRIMARY_SECFOREST,int npft,int year,Config config)
{
  Stand *stand, *new_cutstand;
  int new_pos;
  int j, p, s, pos_stand;
  Pft *pft;
  #ifdef AGECLASS_SECFOREST
  int ss,harvest_standid,pos_lowestDiff;
  Real biomass_exist, biomass_remain, biomass_diff,temp;
  #endif

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // NOTE:  GROSS_LUC does not use the CROP_PASTURE identifier in fn(deforest) for Priority rules in Deforesting       //
  //     :  that is, there is only a priority for deforesting oldest stands first                          //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /* by definition, ONE or BOTH of the flags are defined if landusechange_AGECLASS gets called {AGECLASS_PRIMARY, AGECLASS_SECFOREST} */

  //------------------------------------------------------------
  //CASE #1: NO ageclass structure on primary OR secondary stand
  //     ..return statement added by section
  //------------------------------------------------------------
  #if !defined(AGECLASS_PRIMARY) || !defined(AGECLASS_SECFOREST)

    //---------------------------------------
    // (simple) harvest stand if..
    // ..no ageclass structure, then..
    // ..diff approach
    // ..set fn to return
    //---------------------------------------

    //one of the two flags are on if code gets this far
    //..skip to CASE #2 if stand in loop has ageclass
    //..bracket structure is resolved on compilation
  #if defined(AGECLASS_PRIMARY)
  if(PRIMARY_SECFOREST == PRIMARY){
    //do nothing, and don't return yet
    //..continue to CASE #2
  #else
    //by definition, ONE or BOTH of the flags are defined{AGECLASS_PRIMARY, AGECLASS_SECFOREST}
    //..this section is for condition where only AGECLASS_SECFOREST is defined
    //..ie, if NONE are defined, then landusechange_GROSS.c gets called and not landusechange_AGECLASS_GROSS.c
  if(PRIMARY_SECFOREST == SECFOREST){
    //do nothing, and don't return yet
    //..continue to CASE #2
  #endif

  }else if(PRIMARY_SECFOREST == PRIMARY && (existlandusetype(cell->standlist,&pos_stand,PRIMARY))){

    stand=getstand(cell->standlist,pos_stand);

    new_pos=addstand(cell->standlist);
    new_pos--;
    new_cutstand=getstand(cell->standlist,new_pos);
    new_cutstand->frac=difffrac;

    //copy fire, soil, veg and harvest wood
    new_cutstand->fire_sum=stand->fire_sum;
    copysoil(stand, new_cutstand);
    foreachpft(pft, p, stand->pftlist){
      j=copy_pft(pft, new_cutstand, pftpar, npft);
    }

    #if !defined(NOWDHARVEST)
      new_cutstand->landusetype=PRIMARY_SECFOREST; //temp landuse designation
      woodharvest_updateStand(cell,new_cutstand, difffrac, harvest_biomass, landuse, cellid, TRUE);
    #endif

    //update landuse designation
    new_cutstand->landusetype=STAND_DUMMY;

    if(difffrac >= stand->frac){
      foreachpft(pft,p,stand->pftlist){
        delpft(stand->pftlist,p);
        p--;
      }
      delstand(cell->standlist,pos_stand);
      new_pos=pos_stand;
    }else {stand->frac -= difffrac;}//reduce primary forest fraction by area harvested -> secforest

    //MIX if secforestTemp stand exists
    //.. all secforestTemp stands are mixed together, then mixed with secforest stand at end of wood harvet routines
    if(existlandusetype(cell->standlist, &j,SECFOREST_TEMP)){
      //get secforest_temp stand and mix
      stand=getstand(cell->standlist,j);

      //update fractions and mix secforestTemp stands
      //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
      mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, FALSE);
    }else{
      new_cutstand->landusetype=SECFOREST_TEMP;
    }

    //...................................
    //successful deforest, return okay
    return;

  }else if(PRIMARY_SECFOREST == SECFOREST && (existlandusetype(cell->standlist,&pos_stand,SECFOREST))){
    //stand->frac stays the same w/o tiling

    stand=getstand(cell->standlist,pos_stand);

    #if !defined(NOWDHARVEST)
      woodharvest_updateStand(cell, stand, stand->frac, harvest_biomass, landuse, cellid, TRUE);
    #endif

    //...................................
    //successful deforest, return okay
    return;

  }else{//error trap
    //here: secforestStand = primary || secforest
    foreachstand(stand,pos_stand,cell->standlist){
        printf("\nPROBLEM No PRIMARY || SECFOREST stand for wood harvest ()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, fracConv: %.10f\n",
            stand->landusetype, stand->frac, PRIMARY_SECFOREST, difffrac);
    }
    printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
    fail("\nNo PRIMARY || SECFOREST stand for wood harvest()\n");
  }

  //.......................................................
  //continues code below if either stand has ageclass
  //.......................................................
  #endif

  //------------------------------------------------------------
  //CASE #2: ageclass structure on secondary and/or primary
  //------------------------------------------------------------
  if(existlandusetype(cell->standlist,&pos_stand,PRIMARY_SECFOREST)){
    while(difffrac > 0.0000009){
      //sort stands by LUtype, then age (oldest->youngest) or dbh [for harvest] (greatest to least dbh)
      //for safety in case setaside exists
      sort_ForestTile(cell->standlist,AGESTAND,FOREST,PRIMARY_SECFOREST);

      //get first stand in list, which should be the stand of interest
      stand=(Stand *)(cell->standlist->data[0]);

      //safety loop with error trap
      if(stand->landusetype != PRIMARY_SECFOREST){
        //Sorted list should result in LUtype of interest First position, if it exists
        for(j=0; j < cell->standlist->n && (stand=getstand(cell->standlist,j)); j++){
          printf("\n stand pos: %d, stand->lutype: %d, stand->frac: %f", j, stand->landusetype, stand->frac);
        }
        printf("\n NO FOREST to harvest...difffrac: %0.7f, stand->lutype: %d, stand->frac: %0.7f",
                      difffrac, stand->landusetype, stand->frac);

        fail("\n\n ERROR: NO FOREST left for deforest()");
      }

      #ifdef AGECLASS_SECFOREST
      harvest_standid=stand->ageclass_standid;
      #endif

      //add and store position of new stand (@ end of standlist)
      new_pos=addstand(cell->standlist);
      new_pos--;
      new_cutstand=(Stand *)(cell->standlist->data[new_pos]);
      new_cutstand->frac=0.0;

      //copy fire, soil, veg and harvest wood
      new_cutstand->fire_sum=stand->fire_sum;
      copysoil(stand, new_cutstand);
      foreachpft(pft, p, stand->pftlist){
        j=copy_pft(pft, new_cutstand, pftpar, npft);
      }

      ////////////////////////////////////////////////////////
      /////////  continue harvest until difffrac is zero
      ////////////////////////////////////////////////////////

      //if harvested area is greater/equal to stand area
      //..then delete stand and search for next oldest stand
      if(difffrac >= stand->frac){

        //update the difffrac
        difffrac-=stand->frac;

        //set frac of new stand equal to old_natstand
        new_cutstand->frac=stand->frac;

        //copy years under agriculture to new stand
        //DEVQ: cropyears should be averaged, not simple replace
        new_cutstand->CropYrs=stand->CropYrs;

        //////////////
        // harvest
        //////////////
        #if !defined(NOWDHARVEST)
          new_cutstand->landusetype=PRIMARY_SECFOREST; //temp landuse designation
          harvest_biomass=woodharvest_updateStand(cell,new_cutstand, new_cutstand->frac, harvest_biomass, landuse,cellid, TRUE);
        #endif
        
        //update landuse designation
        new_cutstand->landusetype=STAND_DUMMY;
        
        //delete stand and preserve order (start of list==1, position of stand to delete==0)
        delstand_preserveOrder(cell, stand, 1, 0);

        //update position of new_stand
        new_pos=cell->standlist->n;
        new_pos--;

        //update location of new_cutstand
        //..for safety -- DEVQ: check if needless statement
        new_cutstand=getstand(cell->standlist,new_pos);

        #ifdef AGECLASS_SECFOREST
          //get remaining biomass in cutstand
          biomass_remain=0.0;
          foreachpft(pft, p, new_cutstand->pftlist){
            biomass_remain+=vegc_sum(pft)*new_cutstand->frac;
          }

          if(biomass_remain > epsilon){
            //(1) store biomass existing in harvest stand
            //(2) compare biomass existing and biomass remain
            //(3) merge new_cutstand with secforest ageclass stand with closest biomass
            //  ..(harvest on primary always transitions to secforest stand)
            biomass_diff=1000000;//set really high for comparison; gets updated if stand exists
            pos_lowestDiff=-1;
            foreachstand(stand, s, cell->standlist){
              biomass_exist=0.0;
              if(stand->landusetype==SECFOREST && stand->ageclass_standid != harvest_standid){
                foreachpft(pft, p, stand->pftlist){
                  biomass_exist+=vegc_sum(pft)*stand->frac;
                }
                temp=fabs(biomass_exist-biomass_remain);
                if(temp < biomass_diff){biomass_diff=temp; pos_lowestDiff=s;}
              }
            }

            if(pos_lowestDiff == -1){//if no secforest stand exists...
              //get standid of oldest stand that is not the harvest_standid
              if(harvest_standid == 12){ //see stand.h for ageclass_standid codes
                ss=11;
              }else{
                ss=12;
              }

              new_cutstand->landusetype=SECFOREST;
              new_cutstand->ageclass_standid=ss;
              new_cutstand->frac_transition[0]=new_cutstand->frac;
            }else{
              //get standid of oldest stand that is not the harvest_standid
              if(harvest_standid == 12){
                ss=11;
              }else{
                ss=12;
              }

              if(find_exist_ageclass(cell->standlist,ss,&pos_stand, SECFOREST)){
                //get secforest_temp stand and mix
                //stand=getstand(cell->standlist,pos_lowestDiff);
                stand=getstand(cell->standlist,pos_stand);

                //update fractions and mix secforestTemp stands
                //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
                mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, TRUE);
              }else{
                new_cutstand->landusetype=SECFOREST;
                new_cutstand->ageclass_standid=ss;
                new_cutstand->frac_transition[0]=new_cutstand->frac;
              }
             }
          }else{
            if(existlandusetype(cell->standlist, &j,SECFOREST_TEMP)){
              //get secforest_temp stand and mix
              stand=getstand(cell->standlist,j);

              //update fractions and mix secforestTemp stands
              //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
              mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, FALSE);
            }else{
              new_cutstand->landusetype=SECFOREST_TEMP;
            }
          }
        #else
          //MIX if secforestTemp stand exists
          //..all secforestTemp stands are mixed together, then mixed with secforest stand at end of wood harvet routines
          if(existlandusetype(cell->standlist, &j,SECFOREST_TEMP)){
            //get secforest_temp stand and mix
            stand=getstand(cell->standlist,j);

            //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
            mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, FALSE);
          }else{
            new_cutstand->landusetype=SECFOREST_TEMP;
          }
        #endif //..end of AGECLASS_SECFOREST

      }else{//then difffrac is less than or equal to old_natstand->frac

        //update new stand fracs
        new_cutstand->frac=difffrac;

        //////////////
        // harvest
        //////////////
        #if !defined(NOWDHARVEST)
          new_cutstand->landusetype=PRIMARY_SECFOREST; //temp landuse designation
          harvest_biomass=woodharvest_updateStand(cell,new_cutstand, new_cutstand->frac, harvest_biomass, landuse, cellid,TRUE);
        #endif

        //update landuse designation
        new_cutstand->landusetype=STAND_DUMMY;
        
        //----------------------------------------------
        //--- update old stand fracs
        //--- ..before mixing
        //----------------------------------------------
        if(stand->ageclass_standid==12){ //oldest ageclass, see stand.h for ageclass_standid codes
          //simple reduction of stand fraction
          stand->frac_transition[0]-=difffrac;
          stand->frac-=difffrac;
        }else{
          //update total stand frac
          stand->frac-=difffrac;

          //update within-stand frac_transitions//complex reduction of stand fraction
          for(j=0; j < (stand->len_frac_transition); j++){
            if(difffrac >= stand->frac_transition[j]){
              //reduce difffrac
              difffrac -= stand->frac_transition[j];
              //set fractional transition to zero
              stand->frac_transition[j]=0.0;

            }else{
              //difffrac is less than the fractional transition
              stand->frac_transition[j]-=difffrac;
              difffrac=0.0;
              break;
            }
          }

        } /////////////////// end update old stand fracs
        //-----------------------------------------------

        #ifdef AGECLASS_SECFOREST
          //get remaining biomass in cutstand
          biomass_remain=0.0;
          foreachpft(pft, p, new_cutstand->pftlist){
            biomass_remain+=vegc_sum(pft)*new_cutstand->frac;
          }

          if(biomass_remain > epsilon){
            //(1) store biomass existing in secforest stands
            //(2) compare biomass existing and biomass remain
            //(3) merge new_cutstand with secforest stand with closest biomass
            biomass_diff=1000000;//set really high for comparison; gets updated if SECFOREST exists
            pos_lowestDiff=-1;
            foreachstand(stand, s, cell->standlist){
              biomass_exist=0.0;
              if(stand->landusetype==SECFOREST && stand->ageclass_standid != harvest_standid){
                foreachpft(pft, p, stand->pftlist){
                  biomass_exist+=vegc_sum(pft)*stand->frac;
                }
                temp=fabs(biomass_exist-biomass_remain);
                if(temp < biomass_diff){biomass_diff=temp; pos_lowestDiff=s;}
              }
            }

            if(pos_lowestDiff == -1){//if no secforest stand exists...
              //get standid of oldest stand that is not the harvest_standid
              if(harvest_standid == 12){ //see stand.h for ageclass_standid codes
                ss=11;
              }else{
                ss=12;
              }
              new_cutstand->landusetype=SECFOREST;
              new_cutstand->ageclass_standid=ss;
              new_cutstand->frac_transition[0]=new_cutstand->frac;
            }else{
              //get standid of oldest stand that is not the harvest_standid
              if(harvest_standid == 12){
                ss=11;
              }else{
                ss=12;
              }

              ///////
              if(find_exist_ageclass(cell->standlist,ss,&pos_stand, SECFOREST)){
                //get secforest_temp stand and mix
                //stand=getstand(cell->standlist,pos_lowestDiff);
                stand=getstand(cell->standlist,pos_stand);

                //update fractions and mix secforestTemp stands
                //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
                mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, TRUE);
              }else{
                new_cutstand->landusetype=SECFOREST;
                new_cutstand->ageclass_standid=ss;
                new_cutstand->frac_transition[0]=new_cutstand->frac;
              }
            }

          }else{
            if(existlandusetype(cell->standlist, &j,SECFOREST_TEMP)){
              //get secforest_temp stand and mix
              stand=getstand(cell->standlist,j);

              //update fractions and mix secforestTemp stands
              //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
              mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, FALSE);
            }else{
              new_cutstand->landusetype=SECFOREST_TEMP;
            }
          }
        #else
          //MIX if secforestTemp stand exists
          //.. all secforestTemp stands are mixed together, then mixed with secforest stand at end of wood harvet routines
          if(existlandusetype(cell->standlist, &j,SECFOREST_TEMP)){
            //get secforest_temp stand and mix
            stand=getstand(cell->standlist,j);

            //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2
            mix_standComplex(cell, stand, new_cutstand, new_pos, pftpar, npft, FALSE);
          }else{
            new_cutstand->landusetype=SECFOREST_TEMP;
          }
        #endif //..end of AGECLASS_SECFOREST

        //difffrac is zero, as defined above
        //here as check for floating-point errors, but redundant
        difffrac = 0.0;//end of while loop

      }//end of difffrac less than or equal to old_natstand->frac
    }//end while loop
   }else {//error trap
           //here: stand = primary || secforest
          foreachstand(stand,s,cell->standlist){
                    printf("\nPROBLEM No PRIMARY || SECFOREST stand for wood harvest ()\n standLUtype: %d, standFrac: %.10f, convNatural: %d, fracConv: %.10f\n", stand->landusetype, stand->frac, PRIMARY_SECFOREST, difffrac);
          }
          printf("cell.lat: %f; cell.lon: %f\n", cell->coord.lat, cell->coord.lon);
              fail("\nNo PRIMARY || SECFOREST stand for wood harvest()\n");
   }
  //////////////////////////
  return;
}//..end of main woodharvest routine

void landexpansion_GROSS(
               Cell *cell,
               Real difffrac,
               const Pftpar *pftpar,
               int npft,
               int ntypes,
               Bool irrigation,
               int cultivate_type,
               int year,Config config)
{
  int p,t, pos_grassTemp, pos_setaside, pos_newMgStand;
  int *n_est;
  Real rharvest;
  Real dummy=-9;
  Real current_deforest_product=0; //reqd DEFORESTPRODUCT
  Stand *grassStand, *setasideStand, *newMgStand;
  Pft *pft;
  
  n_est=newvec(int,ntypes);
  check(n_est);
  for(t=0;t<ntypes;t++)
    n_est[t]=0;
    
  if(existlandusetype(cell->standlist,&pos_setaside,SETASIDE)){
    //DEVQ: For PASTURE: the SETASIDE stand with maximum Grass-pft needs to be passed here as Priority Rule
    //    : For CROP   : the SETASIDE stand with oldest Forest needs to passed here as Priority Rule (soil C as priority?)
    //    : DEVQ       : condisider for both (above) pass into the landexpansion(fn) with a TAG (CROP / PASTURE)
    setasideStand=getstand(cell->standlist,pos_setaside);
      
    if(difffrac+epsilon >= setasideStand->frac){          /*setaside stand has not enough space for grassland expansion*/
      newMgStand=getstand(cell->standlist,pos_setaside);
      foreachpft(pft,p,newMgStand->pftlist){
        litter_update(&newMgStand->soil.litter,pft,pft->nind,&rharvest,FALSE, TRUE);
        delpft(newMgStand->pftlist,p);
        p--;
      }
      difffrac=newMgStand->frac;
      pos_newMgStand=pos_setaside;
    } else{
      pos_newMgStand=addstand(cell->standlist);                /*setaside big enough for grassland expansion*/
      pos_newMgStand--;
      newMgStand=getstand(cell->standlist,pos_newMgStand);
      newMgStand->frac=difffrac;
      
      #ifdef DEFORESTPRODUCT
          dummy=0;
      #endif

      //now reclaim land for grassland expansion..    
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

    //mix with GRASSLAND_TEMP if it exists
    if(existlandusetype(cell->standlist, &pos_grassTemp,GRASSLAND_TEMP)){
      grassStand=getstand(cell->standlist,pos_grassTemp);

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2
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
      //------------------
      //establishment in new grassland (pasture) stand occurs at end of landusechange routine, after mixing all new pasture stands
      // DEVQ: grassland stands should be kept separate if using multiple crops and pasture types (irrigated vs. non-irrgated)
      //     : ..then establishment should be here and the stand should only mix with similar LU types
      //------------------
    }

  }//..end of if setaside exists

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
  if(difffrac+epsilon >= grassStand->frac){
    foreachpft(pft,p,grassStand->pftlist){
      litter_update(&grassStand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
      delpft(grassStand->pftlist,p);
      p--;  
    }
    setaside(cell,s,pftpar,landuse->intercrop,npft,FALSE);
  }else{//else just reduce the fraction of GRASSLAND by amount=difffrac
    pos_newSecforestStand=addstand(cell->standlist);
    pos_newSecforestStand--;
    newSecforestStand=getstand(cell->standlist,pos_newSecforestStand);
      
    #ifdef DEFORESTPRODUCT
      dummy=0;
    #endif
    
    //now reclaim land for grassland reduction..
    reclaim_land(grassStand,newSecforestStand,&dummy,&current_deforest_product);

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
    
    newSecforestStand->frac=difffrac;
    grassStand->frac -= difffrac;

    //set establishment to false
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
    //setaside stand is the grassland stand abandoned in previous step (grasslandreduction_GROSS, above)
    setasideStand=getstand(cell->standlist,pos_setaside);

    //-----------------------------------
    //FIRST: litter update or reclaim land
    //-----------------------------------
    //setaside stand has not enough space for regrowth
    if(setasideStand->frac <= difffrac+epsilon){
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
      newSecforestStand->frac_transition[0]=newSecforestStand->frac;

      newSecforestStand->landusetype=SETASIDE;//temp designation, updated few lines down

      newSecforestStand->fire_sum=setasideStand->fire_sum;
      copysoil(setasideStand, newSecforestStand);
      foreachpft(pft,p,setasideStand->pftlist){
        #ifdef DEFORESTPRODUCT
          current_deforest_product=0;
          dummy=0;
        #endif
          
        //update litter pools, if deforestproduct is on, then updated deforest products too..
        litter_update_deforest(&newSecforestStand->soil.litter,pft,pft->nind,&dummy,&current_deforest_product,TRUE); 
          
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
      }
        setasideStand->frac -= difffrac;
    }//..end setaside check

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

    //------------------
    //note: establishment in new secondary stand occurs at end, after mixing all new secondary stands
    //------------------
  }
  return;
}/* of 'abandon mgland and regrow secondary' */

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

  int i, s;
  Stand  *stand;
  Real ageclass_frac;

  if (difffrac > 0) {
    if (existlandusetype(cell->standlist, &s, PRIMARY_SECFOREST)){
      //sum total ageclass fraction and compare with the prescribed difffrac
      ageclass_frac=0.0;
      for(i=0; i < cell->standlist->n && (stand = getstand(cell->standlist, i)); i++){
        if(stand->landusetype == PRIMARY_SECFOREST){ageclass_frac+=stand->frac;}
      }

      //compare diffrac with total ageclass fractions
      //catch floating-point errors
      if(difffrac >= ageclass_frac){
        difffrac = ageclass_frac;
      }

      //if there are pfts OR if there are no pfts and the stand is an ageclass stand,
      //.. then there will be ageClass transitions 
      deforest_GROSS(cell, difffrac, pftpar, landuse, CROP_PASTURE, PRIMARY_SECFOREST, npft, year, config);
      landexpansion_GROSS(cell,difffrac, pftpar,npft, ntypes, irrigation,cultivation_type, year, config);
      
    }//..end of primary,secforest
  }//..end of difffrac
  return;
}//..end of hyper-function deforest expand MgLand

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
  //crop     -> secForest
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
          delstand_preserveOrder(cell, grassStand, 1, pos_grassStand);
        }else {
          //simple reduction of stand fraction
          grassStand->frac -= difffrac;
        }
      } else {
        grasslandreduction_GROSS(cell,difffrac, pftpar,landuse, npft, &pos_grassStand, grassStand, year, config);
        regrowth_GROSS(cell, difffrac,pftpar, npft, ntypes, year, config);
      }
    }//end if grassland exists
  }//..end of difffrac

  return;
}//..end of hyper-function abandon MgLand and regrow

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

  int i, s;
  Stand *stand;
  Real ageclass_frac;

  ////////////////////////////////////////////////////////
  //  W o o d  H a r v e s t                
  ////////////////////////////////////////////////////////
  //harvest...
  //      secforest Old     -> secforest
  //      secforest Young   -> secforest
  //      secforest nonForest -> secforest
  //    primary       -> secforest
  //      primary nonForest   -> secforest

  if (difffrac > 0) {

    if (existlandusetype(cell->standlist, &s, PRIMARY_SECFOREST)){

      //sum total ageclass fractions and compare with the prescribed difffrac
      ageclass_frac=0.0;
      for(i=0; i < cell->standlist->n && (stand = getstand(cell->standlist, i)); i++){
        if(stand->landusetype == PRIMARY_SECFOREST){ageclass_frac+=stand->frac;}
      }

      //compare diffrac with total secforest fraction
      //catch floating-point errors
      if(difffrac+epsilon >= ageclass_frac){
        difffrac = ageclass_frac;
      }

      //if there are pfts OR if there are no pfts,
      //.. then there will be a transition where the area of the youngest ageclass stand increases
      harvest_GROSS(cell, cellid, difffrac, biomass2harvest, pftpar, landuse, PRIMARY_SECFOREST, npft, year,config);
      
    }//..end if primary,secforest
  }//..end if difffrac
  return;
}//..end of hyper-function wood harvest

///////////////////////////////////////////////////////////////////////
///                                  ///
///    M A I N   F N :  L A N D U S E C H A N G E _ G R O S S    ///
///                                  ///
///////////////////////////////////////////////////////////////////////
void landusechange_AGECLASS_GROSS(Cell *cell,
       const Pftpar *pftpar,
       int npft,
       int ncft,
       int ntypes,
       int cellid, 
       Landuse *landuse,
       int year,
       Config config)
//needs to be called before establishment, to ensure that regrowth is possible in the following year
{
 #if !defined(NAT_VEG) && (defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST))
  Real sum;
  int i,p,s,t, pos_grassStand, pos_grassTemp, pos_secforest, pos_secforestTemp, cultivation_type, FORESTTYPE;
  Stand *stand, *grassStand, *grassTemp, *secforestStand, *secforestTemp;
  Bool irrigation, *present;
  int *n_est;
  Pft *pft;
  Real flux_estab;
  Real *fpc_type, fpc_total, dummy;

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
  //    :  BUT, need to keep track of CropYrs, therefore, crop [GRASSLAND] also requires multiple stands
  //DEVQ: separate pastures for irrigated vs non-irrigated
  cultivation_type=1;  //PASTURE
  irrigation=FALSE;

  /////////////////////////////////////////////////////
  //  D e f o r e s t   &   E x p a n d  M g L a n d 
  /////////////////////////////////////////////////////
  //for PriForest
  deforest_expandMgLand(PRIMARY,landuse->PriForest2pasture_frac[cellid], PASTURE, pftpar, npft, ntypes, year, config,  cell, landuse, cultivation_type, irrigation);
  deforest_expandMgLand(PRIMARY,landuse->PriForest2crop_frac[cellid], CROP, pftpar, npft, ntypes, year, config,  cell, landuse, cultivation_type, irrigation);

  //for SecForest
  deforest_expandMgLand(SECFOREST,landuse->SecForest2pasture_frac[cellid], PASTURE, pftpar, npft, ntypes, year, config,  cell, landuse, cultivation_type, irrigation);
  deforest_expandMgLand(SECFOREST,landuse->SecForest2crop_frac[cellid], CROP, pftpar, npft, ntypes, year, config,  cell, landuse, cultivation_type, irrigation);

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
  woodharvest_createSecforest(PRIMARY, landuse->WHarvestPrimary_frac[cellid], landuse->WHarvestPrimary_biomass[cellid], pftpar, npft, year, config, cell, cellid, landuse);
  woodharvest_createSecforest(PRIMARY, landuse->WHarvestPrimaryNonForest_frac[cellid], landuse->WHarvestPrimaryNonForest_biomass[cellid], pftpar, npft, year, config, cell, cellid, landuse);

  //harvest from secondary forests
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestOld_frac[cellid], landuse->WHarvestSecForestOld_biomass[cellid], pftpar, npft, year, config, cell, cellid, landuse);
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestYoung_frac[cellid], landuse->WHarvestSecForestYoung_biomass[cellid], pftpar, npft, year, config, cell, cellid, landuse);
  woodharvest_createSecforest(SECFOREST, landuse->WHarvestSecForestNonForest_frac[cellid], landuse->WHarvestSecForestNonForest_biomass[cellid], pftpar, npft, year, config, cell,cellid, landuse);

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

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2 (temp stand)
      mix_standComplex(cell, grassStand, grassTemp, pos_grassTemp, pftpar, npft, FALSE);
    }else {
      //if no grassland exists, set the temp stand to GRASSLAND
      grassTemp->landusetype = GRASSLAND;
    }
  }
  ///////////////////end mix grassland_temp & grassland
  ////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////
  //      S T A N D - A G E   T R A N S I T I O N S         
  ////////////////////////////////////////////////////////////////
  // -- max of 12 ageclass stands
  // -- stand age codes {1..12}, see ./include/stand.h for ageclass widths for unequal_ and equal_interval ageclasses
  // -- stand->frac_transitions tracks the incoming fractions[0] and outgoing fraction [n]
  // -- .. each year, the outgoing fraction is moved to the next age-class and only vector positions for the within-ageClass transitions are incremented
  // -- mean stand age is a fractional value (Real) for age bins in years, and it is calculated as age*fraction
  // -- NOTE: primary ageclasses are created only via fire, but deforestation and harvest operate on primary ageclasses too
  //       ..primary ageclasses are transitioned (age) in iterateyear.c
  ////////////////////////////////////////////////////
  // ***** WORKHORSE FOR THE TILING APPROACH ***** 
  ////////////////////////////////////////////////////
  #ifdef AGECLASS_SECFOREST
    //sort stand list by standtype (forest first) then by reverse standAge, oldest first (stand->ageclass_standid)
    // -- simplifies the search and merge loop
    sort_ForestTile(cell->standlist, AGESTAND, FOREST, SECFOREST);

    //transition secforest stand fractions based on ageclass_standid
    ageclass_transition(cell, pftpar, npft, SECFOREST);

    //update bool so that ageclasses only transition once-per-year
    cell->transitioned_ageclass_secforest=TRUE;
  #endif
  ///////// end stand-age transitions
  /////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////
  //      M I X :  SECFOREST_TEMP && SECFOREST          
  //                                    
  // secforestTemp = current year area of new secondary stand   
  ////////////////////////////////////////////////////////////////

  //mixing at end (here) avoids flux error due to deforesting recently created secondary stand
  if(existlandusetype(cell->standlist,&pos_secforestTemp,SECFOREST_TEMP)){
    //if there was a grasslandExpansion, or wood harvest (primary-> secondary)
    secforestTemp=getstand(cell->standlist,pos_secforestTemp);

    FORESTTYPE = SECFOREST;

    //-----------------------------------------
    //  Search for existing secforest stand
    //-----------------------------------------
    //NOTE: bracket structure is resolved on compilation
    #ifdef AGECLASS_SECFOREST
    //search for youngest stand in 
    if(ageclass_createORmix(cell->standlist,&pos_secforest,FORESTTYPE)){
    #else
    if(existlandusetype(cell->standlist,&pos_secforest,FORESTTYPE)){
    #endif
      secforestStand=getstand(cell->standlist,pos_secforest);
      secforestStand->TimeSinceDist=0;

      #ifdef AGECLASS_SECFOREST
        //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2 (temp stand)
        mix_standComplex(cell, secforestStand, secforestTemp, pos_secforestTemp, pftpar, npft, TRUE);
      #else
        //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2 (temp stand)
        mix_standComplex(cell, secforestStand, secforestTemp, pos_secforestTemp, pftpar, npft, FALSE);
      #endif  
    
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
          #ifdef OUTPUT_BYAGECLASS
            cell->output.ageclass_sec_fluxestab[secforestStand->ageclass_standid-1]+=flux_estab;
          #endif
        }
      }
    }else{//new secforest stand

      secforestTemp->landusetype=FORESTTYPE;

      #ifdef AGECLASS_SECFOREST
        //youngest secforest stand is standid 1
        //during LU spinup, the age-class transitions naturally
        secforestTemp->ageclass_standid=1;

        //store the length of the vector for fractional transitions
        secforestTemp->len_frac_transition=AGECLASS_WIDTH_1; //incoming fraction

        //first set all fractional transitions to zero
        for(p=0; p < secforestTemp->len_frac_transition; p++){
          secforestTemp->frac_transition[p]=0.0;
        }

        //store incoming fraction
        secforestTemp->frac_transition[0]=secforestTemp->frac;
      #else
        //standid 12 is default for oldest secforest stand, see stand.h for codes
        secforestTemp->ageclass_standid=12;
      #endif

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

          #ifdef OUTPUT_BYAGECLASS
          cell->output.ageclass_sec_fluxestab[0]+=flux_estab; //establishment flux on youngest stand
          #endif
        }
      }
    }//end of new secforest stand
  }
  ///////////////////end mix secforest_temp & secforest
  ////////////////////////////////////////////////////////

  //------------------------------------------------------------------------------
  //Solution for resolving FPC > max after mixing stands (ageClass transitions)
  //.. sends excess number of individuals to litter based
  //.. .. DEVQ: perhaps there is a better solution where trees die-off slower
  //------------------------------------------------------------------------------
  foreachstand(stand, i, cell->standlist){
    fpc_type=newvec(Real,ntypes);
    fpc_sum(fpc_type,ntypes,stand->pftlist);

    foreachpft(pft,p,stand->pftlist)
      if(istree(pft) && fpc_type[TREE]>FPC_TREE_MAX)
        adjust(&stand->soil.litter,pft,fpc_type[pft->par->type],TRUE);

    fpc_total=fpc_sum(fpc_type,ntypes,stand->pftlist);

    if (fpc_total>1.0)
     foreachpft(pft,p,stand->pftlist){
       if(istree(pft)){
        pft->nind/=fpc_total;
        litter_update(&stand->soil.litter,pft,(pft->nind*fpc_total-pft->nind),&dummy,FALSE,TRUE);
        pft->fpc/=fpc_total;
       }else{
           pft->fpc/=fpc_total;
       }
     }
  }
  //---end solution for fpc update ----------------------------------

  //clear memory
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
#endif //of AGECLASS, NAT_VEG (exclusion), reqd for compiling without AGECLASS flag
  return;
} // of 'landusechange'


/*
- LCale: Oct 23, 2015, small carbon loss (0.000XXX) during setaside()->establishment and upon establishment() routines...not sure why
- LCalle: Nov 22 2014; fixed special case bug where no regrowth occurs upon LUC
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
