/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / p f t . h                                   **/
/*******************************************************************/

#ifndef PFT_H /* Already included? */
#define PFT_H

#include "pftpar.h"

#define APREC_MIN 100 //LC set global value; now accessed in LUC

/* Definitions of datatypes */

typedef struct
{
  Real low,high;
} Limit;


typedef struct Pft
{
  const struct  Pftpar
  {
    int id,type;               /* type --> whether CROP or TREE or GRASS*/
    char *name;                /* Pft name */
    Real rootdist[NSOILLAYER]; /* fraction of roots in upper soil layer par1*/
    Real minwscal;             /* water scalar value at which leaves shed by 
                                   drought deciduous PFT par3 */
    Real gmin;                 /* canopy conductance component (4) */
    Real respcoeff;            /* maintenance respiration coefficient (5) */
    Real nmax;                 /* maximum foliar N content (mg/g) (7) */
    Real resist;               /* fire resistance index (8) */
    Real longivity;            /* leaf longivity (10) */
    Real lmro_ratio;           /* leaf to root ratio under non-water stressed 
                                   conditions (18) */
    Real ramp;                 /* number of GDDs to attain full leaf cover 
                                   (par19) */
    Real gdd5min;              /* PFT-specific minimum GDD(30) */
    Real twmax;                /* (31) */
    Real gddbase;              /* GDD base  (33) */
    Real min_temprange;        /*  (34) */
    Real emax;                 /* emax (mm/day)(35) */
    Real intc;                 /* Interception storage parameter (36) */
    Real alphaa;               /* alphaa, fraction of PAR assimilated at ecosystem level, relative to leaf level */
    Real k_beer;               /* for FORKEL_PHENOLOGY; light extinction coefficient in the Beer-Lambert law for attenuation of light in canopy  */
    Real lai_sapl;
    int phenology;             /* par17 */
    int path;                  /* par2 */
    Real sla;
    Limit temp_co2;            /* temperature limit for CO2 uptake (24,27) */
    Limit temp_photos;         /* range of temperature optimum for 
                                   photosynthesis(25,26) */
    Limit temp;                /* bioclimatic limits (28,29) */
    void *data;                /* pointer for PFT specific extensions */

    #ifdef FORKEL_PHENOLOGY
      //-------------------------
      // pft-specific parameters
      // ..Forkel et al. 2014 Supplementary Material
      //-------------------------
      Real slope_cold,slope_heat,slope_light,slope_water;
      Real base_cold,base_heat,base_light,base_water;
      Real tau_cold,tau_heat,tau_light,tau_water;
    #endif

    /* list of pointers for PFT specific functions */

    void (*newpft)(struct Pft *); 
    void (*init)(struct Pft *);   
    Real (*turnover)(Litter *,struct Pft *);
    Real (*npp)(struct Pft*,Real,Real,Real,Bool *,Real, Real *); 
    Real (*fpc)(struct Pft*);
    Real (*fpar) (struct Pft*);
    Bool (*leaf_phenology)(struct Pft *,int,Real,Real,Real[],int,Real,Bool,int,Real,Real); 
    Bool (*fwrite)(FILE *,const struct Pft *);
    void (*fread)(FILE *,struct Pft *,Bool);
    void (*fprint)(FILE *,const struct Pft *);
    Real (*litter_update)(Litter *,struct Pft *,Real,Real *,Bool,Bool);
    void (*allocation)(Litter *,struct Pft *,Real *,Bool);
    Bool (*isneg)(const struct Pft *);
    Real (*establishment)(struct Pft *,Real,Real,int);
    void (*reproduction)(Litter *,struct Pft *);
    void (*mortality)(Litter *,struct Pft *,Real,Real,Real *);
    Real (*fire)(struct Pft *,Real *);
    Real (*lai)(const struct Pft *);
    void (*adjust)(Litter *,struct Pft *,Real,Bool);
    void (*free)(struct Pft *);
    void (*light)(Litter *,struct Pft *,Real,Bool);
    Real (*vegc_sum)(const struct Pft *);
    void (*mix_veg)(struct Pft *,Real);
  } *par;                /* Pft parameters */
  Real fpc;              /* foliar projective cover (FPC) under full leaf 
                            cover as fraction of modelled area */
  Real nind;
  Real gdd;
  Real bm_inc;
  Real monopool;
  Real wscal;
  Real wscal_mean;
  Real phen,aphen;
  Buffer *gddbuf;
  void *data;            /* pointer for PFT specific extensions */

  Real DailyFPAR; /* PHENDATE */

  #ifdef FORKEL_PHENOLOGY
    Real fn_cold_prev;
    Real fn_light_prev;
    Real fn_water_prev;
    Real fn_heat_prev;
  #endif

} Pft;

