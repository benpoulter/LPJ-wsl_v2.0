/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ f s c a n p f t p a r . c                          **/
/*******************************************************************/

#include "lpj.h"

#define fscanreal2(file,var,fcn,pft,name) \
  if(fscanreal(file,var)) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    break; \
  }
#define fscanint(file,var,fcn,pft,name) \
  if(fscanf(file,"%d",var)!=1) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    break; \
  }
#define fscanlimit2(file,var,fcn,pft,name) \
  if(fscanlimit(file,var)) \
  { \
    fprintf(stderr,"Error reading '%s' of PFT '%s' in '%s'.\n",name,pft,fcn); \
    break; \
  }

static Bool fscanlimit(FILE *file,Limit *limit)
{
  double low,high;
  if(fscanf(file,"%lg %lg",&low,&high)!=2)
    return TRUE;
  limit->low=low;
  limit->high=high;
  return FALSE;
} /* of 'fscanlimit' */

static Bool fscanreal_forkel(FILE *file, Pftpar *pft)
{
  //--------------------------------------------------
  //forkel phenology parameters 
  //  ..only available for pfts that are NOT CROPS
  //  ..slope, base, and tau parameters
  //  ..cold, heat, light and water limiting fns
  //--------------------------------------------------
  double slope, base, tau;
  double alphaa,k_beer;
  
  //data holders only exist if using FORKEL_PHENOLOGY
  //else, skip over the parameters in the par file
  //..crop pfts start after natural pfts (no forkel params for crops)
  #ifndef FORKEL_PHENOLOGY
  if(pft->id < NPFT){
   if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
     return TRUE;
   if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
     return TRUE;
   if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
     return TRUE;
   if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
     return TRUE;
   if(fscanf(file,"%lg %lg",&alphaa,&k_beer)!=2)
     return TRUE;
   pft->k_beer=0.5; //default value for k_beer
  }
  #else
  if(pft->id < NPFT){
    if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
      return TRUE;
    pft->slope_cold=slope;
    pft->base_cold =base;
    pft->tau_cold  =tau;
    if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
      return TRUE;
    pft->slope_heat=slope;
    pft->base_heat =base;
    pft->tau_heat  =tau;
    if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
      return TRUE;
    pft->slope_light=slope;
    pft->base_light =base;
    pft->tau_light  =tau;
    if(fscanf(file,"%lg %lg %lg",&slope,&base,&tau)!=3)
      return TRUE;
    pft->slope_water=slope;
    pft->base_water =base;
    pft->tau_water  =tau;
    if(fscanf(file,"%lg %lg",&alphaa,&k_beer)!=2)
      return TRUE;
    pft->alphaa =alphaa;
    pft->k_beer =k_beer;
  }//end if not crop
  #endif
  return FALSE;
} /* end of 'fscanreal_forkel' */

