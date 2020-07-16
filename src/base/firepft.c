/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f i r e p f t . c                                  **/
/**                                                               **/
/**  firepft()      : global fn calls one of two fns below        **/
/**  fire_standard(): internal fn, original fire routines         **/
/**  fire_ageclass(): internal fn, modified for ageclass tiles    **/
/*******************************************************************/

#include "lpj.h"
void fire_standard(Cell *cell, Stand *stand, Real fire_frac){
  //--------------------------------------------
  // Original Fire Routine (Sitch et al. 2003)
  //--------------------------------------------
  //define variables & structure pointers
  Real flux, flux_litter;
  int p;
  Litter *litter;
  Pftlist pftlist;
  Pft *pft;

  //initialize to zero
  flux=flux_litter=0.0;

  //note: fire does not change stand status or age-class
  //set data
  litter  = &stand->soil.litter;
  pftlist = stand->pftlist;

  foreachpft(pft,p,pftlist)
    flux+=fire(pft,&fire_frac);
  if(!isempty(pftlist)){
    flux_litter=(litter->ag_grass+litter->ag_tree+litter->ag_grass_inc+litter->ag_tree_inc)*fire_frac;
    litter->ag_grass*=(1-fire_frac);
    litter->ag_tree*=(1-fire_frac);
    litter->ag_grass_inc*=(1-fire_frac);
    litter->ag_tree_inc*=(1-fire_frac);
  }

  //----------------------------------------
  //  update fire fraction and fire flux
  //----------------------------------------
  if(stand->landusetype==PRIMARY || stand->landusetype==SECFOREST) cell->output.firef+=fire_frac*stand->frac;
  
  cell->output.firec+=(flux+flux_litter)*stand->frac;
  #ifdef OUTPUT_BYSTAND
  if(stand->landusetype==PRIMARY){
    cell->output.firec_primary+=(flux+flux_litter)*stand->frac;
  }else if(stand->landusetype==SECFOREST){
    cell->output.firec_secforest+=(flux+flux_litter)*stand->frac;
  }
  #endif
}

