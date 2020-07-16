/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ o u t p u t . c                                     **/
/*******************************************************************/
#include "lpj.h"
#include "math.h"

FILE **fopenoutput(Config config)
{
  int i;
  FILE **output;
  output=newvec(FILE *,config.n_out);
  check(output);
  for(i=0;i<config.n_out;i++)
   if((output[i]=fopen(config.out_filename[i],"wb"))==NULL)
     printfopenerr("fopenoutput",config.out_filename[i]);
  return output;
} /* of 'fopenoutput' */

void fcloseoutput(FILE **output,int n_out)
{
  int i;
  for(i=0;i<n_out;i++)
    if(output[i]!=NULL)
      fclose(output[i]);
  free(output);
} /* of 'fcloseoutput' */

/*
static int writevec(FILE *file,const Real data[],int n,Real frac)
{
  float *vec;
  int i,rc;
  if(file==NULL)
    return 0;
  vec=newvec(float,n);
  check(vec);
  for(i=0;i<n;i++)
    vec[i]=(float)(data[i]*frac);
  rc=fwrite(vec,sizeof(float)*n,1,file);
  free(vec);
  return rc;
}*/ /* of 'writevec' */

static int writeshort(FILE *file,Real data)
{
  short s;
  if(file==NULL)
   return 0;
  s=(short)(data);
  return fwrite(&s,sizeof(s),1,file);
} /* of 'writeshort' */

static int writefloat(FILE *file,Real data)
{
  float s;
  if(file==NULL)
   return 0;
  s=(float)(data);
  return fwrite(&s,sizeof(s),1,file);
} /* of 'writefloat' */