typedef struct Pftpar Pftpar;


/* Defines a pointer to a function that takes a pointer to FILE, Pftpar and a char and returns a Bool. 
 * This construction is necessary to make a function return a function pointer. */
typedef Bool (*Fscanpftparfcn)(FILE *,Pftpar *,const char *);

/* Declaration of functions */

extern Pft *newpft(const Pftpar *);
extern void freepft(Pft *);
extern int fscanpftpar(Pftpar **,const char *,const Fscanpftparfcn [],int,int *);
extern Bool establish(Real,const Pftpar *,const Climbuf *,int); 
extern Bool survive(const Pftpar *,const Climbuf *);
extern Real temp_stress(const Pftpar *,Real,Real);
extern Real photosynthesis(Real *,Real *,int,Real,Real,Real,Real,Real ,Real,Real,Real);
extern Bool killpft(Litter *,Pft *,const Climbuf *);
extern Real interception(Real *,const Pft *,Real,Real);
extern void initgdd(Real [],int);
extern void updategdd(Real [],const Pftpar [],int,int,Real);
extern Real gp(Pft *,Real,Real,Real,Real,Real *,Real *);
extern Real water_stressed(Pft *,Real [NSOILLAYER],Real [NSOILLAYER],
                           Real,Real,Real,Real *,Real,Real *,Real *,Real,Real,
                           Real,Real,Real,Bool,const Real[NSOILLAYER],Real,Real *, Real [NSOILLAYER],Real, Real *,Real,Real,Real,Real *); 
extern Bool fwritepft(FILE *,const Pft *,Bool);
extern void fprintpft(FILE *,const Pft *);
extern Pft *freadpft(FILE *,const Pftpar[],Bool);
extern void nomortality(Litter *,Pft *,Real,Real,Real *);
extern void noinit(Pft *);
extern Real nofire(Pft *,Real *);
extern void noadjust(Litter *,Pft *,Real,Bool);
extern void noreproduction(Litter *,struct Pft *);
extern void nolight(Litter *,struct Pft *,Real,Bool);
extern Real noturnover(Litter *,struct Pft *);
extern void noallocation(Litter *,struct Pft *,Real *,Bool);
extern void nomix_veg(Pft *,Real);
extern Real noestablishment(Pft *,Real,Real,int);

/* Definition of macros */

#define isphoto(tstress) (tstress>=1e-2)
#define getpftpar(pft,val) (pft)->par->val
#define newgdd(npft) newvec(Real,npft)
#define fpc(pft)
#define fpar(pft) pft->par->fpar(pft)
#define npp(pft,gtemp_air,gtemp_soil,assim,negbm,wdf,bgaresp) pft->par->npp(pft,gtemp_air,gtemp_soil,assim,negbm,wdf,bgaresp) //BP new outputs
//#define leaf_phenology(pft,npft,temp,lat,laimax,day,daylength,irrigation,landusetype,mtemp) pft->par->leaf_phenology(pft,npft,temp,lat,laimax,day,daylength,irrigation,landusetype,mtemp) //BP DAILY INPUTS
#define leaf_phenology(pft,npft,temp,lat,laimax,day,daylength,irrigation,landusetype,mtemp,Real) pft->par->leaf_phenology(pft,npft,temp,lat,laimax,day,daylength,irrigation,landusetype,mtemp,Real) //LC FORKEL_PHENOLOGY added Real for daily avg swrad (as PARx2)
#define allocation(litter,pft,fpc_inc,subannual) pft->par->allocation(litter,pft,fpc_inc,subannual)
#define reproduction(litter,pft) pft->par->reproduction(litter,pft)
#define mortality(litter,pft,turnover_ind,mtemp_max,fpc_inc) pft->par->mortality(litter,pft,turnover_ind,mtemp_max,fpc_inc)
#define isneg(pft) pft->par->isneg(pft)
#define litter_update(litter,pft,frac,rharvest,isharvest,subannual) pft->par->litter_update(litter,pft,frac,rharvest,isharvest,subannual)
#define turnover(litter,pft) pft->par->turnover(litter,pft)
#define fire(pft,fireprob) pft->par->fire(pft,fireprob)
#define lai(pft) pft->par->lai(pft)
#define init(pft) pft->par->init(pft)
#define vegc_sum(pft) pft->par->vegc_sum(pft)
#define mix_veg(pft,scaler) pft->par->mix_veg(pft,scaler)
#define adjust(litter,pft,fpc,subannual) pft->par->adjust(litter,pft,fpc,subannual)
#define establishment(pft,fpc_total,fpc,n_est) pft->par->establishment(pft,fpc_total,fpc,n_est);

#endif
