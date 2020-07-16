/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ e s t a b l i s h m e n t p f t . c                **/
/*******************************************************************/

#include "lpj.h"
#include "tree.h"
#include "grass.h"

#ifdef OPTIMIUM_ESTAB
static Real optimum_establishment(Bool present[], 
                                  Real fpc_type[], 
                                  int n_est[], 
                                  Litter *litter,
                                  Pftlist pftlist,
                                  const Pftpar *pftpar,
                                  int npft,
                                  int ntypes,
                                  const Real gdd[],
                                  const Climbuf *climbuf, 
                                  int pftid,
                                  Bool subannual)
{
  //-----------------------
  // declare variables
  //-----------------------
  Bool update_acflux, update_seedling, nind_seedling_2litter;
  Real orig_acflux_est=0.0, new_acflux_est,adjustment_val,new_nind,new_fpc, nind_seedling;
  //update litter temp holders to zero
  Real tmp_ag_tree=0.0, tmp_ag_tree_inc=0.0, tmp_bg=0.0, tmp_bg_inc=0.0;
  Real nind_old;
  Real acflux_est=0.0,dummy;
  Real fpc_total=0.0;
  int p;
  Pft *pft;
  Pfttree *tree;
  Pfttreepar *treepar;

   //------------------------------------------------------
   //    O P T I M U M   E S T A B L I S H M E N T
   //------------------------------------------------------
   // LCalle 2017..solutions for dilution of mean-individual
   //
   //..ORIGINAL FORMULATION:
   //.. ..establish seedlings
   //.. ..mix seedling C-pools with existing tree for mean-individual
   //.. ..obtain new fpc and allometries
   //.. ..if fpc > fpc_max, then..
   //.. ..reduce the tree density for each pft by an equal amount
   //.. ..send adjustment to litter
   //..
   //..OPTIMUM FORMULATION:
   //.. ..establish seedlings
   //.. ..mix seedling C-pools with existing tree for mean-individual
   //.. ..obtain new fpc and allometries
   //.. ..if fpc > fpc_max, then..
   //.. ..reset carbon pools and density to original values
   //.. ..(i) determine surviving seedlings and send excess seedlings to litter
   //.. ..(ii) obtain the number of surviving seedlings from (i)
   //.. ..mix seedling C-pools with existing tree for mean-individual
   //.. ..obtain new fpc and allometries
   //.. ..calculate seedlings established based on original formulation..
   //.. .. ..finally, reduce density if fpc is still greater than fpc_max
   //
   //    FIRST: estimate num. of seedlings established
   //    ..store original biomass and density
   //
   //    SECOND: if FPC_TREE > MAX adjust the tree density
   //    ..reset carbon pools and density
   //    ..calculate number of seedlings successfully established while FPC_TREE <= MAX
   //    ..send exess seedlings to litter
   //
   //    THIRD: if total fpc > 1, adjust nind for all
   //    ..this is the veg dynamics part
   //    ..
   //    ..reset carbon pool and density values
   //    ..adjust seedling density again and re-calculate successful seedling number
   //    ..send exess seedlings to litter
   //
   //    FOURTH: update litter pools and establishment flux if necessary
   //-------------------------------------------------

   fpc_total=fpc_sum(fpc_type,ntypes,pftlist);
   foreachpft(pft,p,pftlist){
       //store original data for carbon pool and plant density
       if(istree(pft)){
         tree=pft->data;
         tree->ind.old_leaf=tree->ind.leaf;
         tree->ind.old_root=tree->ind.root;
         tree->ind.old_sapwood=tree->ind.sapwood;
         tree->ind.old_heartwood=tree->ind.heartwood;
         tree->ind.old_debt=tree->ind.debt;
         tree->ind.old_nind=pft->nind;
       }

       if(establish(gdd[pft->par->id],pftpar+pft->par->id,climbuf,pftid)){
         if(istree(pft)){
           orig_acflux_est+=establishment(pft,fpc_total,fpc_type[pft->par->type],n_est[pft->par->type]);
         }else{
           acflux_est+=establishment(pft,fpc_total,fpc_type[pft->par->type],n_est[pft->par->type]);
         }
       }//end establish()
   }//end first pft loop

   //--------------------------------
   //update values for fpc_type
   fpc_sum(fpc_type,ntypes,pftlist);

   //------------------------------------------------------------
   //SECOND: adjust the tree density if FPC > MAX
   //------------------------------------------------------------

   //----------------------------
   //initialize to zero & false
   new_acflux_est = 0.0;
   update_acflux  = FALSE;
   //----------------------------------------------
   //reset and update new est. nind if fpc > max
   foreachpft(pft,p,pftlist){
     if(istree(pft) && fpc_type[TREE]>FPC_TREE_MAX){
      update_seedling = FALSE;
       adjustment_val = FPC_TREE_MAX/fpc_type[pft->par->type];
       new_nind       = pft->nind*adjustment_val;
       new_fpc      = pft->fpc*adjustment_val;

     //-------------------------
     // Reset Carbon Pools
     //-------------------------
     //..adjustment based on updated number of seedlings established
     //.. .. that will not cause fpc_type[TREE] to be greater than MAX
     tree=pft->data;
     treepar=getpftpar(pft,data);

     tree->ind.leaf      = tree->ind.old_leaf;
     tree->ind.root      = tree->ind.old_root;
     tree->ind.sapwood   = tree->ind.old_sapwood;
     tree->ind.heartwood = tree->ind.old_heartwood;
     tree->ind.debt      = tree->ind.old_debt;

     //-------------------
     // seedling number
     //-------------------
     //eq: assumes new_nind is always greater than old nind
     nind_seedling = new_nind - tree->ind.old_nind;
     //if nind_seedling is negative, then don't update and send all seedlings to litter
     if(nind_seedling < 0){update_seedling = TRUE; nind_seedling = pft->nind - tree->ind.old_nind;}

     //eq: assumes that old seedling num. is greater than new seedling num.
     //  .. (seedling nind) - (new seedling nind);
     nind_seedling_2litter = (pft->nind - tree->ind.old_nind) - nind_seedling;

     //RESET DENSITY
     pft->nind = tree->ind.old_nind;

     //if negative seedling establishment
     //..simply pass seedling establishment to litter
     //..do not update density and biomass pools
     if(update_seedling== TRUE){
       //---------------------------------------------
       //send excess seedlings to litter temp holder
       //..taken from litter_update_tree
       if(subannual){
         tmp_ag_tree +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling;
         tmp_bg      +=treepar->sapl.leaf*nind_seedling;
       }else{
         tmp_ag_tree_inc +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling;
         tmp_bg_inc      +=treepar->sapl.leaf*nind_seedling;
       }
       //end litter update
     }else{
       //update establishment flux
       update_acflux = TRUE;

       //update carbon pools with new establishment estimate
       if (pft->nind<0.0) pft->nind=0.0;
       nind_old=pft->nind;
       pft->nind+=nind_seedling;

       // Account for flux from the atmosphere to new saplings
       new_acflux_est+= phys_sum_tree(treepar->sapl)*nind_seedling;

       // Adjust average individual C biomass based on average biomass and density of the new saplings
       tree->ind.leaf=(tree->ind.leaf*nind_old+treepar->sapl.leaf*nind_seedling)/pft->nind;
       tree->ind.root=(tree->ind.root*nind_old+treepar->sapl.root*nind_seedling)/pft->nind;
       tree->ind.sapwood=(tree->ind.sapwood*nind_old+treepar->sapl.sapwood*nind_seedling)/pft->nind;
       tree->ind.heartwood=(tree->ind.heartwood*nind_old+treepar->sapl.heartwood*nind_seedling)/pft->nind;
       tree->ind.debt=tree->ind.debt*nind_old/pft->nind;

       //---------------------------------------------
       //send excess seedlings to litter temp holder
       //..taken from litter_update_tree
       if(subannual){
         tmp_ag_tree +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling_2litter;
         tmp_bg      +=treepar->sapl.leaf*nind_seedling_2litter;
       }else{
         tmp_ag_tree_inc +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling_2litter;
         tmp_bg_inc      +=treepar->sapl.leaf*nind_seedling_2litter;
       }
       //end litter update
     }
      allometry_tree(pft);
      fpc_tree(pft);
     }//end is tree
   }//end pft loop

   //----------------------------------------------
   //THIRD: adjust nind for all if total fpc > 1
   //..this is the veg dynamics part
   //..
   //.. ..(1) resett carbon pool and density values
   //.. ..(2) adjust seedling density again and re-calculate
   //----------------------------------------------
   fpc_total=fpc_sum(fpc_type,ntypes,pftlist);

   if(fpc_total>1.0){
     //initialize to zero & false
     new_acflux_est = 0.0;
     update_acflux  = FALSE;

     //reset accounting for seedlings to litter
     //tmp_ag_tree=tmp_ag_tree_inc=tmp_bg=tmp_bg_inc=0.0;

     foreachpft(pft,p,pftlist){
     if(istree(pft)){//reduce nind regardless if fpc_type[TREE] greater than max; fpc_total cannot be > 1
       update_seedling = FALSE;
       adjustment_val = fpc_total;
       new_nind       = pft->nind/adjustment_val;
       new_fpc     = pft->fpc/adjustment_val;

       //-------------------------
       // Reset Carbon Pools
       //-------------------------
       //..adjustment based on updated number of seedlings established
       //.. .. that will not result in fpc_type[TREE] to be greater than MAX
       tree=pft->data;
       treepar=getpftpar(pft,data);

       tree->ind.leaf      = tree->ind.old_leaf;
       tree->ind.root      = tree->ind.old_root;
       tree->ind.sapwood   = tree->ind.old_sapwood;
       tree->ind.heartwood = tree->ind.old_heartwood;
       tree->ind.debt      = tree->ind.old_debt;

       //-------------------
       // seedling number
       //-------------------
       //eq: assumes new_nind is always greater than old nind
       nind_seedling = new_nind - tree->ind.old_nind;
       //if nind_seedling is negative, then don't update and send all seedlings to litter
       if(nind_seedling < 0){update_seedling = TRUE; nind_seedling = pft->nind - tree->ind.old_nind;}

       //eq: assumes that old seedling num. is greater than new seedling num.
       //  .. (seedling nind) - (new seedling nind);
       nind_seedling_2litter = (pft->nind - tree->ind.old_nind) - nind_seedling;

       //RESET DENSITY
       pft->nind  = tree->ind.old_nind;

       //if negative seedling establishment
       //..simply pass seedling establishment to litter
       //..do not update density and biomass pools
       if(update_seedling== TRUE){
         //---------------------------------------------
         //send excess seedlings to litter temp holder
         //..taken from litter_update_tree
         if(subannual){
           tmp_ag_tree +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling;
           tmp_bg      +=treepar->sapl.leaf*nind_seedling;
         }else{
           tmp_ag_tree_inc +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling;
           tmp_bg_inc      +=treepar->sapl.leaf*nind_seedling;
         }
         //end litter update
       }else{
         //update establishment flux
         update_acflux = TRUE;

         //update carbon pools with new establishment estimate
         if (pft->nind<0.0) pft->nind=0.0;
         nind_old=pft->nind;
         pft->nind+=nind_seedling;

         //Account for flux from the atmosphere to new saplings
         new_acflux_est+= phys_sum_tree(treepar->sapl)*nind_seedling;

         //Adjust average individual C biomass based on average biomass and density of the new saplings
         tree->ind.leaf=(tree->ind.leaf*nind_old+treepar->sapl.leaf*nind_seedling)/pft->nind;
         tree->ind.root=(tree->ind.root*nind_old+treepar->sapl.root*nind_seedling)/pft->nind;
         tree->ind.sapwood=(tree->ind.sapwood*nind_old+treepar->sapl.sapwood*nind_seedling)/pft->nind;
         tree->ind.heartwood=(tree->ind.heartwood*nind_old+treepar->sapl.heartwood*nind_seedling)/pft->nind;
         tree->ind.debt=tree->ind.debt*nind_old/pft->nind;

         //---------------------------------------------
         //send excess seedlings to litter temp holder
         //..taken from litter_update_tree
         if(subannual){
           tmp_ag_tree +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling_2litter;
           tmp_bg      +=treepar->sapl.leaf*nind_seedling_2litter;
         }else{
           tmp_ag_tree_inc +=(treepar->sapl.leaf+treepar->sapl.sapwood+treepar->sapl.heartwood)*nind_seedling_2litter;
           tmp_bg_inc      +=treepar->sapl.leaf*nind_seedling_2litter;
         }
         //end litter update
       }
       //update allometry and fpc
        allometry_tree(pft);
        pft->fpc=new_fpc;
     }//end is tree
     }//end pft loop
   }//end IF FPC_TOTAL > 1

   //update the establishment flux if different
   if(update_acflux==TRUE){ acflux_est += new_acflux_est;}else{acflux_est += orig_acflux_est;}

   //-----------------------------------------
   // adjust grasses so that fpc_total <= 1.0
   //-----------------------------------------
   if (fpc_total>1.0)
    foreachpft(pft,p,pftlist){
     if(!istree(pft)){
       pft->fpc/=fpc_total;
     }
   }

   //--------------------------------
   // update litter pools
   // ..sends excess seedlings to directly to litter
   // ..zeroes if seedlings not sent to litter
   //--------------------------------
   if(subannual){
     litter->ag_tree += tmp_ag_tree;
     litter->bg      += tmp_bg;
   }else{
     litter->ag_tree_inc += tmp_ag_tree_inc;
     litter->bg_inc      += tmp_bg_inc;
   }

  //return adjusted flux for new establishment
  //..litter is updated with biomass/nind of seedlings that couldn't establish
  //..mean individual tree size is maintained without dilution
  return(acflux_est);
}/*..end optimum establishment; static call from establishmentpft(), below */
#endif

