/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ m i x _ s t a n d s . c                           **/
/*******************************************************************/
#include "lpj.h"
#include "grass.h"
#include "tree.h"


Bool exist_pft(int pft_id, Stand *stand,int *pft_pos)
{
  Pft *pft;
  int k;

  foreachpft(pft,k,stand->pftlist){
    if(pft->par->id == pft_id){
          *pft_pos=k;
          return TRUE;
     }
  }
  return FALSE;
}

int copy_pft(Pft *pft, Stand *stand, const Pftpar pftpar[], int npft)
{
  Buffer *buffer, *buffer_dup;
  Pft *pft_dup;
  Pfttree *tree, *tree_dup;
  Pftgrass *grass, *grass_dup;
  int j, k;

  /////////////////////////////
  // deep copy, pft structure
  /////////////////////////////
  addpft(stand->pftlist,pftpar+pft->par->id);
  k=getlistlen(stand->pftlist);
  k--;
  pft_dup=getpft(stand->pftlist,k);

  //copy data
  pft_dup->DailyFPAR=pft->DailyFPAR;
  pft_dup->aphen=pft->aphen;
  pft_dup->bm_inc=pft->bm_inc;
  pft_dup->fpc=pft->fpc;
  pft_dup->gdd=pft->gdd;
  pft_dup->nind=pft->nind;
  pft_dup->phen=pft->phen;
  pft_dup->wscal=pft->wscal;
  pft_dup->wscal_mean=pft->wscal_mean;

  //get buffer structure
  buffer=pft->gddbuf;
  buffer_dup=pft_dup->gddbuf;

  buffer_dup->avg=buffer->avg;
  buffer_dup->index=buffer->index;
  buffer_dup->n=buffer->n;

  for(j=0; j < buffer_dup->n; j++){
    buffer_dup->data[j]=buffer->data[j];
  }

  if(istree(pft)){
    //assign holders
    tree=pft->data;
    tree_dup=pft_dup->data;

    //copy data
    tree_dup->aphen_raingreen=tree->aphen_raingreen;
    tree_dup->crownarea=tree->crownarea;
    tree_dup->gddtw=tree->gddtw;
    tree_dup->height=tree->height;
    tree_dup->ind.debt=tree->ind.debt;
    tree_dup->ind.heartwood=tree->ind.heartwood;
    tree_dup->ind.sapwood=tree->ind.sapwood;
    tree_dup->ind.leaf=tree->ind.leaf;
    tree_dup->ind.root=tree->ind.root;
  }else if(isgrass(pft)){
    //assign holders
    grass=pft->data;
    grass_dup=pft_dup->data;

    //copy data
    grass_dup->ind.leaf=grass->ind.leaf;
    grass_dup->ind.root=grass->ind.root;
  }
  return k;
} // of 'litter_update_deforest'

