/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e /  c r o p . h                                **/
/*******************************************************************/

#ifndef CROP_H /* Already included? */
#define CROP_H

/* Definition of constants */

#define LAMBERTBEER_K 0.5
#define RESIDUES_OUT FALSE

/* Declaration of datatypes */

typedef struct
{
  int sdatenh,sdatesh;          
}Initdate; 

typedef struct
{
  Real root,so,pool;
}Cropphys;

typedef struct
{
  Real root,so,pool,leaf;
}Cropphys2;

typedef struct
{
  int calcmethod_sdate;     /* different methods for determining the crop dates */
  Initdate initdate;        /* init sowing date for n/shemisphere */
  int hlimit;               /* maximum length of crop cycle */
  int fallow_days;          /* length of fallow period between crop cycles */
  Real temp_fall;           /* threshold for decreasing temperature to determine the crop date */
  Real temp_spring;         /* threshold for increasing temperature to determine the crop date */
  Real temp_vern;           /* threshold for increasing temperature to determine the crop date */
  Real trg;                 /* temperature under which vernalization is possible (deg C) */
  Real pvd;                 /* number of vernalization days required */
  Real psens;               /* sensitivity to the photoperiod effect [0-1] (1 means no sensitivity) */
  Real pb;                  /* basal photoperiod (h) (pb<ps for longer days plants) */
  Real ps;                  /* saturating photoperiod (h) (ps<pb for shorter days plants) */
  Limit phu;                /* minimum/maximum potential heat units required for plant maturity (deg C)*/
  Real phu_par;             /* parameter for determining the variability of phu */
  Limit basetemp;           /* minimum/maximum base temperature */ 
  Real fphuc;               /* fraction of growing season 1 [0-1] */
  Real flaimaxc;            /* fraction of plant maximal LAI 1 [0-1] */
  Real fphuk;               /* fraction of growing season 2 [0-1] */
  Real flaimaxk;            /* fraction of plant maximal LAI 2 [0-1] */
  Real fphusen;             /* fraction of growing period at which LAI starts decreasing [0-1] */
  Real flaimaxharvest;      /* fraction of plant maximal LAI still present at harvest [0-1]*/
  Real laimax;              /* plant maximal LAI (m2leaf/m2)*/
  Real laimin;              /* plant minimal LAI (m2leaf/m2)*/
  Real hiopt;               /* optimum harvest index HI reached at harvest*/
  Real himin;               /* minimum harvest index HI reached at harvest*/
  Real shapesenescencenorm; /* parameter for calculating the fraction of maximal LAI */
  Cropphys cn_ratio;        /* C:N mass ratio for root, storage organ, and pool */
  Real pip;                 /* Potential Isoprene Production Is (μg g−1h−1) */
  Real pmp;                 /* Potential Monoterpene Production (μg g−1h−1) */
  Real ms;                  /* Monoterpene storage */
} Pftcroppar;


typedef struct
{
  Bool wtyp;                /* distinguish between winter and summer crop */
  int growingdays;          /* counter for the days of the crop cycle */
  Real pvd;                 /* vernalization days */
  Real phu;                 /* phenological heat unit */
  Real basetemp;            /* base temperature */
  Bool senescence;          /* current senescence period */
  Bool senescence0;         /* senescence period of yesterday */
  Real flaimax;             /* fraction of maximum lai */
  Real husum;               /* sum of heat units */
  Real vdsum;               /* sum of vernalization days */
  Real fphu;                /* fraction of phenological heat unit */
  Cropphys2 ind;
  Real lai;                 /* current leaf area index */
  Real lai000;              /* leaf area index of yesterday */
  Real laimax_adjusted;     /* adjusted maximum lai */
  Real lai_nppdeficit;      /* LAI reduction due to insufficient NPP */
  Real demandsum;
  Real supplysum;
} Pftcrop;

/* Declaration of functions */

extern void new_crop(Pft *);
extern Real npp_crop(Pft *,Real,Real,Real,Bool *,Real, Real *); 
extern Real fpc_crop(Pft *);
extern Real fpar_crop(Pft *);
extern Real litter_update_crop(Litter *,Pft *,Real,Real *,Bool,Bool);
extern Real lai_crop(const Pft *);
extern Bool phenology_crop(Pft *,int,Real,Real,Real[],int,Real,Bool,int,Real,Real); 
extern void laimax_manage(Real *,Real,Real,const Pftpar *,int,int,int);
extern Bool fwrite_crop(FILE *,const Pft *);
extern void fprint_crop(FILE *,const Pft *);
extern void fread_crop(FILE *,Pft *,Bool);
extern Bool fscanpft_crop(FILE *,Pftpar *,const char *);
extern Bool isneg_crop(const Pft *);
extern Real establishment_crop(Pft *,Real,Real,int);
extern void init_crop(Pft *);
extern Real vegc_sum_crop(const Pft *);
extern void free_crop(Pft *);
extern void allocation_daily_crop(Pft *,Real,Real);
extern void phen_variety(Pftcrop *,const Pftcroppar *,int,Real,int,Bool);

/* Definitions of macros */

#define iscrop(pft) (getpftpar(pft,type)==CROP)
#define phys_sum_crop(ind) (ind.leaf+ind.root+ind.so+ind.pool)

#endif