int fwriteoutput_annual(FILE **files,const Cell *cell,int n_out, Bool CSV_EXISTS)
{
  int count,s,p;
  Stand *stand;
  Pft *pft;

  //primary,secondary stand outputs
  Real soilc,litc,vegc,frac,soilc_primary,litc_primary,vegc_primary,soilc_secforest,litc_secforest,vegc_secforest;
  Real frac_primary,frac_secforest;

  #ifdef SINGLECELL_CSVOUTPUT
  Real csvfpcpft[NPFT] = {0};
  #endif

  #ifdef OUTPUT_BYAGECLASS
  Real ageclass_pri_pftfpc[(NPFT+1)*12] = {0}; //for pft_fpc outputs by ageclass (12), where first element is total
  Real ageclass_pri_frac[12] = {0};
  Real ageclass_pri_vegc[12] = {0};
  Real ageclass_sec_pftfpc[(NPFT+1)*12] = {0}; //for pft_fpc outputs by ageclass (12), where first element is total
  Real ageclass_sec_frac[12] = {0};
  Real ageclass_sec_vegc[12] = {0};
  #endif

  //initialize to zero
  vegc=litc=soilc=vegc_primary=litc_primary=soilc_primary=vegc_secforest=litc_secforest=soilc_secforest=0.0;
  frac_primary=frac_secforest=0;

  frac=1;
  count=0;
  foreachstand(stand,s,cell->standlist)
  { 
    if(stand->landusetype==PRIMARY)   frac_primary   += stand->frac; 
    if(stand->landusetype==SECFOREST) frac_secforest += stand->frac;

    foreachpft(pft,p,stand->pftlist){
      if(stand->landusetype==PRIMARY){
        vegc_primary+=vegc_sum(pft)*stand->frac;
      }else if(stand->landusetype==SECFOREST){
        vegc_secforest+=vegc_sum(pft)*stand->frac;
      }

      vegc+=vegc_sum(pft)*stand->frac;
    }
    #ifndef MONTHLYLITTER
      #ifndef DAILYLITTER
        //annual litter scheme
        if(stand->landusetype==PRIMARY){
          soilc_primary+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg)*stand->frac;
          litc_primary+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass)*stand->frac;
        }else if(stand->landusetype==SECFOREST){
          soilc_secforest+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg)*stand->frac;
          litc_secforest+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass)*stand->frac;
        }
        soilc+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg)*stand->frac;
        litc+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass)*stand->frac;
      #else
        if(stand->landusetype==PRIMARY){
          soilc_primary+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
          litc_primary+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
        }else if(stand->landusetype==SECFOREST){
          soilc_secforest+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
          litc_secforest+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
        }
            soilc+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
        litc+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
      #endif
    #else
      if(stand->landusetype==PRIMARY){
        soilc_primary+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
        litc_primary+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
      }else if(stand->landusetype==SECFOREST){
        soilc_secforest+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
        litc_secforest+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
      }
      soilc+=(stand->soil.cpool.slow+stand->soil.cpool.fast+stand->soil.litter.bg+stand->soil.litter.bg_inc)*stand->frac;
      litc+=(stand->soil.litter.ag_tree+stand->soil.litter.ag_grass+stand->soil.litter.ag_tree_inc+stand->soil.litter.ag_grass_inc)*stand->frac;
    #endif
  }//..end stand loop

  count+=writefloat(files[FIREC],cell->output.firec*frac);
  count+=writefloat(files[FIREF],cell->output.firef*frac);
  count+=writefloat(files[VEGC],vegc*frac);
  count+=writefloat(files[SOILC],soilc*frac);
  count+=writefloat(files[LITC],litc*frac);
  count+=writefloat(files[FLUX_ESTAB],cell->output.flux_estab*frac);
  count+=writefloat(files[HARVEST],cell->output.flux_harvest*frac);
  count+=writefloat(files[FLUX_LUC],cell->output.flux_luc*frac);    
   
  //OUTPUT_BYSTAND (default output)
  count+=writefloat(files[VEGC_PRIMARY],vegc_primary*frac);
  count+=writefloat(files[SOILC_PRIMARY],soilc_primary*frac);
  count+=writefloat(files[LITC_PRIMARY],litc_primary*frac);
  count+=writefloat(files[FLUX_LUC_PRIMARY],cell->output.flux_luc_primary*frac);    

  count+=writefloat(files[VEGC_SECFOREST],vegc_secforest*frac);
  count+=writefloat(files[SOILC_SECFOREST],soilc_secforest*frac);
  count+=writefloat(files[LITC_SECFOREST],litc_secforest*frac);
  count+=writefloat(files[FLUX_LUC_SECFOREST],cell->output.flux_luc_secforest*frac); 

  #ifndef NOWDHARVEST   
  //outputs for LUC, wood harvest products (emitted and remaining), and stand-level metrics
  count+=writefloat(files[WOODHARVEST_100YR],cell->output.woodharvest_100yr*frac); 
  count+=writefloat(files[WOODHARVEST_10YR],cell->output.woodharvest_10yr*frac); 
  count+=writefloat(files[WOODHARVEST_1YR],cell->output.woodharvest_1yr*frac); 
  count+=writefloat(files[WOODHARVEST_100YR_REMAIN],cell->output.woodharvest_100yr_remain*frac); 
  count+=writefloat(files[WOODHARVEST_10YR_REMAIN],cell->output.woodharvest_10yr_remain*frac); 
  count+=writefloat(files[WOODHARVEST_1YR_REMAIN],cell->output.woodharvest_1yr_remain*frac); 
  #endif

  #ifdef DEFORESTPRODUCT
  //delayed deforestation emissions, current and remaining
  count+=writefloat(files[DEFORESTPRODUCT_25YR],cell->output.deforestProduct_25yr*frac); 
  count+=writefloat(files[DEFORESTPRODUCT_2YR],cell->output.deforestProduct_2yr*frac); 
  count+=writefloat(files[DEFORESTPRODUCT_25YR_REMAIN],cell->output.deforestProduct_25yr_remain*frac); 
  count+=writefloat(files[DEFORESTPRODUCT_2YR_REMAIN],cell->output.deforestProduct_2yr_remain*frac); 
  #endif

  #ifdef OUTPUT_BYSTAND
    //.....................................................
    //additional outputs by stand (primary,secondary)
    //..NBP by natural stand require these outputs:
    //        fluxes: flux_estab,firec,
    //          woodharvest,deforestProducts
    //..standard output by stand already includes:
    //  carbon pools: vegc,litc,soilc
    //        fluxes: mnpp,mgpp,mrh,flux_luc
    //.....................////////........................
    count+=writefloat(files[FRAC_PRIMARY],frac_primary*frac);
    count+=writefloat(files[FLUX_ESTAB_PRIMARY],cell->output.flux_estab_primary*frac);
    count+=writefloat(files[FIREC_PRIMARY],cell->output.firec_primary*frac);
    count+=writefloat(files[FRAC_SECFOREST],frac_secforest*frac);
    count+=writefloat(files[FLUX_ESTAB_SECFOREST],cell->output.flux_estab_secforest*frac);
    count+=writefloat(files[FIREC_SECFOREST],cell->output.firec_secforest*frac);
  
    #ifndef NOWDHARVEST
      count+=writefloat(files[WOODHARVEST_100YR_PRIMARY],cell->output.woodharvest_100yr_primary*frac);
      count+=writefloat(files[WOODHARVEST_10YR_PRIMARY],cell->output.woodharvest_10yr_primary*frac);
      count+=writefloat(files[WOODHARVEST_1YR_PRIMARY],cell->output.woodharvest_1yr_primary*frac);  
      count+=writefloat(files[WOODHARVEST_100YR_REMAIN_PRIMARY],cell->output.woodharvest_100yr_remain_primary*frac);
      count+=writefloat(files[WOODHARVEST_10YR_REMAIN_PRIMARY],cell->output.woodharvest_10yr_remain_primary*frac);
      count+=writefloat(files[WOODHARVEST_1YR_REMAIN_PRIMARY],cell->output.woodharvest_1yr_remain_primary*frac); 
  
       count+=writefloat(files[WOODHARVEST_100YR_SECFOREST],cell->output.woodharvest_100yr_secforest*frac);
      count+=writefloat(files[WOODHARVEST_10YR_SECFOREST],cell->output.woodharvest_10yr_secforest*frac);
      count+=writefloat(files[WOODHARVEST_1YR_SECFOREST],cell->output.woodharvest_1yr_secforest*frac);  
      count+=writefloat(files[WOODHARVEST_100YR_REMAIN_SECFOREST],cell->output.woodharvest_100yr_remain_secforest*frac);
      count+=writefloat(files[WOODHARVEST_10YR_REMAIN_SECFOREST],cell->output.woodharvest_10yr_remain_secforest*frac);
      count+=writefloat(files[WOODHARVEST_1YR_REMAIN_SECFOREST],cell->output.woodharvest_1yr_remain_secforest*frac);   
    #endif
  
    #ifdef DEFORESTPRODUCT
      count+=writefloat(files[DEFORESTPRODUCT_25YR_PRIMARY],cell->output.deforestProduct_25yr_primary*frac);
      count+=writefloat(files[DEFORESTPRODUCT_2YR_PRIMARY],cell->output.deforestProduct_2yr_primary*frac);
      count+=writefloat(files[DEFORESTPRODUCT_25YR_REMAIN_PRIMARY],cell->output.deforestProduct_25yr_remain_primary*frac);
      count+=writefloat(files[DEFORESTPRODUCT_2YR_REMAIN_PRIMARY],cell->output.deforestProduct_2yr_remain_primary*frac);
  
      count+=writefloat(files[DEFORESTPRODUCT_25YR_SECFOREST],cell->output.deforestProduct_25yr_secforest*frac);
      count+=writefloat(files[DEFORESTPRODUCT_2YR_SECFOREST],cell->output.deforestProduct_2yr_secforest*frac);
      count+=writefloat(files[DEFORESTPRODUCT_25YR_REMAIN_SECFOREST],cell->output.deforestProduct_25yr_remain_secforest*frac);
      count+=writefloat(files[DEFORESTPRODUCT_2YR_REMAIN_SECFOREST],cell->output.deforestProduct_2yr_remain_secforest*frac);
    #endif
  
  #endif  //.. end of OUTPUT_BYSTAND


  #ifdef OUTPUT_BYAGECLASS
    foreachstand(stand,s,cell->standlist){
      if(stand->landusetype==PRIMARY){
        foreachpft(pft,p,stand->pftlist){
          ageclass_pri_pftfpc[(NPFT+1)*(stand->ageclass_standid-1)]+=pft->fpc;                    //total pft fpc  
          ageclass_pri_pftfpc[(pft->par->id+1)+((NPFT+1)*(stand->ageclass_standid-1))]=pft->fpc;  //pft fpc    
          ageclass_pri_vegc[stand->ageclass_standid-1]+=vegc_sum(pft)*stand->frac;
        }
        //update ageclass stand frac
        ageclass_pri_frac[stand->ageclass_standid-1]=stand->frac;
      }else if(stand->landusetype==SECFOREST){
        foreachpft(pft,p,stand->pftlist){
                ageclass_sec_pftfpc[(NPFT+1)*(stand->ageclass_standid-1)]+=pft->fpc;                    //total pft fpc  
                ageclass_sec_pftfpc[(pft->par->id+1)+((NPFT+1)*(stand->ageclass_standid-1))]=pft->fpc;  //pft fpc    
                ageclass_sec_vegc[stand->ageclass_standid-1]+=vegc_sum(pft)*stand->frac;
        }
        //update ageclass stand frac
        ageclass_sec_frac[stand->ageclass_standid-1]=stand->frac;
      }//..end secforest
    }//..end for each stand
  
    //now write ageclass outputs to file..
    for(s=0; s<12; ++s){//for each ageclass (12)
      for(p=0; p<NPFT+1;++p){
        count+=writefloat(files[AGECLASS_PRI_PFTFPC], ageclass_pri_pftfpc[p+((NPFT+1)*s)]);
        count+=writefloat(files[AGECLASS_SEC_PFTFPC], ageclass_sec_pftfpc[p+((NPFT+1)*s)]);
      }
      count+=writefloat(files[AGECLASS_PRI_VEGC], ageclass_pri_vegc[s]*frac);
      count+=writefloat(files[AGECLASS_PRI_FRAC], ageclass_pri_frac[s]*frac);
      count+=writefloat(files[AGECLASS_PRI_FIREC], cell->output.ageclass_pri_firec[s]*frac);
      count+=writefloat(files[AGECLASS_PRI_FLUXESTAB], cell->output.ageclass_pri_fluxestab[s]*frac);
      
      count+=writefloat(files[AGECLASS_SEC_VEGC], ageclass_sec_vegc[s]*frac);
      count+=writefloat(files[AGECLASS_SEC_FRAC], ageclass_sec_frac[s]*frac);
      count+=writefloat(files[AGECLASS_SEC_FIREC], cell->output.ageclass_sec_firec[s]*frac);
      count+=writefloat(files[AGECLASS_SEC_FLUXESTAB], cell->output.ageclass_sec_fluxestab[s]*frac);
    }
  #endif

  #ifdef SINGLECELL_CSVOUTPUT
    if(!CSV_EXISTS){
      //create text file
      FILE *f = fopen("singlecell_csvoutput_annual.txt", "w");
  
      //write the header, then write the data
      //DEVQ: add variables for climate MAP,MAT and MAP_STD,MAT_STD
      fprintf(f, "firec,fire_frac,flux_luc,flux_estab,harvest,vegc,litc,soilc,primary_vegc,primary_litc,primary_soilc,primary_flux_luc,secforest_vegc,secforest_litc,secforest_soilc,secforest_flux_luc\n");
      fprintf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",cell->output.firec,cell->output.firef,cell->output.flux_luc,cell->output.flux_estab,cell->output.flux_harvest,vegc,litc,soilc,vegc_primary,litc_primary,soilc_primary,cell->output.flux_luc_primary,vegc_secforest,litc_secforest,soilc_secforest,cell->output.flux_luc_secforest);
      fclose(f);
    }else{
      //append text file
      FILE *f = fopen("singlecell_csvoutput_annual.txt", "a");
  
      //write the data
      fprintf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",cell->output.firec,cell->output.firef,cell->output.flux_luc,cell->output.flux_estab,cell->output.flux_harvest,vegc,litc,soilc,vegc_primary,litc_primary,soilc_primary,cell->output.flux_luc_primary,vegc_secforest,litc_secforest,soilc_secforest,cell->output.flux_luc_secforest);
      fclose(f);
    }
  
    //---------------------------------
    // fpc by pft for single cell
    //---------------------------------
    //first update data
    foreachpft(pft,p,stand->pftlist){
      //update fpc if pft exists
      csvfpcpft[pft->par->id]= pft->fpc;
    }
  
    if(!CSV_EXISTS){
      //create text file
      FILE *f = fopen("singlecell_csvfpcpft_annual.txt", "w");
  
      //write header, then data
      for(p=1; p < NPFT; p++){
        fprintf(f,"pft%d,",p);
      }
      //add end-line to header
      fprintf(f,"pft%d\n",NPFT);
  
      //now write data
      for(p=0; p < NPFT-1; p++){
        fprintf(f,"%f,",csvfpcpft[p]);
      }
      //add end-line w last pft data
      fprintf(f,"%f\n",csvfpcpft[NPFT-1]);
      fclose(f);
    }else{
    //append data to file
    FILE *f = fopen("singlecell_csvfpcpft_annual.txt", "a");
      //write data only
      for(p=0; p < NPFT-1; p++){
        fprintf(f,"%f,",csvfpcpft[p]);
      }
      //add end-line w last pft data
      fprintf(f,"%f\n",csvfpcpft[NPFT-1]);
      fclose(f);
    }
  #endif //..end SINGLECELL_CSVOUTPUT

  return count;
} /* of 'fwriteoutput_annual' */