void mixingRule_pft(Pft *pft1, Real stand1_frac, Pft *pft2, Real stand2_frac, const Pftpar pftpar[], int npft)
{
  Buffer *buffer, *buffer_dup;
  Pfttree *tree, *tree_dup;//_dup is duplicate
  Pftgrass *grass, *grass_dup;
  int j;
  Real new_frac, new_nind;

  //--------------------------------------------------------
  // complex mixing to maintain carbon mass balance
  // .. take the area- and density-weighted mean of values
  // .. new stand fractions, new number of individuals
  //--------------------------------------------------------

  //new stand fraction and area-weighted density of new stand
  new_frac=stand1_frac+stand2_frac;
  new_nind=((stand1_frac*pft1->nind)+(stand2_frac*pft2->nind)) /new_frac;

  /////////////////////////////
  // deep mix, pft structure
  /////////////////////////////
  //copy non-carbon data data
  pft1->DailyFPAR=0;//fpar at day1==0;

  //take mean of the growing degree days, should be the same for pft in a cell
  pft1->gdd=((stand1_frac*pft1->gdd)+(stand2_frac*pft2->gdd))/(new_frac);
  //phenology is copied
  pft1->aphen=max(pft1->aphen,pft2->aphen);
  pft1->phen=max(pft1->phen,pft2->phen);

  //biomass increment is the area-weighted mean between the two pfts
  pft1->bm_inc=((stand1_frac*pft1->bm_inc)+(stand2_frac*pft2->bm_inc))/(new_frac);
  //water stress scalar is also taken as the area-weighted mean between the two pfts
  pft1->wscal=((stand1_frac*pft1->wscal)+(stand2_frac*pft2->wscal))/(new_frac);
  pft1->wscal_mean=((stand1_frac*pft1->wscal_mean)+(stand2_frac*pft2->wscal_mean))/(new_frac);

  //get buffer structure
  buffer=pft2->gddbuf;
  buffer_dup=pft1->gddbuf;

  buffer_dup->avg=buffer->avg;
  buffer_dup->index=buffer->index;
  buffer_dup->n=buffer->n;
  for(j=0; j < buffer_dup->n; j++){
    buffer_dup->data[j]=buffer->data[j];
  }

  if(istree(pft1)){
    //assign holders
    tree=pft2->data;
    tree_dup=pft1->data;

    //copy data
    tree_dup->aphen_raingreen=tree->aphen_raingreen;
    tree_dup->gddtw=tree->gddtw;

    //take the area-weighted and density-weighted means for biomass pools, including the debt term
    tree_dup->ind.debt=((stand1_frac*pft1->nind*tree_dup->ind.debt)+(stand2_frac*pft2->nind*tree->ind.debt))/(new_frac*new_nind);
    tree_dup->ind.heartwood=((stand1_frac*pft1->nind*tree_dup->ind.heartwood)+(stand2_frac*pft2->nind*tree->ind.heartwood))/(new_frac*new_nind);
    tree_dup->ind.sapwood=((stand1_frac*pft1->nind*tree_dup->ind.sapwood)+(stand2_frac*pft2->nind*tree->ind.sapwood))/(new_frac*new_nind);
    tree_dup->ind.leaf=((stand1_frac*pft1->nind*tree_dup->ind.leaf)+(stand2_frac*pft2->nind*tree->ind.leaf))/(new_frac*new_nind);
    tree_dup->ind.root=((stand1_frac*pft1->nind*tree_dup->ind.root)+(stand2_frac*pft2->nind*tree->ind.root))/(new_frac*new_nind);

    //check allometry after changing the mean-individual
    allometry_tree(pft1); //height and crownarea updated for allometric consistency
    //update fpc after change in crownarea and tree density
    //..from fpc_tree.c, but explicit here because density differs
    //..k_beer is the light exitinction coefficient in the beer-lambert law
    pft1->fpc=(tree_dup->crownarea>0.0) ? tree_dup->crownarea*new_nind*(1.0-exp(-pft1->par->k_beer*lai_tree(pft1))) : 0; 
  }else if(isgrass(pft1)){
    //assign holders
    grass=pft2->data;
    grass_dup=pft1->data;

    //take the area-weighted and density-weighted means for biomass pools
    grass_dup->ind.leaf=((stand1_frac*pft1->nind*grass_dup->ind.leaf)+(stand2_frac*pft2->nind*grass->ind.leaf))/(new_frac*new_nind);
    grass_dup->ind.root=((stand1_frac*pft1->nind*grass_dup->ind.root)+(stand2_frac*pft2->nind*grass->ind.root))/(new_frac*new_nind);

    //update fpc after change in leaf biomass...leaf area
    //..from fpc_grass.c, but explicit here because density differs
    //..k_beer is the light exitinction coefficient in the beer-lambert law
    pft1->fpc=new_nind*(1.0-exp(-pft1->par->k_beer*lai_grass(pft1))); 
  }
  //update the new density of individuals in stand
  pft1->nind=new_nind;

  return ;
} // of 'mixing_pft'