void fire_ageclass(Cell *cell, Stand *stand, int pos_stand, Real fire_frac, const Pftpar pftpar[], int npft, Bool *skip_stand){
  //define variables
  int p, pos_new, pos_temp;
  Real fire_trans, flux, flux_litter;
  Stand *stand_new, *stand_temp;
  Pft *pft;
  Pftlist pftlist;
  Litter *litter;

  //initialize to zero
  flux=flux_litter=0.0;

  //----------------------------
  // fire w/ age classes
  // ..(a) only reduce litter in the grid-cell fraction w/ fire
  // ..(b) only reduce tree density in the grid-cell fraction w/ fire
  // ..(c) create temp stand and mix the fire fraction with the youngest age-class at end of update_annual
  // .. .. (d) but, not all tree pfts are removed, so fire fraction might still have tree pfts with large biomass
  // .. .. DEVQ: solution (1) -- have separate height-based cohorts in each age-class, mix by height class
  //----------------------------
  // S T E P S
  // (0): first, update the fire fraction as fraction of existing stand
  // (1): add stand & copy soil, veg
  //   : ..(b) set new stand->frac = fire fraction
  // (2): set new stand to TEMP, else mix with TEMP if exists
  // (3): do fire routine on new stand
  // (4): update stand->frac (origin) -= fire fraction, or delete if > than stand_frac
  // (5): at end of update_annual.c: mix new stand (TEMP) with youngest age-class, if exists, else create
  //----------------------------

  //--------------------------------------
  // update fire fraction for transition
  // ..set fire_frac to 1
  // ..fire_frac is always <= 1
  //--------------------------------------
  fire_trans = round(1e6*fire_frac*stand->frac)/1e6;
  fire_frac  = 1;

  //------------------------
  // copy stand attributes
  //------------------------
  //add new stand
  pos_new=addstand(cell->standlist);
  pos_new--;
  stand_new=getstand(cell->standlist,pos_new);
  stand_new->frac=fire_trans;
  stand_new->landusetype=SETASIDE;//temp LUtype designation; DEVQ: consider setting all new primary forest to secondary?

  //copy fire, soil, veg and harvest wood
  stand_new->fire_sum=stand->fire_sum;
  copysoil(stand, stand_new);
  foreachpft(pft, p, stand->pftlist){
    copy_pft(pft, stand_new, pftpar, npft);
  }

  //----------------------------------
  // fire routine on new/temp stand
  //----------------------------------
  //set data
  litter  = &stand_new->soil.litter;
  pftlist = stand_new->pftlist;

  //determine fire flux
  foreachpft(pft,p,pftlist)
  flux+=fire(pft,&fire_frac);
  if(!isempty(pftlist)){
    //DEVQ: should litter be zero?..fire flux..&&..tree & grass pfts can still exist after fire...
    flux_litter=(litter->ag_grass+litter->ag_tree+litter->ag_grass_inc+litter->ag_tree_inc)*fire_frac;
    litter->ag_grass*=(1-fire_frac);
    litter->ag_tree*=(1-fire_frac);
    litter->ag_grass_inc*=(1-fire_frac);
    litter->ag_tree_inc*=(1-fire_frac);
  }

  //----------------------------------------------
  //  update output: fire fraction and fire flux
  //----------------------------------------------
  cell->output.firef+=fire_trans;
  cell->output.firec+=(flux+flux_litter)*fire_trans;

  #ifdef OUTPUT_BYSTAND
  if(stand->landusetype==PRIMARY){
    cell->output.firec_primary+=(flux+flux_litter)*fire_trans;
  }else if(stand->landusetype==SECFOREST){
    cell->output.firec_secforest+=(flux+flux_litter)*fire_trans;
  }
  #endif
  
  #ifdef OUTPUT_BYAGECLASS
  if(stand->landusetype==PRIMARY){
    cell->output.ageclass_pri_firec[stand->ageclass_standid-1]+=(flux+flux_litter)*fire_trans;
  }else if(stand->landusetype==SECFOREST){
    cell->output.ageclass_sec_firec[stand->ageclass_standid-1]+=(flux+flux_litter)*fire_trans;
  } 
  #endif 
  //---------------------------------
  // mix/create temp stand
  // ..for each primary, secforest
  //---------------------------------
  if(stand->landusetype==PRIMARY){
    //if temporary stand exists, then mix, else create
    if(existlandusetype(cell->standlist,&pos_temp,PRIMARY_TEMP)){
      //mixing of natural vegetation with regrowth
      stand_temp=getstand(cell->standlist,pos_temp);

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2 (stand_new)
      mix_standComplex(cell, stand_temp, stand_new, pos_new, pftpar, npft, TRUE);
    }else{
      //temp designation for mixing, gets set to youngest age-class at end of update_annual
      stand_new->landusetype=PRIMARY_TEMP;
      stand_new->TimeSinceDist=0;
    }
  }else if(stand->landusetype==SECFOREST){
    //if temporary stand exists, then mix, else create
    if(existlandusetype(cell->standlist,&pos_temp,SECFOREST_TEMP)){
      //mixing of natural vegetation with regrowth
      stand_temp=getstand(cell->standlist,pos_temp);

      //complex mix: copy soil, mix pft->nind, increase frac, and delete stand#2 (stand_new)
      mix_standComplex(cell, stand_temp, stand_new, pos_new, pftpar, npft, TRUE);
    }else{
      //temp designation for mixing, gets set to youngest age-class at end of update_annual
      stand_new->landusetype=SECFOREST_TEMP;
      stand_new->TimeSinceDist=0;
    }
  }//..end mix/create temp stand

  //-------------------------
  // update stand fractions
  //-------------------------
  if(fire_trans >= stand->frac){
    //if full stand has fire, then stand_new is fully copied and old stand can be deleted

    //------------------
    //if delete stand, then skip stand in update_annual
    //..solves issue in forloop over standlist
    //-------------------
    *skip_stand=TRUE;

    foreachpft(pft,p,stand->pftlist){
      delpft(stand->pftlist,p);
      p--;
    }
    delstand_preserveOrder(cell,stand,1,pos_stand);
  }else{
    //reduce total fraction by burned area (fire_frac)
    stand->frac -= fire_trans;

    //update within-stand frac_transitions
    //..complex reduction of stand fraction
    for(p=0; p < (stand->len_frac_transition); p++){
      if(fire_trans > stand->frac_transition[p]){
        //reduce difffrac
        fire_trans -= stand->frac_transition[p];
        //set fractional transition to zero
        stand->frac_transition[p]=0.0;

      }else{
        //difffrac is less than the fractional transition
        stand->frac_transition[p] -= fire_trans;
        fire_trans=0.0;
        break;
      }
    }//end update within-stand frac transitions
  }//..end update stand fractions

  //----------------------------
  // End fire on tiles...
  //----------------------------
  //..in update_annual.c, skip everything except establishment for newly created stand
  //..after all stands pass, set temp stand (should only be 1) to youngest stand or mix
  //..run establishment on new stand or the youngest stand, if mixed
  //----------------------------
}