int fwriteoutput_pft(FILE **files,Cell *grid,Config config,int npft,int ncft,int year) 
{
  int count,i,cell,b,p;
  count=0;
 
  for(i=0;i<ncft*2;i++)
    for(cell=0;cell<config.ngridcell;cell++)
      if(!(grid+cell)->skip)
        count+=writeshort(files[SDATE],(Real)(grid+cell)->output.sdate[i]);

  for(i=0;i<npft+(ncft+NGRASS)*2;i++)
    for(cell=0;cell<config.ngridcell;cell++)
      if(!(grid+cell)->skip){
        count+=writefloat(files[PFT_NPP],(grid+cell)->output.pft_npp[i]);
        count+=writefloat(files[PFT_TRANSP],(grid+cell)->output.pft_transp[i]);
        count+=writefloat(files[PFT_GPP],(grid+cell)->output.pft_gpp[i]);
        count+=writefloat(files[PFT_GC],(grid+cell)->output.pft_gc[i]);
        count+=writefloat(files[PFT_FO3UPTAKE],(grid+cell)->output.pft_fo3uptake[i]);
        count+=writefloat(files[PFT_LAI],(grid+cell)->output.pft_lai[i]);

        if((grid+cell)->output.gcgp_count[i] > 0)
          (grid+cell)->output.pft_gcgp[i]/=(grid+cell)->output.gcgp_count[i];
        else
          (grid+cell)->output.pft_gcgp[i]=-9;

        count+=writefloat(files[PFT_GCGP],(grid+cell)->output.pft_gcgp[i]);
    }
  for(i=0;i<(ncft+NGRASS)*2;i++)
    for(cell=0;cell<config.ngridcell;cell++)
      if(!(grid+cell)->skip){
        count+=writefloat(files[PFT_HARVEST],(grid+cell)->output.pft_harvest[i]);
        count+=writefloat(files[PFT_RHARVEST],(grid+cell)->output.pft_rharvest[i]);
      }

  for(i=0; i<npft+1;++i)
    for(cell=0;cell<config.ngridcell;cell++)
      if(!(grid+cell)->skip){
        #ifdef OUTPUT_BYSTAND
          count+=writefloat(files[FPC_PRIMARY],   (grid+cell)->output.fpc_primary[i]);   //corrected for stand area in update_annual.c
          count+=writefloat(files[FPC_SECFOREST], (grid+cell)->output.fpc_secforest[i]);
        #endif
        count+=writefloat(files[FPC], (grid+cell)->output.fpc[i]);
      }

  for(i=0; i<npft;++i)
    for(cell=0;cell<config.ngridcell;cell++)
      if(!(grid+cell)->skip){
        count+=writefloat(files[PFT_VEGC], (grid+cell)->output.pft_vegc[i]);
        count+=writefloat(files[PFT_NIND], (grid+cell)->output.pft_nind[i]);
        count+=writefloat(files[PFT_MORT], (grid+cell)->output.pft_mort[i]);
        count+=writefloat(files[PFT_MAXPHENDAY], (grid+cell)->output.pft_maxphenday[i]);
      }

  if(year > 1971){
  for(cell=0;cell<config.ngridcell;cell++){
      if(!(grid+cell)->skip){
      for(p=0;p<npft;++p){
          for(b=0;b<53;++b){  //Write 7 days
            count+=writefloat(files[PFT_BIMONFPAR], (grid+cell)->output.pft_bimonfpar[p+(b*npft)]);
          }
        }
      }
    }
  }

  for(cell=0;cell<config.ngridcell;cell++){
    if(!(grid+cell)->skip){
      for(p=0;p<npft;++p){
        for(b=0;b<12;++b){  //Write monthly
           count+=writefloat(files[MPFT_LAI], (grid+cell)->output.mpft_lai[p+(b*npft)]);
           count+=writefloat(files[MPFT_GC], (grid+cell)->output.mpft_gc[p+(b*npft)]);
           count+=writefloat(files[MPFT_CI], (grid+cell)->output.mpft_ci[p+(b*npft)]);
           count+=writefloat(files[MPFT_TRANSP], (grid+cell)->output.mpft_transp[p+(b*npft)]);
           count+=writefloat(files[MPFT_GPP], (grid+cell)->output.mpft_gpp[p+(b*npft)]);
        }
      }
    }
  }

  #ifdef OUTPUT_AGRICULTURE_MPFT
  //monthly AGRICULTURE outputs
  for(cell=0;cell<config.ngridcell;cell++){
    if(!(grid+cell)->skip){
      for(p=0;p<(ncft+NGRASS)*2;++p){ //..each pft
        for(b=0;b<12;++b){  //Write monthly
           count+=writefloat(files[MPFT_AG_NPP], (grid+cell)->output.mpft_ag_npp[p+(b*((ncft+NGRASS)*2))]);
           count+=writefloat(files[MPFT_AG_GPP], (grid+cell)->output.mpft_ag_gpp[p+(b*((ncft+NGRASS)*2))]);
           count+=writefloat(files[MPFT_AG_LAI], (grid+cell)->output.mpft_ag_lai[p+(b*((ncft+NGRASS)*2))]);
           count+=writefloat(files[MPFT_AG_TRANSP], (grid+cell)->output.mpft_ag_transp[p+(b*((ncft+NGRASS)*2))]);
        }
      }
    }
  }
  #endif
  return count;
}