Real establishmentpft(Litter *litter,
          Pftlist pftlist,
          const Pftpar *pftpar,
          int npft,
          int ntypes,
          const Real gdd[],
          const Climbuf *climbuf, 
          int pftid,
          Bool subannual)
{

  /*
   *  DESCRIPTION
   *
   *  Establishment in population (standard LPJ) mode.
   *  Simulates population increase through establishment each simulation 
   *  year for trees and grasses and introduces new PFTs if climate conditions
   *  become suitable.
   *  This function assumes each Individual object represents the average
   *  individual of a PFT population, and that there is (at most) one 
   *  individual object per PFT per modelled area (stand).
   *
   */

  Real acflux_est,dummy;
  Real fpc_total,*fpc_type;
  int *n_est; 
  Bool *present;
  int p,t;
  Pft *pft;

  present=newvec(Bool,npft);
  check(present);
  fpc_type=newvec(Real,ntypes);
  check(fpc_type);
  n_est=newvec(int,ntypes);
  check(n_est);
  for(p=0;p<npft;p++)
    present[p]=FALSE;
  foreachpft(pft,p,pftlist)
    present[pft->par->id]=TRUE;
  acflux_est=0.0;

  #ifdef DEBUG2
  printf("Number of pfts: %d\n",pftlist->n);
  for(p=0;p<npft;p++)
    printf("%s ",present[p] ? "true" : "false");
  printf("\n");
  #endif

  fpc_total=fpc_sum(fpc_type,ntypes,pftlist);
  for(t=0;t<ntypes;t++)
    n_est[t]=0;
  for(p=0;p<npft;p++)
    if(establish(gdd[p],pftpar+p,climbuf,pftid))
    {
      if(!present[p])
        addpft(pftlist,pftpar+p);
      n_est[pftpar[p].type]++;
    } 
  #ifdef DEBUG2
  printf("new npft=%d\n",pftlist->n);
  #endif

  #ifdef OPTIMUM_ESTAB
  //--------------------------------
  // Optimum Establishment
  //-------------------------------
  // ..LC solution to dilution of mean-individual
  // ..static call to function (top of page)
  // ..see details on approach in function
  acflux_est=optimum_establishment(present, fpc_type, n_est, litter,pftlist,pftpar,npft,ntypes,gdd,climbuf,pftid,subannual);
  #else
  //--------------------------------
  // Original Formulation
  //--------------------------------
  foreachpft(pft,p,pftlist)
    if(establish(gdd[pft->par->id],pftpar+pft->par->id,climbuf,pftid))
      acflux_est+=establishment(pft,fpc_total,fpc_type[pft->par->type],n_est[pft->par->type]);
  fpc_sum(fpc_type,ntypes,pftlist);
  foreachpft(pft,p,pftlist)
    if(istree(pft) && fpc_type[TREE]>FPC_TREE_MAX)
      adjust(litter,pft,fpc_type[pft->par->type],subannual);
  fpc_total=fpc_sum(fpc_type,ntypes,pftlist);
  if (fpc_total>1.0)
   foreachpft(pft,p,pftlist){
     if(istree(pft))
    {
     pft->nind/=fpc_total;
     litter_update(litter,pft,(pft->nind*fpc_total-pft->nind),&dummy,FALSE,subannual);
     pft->fpc/=fpc_total;
    }
    else
    pft->fpc/=fpc_total;
   }
  #endif //end of establishment 

  #ifdef DEBUG2
  printf("new npft=%d\n",pftlist->n);
  #endif
  free(present);
  free(fpc_type);
  free(n_est);
  return acflux_est;
} /* of 'establishmentpft' */
