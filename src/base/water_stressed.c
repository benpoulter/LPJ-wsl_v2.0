/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ w a t e r s t r e s s e d . c                      **/
/*******************************************************************/

#include "lpj.h"
#include "tree.h"    // OZONE - read tree parameters
#include "grass.h"   // OZONE - read grass parameters

#define GM 3.26      /* empirical parameter in demand function */
#define EPSILON 0.1  /* min precision of solution in bisection method */
#define LAMBDAM 0.8  /* optimal Ci/Ca ratio */
#define ALPHAM 1.391 /* Priestley-Taylor coefficient (Demand,maximum-value)*/ 
#define KO3 1.67     /* OZONE - ratio of leaf diffusion for O3 to leaf resistence for H2O*/

typedef struct
{
  Real fac,co2,temp,par,daylength,alphaa,tstress,fpar;
  int path;
} Data;

static Real fcn(Real lambda,Data *data)
{
  Real agd,rd;

/*
 *  Call photosynthesis to determine alternative total
 *  daytime photosynthesis estimate (adt2) implied by
 *  Eqns 2 & 19, Haxeltine & Prentice 1996, and current
 *  guess for lambda (xmid)
 */
  
  return data->fac*(1-lambda)-photosynthesis(&agd,&rd,data->path,lambda,
               data->tstress,data->co2,
               data->temp,data->par,data->fpar,
               data->daylength,data->alphaa); 
/*
 *   Calculate total daytime photosynthesis implied by
 *   canopy conductance from water balance routine and
 *   current guess for lambda (xmid).  Units are mm/m2/day
 *   (mm come from gpd value, mm/day)
 *   Eqn 18, Haxeltine & Prentice 1996
 */

} /* of 'fcn' */