void firepft(Cell *cell, Stand *stand, int pos_stand, Real fire_frac, const Pftpar pftpar[], int npft, Bool *skip_stand)
{
  //-----------------------------------------------------------------------
  // fire_frac: defined in Sitch 2003 as fraction of grid-cell fire,
  //      : ..but, treated here as fraction of stand w/ fire
  //-----------------------------------------------------------------------

  //only do fire if greater than smallest stand fraction
  //DEVQ: should be larger than some area (e.g. 1 km^2 ?)
  //    : ..default has fire ALWAYS for small fraction ~2.5 km^2)
  //    : ..grid-cell fraction..1e-4 ~ 0.25 km^2; 1e-3 ~ 2.5 km^2

  if(fire_frac*stand->frac >= 1e-3){
    //if ageclass, then call fire_ageclass; else, call fire_standard (original formulation)
    //..fire ageclass:
    //.. ..call fire_ageclass routine for multiple ageclasses
    //.. ..runs fire routine for temp stand
    //.. ..merges temp stands if exists
    //.. ..reduced within-stand fractional transitions
    //.. ..mixes new temp stand (fire fraction) w/ young stand at end of update_annual.c

    #if defined(AGECLASS_PRIMARY) && defined(AGECLASS_SECFOREST)
    if(stand->ageclass_standid !=1 && (stand->landusetype==PRIMARY || stand->landusetype==SECFOREST)){
      fire_ageclass(cell, stand, pos_stand, fire_frac, pftpar, npft, skip_stand);
    }else{fire_standard(cell, stand, fire_frac);}
    #elif defined(AGECLASS_PRIMARY) && !defined(AGECLASS_SECFOREST)
    //if stand is primary (ageclasses) only
    if(stand->ageclass_standid !=1 && stand->landusetype==PRIMARY && stand->landusetype!=SECFOREST){
      fire_ageclass(cell, stand, pos_stand, fire_frac, pftpar, npft, skip_stand);
    }else{fire_standard(cell, stand, fire_frac);}
    #elif !defined(AGECLASS_PRIMARY) && defined(AGECLASS_SECFOREST)
    //if stand is secforest (ageclasses) only
    if(stand->ageclass_standid !=1 && stand->landusetype!=PRIMARY && stand->landusetype==SECFOREST){
      fire_ageclass(cell, stand, pos_stand, fire_frac, pftpar, npft, skip_stand);
    }else{fire_standard(cell, stand, fire_frac);}
    #else
    //standard (original) fire routine
    fire_standard(cell, stand, fire_frac);
    #endif
  }//..end if firefrac > small number
} /* of 'firepft' */
