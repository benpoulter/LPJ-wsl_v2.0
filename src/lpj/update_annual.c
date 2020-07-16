/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ u p d a t e _ a n n u a l . c                       **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

//==========================================================================================
//                           M A I N   F U N C T I O N 
//
// update_annual() : global function, main routines and calls hyper functions
//==========================================================================================
//                     H Y P E R   F U N C T I O N S 
//               internal hyper-functions called in update_annual
//            ..solves multi-tile issues, primary/secforest,ageclasses..
//==========================================================================================
//  total_carbon()                               : gets total carbon in cell
//  update_pftOutputs()                          : updates pft outputs
//  establishment_and_litterUpdate()             : establishes pfts 
//                                                ..and updates litter on new tiles
//  fire_mixCreate_youngStand()                  : ageclass-only call for fire integration  
//  output_update_woodharvest_deforestproducts() : updates wood product pools and outputs
//==========================================================================================


Real total_carbon(Cell* cell) {
  //----------------------------------------------------------------------------
  // get total carbon in cell
  // ..separate loop for total carbon (carbon balance)
  // ..solves issue regarding adding/removing stands from fire w/ age-classes
  //----------------------------------------------------------------------------

  //declar variables
  int p,s;
  Real totc=0.0; //initialize to zero
  Pft* pft;
  Stand *stand;

  foreachstand(stand,s,cell->standlist){
    foreachpft(pft,p,stand->pftlist)
    {
      totc += vegc_sum(pft)*stand->frac;
    }
    #if !defined(MONTHLYLITTER) && !defined(DAILYLITTER)
      //the original Annual litter scheme
      totc+=(stand->soil.cpool.slow+stand->soil.cpool.fast+
             stand->soil.litter.bg+stand->soil.litter.ag_tree+stand->soil.litter.ag_grass)*stand->frac;
    #else
      //add new litter to totc to make carbon balanced
      totc+=(stand->soil.cpool.slow+stand->soil.cpool.fast+
             stand->soil.litter.bg+stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+
             stand->soil.litter.bg_inc+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
    #endif
  }

  return(totc);
}

void update_pftOutputs(Cell *cell, Stand *stand){
  int p;
  Pft *pft;

  //-----------------------------------------------------------
  // update remaining pft-specific outputs
  // ..internal fn
  // ..if AGECLASS, 
  //   then fn called after main stand loop in update_annual
  // ..otherwise, fn called in main stand loop
  //------------------------------------------------------------
  //potential grid-cell natural plant fractional cover
  cell->output.fpc[0] = 1 - cell->cropfrac - cell->lakefrac;
  #ifdef OUTPUT_BYSTAND
    //potential total grid-cell natural plant fractional cover
    if(stand->landusetype==PRIMARY)   cell->output.fpc_primary[0]   = 1 - cell->cropfrac - cell->lakefrac;
    if(stand->landusetype==SECFOREST) cell->output.fpc_secforest[0] = 1 - cell->cropfrac - cell->lakefrac;
  #endif
  if(stand->landusetype == PRIMARY || stand->landusetype == SECFOREST)
    foreachpft(pft,p,stand->pftlist){
      #ifdef OUTPUT_BYSTAND
        if(stand->landusetype==PRIMARY)   cell->output.fpc_primary[getpftpar(pft,id) + 1]   += pft->fpc*stand->frac;
        if(stand->landusetype==SECFOREST) cell->output.fpc_secforest[getpftpar(pft,id) + 1] += pft->fpc*stand->frac;
      #endif
      cell->output.fpc[getpftpar(pft,id) + 1] += pft->fpc*stand->frac;
    }

  foreachpft(pft,p,stand->pftlist)
  {
    //total carbon now updated in private fn total_carbon()
    //*totc += vegc_sum(pft)*stand->frac;
    if (stand->landusetype == PRIMARY || stand->landusetype == SECFOREST) {
      cell->output.pft_vegc[getpftpar(pft, id)] = vegc_sum(pft)*stand->frac;
      cell->output.pft_nind[getpftpar(pft, id)] = pft->nind; 
    }
  }
}//..end update pft outputs

void establishment_and_litterUpdate(Cell *cell, Stand *stand,
    const Pftpar pftpar[], int npft, int ntypes, Bool intercrop, Bool *present, Bool already_litterUpdate) {
  //--------------------------------------------------
  // annual establishment and litter update
  // ..internal fn
  // ..called in main stand-loop in update_annual
  // ..if AGECLASS, then also called for establishing on fire fraction
  //--------------------------------------------------
  
  //declare variables
  int p;
  Pft* pft;
  Real acflux_estab=0.0;//initialize to zero

  //------------------------------
  //  annual establishment
  //------------------------------
  if (cell->aprec >= APREC_MIN && (stand->landusetype == PRIMARY || stand->landusetype == SECFOREST)) {
    acflux_estab = establishmentpft(&stand->soil.litter, stand->pftlist,
                                    pftpar, npft, ntypes, cell->gdd, &cell->climbuf, cell->pft,
                                    FALSE);
  } else if ((stand->landusetype == SETASIDE && intercrop) || stand->landusetype == GRASSLAND) {
    for (p = 0; p < npft; p++) {
      if (establish(cell->gdd[p], pftpar + p, &cell->climbuf, cell->pft)
                    && pftpar[p].type == GRASS && (!present[p]))
      { 
        addpft(stand->pftlist, pftpar + p);
      }
    }
    foreachpft(pft,p,stand->pftlist)
      acflux_estab += establishment(pft, 0, 0, 1);
  }

  //update establishment flux
  cell->output.flux_estab += acflux_estab * stand->frac;
  #ifdef OUTPUT_BYSTAND
    if(stand->landusetype==PRIMARY){
      cell->output.flux_estab_primary += acflux_estab*stand->frac;
    }else if(stand->landusetype==SECFOREST){
      cell->output.flux_estab_secforest += acflux_estab*stand->frac;
    }
  #endif

  #ifdef OUTPUT_BYAGECLASS
    if(stand->landusetype==PRIMARY){
      cell->output.ageclass_pri_fluxestab[stand->ageclass_standid-1]+= acflux_estab*stand->frac;
    }else if(stand->landusetype==SECFOREST){
      cell->output.ageclass_sec_fluxestab[stand->ageclass_standid-1]+= acflux_estab*stand->frac;
    }
  #endif

  //------------------------
  //    update litter
  //------------------------
  //new litter scheme -- Add all the increment of litter in current year to litter pools as the original code did
  //total carbon now updated in private fn total_carbon()
  if(already_litterUpdate == FALSE){
    #if !defined(MONTHLYLITTER) && !defined(DAILYLITTER)
      //the original Annual litter scheme
      stand->soil.litter.bg       +=stand->soil.litter.bg_inc;
      stand->soil.litter.ag_tree  +=stand->soil.litter.ag_tree_inc;
      stand->soil.litter.ag_grass +=stand->soil.litter.ag_grass_inc;
    #endif
  }
}

void fire_mixCreate_youngStand(Cell *cell,const Pftpar pftpar[], int npft, int ncft, int ntypes, Bool intercrop, Bool *present){
#if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
  //define variables
  int p, pos_yng, pos_temp;
  Stand *stand_yng, *stand_temp, *stand_estab;
  Bool already_litterUpdate=TRUE; // litter is updated for temp stand before mixing, therefore always TRUE

  //-------------------------------------------------------------
  // A G E - C L A S S : 
  //  ..for both primary and secforest ageclasses,
  //  ..after fire, create young stand or mix if one exists
  //  ..allow establihsment on youngest stand
  //-------------------------------------------------------------
  
  #ifdef AGECLASS_PRIMARY
    //...................................
    // P R I M A R Y _ T E M P
    //...................................
    if(existlandusetype(cell->standlist,&pos_temp,PRIMARY_TEMP)){
      //if there exists a temporary stand
      stand_temp=getstand(cell->standlist,pos_temp);

      #if !defined(MONTHLYLITTER) && !defined(DAILYLITTER)
        //the original Annual litter scheme
        stand_temp->soil.litter.bg       +=stand_temp->soil.litter.bg_inc;
        stand_temp->soil.litter.ag_tree  +=stand_temp->soil.litter.ag_tree_inc;
        stand_temp->soil.litter.ag_grass +=stand_temp->soil.litter.ag_grass_inc;
      #endif

      //if the youngest secforest stand exists, else set temp stand to youngest stand
      if(ageclass_createORmix(cell->standlist,&pos_yng,PRIMARY)){
        stand_yng=getstand(cell->standlist,pos_yng);
        stand_yng->TimeSinceDist=0;

        //set pointer for establishment
        stand_estab=stand_yng;

        //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2 (temp stand)
        mix_standComplex(cell, stand_yng, stand_temp, pos_temp, pftpar, npft, TRUE);

      }else{//new young secforest stand
        //set pointer for establishment
        stand_estab=stand_temp;

        stand_temp->landusetype=PRIMARY;

        //youngest ageclass stand is standid 1
        //during LU spinup, the age-class transitions naturally
        stand_temp->ageclass_standid=1;

        //store the length of the vector for fractional transitions
        stand_temp->len_frac_transition=AGECLASS_WIDTH_1; //incoming fraction

        //first set all fractional transitions to zero
        for(p=0; p < stand_temp->len_frac_transition; p++){
          stand_temp->frac_transition[p]=0.0;
        }

        //store incoming fraction
        stand_temp->frac_transition[0]=stand_temp->frac;
      }

      //-------------------------------------------
      // Establishment and litter update
      // ..now internal, void fn
      // ..solves issue regarding establishment and litter update for temp stand (fire w/ age-class)
      //-------------------------------------------
      //vector 'present' is dummy (reqd), as its not used for PRIMARY || SECFOREST stands
      establishment_and_litterUpdate(cell, stand_estab, pftpar, npft, ntypes,intercrop, present, already_litterUpdate);
    }
  #endif //..end primary_temp update

  #ifdef AGECLASS_SECFOREST
    //...................................
    // S E C F O R E S T _ T E M P
    //...................................
    if(existlandusetype(cell->standlist,&pos_temp,SECFOREST_TEMP)){
      //if there exists a temporary stand
      stand_temp=getstand(cell->standlist,pos_temp);

      #if !defined(MONTHLYLITTER) && !defined(DAILYLITTER)
        //the original Annual litter scheme
        stand_temp->soil.litter.bg       +=stand_temp->soil.litter.bg_inc;
        stand_temp->soil.litter.ag_tree  +=stand_temp->soil.litter.ag_tree_inc;
        stand_temp->soil.litter.ag_grass +=stand_temp->soil.litter.ag_grass_inc;
      #endif

      //if the youngest secforest stand exists, else set temp stand to youngest stand
      if(ageclass_createORmix(cell->standlist,&pos_yng,SECFOREST)){
        stand_yng=getstand(cell->standlist,pos_yng);
        stand_yng->TimeSinceDist=0;

        //set pointer for establishment
        stand_estab=stand_yng;

        //complex mix: copy soil, *mix/copy pfts, increase frac, and delete stand#2 (temp stand)
        mix_standComplex(cell, stand_yng, stand_temp, pos_temp, pftpar, npft, TRUE);

      }else{//new young secforest stand
        //set pointer for establishment
        stand_estab=stand_temp;

        stand_temp->landusetype=SECFOREST;

        //youngest ageclass stand is standid 1
        //during LU spinup, the age-class transitions naturally
        stand_temp->ageclass_standid=1;

        //store the length of the vector for fractional transitions
        stand_temp->len_frac_transition=AGECLASS_WIDTH_1; //incoming fraction

        //first set all fractional transitions to zero
        for(p=0; p < stand_temp->len_frac_transition; p++){
          stand_temp->frac_transition[p]=0.0;
        }

        //store incoming fraction
        stand_temp->frac_transition[0]=stand_temp->frac;
      }

      //-------------------------------------------
      // Establishment and litter update
      // ..now internal, void fn
      // ..solves issue regarding establishment and litter update for temp stand (fire w/ age-class)
      //-------------------------------------------
      //vector 'present' is dummy (reqd), as its not used for PRIMARY || SECFOREST stands
      establishment_and_litterUpdate(cell, stand_estab, pftpar, npft, ntypes,intercrop, present, already_litterUpdate);
    }//..end secforest_temp update
  #endif //..end secforest_temp update

#endif //..end of AGECLASS routines 
}//..end fire ageclass update and establishment

void output_update_woodharvest_deforestproducts(Cell *cell){

  #if defined(DEFORESTPRODUCT) && !defined(NAT_VEG) && !defined(GROSS_LUC) && !defined(NETinGROSS)
    //occurs if running HYDE (NET) with deforestation product pools
    //update outputs for the deforestation product pools (delayed emission of deforested wood)
    cell->output.deforestProduct_25yr  +=cell->product_25yr *decayrate_product_25yr;
    cell->output.deforestProduct_2yr   +=cell->product_2yr  *decayrate_product_2yr;

    //update product pools
    cell->product_25yr  -=cell->product_25yr *decayrate_product_25yr;
    cell->product_2yr   -=cell->product_2yr  *decayrate_product_2yr;

    //update carbon in product pools remaining (output and reset annually)
    cell->output.deforestProduct_25yr_remain  =cell->product_25yr;
    cell->output.deforestProduct_2yr_remain   =cell->product_2yr;
  #endif
 
  #if (defined(GROSS_LUC) || defined(NETinGROSS)) && !defined(NAT_VEG)
    #ifndef NOWDHARVEST  
      //update outputs for wood harvest
      //update woodharvest productpool decay
      cell->output.woodharvest_100yr +=cell->product_100yr*decayrate_product_100yr;
      cell->output.woodharvest_10yr  +=cell->product_10yr *decayrate_product_10yr;
      cell->output.woodharvest_1yr   +=cell->product_1yr  *decayrate_product_1yr;

      //update product pools
      cell->product_100yr -=cell->product_100yr*decayrate_product_100yr;
      cell->product_10yr  -=cell->product_10yr *decayrate_product_10yr;
      cell->product_1yr   -=cell->product_1yr  *decayrate_product_1yr;
      if(cell->product_1yr <0)cell->product_1yr=0.0;//catch floating-point errors

      //update carbon in product pools remaining (output and reset annually)
      cell->output.woodharvest_100yr_remain =cell->product_100yr;
      cell->output.woodharvest_10yr_remain  =cell->product_10yr;
      cell->output.woodharvest_1yr_remain   =cell->product_1yr;
    #endif

    #ifdef DEFORESTPRODUCT
      //update outputs for the deforestation product pools (delayed emission of deforested wood)
      cell->output.deforestProduct_25yr  +=cell->product_25yr *decayrate_product_25yr;
      cell->output.deforestProduct_2yr   +=cell->product_2yr  *decayrate_product_2yr;

      //update product pools
      cell->product_25yr  -=cell->product_25yr *decayrate_product_25yr;
      cell->product_2yr   -=cell->product_2yr  *decayrate_product_2yr;

      //update carbon in product pools remaining (output and reset annually)
      cell->output.deforestProduct_25yr_remain  =cell->product_25yr;
      cell->output.deforestProduct_2yr_remain   =cell->product_2yr;
    #endif
  
    #ifdef OUTPUT_BYSTAND
      #ifndef NOWDHARVEST  
        //...........................................
        // primary stand outputs
        // update outputs for wood harvest
        // update woodharvest productpool decay
        cell->output.woodharvest_100yr_primary +=cell->product_100yr_primary *decayrate_product_100yr;
        cell->output.woodharvest_10yr_primary  +=cell->product_10yr_primary  *decayrate_product_10yr;
        cell->output.woodharvest_1yr_primary   +=cell->product_1yr_primary   *decayrate_product_1yr;

        //update product pools
        cell->product_100yr_primary -=cell->product_100yr_primary *decayrate_product_100yr;
        cell->product_10yr_primary  -=cell->product_10yr_primary  *decayrate_product_10yr;
        cell->product_1yr_primary   -=cell->product_1yr_primary   *decayrate_product_1yr;
        if(cell->product_1yr_primary <0)cell->product_1yr_primary =0.0;//catch floating-point errors

        //update carbon in product pools remaining (output and reset annually)
        cell->output.woodharvest_100yr_remain_primary =cell->product_100yr_primary;
        cell->output.woodharvest_10yr_remain_primary  =cell->product_10yr_primary;
        cell->output.woodharvest_1yr_remain_primary   =cell->product_1yr_primary;

        //...........................................
        // secondary stand outputs
        //update outputs for wood harvest
        //update woodharvest productpool decay
        cell->output.woodharvest_100yr_secforest +=cell->product_100yr_secforest *decayrate_product_100yr;
        cell->output.woodharvest_10yr_secforest  +=cell->product_10yr_secforest  *decayrate_product_10yr;
        cell->output.woodharvest_1yr_secforest   +=cell->product_1yr_secforest   *decayrate_product_1yr;
  
        //update product pools
        cell->product_100yr_secforest -=cell->product_100yr_secforest *decayrate_product_100yr;
        cell->product_10yr_secforest  -=cell->product_10yr_secforest  *decayrate_product_10yr;
        cell->product_1yr_secforest   -=cell->product_1yr_secforest   *decayrate_product_1yr;
        if(cell->product_1yr_secforest <0)cell->product_1yr_secforest =0.0;//catch floating-point errors

        //update carbon in product pools remaining (output and reset annually)
        cell->output.woodharvest_100yr_remain_secforest =cell->product_100yr_secforest;
        cell->output.woodharvest_10yr_remain_secforest  =cell->product_10yr_secforest;
        cell->output.woodharvest_1yr_remain_secforest   =cell->product_1yr_secforest;
      #endif

      #ifdef DEFORESTPRODUCT
        //...........................................
        // primary stand outputs
        //update outputs for the deforestation product pools (delayed emission of deforested wood)
        cell->output.deforestProduct_25yr_primary +=cell->product_25yr_primary *decayrate_product_25yr;
        cell->output.deforestProduct_2yr_primary  +=cell->product_2yr_primary  *decayrate_product_2yr;

        //update product pools
        cell->product_25yr_primary -=cell->product_25yr_primary *decayrate_product_25yr;
        cell->product_2yr_primary  -=cell->product_2yr_primary  *decayrate_product_2yr;

        //update carbon in product pools remaining (output and reset annually)
        cell->output.deforestProduct_25yr_remain_primary =cell->product_25yr;
        cell->output.deforestProduct_2yr_remain_primary  =cell->product_2yr;

        //...........................................
        // secondary stand outputs
        //update outputs for the deforestation product pools (delayed emission of deforested wood)
        cell->output.deforestProduct_25yr_secforest +=cell->product_25yr_secforest *decayrate_product_25yr;
        cell->output.deforestProduct_2yr_secforest  +=cell->product_2yr_secforest  *decayrate_product_2yr;

        //update product pools
        cell->product_25yr_secforest -=cell->product_25yr_secforest *decayrate_product_25yr;
        cell->product_2yr_secforest  -=cell->product_2yr_secforest  *decayrate_product_2yr;

        //update carbon in product pools remaining (output and reset annually)
        cell->output.deforestProduct_25yr_remain_secforest =cell->product_25yr;
        cell->output.deforestProduct_2yr_remain_secforest  =cell->product_2yr;
      #endif  

    #endif  //..end output by stand
  #endif //..end of output update
  
} //..end of output_update_woodharvest_deforestproducts

//------------------------------------------------
// main function
// ..calls hyper-functions (static, internal fns)
//------------------------------------------------
Real update_annual(FILE **output,
                   Cell *cell,
                   const Pftpar pftpar[],
                   int npft,
                   int ncft,
                   int ntypes,
                   int year,
                   Config config,
                   Bool intercrop
                  )
{
  int s,p;
  Bool *present;
  #ifdef DEBUGEFENCE
  Bool test=FALSE;
  #endif 
  Pft *pft;
  Real *fpc_inc=NULL;
  Real totc,delta_totc,rharvest;
  Real turnover_ind,fire_frac;
  Stand *stand;
  Real *pftNindInit=NULL; 
  Real harvestflux;                //STANDAGE output
  harvestflux=0.;                  //STANDAGE output
  Bool skip_stand;                 //for fire w/ age-classes
  Bool already_litterUpdate=FALSE; //for fire w/ age-classes

  present=newvec(Bool,npft+ncft);
  check(present);
  annual_climbuf(&cell->climbuf);

  delta_totc=totc=0.0;
  
  /* count number of years without harvest 
   * and set 20-year mean to zero if no harvest
   * occurred within the last 10 years */
  for(p=0;p<ncft;p++){
    if(cell->cropdates.last_update_fall[p]>=0){
      cell->cropdates.last_update_fall[p]++;
      cell->cropdates.last_update_spring[p]++;
      cell->cropdates.last_update_vern[p]++;

      if(cell->cropdates.last_update_fall[p]>10) cell->cropdates.fall_sdate20[p]=0;
      if(cell->cropdates.last_update_spring[p]>10) cell->cropdates.spring_sdate20[p]=0;
      if(cell->cropdates.last_update_vern[p]>10) cell->cropdates.vern_date20[p]=0; 
    }   
  }

  //update outputs for woodharvest and deforestation products
  //..hyper-function call
  output_update_woodharvest_deforestproducts(cell);
 
  foreachstand(stand,s,cell->standlist)
  {
    #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
    //skip stand routine for the temp stand, created during fire w/ age-classes routine
    //establishment, litter update and totc updated for temp stand separately
    //after stand loop, temp stand mixed with existing youngest secondary stand if exists
    //..else creates youngest stand
    if(stand->landusetype==PRIMARY_TEMP || stand->landusetype==SECFOREST_TEMP) continue;
    #endif

    fire_frac=0.0;
    for(p=0;p<npft+ncft;p++)
      present[p]=FALSE;
  
    #ifdef DEBUGEFENCE
    test=FALSE;
    if(getnpft(stand->pftlist)>0){
      test=TRUE;
    #endif

    pftNindInit=newvec(Real,npft);
    check(pftNindInit);  

    fpc_inc=newvec(Real,getnpft(stand->pftlist));
    if(getnpft(stand->pftlist)!=0) check(fpc_inc);
    #ifdef DEBUGEFENCE
    }
    #endif

    //----------------------------------------------------------
    // DEVQ: reqd ageclass update to TimeSinceDist, CropYrs
    //----------------------------------------------------------
    //TimeSinceDisturbance and time under CropMgmt increments
    //..resets to zero only during LUC, and Gross Transition
    //..BUT, no current code to reset to zero under other disturbance type (Fire)
    #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
    if(stand->landusetype == GRASSLAND) stand->CropYrs++;
    #endif
 
    #ifdef AGECLASS_PRIMARY
    if(stand->landusetype==PRIMARY) stand->TimeSinceDist++;
    #endif

    #ifdef AGECLASS_SECFOREST
    if(stand->landusetype==SECFOREST) stand->TimeSinceDist++;
    #endif
    //..end up ageclass update to TSD, CropYrs
  
    foreachpft(pft,p,stand->pftlist)
    {
      #ifdef DEBUG2
        printf("PFT:%s fpc_inc=%g fpc=%g\n",pft->par->name,fpc_inc[p],pft->fpc);
        printf("PFT:%s %d  bm_inc=%g vegc=%g soil=%g\n",pft->par->name,year,pft->bm_inc,vegc_sum(pft),
        stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.ag_tree+stand->soil.litter.ag_grass);
      #endif

      //store initial nind before mortality occurs
      if(stand->landusetype==PRIMARY || stand->landusetype==SECFOREST)
      pftNindInit[getpftpar(pft,id)] = pft->nind; 

      present[pft->par->id]=TRUE;

      //-----------------------------------------
      //reproduction costs and turnvoer
      //-----------------------------------------
      reproduction(&stand->soil.litter,pft);
      turnover_ind=turnover(&stand->soil.litter,pft);

      //-----------------------------------------
      //allocate biomass inc from year to grow
      //-----------------------------------------
      allocation(&stand->soil.litter,pft,fpc_inc+p,FALSE);

      //-----------------------------------------
      // kill pft if negative biomass
      // ..occurs if C-reserve pool cannot meet..
      // ..neg NPP demand (min. respiration costs)
      // ..DEVQ: potentially solved w/ NSC module
      //-----------------------------------------
      if(isneg(pft)){ 
        fpc_inc[p]=fpc_inc[getnpft(stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind,&rharvest,FALSE,FALSE);
        delpft(stand->pftlist,p);
        p--;  
        continue;
      } 

      mortality(&stand->soil.litter,pft,turnover_ind,cell->climbuf.temp_max,&fpc_inc[p]);

      updatebuffer(pft->gddbuf,cell->gdd[getpftpar(pft,id)]);

      //DEVQ: why another check for negative biomass/bm_inc?
      if(isneg(pft)){ 
        fpc_inc[p]=fpc_inc[getnpft(stand->pftlist)-1];
        litter_update(&stand->soil.litter,pft,pft->nind,&rharvest,FALSE,FALSE);
        delpft(stand->pftlist,p);
        p--;  
        continue;
      }

      if(killpft(&stand->soil.litter,pft,&cell->climbuf))
      {
        fpc_inc[p]=fpc_inc[getnpft(stand->pftlist)-1];
        delpft(stand->pftlist,p);
        p--;  /* adjust loop index */
    
        #ifdef DEBUG2
        printf("killpft\n");
        #endif
        continue;
      }
    } /* of foreachpft */

    #ifdef DEBUG2
    printf("Number of updated pft: %d\n",stand->pftlist->n);
    #endif

    if(stand->landusetype!=AGRICULTURE)
      light(&stand->soil.litter,stand->pftlist,ntypes,fpc_inc,FALSE);

    //clearcut PFTs from PRIMARY stands only
    //Set up so that if stand is older than year 1901, no management used
    //For now, just edit this out to disable the clearcutpft routine
    #ifdef STANDAGEINIT
    //DEVQ: standage flag is setup to only run with PRIMARY stands, not SECFOREST (secondary)
    if(year == (cell->standage-1)){
      if(stand->landusetype==PRIMARY)
        clearcutpft(&stand->soil.litter,stand->pftlist,&harvestflux);
      printf("Clearcut happened %f removed\n", harvestflux);
    }
    #endif

    if(iffire && (stand->landusetype == PRIMARY || stand->landusetype == SECFOREST)){
      skip_stand=FALSE;//initialize to false
      //fire flux is calculated & updated in firepft 
      fire_frac=fire_prob(&stand->soil.litter,stand->fire_sum);
      firepft(cell, stand, s, fire_frac, pftpar, npft, &skip_stand);

      //if stand was deleted then go to next stand in standlist
      if(skip_stand==TRUE) {free(fpc_inc); continue;}
    }

    //after all mortality calls are finished, calc. difference between initial nind and final
    if (stand->landusetype == PRIMARY || stand->landusetype == SECFOREST)
      foreachpft(pft,p,stand->pftlist)
        cell->output.pft_mort[getpftpar(pft, id)] = pftNindInit[getpftpar(pft, id)] - pft->nind;

    //-------------------------------------------
    // Establishment and litter update
    // ..now internal, void fn
    // ..solves issue regarding establishment and litter update for temp stand (fire w/ age-class)
    //-------------------------------------------
    establishment_and_litterUpdate(cell, stand, pftpar, npft, ntypes,intercrop, present, already_litterUpdate);

    //-------------------------------------------
    // A G E C L A S S :update outputs if no ageclass
    //-------------------------------------------
    #if !defined(AGECLASS_PRIMARY) && !defined(AGECLASS_SECFOREST)
      //-------------------------------
      //  update pft-specific outputs
      //-------------------------------
      update_pftOutputs(cell, stand);
    #elif defined(AGECLASS_PRIMARY) && defined(AGELASS_SECFOREST)
      //do nothing
    #elif !defined(AGECLASS_PRIMARY) && defined(AGELASS_SECFOREST)
      //check if stand is primary (no ageclass)
      if(stand->landusetype==PRIMARY) update_pftOutputs(cell, stand);
    #else 
      //..condition: defined(AGECLASS_PRIMARY) && !defined(AGELASS_SECFOREST)
      //check if stand is secondary (no ageclass)
      if(stand->landusetype == SECFOREST) update_pftOutputs(cell, stand);
    #endif
    //------------end ageclass update stand checks

    #ifdef DEBUGEFENCE
    if(test==TRUE)
    #endif  
    //clear memory
    free(fpc_inc);
  } /* of foreachstand */

  #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
    //--------------------------------------------------------
    // Solution for Fire w/ Age-classes
    //  ..catch temporary stands created during fire routine
    //  ..set to youngest age-class or mix if it exists
    //  ..allow establishment and update litter only once for the new stand fracion
    //  .. -- calls internal void fn establishment_and_litterUpdate()
    //----------------------------------------------------------
    fire_mixCreate_youngStand(cell,pftpar,npft,ncft,ntypes,intercrop,present);

    //-------------------------------
    //  update pft-specific outputs
    //-------------------------------
    #if defined(AGECLASS_PRIMARY) && defined(AGECLASS_SECFOREST)
      foreachstand(stand,s, cell->standlist) update_pftOutputs(cell, stand);
    #elif defined(AGECLASS_PRIMARY) && !defined(AGECLASS_SECFOREST)
      foreachstand(stand,s, cell->standlist){if(stand->landusetype==PRIMARY){update_pftOutputs(cell, stand);}}
    #else //..condition: !defined(AGECLASS_PRIMARY) && defined(AGECLASS_SECFOREST)
      foreachstand(stand,s, cell->standlist){if(stand->landusetype==SECFOREST){update_pftOutputs(cell, stand);}}
    #endif
  #endif

  //update total carbon for C-balance check
  //..solves issue regarding carbon balance and adding/removing stands from fire w/ age-class
  //..get total carbon in cell
  totc=total_carbon(cell);

  if((config.firstyear!=config.lastyear) && (year>=(config.firstyear-config.nspinup))){
    #ifdef SINGLECELL_CSVOUTPUT
      //........................
      //  cell info to csv 
      //........................
      if(year==(config.firstyear-config.nspinup)){
        //create text file
        FILE *f = fopen("singlecell_csvoutput_cellinfo.txt", "w");

        //write the header, then write the data
        fprintf(f, "year,lat,lon\n");
        fprintf(f, "%d,%f,%f\n",year,cell->coord.lat,cell->coord.lon);
        fclose(f);
      }else{
        //append text file
        FILE *f = fopen("singlecell_csvoutput_cellinfo.txt", "a");

        //write the data
        fprintf(f, "%d,%f,%f\n",year,cell->coord.lat,cell->coord.lon);
        fclose(f);
      }

      //........................
      // variables to csv
      //........................
      if(year==(config.firstyear-config.nspinup)){
        fwriteoutput_annual(output,cell,config.n_out,FALSE);
      }else{fwriteoutput_annual(output,cell,config.n_out,TRUE);}
    #else
      fwriteoutput_annual(output,cell,config.n_out,FALSE);
    #endif
  }

  delta_totc=(totc-cell->totc+harvestflux);
  cell->totc=totc;
  free(present);  

  return delta_totc;
} /* of 'update_annual' */