Real water_stressed(Pft *pft,
                    Real aet_layer[NSOILLAYER],
                    Real w[NSOILLAYER],          /* water in soil layers */
                    Real gp_stand,
                    Real gp_stand_leafon,
                    Real gp_pft,
                    Real *gc_pft,
                    Real fpc,
                    Real *rd,
                    Real *wet,
                    Real pet,
                    Real co2,                    /* Atmos. CO2 partial pressure (ppm) */
                    Real temp,                   /* Temperature (deg C) */
                    Real par,
                    Real daylength,              /* Daylength (h) */
                    Bool irrigation,
                    const Real whcs[NSOILLAYER],
                    Real irrig_amount,
                    Real *wdf,
                    Real aet_pft[NSOILLAYER],
                    Real ozone,
                    Real *fo3uptake,
                    Real lat,
                    Real lon,
                    Real elev,
                    Real *ci
                   ) 
{
  int l; 
  Real supply,supply_pft,demand,demand_pft,wr,lambda,gpd,agd,gc,aet;
  Real F = 1; //OZONE initialize scaling factor
  Data data;
  Pftcrop *crop;
  Real w_new_0;
  #ifdef OZONE
  Pfttreepar *treepar;  
  Pftgrasspar *grasspar; 
  Real FO3a, FO3crit, FO3, R, p, ozoneNMol, FO3up;
  //Real gcconv;
  
  *fo3uptake = 0; //initialize o3 uptake
  
  //Convert to nano-moles
  R = 8.314;                                //Ideal gas constant (m3 Pa / mol K)
  p = 1.0133E5*exp(-elev/8200);             //Pressure conversion for elevation (from eq 3.7 Campbell and Norman)
  ozoneNMol = (p/(R*degCtoK(temp)))*ozone;  //Convert ppb to nmol m-3 (p 21 in Seinfeld - Atmospheric Chemistry and Physics textbook)
                                            //..keep as mol, don't use g/mol - keep in nmol, keep ppb)
  #endif


  wr=gpd=0;
  w_new_0=w[0]+irrig_amount/whcs[0];
  wr=pft->par->rootdist[0]*w_new_0;
  for(l=1;l<NSOILLAYER;l++) 
    wr+=pft->par->rootdist[l]*w[l];
  
  if (*wet>0.9999) *wet=0.9999;

  supply=pft->par->emax*wr*pft->phen;        
  supply_pft=pft->par->emax*wr*pft->phen*pft->fpc;
  demand=(gp_stand>0) ? (1.0-*wet)*pet*ALPHAM/(1+GM/gp_stand) : 0;
  demand_pft=(gp_pft>0) ? (1.0-*wet)*pet*ALPHAM/(1+GM/gp_pft) : 0;
 
  if(!irrigation && pft->par->type==CROP){
    crop=pft->data;
    crop->demandsum+=demand; 
    crop->supplysum+=(supply>demand) ? demand : supply;
    *wdf=(crop->demandsum>0.0) ? 100.0*crop->supplysum/crop->demandsum: 100.0;  
  } 

  if(pet>0 && gp_stand_leafon>0 && pft->fpc>0){
    /*pft->wscal=(pft->par->emax*wr*pft->fpc)/(pet*ALPHAM/(1+GM/(gp_stand_leafon+pft->par->gmin*pft->fpc)));*/
    pft->wscal=(pft->par->emax*wr)/(pet*ALPHAM/(1+GM/gp_stand_leafon));
    if(pft->wscal>1)
      pft->wscal=1;
  }else{ 
    pft->wscal=1;
  }

  pft->wscal_mean+=pft->wscal;
  
  if(supply_pft>=demand_pft){  
    *gc_pft=gp_pft;
  }else if(pet>0){
    *gc_pft=GM*supply_pft/((1.0-*wet)*pet*ALPHAM-supply_pft);
    if(gc_pft<0)
      *gc_pft=0;
  }else{
    *gc_pft=0;
  }

  if(supply>=demand){
    gc=gp_stand;
  }else if(pet>0){
    gc=GM*supply/((1.0-*wet)*pet*ALPHAM-supply);
    if(gc<0)
      gc=0;
  }else{
    gc=0;
  }

#ifdef OZONE
  if(ozone == 999.9)
    ozoneNMol = 0;

  //ozone params
  //  Feedback on transpiration probably not consistent & so leave out and calculate O3 flux with gactual
  //  MOSES scheme calculates conductance directly, accounting for humidity effect (so, gl = gactual)
  //  Scaling gactual will have linear effect on A via ci-
  //  MUST nest in TREE or GRASS conditional because crops have no ozone damage criteria
  if(getpftpar(pft,type)==TREE){
    //---------------------------------
    //     T r e e   O z o n e
    //---------------------------------
    treepar = getpftpar(pft,data);
    FO3a = treepar->o3aparam.high;
    FO3crit = treepar->o3crit;

    //Flux of ozone based on temp/light/water limited gc
    //Scale gc to m s-1 from mm s-1

    //gcconv = ((p*(gc*pft->fpc)/1000)/(R*degCtoK(temp)));
    //FO3 = ozoneNMol / (KO3/gcconv);
    //printf("%f\t%f\t%f\n",(gc*pft->fpc), gcconv, FO3);

    FO3 = ozoneNMol / (KO3/((gc*pft->fpc)/1000));
    FO3up = (FO3-FO3crit < 0) ? 0 : FO3-FO3crit;
    *fo3uptake = hour2sec(daylength)*(FO3up/1000000);

    //Analytical solution for ozone scalar
    F = (1 + FO3a*FO3crit)/(1 + FO3a*FO3);

    //Check F is > 0 or < 1
    if(F < 0 ){
      F = 0;
    //  printf("WARNING: F is out of bounds");
    }
    if(F > 1 ){
      F = 1;
    //  printf("WARNING: F is out of bounds");
    }
    //printf("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",ozone, ozoneNMol, FO3a, FO3crit, FO3, F, gc, KO3);

    //Fix F to 0 if no ozone data
    if(ozone == 999.9)
      F = 0;

    //Scale gc; A and Rd are linearly related and scaled accordingly
    //gc=gc*F;
  }else if(getpftpar(pft,type)==GRASS){
    //---------------------------------
    //     G r a s s   O z o n e
    //---------------------------------	  
    grasspar = getpftpar(pft,data);
    FO3a = grasspar->o3aparam.high;
    FO3crit = grasspar->o3crit;

    //Flux of ozone based on temp/light/water limited gc
    //Scale gc to m s-1 from mm s-1
    //gcconv = ((p*(gc*pft->fpc)/1000)/(R*degCtoK(temp)));
    //FO3 = ozoneNMol / (KO3/gcconv);
    //printf("%f\t%f\t%f\n",(gc*pft->fpc), gcconv, FO3);

    FO3 = ozoneNMol / (KO3/((gc*pft->fpc)/1000));
    FO3up = (FO3-FO3crit < 0) ? 0 : FO3-FO3crit;
    *fo3uptake = hour2sec(daylength)*(FO3up/1000000);

    //Analytical solution for ozone scalar
    F = (1 + FO3a*FO3crit)/(1 + FO3a*FO3);

    //Check F is > 0 or < 1
    if(F < 0 ){
      F = 0;
    //  printf("WARNING: F is out of bounds");
    }
    if(F > 1 ){
      F = 1;
    //  printf("WARNING: F is out of bounds");
    }

    //Fix F to 0 if no ozone data
    if(ozone == 999.9)
      F = 0;
    //Scale gc; A and Rd are linearly related and scaled accordingly
    //  gc=gc*F;

    //if(lon == 7.75 && lat == 46.25) //DEVQ: debug
    // printf("%f\t%f\t%f\t%f\t%f\t%f\n", ozoneNMol, (gc*pft->fpc)/1000, FO3, F, fo3uptake, temp);
  }//..end of tree/grass ozone
#endif

  aet=(wr>0 ? min(supply,demand)/wr*pft->fpc : 0);
  aet_layer[0]+=aet*pft->par->rootdist[0]*w_new_0;
  aet_pft[0]=aet*pft->par->rootdist[0]*w_new_0;

  for(l=1;l<NSOILLAYER;l++){
    aet_layer[l]+=aet*pft->par->rootdist[l]*w[l];
    aet_pft[l]=aet*pft->par->rootdist[l]*w[l];
  }
  
  /*gpd=hour2sec(daylength)*(gc-pft->par->gmin*pft->fpc*pft->phen);*/
  gpd=hour2sec(daylength)*(gc-pft->par->gmin*pft->phen)*pft->fpc;
  data.tstress=temp_stress(pft->par,temp,daylength);

  if(gpd>1e-5 && isphoto(data.tstress)){
    data.fac=gpd/1.6*ppm2bar(co2);
    data.path=pft->par->path;
    data.temp=temp;
    data.co2=ppm2Pa(co2);
    data.par=par;
    data.daylength=daylength;
    data.alphaa=pft->par->alphaa;
    data.fpar=fpar(pft);
    lambda=bisect((Bisectfcn)fcn,0.02,LAMBDAM+0.05,&data,0,EPSILON,10); 
    photosynthesis(&agd,rd,data.path,lambda,data.tstress,data.co2,
                   temp,par,data.fpar,daylength,data.alphaa);
    *ci=lambda*co2;
    *rd=*rd*F;      /* DON'T DELETE THIS LINE, rqd for Ozone */
  }else{
    *ci=LAMBDAM*co2;
    *rd=0; 
    return 0.0;
  }
  return agd*F;   //OZONE - scale GPP directly - this is agd from 'gpd>1e-5' check
} /* of 'water_stressed' */
