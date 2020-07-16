/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / s o i l .h                                  **/
/*******************************************************************/

#ifndef SOIL_H /* Already included? */
#define SOIL_H

/* Definition of constants */

#define NSOILLAYER 2 /* Number of soil layers */
#define NLAYERS 11
#define TOPLAYER 0
#define BOTTOMLAYER (NSOILLAYER-1)
#define soil_equil_year 400
#define atmfrac 0.7
#define soilfrac (1-atmfrac)
#define fastfrac 0.98

//extern  Real soildepth[NSOILLAYER]; //SOILDEPTH - disable global definition for soildepth to use dynamic soil depth

#include "soilpar.h"
#include "pftpar.h"  
#include "climate.h"
/* Definition of datatypes */

typedef struct
{
  Real fast,slow;
} Pool;

typedef struct
{
  Real ag_tree,ag_grass,bg;
  Real ag_tree_inc,ag_grass_inc,bg_inc;                  //accumulated litter in current year
  Real ag_tree_pinc,ag_grass_pinc,bg_pinc;               //litter increment in previous year
  Real ag_pft[NPFT],ag_pft_inc[NPFT],ag_pft_pinc[NPFT];  //litter for pft
} Litter;

typedef struct
{
  int type;
  char *name;                      //soil name
  Real k1,k2;
  Real whc[NSOILLAYER];
  Real tdiff_0,tdiff_15,tdiff_100;
  Real f;                          //hydraulic parameter
  Real min_con;                    //mineral content 5(index)
  Real org_con;                    //organic content 6 
  Real wat_hld;                    //water hold capacity below wilting point 7 
  Real porosity;                   //porosity 8
} Soilpar;

typedef struct
{
  const Soilpar *par;
  Pool cpool,k_mean;
  Real w[NSOILLAYER];
  Real w_evap;
  Real alag,amp,meanw1;
  Real snowpack;
  Real decomp_litter_mean;
  Real whcs[NSOILLAYER];          
  Real awhcs[NSOILLAYER];          //actual WHC considering ice fraction for upper and lower soil layer. Unit: mm
  Real awhc[NLAYERS];              //actual WHC for the 8 soil layer of permafrost module. Unit: mm
  Litter litter;
  Real soildepth[NSOILLAYER];      
} Soil; 

/* Declaration of functions */

extern int fscansoilpar(Soilpar **,const char *);
extern void fscansoilpar_ncdf(Soilpar **, Real, Real, Real, Real, Real, Real, Real, Real, Real);
extern void initsoil(Soil *soil,const Soilpar *,Real[NSOILLAYER]); 
extern Real snow(Real *,Real *,Real *,Real);
extern Real soiltemp(const Soil *,const Climbuf *);
extern Real temp_response(Real);
extern Real littersom(Soil *,Real);
extern Real waterbalance(Soil *,Real [NSOILLAYER],Real *,Real,Real,Real,Real,Real,int);
extern void getlag(Soil *,int);
extern Bool fwritesoil(FILE *,const Soil *,Bool);
extern Bool freadsoil(FILE *,Soil *,const Soilpar *,Bool);
extern Real fire_sum(const Litter *,Real);
extern Real fire_prob(const Litter *,Real);
extern void fprintsoil(FILE *,const Soil *);
extern void equilsoil(Soil *);

/* Definition of macros */

#define getsoilpar(soil,var) (soil)->par->var
#define foreachsoillayer(l) for(l=0;l<NSOILLAYER;l++)
#define fprintpool(file,pool) fprintf(file,"%5.2f %5.2f",pool.slow,pool.fast)
#define fprintlitter(file,litter) fprintf(file,"%5.2f %5.2f %5.2f",litter.ag_tree,litter.ag_grass,litter.bg)
#define littersum(litter) (litter.bg+litter.ag_tree+litter.ag_grass)

#endif
