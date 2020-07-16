/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / c e l l . h                                 **/
/*******************************************************************/

#ifndef CELL_H /* Already included? */
#define CELL_H

//define constants 
//.....................................................................
//NOTE: decay rates of wood products from deforestation 
//..these are alternate decay rates for deforestation biomass
//..based on LPX, 75% of wood goes into product pools
//..25% wood + leaf,root decomp/immediate release in same year 
//.....................................................................
#define decayrate_product_2yr   0.5    //annual decay rate for  2 yrs
#define decayrate_product_25yr  0.04   //annual decay rate for  25 yrs
//.....................................................................
//decay rate of wood harvest products
//.....................................................................
#define decayrate_product_1yr   1      //annual decay rate for   1 yr
#define decayrate_product_10yr  0.1    //annual decay rate for  10 yrs
#define decayrate_product_100yr 0.01   //annual decay rate for 100 yrs

/* Definition of datatypes */

typedef struct
{
  Coord coord;
  Standlist standlist;
  Climbuf climbuf;
  Manage manage;
  Cropdates cropdates;
  Real *gdd;
  Real cropfrac;
  Real pasturefrac; 
  Real *laimax;
  Real lakefrac;
  int pft; 
  Bool skip;
  Output output;
  Discharge discharge;
  Topmodel topmodel; 
  /* balance checks */
  Real nep;
  Real awater_flux;
  Real aprec;
  Real totc;
  Real totw;
  Real elev;     //for OZONE
  Real standage; //for STANDAGE
  
  //for tracking wood harvest and deforestation product pools at cell-level
  Real product_1yr;       //..running sums, with annual decay (emission)
  Real product_10yr;      //..pools track legacy harvest and emissions
  Real product_100yr;
  Real woodproducts_temp; //added to track wood products for carbon balance (product decay occurs before C-balance check)
  Real product_2yr;       //..deforestation product pools as running sums, with annual decay (emission)
  Real product_25yr;      //..pools track legacy emissions from deforestation not wood harvest
 
  #if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
    //ageclass transition tracker for secforest
    Bool transitioned_ageclass_secforest;
  #endif   
 
  #ifdef OUTPUT_BYSTAND
   Real product_1yr_primary;       
   Real product_10yr_primary;      
   Real product_100yr_primary;
   Real product_1yr_secforest;       
   Real product_10yr_secforest;      
   Real product_100yr_secforest;

   Real product_25yr_primary;
   Real product_2yr_primary;
   Real product_25yr_secforest;
   Real product_2yr_secforest;
  #endif  
} Cell;

/* Declaration of functions */

extern void freegrid(Cell *,int,Bool);
extern void update_daily(FILE **,Cell *,Config,Real,Real,Real,Real,Real,Real,int,int,int, const Pftpar [], int,int,int, Real *, Bool,int,Real,int,Real,Real);
extern Real update_annual(FILE **,Cell *,const Pftpar [],int,int,int,int,Config,Bool);
extern void update_monthly(FILE **,Cell *,Real,int,int,Config,int); 
extern void init_annual(Cell *,int);
extern int fwritecell(FILE *,int [],const Cell [],int,int,int,Bool);
extern void fprintcell(FILE *,const Cell [],int);
extern Bool freadcell(FILE *,Cell *,const Pftpar[],int,int,const Soilpar *,Bool,Landuse *,int);
extern int writecoords(FILE *,const Cell [],int);
extern int iterate(FILE **,Cell *,Climate *,Landuse *,const Pftpar *,int,int,int,Config);
extern void iterateyear(FILE **,Cell *,Climate *,Landuse *,Real,const Pftpar *,int,
                        int,int,Config,int);
extern int fwriteoutput_annual(FILE **,const Cell *,int, Bool); 
extern int fwriteoutput_monthly(FILE **,const Cell *, int, int, Bool); 
extern int fwriteoutput_daily(FILE **,const Cell *, int, int, int); 
extern int fwriteoutput_pft(FILE **,Cell *,Config,int,int, int); 

extern void equilsom(Cell *);
extern void landusechange(Cell *,const Pftpar[],int,int,int,int,Landuse *,int,Config);

extern void landusechange_NET(Cell *,const Pftpar[],int,int,int,int,Landuse *,int,Config); 
extern void landusechange_GROSS(Cell *,const Pftpar[],int,int,int,int,Landuse *,int,Config);
extern void landusechange_AGECLASS_GROSS(Cell *,const Pftpar[],int,int,int,int,Landuse *,int,Config); 
extern Real woodharvest_updateStand(Cell *,Stand *,Real,Real,Landuse *,int,Bool);
extern void mix_standComplex(Cell *, Stand *, Stand *, int , const Pftpar *, int, Bool);

extern void delstand_preserveOrder(Cell *, Stand *, int , int );
extern void ageclass_transition(Cell *, const Pftpar [], int, int); 
extern void firepft(Cell *, Stand *, int, Real, const Pftpar [], int, Bool *); 

extern void setaside(Cell *,int *,const Pftpar[],Bool,int,Bool);
extern void check_fluxes(Cell *,Config,int,int,Real);

extern Bool initdrain(Cell *,Config *);
extern void drain(Cell *,Config);
extern void irrig_amount(Cell *,Config);
extern Real wateruse(Discharge *,Real *,Real *,Real);
extern void irrigation(Pft *,Real [],Real,Real *,Real,const Real [],Real *);
extern void litter_update_monthly(Cell *);
extern void litter_update_daily(Cell *);

#define printcell(cell,n) fprintcell(stdout,cell,n)

#endif