static void check_pft_exclusion(Pftpar *pft){
  #ifdef PFTEXCLUDE_TrBE
    if(pft->id==0){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_TrBR
   if(pft->id==1){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_TeNE
  if(pft->id==2){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_TeBE
   if(pft->id==3){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_TeBS
   if(pft->id==4){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_BoNE
  if(pft->id==5){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_BoBS
  if(pft->id==6){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_BoNS
   if(pft->id==7){pft->gdd5min=1e7;}
  #endif
  #ifdef PFTEXCLUDE_C3GRASS
   if(pft->id==8){pft->gdd5min=1e7;}
  #endif
  #ifdef FORKEL_PHENOLOGY
    #ifdef PFTEXCLUDE_C3POLARGRASS
     if(pft->id==9){pft->gdd5min=1e7;}
    #endif
    #ifdef PFTEXCLUDE_C4GRASS
     if(pft->id==10){pft->gdd5min=1e7;}
    #endif
  #else
    #ifdef PFTEXCLUDE_C3GRASS
     if(pft->id==9){pft->gdd5min=1e7;}
    #endif
    #ifdef PFTEXCLUDE_C4GRASS
     if(pft->id==10){pft->gdd5min=1e7;}
    #endif
  #endif
} /* end of check_pft_exclusion  */
 
int fscanpftpar(Pftpar **pftpar,                /* Pointer to Pftpar array */
                const char *filename,           /* filename */
                const Fscanpftparfcn scanfcn[],
                int ntypes,                     /* number of PFT types */
                int *ncftype                    /* number of CFT */
               )                               /* returns number of elements in array */
{
  int npft,ncft,n,id,l;
  String s;
  char *cmd;
  FILE *file;
  Pftpar *pft;
  #ifdef USE_CPP
  cmd=malloc(strlen(filename)+strlen(cpp_cmd)+1);
  strcat(strcpy(cmd,cpp_cmd),filename);
  file=pt_popen(cmd,"r");
  free(cmd);
  #else
  file=fopen(filename,"r");
  #endif
  if(file==NULL)
  {
    printfopenerr("fscanpftpar",filename);
    return 0;
  }

  if(fscanf(file,"%d",&npft)!=1)
  {
    readinterr(filename,"npft");
    pt_pclose(file);
    return 0;
  }
  if(fscanf(file,"%d",&ncft)!=1)
  {
    readinterr(filename,"ncft");
    pt_pclose(file);
    return 0;
  }
  *pftpar=newvec(Pftpar,(npft+ncft));
  check(*pftpar);
  for(n=0;n<npft+ncft;n++)
  {
    /* Read pft->id, defined in pftpar.h */
    if(fscanf(file,"%d",&id)!=1)
    {
      readinterr(filename,"id");
      break;
    } 
    if(id<0 || id>=npft+ncft)
    {
      fprintf(stderr,"Error in '%s': invalid range of 'id'=%d.\n",filename,id);
      break;
    } 
    pft=(*pftpar)+id;
    pft->id=id;
    
    /* Read pft->name */
    if(fscanstring(file,s))
    {
      readstringerr(filename,"name");
      break;
    }
    pft->name=strdup(s);

    /* Read pft->type, defined in pftpar.h */
    fscanint(file,&pft->type,filename,pft->name,"type");
    pft->rootdist[BOTTOMLAYER]=1;
    for(l=0;l<BOTTOMLAYER;l++)
    {
      fscanreal2(file,pft->rootdist+l,filename,pft->name,"rootdist");
      pft->rootdist[BOTTOMLAYER]-=pft->rootdist[l];
    }
    fscanreal2(file,&pft->minwscal,filename,pft->name,"minwscal");
    fscanreal2(file,&pft->gmin,filename,pft->name,"gmin");
    fscanreal2(file,&pft->respcoeff,filename,pft->name,"respcoeff");
    fscanreal2(file,&pft->nmax,filename,pft->name,"nmax");
    fscanreal2(file,&pft->resist,filename,pft->name,"resist");
    fscanreal2(file,&pft->longivity,filename,pft->name,"longivity");
    pft->sla=2e-4*exp(6.15-0.46*log(pft->longivity*12));
    fscanreal2(file,&pft->lmro_ratio,filename,pft->name,"lmro_ratio");
    fscanreal2(file,&pft->ramp,filename,pft->name,"ramp");
    pft->ramp=1/pft->ramp;
    fscanreal2(file,&pft->lai_sapl,filename,pft->name,"lai_sapl");
    fscanreal2(file,&pft->gdd5min,filename,pft->name,"gdd5min");
    fscanreal2(file,&pft->twmax,filename,pft->name,"twmax");
    fscanreal2(file,&pft->gddbase,filename,pft->name,"gddbase");
    fscanreal2(file,&pft->min_temprange,filename,pft->name,"min_temprange");
    fscanreal2(file,&pft->emax,filename,pft->name,"emax");
    fscanreal2(file,&pft->intc,filename,pft->name,"intc");
    fscanreal2(file,&pft->alphaa,filename,pft->name,"alphaa");
    fscanint(file,&pft->phenology,filename,pft->name,"phenology");
    fscanint(file,&pft->path,filename,pft->name,"path");
    fscanlimit2(file,&pft->temp_co2,filename,pft->name,"temp_co2");
    fscanlimit2(file,&pft->temp_photos,filename,pft->name,"temp_photos");
    fscanlimit2(file,&pft->temp,filename,pft->name,"temp");
    if(pft->type<0 || pft->type>=ntypes)
    {
      fprintf(stderr,"Invalid pft type=%d.\n",pft->type);
      break;
    }
    pft->mortality=nomortality;
    pft->init=noinit;
    pft->fire=nofire;
    pft->mix_veg=nomix_veg;
    pft->adjust=noadjust;
    pft->reproduction=noreproduction;
    pft->turnover=noturnover;
    pft->allocation=noallocation;
    pft->light=nolight;
    pft->establishment=noestablishment;
    
    //...............................................
    //scan parameters for FORKEL_PHENOLOGY
    // data holders only exist if using flag
    // else data is read from par file, but skipped
    // forkel params need to be in par file
    // ..params do not exist for crops (skipped)
    //...............................................
    fscanreal_forkel(file,pft);

    //...............................................
    //check flags for pft exlusion
    //..if exluding pfts, then set gdd5min to 1e7
    //..effectively does not allow establishment
    //...............................................
    check_pft_exclusion(pft);

    /* Now scan pft specific parameters and set specific functions */
    if(scanfcn[pft->type](file,pft,filename))
      break;
  }//..end pft loop
  pt_pclose(file);
  if(n<npft+ncft)
    fprintf(stderr,"Warning: number of PFTs truncated to %d.\n",n); 
  *ncftype=ncft;
  return n-ncft;
} /* of 'fscanpftpar' */