int fwriteoutput_monthly(FILE **files,const Cell *cell, int npft, int year, Bool CSV_EXISTS) 
{
  
  int count; 
  Real frac;

  #ifdef OUTPUT_BYAGECLASS
  int s;
  Stand *stand;
  Real ageclass_pri_mnpp[12]={0};
  Real ageclass_pri_mrh[12]={0};
  Real ageclass_sec_mnpp[12]={0};
  Real ageclass_sec_mrh[12]={0};
  #endif

  frac=1;
  count=0;
 
  count+=writefloat(files[MNPP],cell->output.mnpp*frac);
  count+=writefloat(files[MGPP],cell->output.mgpp*frac); 
  count+=writefloat(files[MSOILTEMP],cell->output.msoiltemp); 
  count+=writefloat(files[MPET],cell->output.mpet); 
  count+=writefloat(files[MPAR],cell->output.mpar*frac);
  count+=writefloat(files[MRA],cell->output.mra*frac); 
  count+=writefloat(files[MSNOWPACK],cell->output.msnowpack); 
  count+=writefloat(files[MRH],cell->output.mrh*frac);
  count+=writefloat(files[MRUNOFF],cell->output.mrunoff*frac);
  count+=writefloat(files[MDISCHARGE],cell->output.mdischarge*frac); /* 1.000.000 md mean monthly discharge */
  count+=writefloat(files[MTRANSP],cell->output.mtransp*frac);
  count+=writefloat(files[MEVAP],cell->output.mevap*frac);
  count+=writefloat(files[MINTERC],cell->output.minterc*frac);
  count+=writefloat(files[MSWC1],cell->output.mswc[0]*frac); 
  count+=writefloat(files[MSWC2],cell->output.mswc[1]*frac);
  count+=writefloat(files[MIRRIG_WD],cell->output.mirrig_wd*frac);

  //TOPMODEL outputs
  count+=writefloat(files[WTD],cell->output.wtd);
  count+=writefloat(files[WETFRAC],cell->output.wetfrac);
  count+=writefloat(files[MCH4E],cell->output.mch4e);
  count+=writefloat(files[CH4O],cell->output.ch4o);

  //PERMAFROST outputs
  count+=writefloat(files[MTSOIL0],cell->output.mtsoil_0);
  count+=writefloat(files[MTSOIL10],cell->output.mtsoil_10);
  count+=writefloat(files[MTSOIL25],cell->output.mtsoil_25);
  count+=writefloat(files[MTSOIL50],cell->output.mtsoil_50);
  count+=writefloat(files[MTSOIL70],cell->output.mtsoil_70);
  count+=writefloat(files[MTSOIL100],cell->output.mtsoil_100);
  count+=writefloat(files[MTSOIL150],cell->output.mtsoil_150);
  count+=writefloat(files[MTSOIL200],cell->output.mtsoil_200);
  count+=writefloat(files[MTSOILTOP],cell->output.mtsoil_top);
  count+=writefloat(files[MTHAW_DEPTH],cell->output.mthaw_depth);
  count+=writefloat(files[MFWATER_10],cell->output.mFwater_10);
  count+=writefloat(files[MFWATER_20],cell->output.mFwater_20);
  count+=writefloat(files[MFWATER_30],cell->output.mFwater_30);
  count+=writefloat(files[MFWATER_50],cell->output.mFwater_50);
  count+=writefloat(files[MFWATER_70],cell->output.mFwater_70);
  count+=writefloat(files[MFWATER_100],cell->output.mFwater_100);
  count+=writefloat(files[MFWATER_150],cell->output.mFwater_150);
  count+=writefloat(files[MFWATER_200],cell->output.mFwater_200);
  count+=writefloat(files[MFICE_10],cell->output.mFice_10);
  count+=writefloat(files[MFICE_20],cell->output.mFice_20);
  count+=writefloat(files[MFICE_30],cell->output.mFice_30);
  count+=writefloat(files[MFICE_50],cell->output.mFice_50);
  count+=writefloat(files[MFICE_70],cell->output.mFice_70);
  count+=writefloat(files[MFICE_100],cell->output.mFice_100);
  count+=writefloat(files[MFICE_150],cell->output.mFice_150);
  count+=writefloat(files[MFICE_200],cell->output.mFice_200);
  count+=writefloat(files[FROZEN_DAYS],cell->output.frozen_days);

  //BVOC outputs
  count+=writefloat(files[MISOPR],cell->output.misopr);
  count+=writefloat(files[MMONOPR],cell->output.mmonopr);


  //climate variables
  count+=writefloat(files[MTAIR],cell->output.mtair);
  count+=writefloat(files[MPPT],cell->output.mppt);
  count+=writefloat(files[MWETDAYS],cell->output.mwetdays);
  count+=writefloat(files[MSWDOWN],cell->output.mswdown);
  count+=writefloat(files[MLWDOWN],cell->output.mlwdown);
  count+=writefloat(files[MCLD],cell->output.mcld);
  count+=writefloat(files[MCO2CONC],cell->output.mco2conc);

  #ifdef OUTPUT_BYSTAND 
  count+=writefloat(files[MNPP_PRIMARY],cell->output.mnpp_primary*frac); 
  count+=writefloat(files[MRH_PRIMARY],cell->output.mrh_primary*frac); 
  count+=writefloat(files[MGPP_PRIMARY],cell->output.mgpp_primary*frac); 
  count+=writefloat(files[MNPP_SECFOREST],cell->output.mnpp_secforest*frac); 
  count+=writefloat(files[MRH_SECFOREST],cell->output.mrh_secforest*frac); 
  count+=writefloat(files[MGPP_SECFOREST],cell->output.mgpp_secforest*frac);
  #endif

  #ifdef OUTPUT_BYAGECLASS
  // write ageclass outputs to file..
  for(s=0; s<12; ++s){//for each ageclass (12)
    count+=writefloat(files[AGECLASS_PRI_MNPP], cell->output.ageclass_pri_mnpp[s]);
    count+=writefloat(files[AGECLASS_PRI_MRH], cell->output.ageclass_pri_mrh[s]);
    count+=writefloat(files[AGECLASS_SEC_MNPP], cell->output.ageclass_sec_mnpp[s]);
    count+=writefloat(files[AGECLASS_SEC_MRH], cell->output.ageclass_sec_mrh[s]);
  }
  #endif  //..end of outputs_by ageclass


  #ifdef SINGLECELL_CSVOUTPUT
    if(!CSV_EXISTS){
      //create text file
      FILE *f = fopen("singlecell_csvoutput_monthly.txt", "w");

      //write the header, then write the data
      //DEVQ: add variables for climate MAP,MAT and MAP_STD,MAT_STD
      fprintf(f, "gpp,npp,rh,ra,gpp_primary,npp_primary,rh_primary,gpp_secforest,npp_secforest,rh_secforest\n");
      fprintf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",cell->output.mgpp,cell->output.mnpp,cell->output.mrh,cell->output.mra,cell->output.mgpp_primary,cell->output.mnpp_primary,cell->output.mrh_primary,cell->output.mgpp_secforest,cell->output.mnpp_secforest,cell->output.mrh_secforest);
      fclose(f);
    }else{
      //append text file
      FILE *f = fopen("singlecell_csvoutput_monthly.txt", "a");

      //write the data
      fprintf(f, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",cell->output.mgpp,cell->output.mnpp,cell->output.mrh,cell->output.mra,cell->output.mgpp_primary,cell->output.mnpp_primary,cell->output.mrh_primary,cell->output.mgpp_secforest,cell->output.mnpp_secforest,cell->output.mrh_secforest);
      fclose(f);
    }
  #endif
 
  return count;
} /* of 'fwriteoutput_monthly' */

void initoutput_annual(Output *output, int npft, int ncft)
{
  int i,b;
  output->firec=output->firef=output->flux_harvest=output->flux_estab=output->flux_luc=0;
  output->aevap_lake=0.0;
  //landusechange outputs
  output->woodharvest_100yr=output->woodharvest_10yr=output->woodharvest_1yr=0.0;
  output->woodharvest_100yr_remain=output->woodharvest_10yr_remain=output->woodharvest_1yr_remain=0.0;
  output->flux_luc_primary=output->flux_luc_secforest=0.0;
  output->deforestProduct_25yr=output->deforestProduct_2yr=0.0;
  output->deforestProduct_25yr_remain=output->deforestProduct_2yr_remain=0.0;

  #ifdef OUTPUT_BYSTAND
  output->flux_estab_primary=output->firec_primary=output->woodharvest_100yr_primary=output->woodharvest_10yr_primary=output->woodharvest_1yr_primary=output->woodharvest_100yr_remain_primary=output->woodharvest_10yr_remain_primary=output->woodharvest_1yr_remain_primary=output->deforestProduct_25yr_primary=output->deforestProduct_2yr_primary=output->deforestProduct_25yr_remain_primary=output->deforestProduct_2yr_remain_primary=0.0;

  output->flux_estab_secforest=output->firec_secforest=output->woodharvest_100yr_secforest=output->woodharvest_10yr_secforest=output->woodharvest_1yr_secforest=output->woodharvest_100yr_remain_secforest=output->woodharvest_10yr_remain_secforest=output->woodharvest_1yr_remain_secforest=output->deforestProduct_25yr_secforest=output->deforestProduct_2yr_secforest=output->deforestProduct_25yr_remain_secforest=output->deforestProduct_2yr_remain_secforest=0.0;
  #endif

  #ifdef OUTPUT_BYAGECLASS
  for(i=0;i<12;i++){//for each ageclass
    output->ageclass_pri_firec[i]=0.0;
    output->ageclass_pri_fluxestab[i]=0.0;
    output->ageclass_sec_firec[i]=0.0;
    output->ageclass_sec_fluxestab[i]=0.0;
  }
  #endif
 
  /* memory allocation now in newgrid.c */
  for(i=0;i<(ncft+NGRASS)*2;i++){
    output->pft_harvest[i]=0;
    output->pft_rharvest[i]=0;
  }
  for(i=0;i<(ncft*2);i++)
    output->sdate[i]=0;
  for(i=0;i<(ncft+NGRASS)*2+npft;i++){
    output->pft_transp[i]=0;
    output->pft_gpp[i]=0;
    output->pft_gc[i]=0;
    output->pft_lai[i]=0;
    output->pft_npp[i]=0; 
    output->pft_fo3uptake[i]=0; 
    output->pft_gcgp[i]=0;
    output->gcgp_count[i]=0;
  }
  for (i=0; i<npft+1;++i)
    output->fpc[i] = 0;
  for (i=0; i<npft;++i){
    output->pft_vegc[i] = 0;
    output->pft_nind[i] = 0;
    output->pft_mort[i] = 0;
    output->pft_maxphenday[i] = 0;
  }
  
  for(b=0;b<53;++b){  //Write 7 days
    for(i=0;i<npft;++i){
      output->pft_bimonfpar[i+(b*npft)]=0;
    }
  }

  //pft monthly outputs
  for(b=0;b<12;++b){  //Write monthly days
    for(i=0;i<npft;++i){
      output->mpft_lai[i+(b*npft)]=0;
      output->mpft_gc[i+(b*npft)]=0;
      output->mpft_ci[i+(b*npft)]=0;
      output->mpft_transp[i+(b*npft)]=0;
      output->mpft_gpp[i+(b*npft)]=0;
    }
  }

  #ifdef OUTPUT_AGRICULTURE_MPFT
  //crop pfts only..monthly outputs
  for(b=0;b<12;++b){
    for(i=0;i<(ncft+NGRASS)*2;++i){
      output->mpft_ag_npp[i+(b*(ncft+NGRASS)*2)]=0;
      output->mpft_ag_gpp[i+(b*(ncft+NGRASS)*2)]=0;
      output->mpft_ag_lai[i+(b*(ncft+NGRASS)*2)]=0;
      output->mpft_ag_transp[i+(b*(ncft+NGRASS)*2)]=0;
    }
  }
  #endif
  
} /* of 'initoutput_annual' */

void freeoutput(Output *output)
{
  free(output->sdate);
  free(output->pft_harvest);
  free(output->pft_rharvest);
  free(output->pft_transp); 
  free(output->pft_gpp); 
  free(output->pft_gc); 
  free(output->pft_lai); 
  free(output->pft_npp);
  free(output->pft_fo3uptake);  
  free(output->pft_gcgp);
  free(output->gcgp_count);
  free(output->pft_vegc); 
  free(output->pft_nind); 
  free(output->pft_mort); 
  free(output->pft_maxphenday); 
  free(output->pft_bimonfpar); 
  free(output->mpft_lai); 
  free(output->mpft_gc); 
  free(output->mpft_ci); 
  free(output->mpft_transp); 
  free(output->mpft_gpp); 
  free(output->fpc);
  output->sdate=NULL;
  output->pft_harvest=output->pft_rharvest=output->pft_npp=output->pft_transp=output->pft_gpp=output->pft_gc=output->pft_fo3uptake=output->fpc=output->pft_lai=output->pft_vegc=output->pft_nind=output->pft_mort=output->pft_maxphenday=output->pft_bimonfpar=output->mpft_lai=output->mpft_gc=output->mpft_ci=output->mpft_gpp=output->mpft_transp=NULL;
} /* of 'freeoutput' */
         
void initoutput_monthly(Output *output, int npft) 
{
  int l;
    
  output->mnpp=output->mrh=output->mtransp=output->mrunoff=output->mra=output->mpet=output->mpar=output->mdischarge=output->mevap=output->minterc=output->mirrig_wd=output->mirrig=output->mgpp=output->msoiltemp=output->msnowpack=0.0;
  //TOPMODEL
  output->wtd=output->wetfrac=output->mch4e=output->ch4o=0.0;
  //PERMAFROST
  output->frozen_days=output->mtsoil_0=output->mtsoil_10=output->mtsoil_25=output->mtsoil_50=output->mtsoil_70=output->mtsoil_100=output->mtsoil_150=output->mtsoil_200=output->mtsoil_top=output->mFwater_10=output->mFwater_20=output->mFwater_30=output->mFwater_50=output->mFwater_70=output->mFwater_100=output->mFwater_150=output->mFwater_200=output->mFice_10=output->mFice_20=output->mFice_30=output->mFice_50=output->mFice_70=output->mFice_100=output->mFice_150=output->mFice_200=0.0;
  //BVOC
  output->misopr=output->mmonopr=0.0;;
  //stand-level outputs, primary & secondary forests; get mgLand by difference from cell-level
  output->mnpp_primary=output->mrh_primary=output->mgpp_primary=output->mnpp_secforest=output->mrh_secforest=output->mgpp_secforest=0.0;
  //climate variables
  output->mtair=output->mppt=output->mwetdays=output->mswdown=output->mlwdown=output->mcld=output->mco2conc=0.0;

  for(l=0;l<NSOILLAYER;l++)
    output->mswc[l]=0;

  #ifdef OUTPUT_BYAGECLASS
  for(l=0; l<12; ++l){
    output->ageclass_pri_mnpp[l]=0.0;
    output->ageclass_pri_mrh[l]=0.0;
    output->ageclass_sec_mnpp[l]=0.0;
    output->ageclass_sec_mrh[l]=0.0;
  }
  #endif
} /* of 'initoutput_monthly' */

int fwriteoutput_daily(FILE **files,const Cell *cell, int day,int month, int year)
{
  
  int count;
  //Real frac;
  //frac=1;
  count=0;
 
  //Carbon outputs
  count+=writefloat(files[DGPP],cell->output.dgpp);
  count+=writefloat(files[DNPP],cell->output.dnpp);
  count+=writefloat(files[DRH],cell->output.drh);
  count+=writefloat(files[DWET_FRAC],cell->output.dwet_frac);
  count+=writefloat(files[DCH4E],cell->output.dch4e);
  
  //TOPMODEL related outputs
  count+=writefloat(files[DFWATER_10],cell->output.dFwater_10);
  count+=writefloat(files[DFWATER_20],cell->output.dFwater_20);
  count+=writefloat(files[DFWATER_30],cell->output.dFwater_30);
  count+=writefloat(files[DFWATER_50],cell->output.dFwater_50);
  count+=writefloat(files[DFWATER_70],cell->output.dFwater_70);
  count+=writefloat(files[DFWATER_100],cell->output.dFwater_100);
  count+=writefloat(files[DFWATER_150],cell->output.dFwater_150);
  count+=writefloat(files[DFWATER_200],cell->output.dFwater_200);
  count+=writefloat(files[DFICE_10],cell->output.dFice_10);
  count+=writefloat(files[DFICE_20],cell->output.dFice_20);
  count+=writefloat(files[DFICE_30],cell->output.dFice_30);
  count+=writefloat(files[DFICE_50],cell->output.dFice_50);
  count+=writefloat(files[DFICE_70],cell->output.dFice_70);
  count+=writefloat(files[DFICE_100],cell->output.dFice_100);
  count+=writefloat(files[DFICE_150],cell->output.dFice_150);
  count+=writefloat(files[DFICE_200],cell->output.dFice_200);
  
  //Climate variables
  count+=writefloat(files[DTAIR],cell->output.dtair);
  count+=writefloat(files[DPPT],cell->output.dppt);
  count+=writefloat(files[DSWDOWN],cell->output.dswdown);
  count+=writefloat(files[DLWDOWN],cell->output.dlwdown);
  count+=writefloat(files[DCLD],cell->output.dcld);


  return count;
} /* of 'fwriteoutput_daily' */

void initoutput_daily(Output *output, int day) 
{
  //Carbon variables
  output->dwet_frac=output->dch4e=output->dgpp=output->dnpp=output->drh=0.0;

  //TOPMODEL
  output->dFwater_10=output->dFwater_20=output->dFwater_30=output->dFwater_50=output->dFwater_70=output->dFwater_100=output->dFwater_150=output->dFwater_200=output->dFice_10=output->dFice_20=output->dFice_30=output->dFice_50=output->dFice_70=output->dFice_100=output->dFice_150=output->dFice_200=0.0;

  //Climate variables
  output->dtair=output->dppt=output->dswdown=output->dlwdown=output->dcld=0.0;


} /* of 'initoutput_daily' */

/*
- order of PFT-specific output depends on number (X) PFTs used in simulation

0-X     (X+1) to (X+1+19)   ((X+1+19) + 1)   (((X+1+19)+1)+1) to (((((X+1+19)+1)+1))+ 10)     (((((X+1+19)+1)+1))+ 10) + 1 
PFT     CFT                 PASTURE          CFT_irr                                         PASTURE_irr
 
*/      
