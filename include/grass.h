/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / g r a s s . h                               **/
/*******************************************************************/

#ifndef GRASS_H /* Already included? */
#define GRASS_H


/* Definition of datatypes */

typedef struct
{
  Real leaf,root;
}Grassphys;

typedef struct
{
  Grassphys turnover;   /* turnover period (years) (9,11,12)*/
  Grassphys cn_ratio;   /* C:N mass ratio (13-15) */
  Grassphys sapl;       /* sapling */
  Real o3crit;          // OZONE - critical threshold
  Limit o3aparam;       // OZONE - low / high sensitivity parameters
  Real max_wtd;         /* inund_height: max WTP tolerated 37*/
  Real maxinunddays;    /* max days of inundation tolerated 42*/
  Real bulkdens;        /* Bulk density (kg biom./m3) from SPITFIRE v1 - what the BD for dead sphagnum is 43*/
  int ispeat;           /* Peatland PFT (0 not a peatland PFT;1 peatland PFT ) 44*/
  Real pip;             /* Potential Isoprene Production Is (μg g−1h−1) */
  Real pmp;             /* Potential Monoterpene Production (μg g−1h−1) */
  Real ms;              /* Monoterpene storage */
} Pftgrasspar;

typedef struct
{
  Grassphys ind;
} Pftgrass;

/* Declaration of functions */

extern void new_grass(Pft *);
extern Real npp_grass(Pft *,Real,Real,Real,Bool *,Real, Real *); 
extern Real fpc_grass(Pft *);
extern Real fpar_grass(Pft *);
extern Real litter_update_grass(Litter *, Pft*,Real,Real *,Bool,Bool);
extern void allocation_grass(Litter *,Pft *,Real *,Bool);
extern Real lai_grass(const Pft *);
extern Real turnover_grass(Litter *,Pft *);
extern Bool phenology_grass(Pft *,int,Real,Real,Real[],int,Real,Bool,int,Real,Real); 
extern Bool fwrite_grass(FILE *,const Pft *);
extern void fprint_grass(FILE *,const Pft *);
extern void fread_grass(FILE *,Pft *,Bool);
extern Bool fscanpft_grass(FILE *,Pftpar *,const char *);
extern Bool isneg_grass(const Pft *);
extern Real establishment_grass(Pft *,Real,Real,int);
extern Real vegc_sum_grass(const Pft *);
extern void mix_veg_grass(Pft *,Real);
extern Real fire_grass(Pft *,Real *);
extern void reproduction_grass(Litter *,Pft *);
extern void init_grass(Pft *);
extern void free_grass(Pft *);
extern void light_grass(Litter *,Pft *,Real,Bool);

/* Definition of macros */

#define isgrass(pft) (getpftpar(pft,type)==GRASS)
#define fprintgrassphys(file,phys) fprintf(file,"%6.2f %6.2f",phys.leaf,phys.root)
#define phys_sum_grass(ind) (ind.leaf+ind.root)

#endif