void delstand_preserveOrder(Cell *cell, Stand *stand, int start_mid_end, int pos_stand2delete){
  //////////////////////////////////////////////////////////////
  // deletes a stand and preserves the order of the standlist
  // ..this fn is alternate to the delstand() fn, 
  // ..which replaces the deleted stand with the stand in the last position in the standlist (shuffles)
  // ..preserves order of stands in standlist, which makes this quicker for the stand-age transition procedures
  //
  //////////////////////////////////////////////////////////////
  int j;

  //free memory in pftlist
  freepftlist(stand->pftlist);
  //free memory in stand
  free(stand);

  //----------------------------------------------------------------
  //'bubble sort' method:
  //    ..remove deforested stand from list & keep the order
  //    ..replace deleted stand with next stand (?a linked-list structure might be more efficient)
  //    ..OLD METHOD IN [delstand() in standlist.c] cell->standlist->data[i]=cell->standlist->data[cell->standlist->n];
  //----------------------------------------------------------------
  if(start_mid_end==0 || start_mid_end==1){
    for (j = pos_stand2delete; j < (cell->standlist->n - 1); j++){
      cell->standlist->data[j] = cell->standlist->data[j+1];
    }
  }else if(start_mid_end==2){
    //do nothing as memory in stand is freed and standlist will update normally at end of function
  }else{
    fail("\n POSITION OF STAND TO BE DELETED REQUIRED {START==0, MID==1, END==2}...");
  }

  //update list length & reallocate memory for lenght of array
  cell->standlist->n--;
  cell->standlist->data=(void **)realloc(cell->standlist->data,sizeof(void *)*cell->standlist->n);

  return;
}
////////////////////////////////////////////////////////////////////////////
// mixing stand functions {mix soil, pfts, update fracs, delete stand#2  ///
////////////////////////////////////////////////////////////////////////////
void mix_standSimple(Cell *cell, Stand *stand1, Stand *stand2, int pos_stand2) {
  //--------------------------------------------------------------------------
  // NOTE: FN NOT USED, BUT ACCESSIBLE GLOBALLY AS EXTERNAL FN
  //--------------------------------------------------------------------------
  /// function for simple mixing two stands
  ///    :mix soil
  ///    :reduce pft->nind, simple
  ///    :delete stand#2
  ///    :increase frac of stand#1
  //--------------------------------------------------------------------------
  int p;
  Pft *pft;

  mixsoil(stand1, stand2);
  foreachpft(pft,p,stand1->pftlist){
    //reduces the Nind by a fraction
    mix_veg(pft, stand1->frac/ (stand1->frac + stand2->frac));
  }
  stand1->frac += stand2->frac;
  delstand(cell->standlist, pos_stand2); // deleting temporary mixstand copy
  return;
}
void mix_standComplex(Cell *cell, Stand *stand1, Stand *stand2, int pos_stand2, const Pftpar *pftpar, int npft, Bool ISAGECLASS) {
  //////////////////////////////////////////////////////////////////////////////////////////////
  /// function for complex mixing two stands
  ///    :mix soil
  ///    :differs from 'mix_simple()' -> *mix/copy pfts (mixingRule_pft)*
  ///    :delete stand#2
  ///    :increase frac of stand#1
  //////////////////////////////////////////////////////////////////////////////////////////////
  int j, p, pft_pos;
  Pft *pft, *pft_mix;

  //mix soil and litter
  //..simple area-weighted mean
  mixsoil(stand1,stand2);

  //reduce fractions of pfts in SecForest stand that are Not present in TempStand
  foreachpft(pft,p,stand1->pftlist){
    if( !exist_pft(pft->par->id, stand2, &pft_pos)){
      mix_veg(pft, stand1->frac/(stand1->frac+stand2->frac));//simple reduce fraction
    }
  }
  //mix or copy pfts from TempStand to SecForest stand
  foreachpft(pft,p,stand2->pftlist){
    if(exist_pft(pft->par->id, stand1, &pft_pos)){
      ///////////////////////////////////////////
      //DEVQ: consider mixing rules to check density similarity or biomass similarity before mixing fractions
      //  : ..better solution is to have fixed age-classes and within-ageClass fractional transitions
      pft_mix=getpft(stand1->pftlist, pft_pos);
      mixingRule_pft(pft_mix, stand1->frac, pft, stand2->frac, pftpar, npft);

    }else{
      j=copy_pft(pft, stand1, pftpar, npft);
      pft_mix=getpft(stand1->pftlist,j);
      mix_veg(pft_mix, stand2->frac/(stand1->frac+stand2->frac));
    }
  }

  if(ISAGECLASS == TRUE){
    if(stand1->landusetype == PRIMARY || stand1->landusetype == SECFOREST){//accomodates primary and secondary ageclasses
      //update incoming fraction
      // "+=" bc incoming fraction can be from PRIMARY_TEMP or SECFOREST_TEMP 
      //      ..and fractional transitions within diff aged stands
      stand1->frac_transition[0]+=stand2->frac;

      //update total fraction
      stand1->frac+=stand2->frac;
    }else{
      //simple bc temp stands have only 1 fraction and are inter-mixing
      stand1->frac+=stand2->frac;
    }

    //delete temporary mixstand copy
    delstand_preserveOrder(cell,stand2,1, pos_stand2);
  }else{
    stand1->frac+=stand2->frac;

    //delete temporary mixstand copy
    delstand(cell->standlist,pos_stand2);
  }

  return;
}
