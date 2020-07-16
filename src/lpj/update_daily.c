/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/lpj/ u p d a t e _ d a i l y . c                         **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h" 

#define ALPHAM 1.391 /* Priestley-Taylor coefficient (Demand,maximum-value)*/

//====================================================================================
//                         M A I N   F U N C T I O N
// update_daily():  global fn runs daily routines 
//                  ..calls update_pftOutputs_daily(), and internal fn
//====================================================================================
//                        H Y P E R   F U N C T I O N
// update_pftOutputs_daily():  called by update_daily(), helps simplify code
//====================================================================================

//internal fn simplify code in update_daily
void update_pftOutputs_daily(Cell *cell,
                     Stand *stand,
                         Pft *pft,
                        int month,
                   int ndaysmonth,
                         int npft, 
                         int ncft,
                       Real molcv, 
                      Real gc_pft,
                   Real daylength,
                         Real npp, 
                         Real gpp, 
                          Real ci,
         Real aet_pft[NSOILLAYER],
                   Real fo3uptake)
{
  //declare variables and structures
  int l;
  Pfttree *tree;

  //PFT outputs
  if(stand->irrigation){
    //only GRASSLAND and AGRICULTURE stands can have irrigation
    if(stand->landusetype==GRASSLAND){
      cell->output.pft_lai[npft+2*(ncft+NGRASS)-1]      +=-2*log(1-fpar(pft)/(1*pft->nind))/365;
      cell->output.pft_npp[npft+2*(ncft+NGRASS)-1]      +=npp;
      cell->output.pft_gpp[npft+2*(ncft+NGRASS)-1]      +=gpp;
      cell->output.pft_gc[npft+2*(ncft+NGRASS)-1]       +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.pft_fo3uptake[npft+2*(ncft+NGRASS)-1]+=fo3uptake;
      foreachsoillayer(l)
          cell->output.pft_transp[npft+2*(ncft+NGRASS)-1]+=aet_pft[l];
      #ifdef OUTPUT_AGRICULTURE_MPFT
        cell->output.mpft_ag_npp[((2*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=npp;
        cell->output.mpft_ag_gpp[((2*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=gpp;
        cell->output.mpft_ag_lai[((2*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=-2*log(1-fpar(pft)/(1*pft->nind))/ndaysmonth;
        foreachsoillayer(l)
          cell->output.mpft_ag_transp[((2*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=aet_pft[l];
      #endif
    }else{
      cell->output.pft_lai[pft->par->id+ncft+NGRASS]      +=-2*log(1-fpar(pft)/(1*pft->nind))/365;
      cell->output.pft_npp[pft->par->id+ncft+NGRASS]      +=npp;
      cell->output.pft_gpp[pft->par->id+ncft+NGRASS]      +=gpp;
      cell->output.pft_gc[pft->par->id+ncft+NGRASS]       +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.pft_fo3uptake[pft->par->id+ncft+NGRASS]+=0.;
      foreachsoillayer(l)
        cell->output.pft_transp[pft->par->id+ncft+NGRASS] +=aet_pft[l];
      #ifdef OUTPUT_AGRICULTURE_MPFT  
        cell->output.mpft_ag_npp[(pft->par->id-npft+ncft+NGRASS)+(month*(ncft+NGRASS)*2)]+=npp;
        cell->output.mpft_ag_gpp[(pft->par->id-npft+ncft+NGRASS)+(month*(ncft+NGRASS)*2)]+=gpp;
        cell->output.mpft_ag_lai[(pft->par->id-npft+ncft+NGRASS)+(month*(ncft+NGRASS)*2)]+=-2*log(1-fpar(pft)/(1*pft->nind))/ndaysmonth;
        foreachsoillayer(l)
          cell->output.mpft_ag_transp[(pft->par->id-npft+ncft+NGRASS)+(month*(ncft+NGRASS)*2)]+=aet_pft[l];
      #endif
    }
  }else{ 
    //..............................
    // n o   i r r i g a t i o n
    //.............................
    if(stand->landusetype==GRASSLAND){
      cell->output.pft_lai[npft+ncft+NGRASS-1]      +=-2*log(1-fpar(pft)/(1*pft->nind))/365;
      cell->output.pft_npp[npft+ncft+NGRASS-1]      +=npp;
      cell->output.pft_gpp[npft+ncft+NGRASS-1]      +=gpp;
      cell->output.pft_gc[npft+ncft+NGRASS-1]       +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.pft_fo3uptake[npft+ncft+NGRASS-1]+=fo3uptake;
      foreachsoillayer(l)
        cell->output.pft_transp[npft+ncft+NGRASS-1] +=aet_pft[l];
      #ifdef OUTPUT_AGRICULTURE_MPFT
        cell->output.mpft_ag_npp[((1*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=npp;
        cell->output.mpft_ag_gpp[((1*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=gpp;
        cell->output.mpft_ag_lai[((1*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=-2*log(1-fpar(pft)/(1*pft->nind))/ndaysmonth;
        foreachsoillayer(l)
          cell->output.mpft_ag_transp[((1*(ncft+NGRASS))-1)+(month*(ncft+NGRASS)*2)]+=aet_pft[l];
      #endif
    }else if(stand->landusetype==AGRICULTURE){
      cell->output.pft_lai[pft->par->id]      +=-2*log(1-fpar(pft)/(1*pft->nind))/365;
      cell->output.pft_npp[pft->par->id]      +=npp;
      cell->output.pft_gpp[pft->par->id]      +=gpp;
      cell->output.pft_gc[pft->par->id]       +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.pft_fo3uptake[pft->par->id]+=fo3uptake;
      foreachsoillayer(l)
        cell->output.pft_transp[pft->par->id] +=aet_pft[l];
      #ifdef OUTPUT_AGRICULTURE_MPFT
        cell->output.mpft_ag_npp[(pft->par->id-npft)+(month*(ncft+NGRASS)*2)]+=npp;
        cell->output.mpft_ag_gpp[(pft->par->id-npft)+(month*(ncft+NGRASS)*2)]+=gpp;
        cell->output.mpft_ag_lai[(pft->par->id-npft)+(month*(ncft+NGRASS)*2)]+=-2*log(1-fpar(pft)/(1*pft->nind))/ndaysmonth;
        foreachsoillayer(l)
          cell->output.mpft_ag_transp[(pft->par->id-npft)+(month*(ncft+NGRASS)*2)]+=aet_pft[l];
      #endif
    }else{
      //PRIMARY & SECFOREST stands..
      if(istree(pft)){
        tree=pft->data;
        if(fpar(pft)/(tree->crownarea*pft->nind) < 1)    /* must check whether this calc is < 1 or else on AIX, rounding errors crash*/
          cell->output.pft_lai[pft->par->id]+=-2*log(1-fpar(pft)/(tree->crownarea*pft->nind))/365;
        cell->output.mpft_lai[(month*npft)+pft->par->id]+=-2*log(1-fpar(pft)/(tree->crownarea*pft->nind))/ndaysmonth;
      }else{ 
        //Grass
        cell->output.pft_lai[pft->par->id]+=-2*log(1-fpar(pft)/(1*pft->nind))/365;
        cell->output.mpft_lai[(month*npft)+pft->par->id]+=-2*log(1-fpar(pft)/(1*pft->nind))/ndaysmonth;
      }
      cell->output.pft_npp[pft->par->id]              +=npp;
      cell->output.pft_gpp[pft->par->id]              +=gpp;
      cell->output.pft_gc[pft->par->id]               +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.mpft_gc[(month*npft)+pft->par->id] +=hour2sec(daylength)*gc_pft/molcv;
      cell->output.mpft_ci[(month*npft)+pft->par->id] +=ci/ndaysmonth;
      foreachsoillayer(l)
        cell->output.mpft_transp[(month*npft)+pft->par->id]+=aet_pft[l];
      cell->output.mpft_gpp[(month*npft)+pft->par->id]+=gpp;
      cell->output.pft_fo3uptake[pft->par->id]        +=fo3uptake;
      foreachsoillayer(l)
        cell->output.pft_transp[pft->par->id]         +=aet_pft[l];
    }//..end non-irrigated stand updates
  }//..end pft outputs
 
}//..end update_pftOutputs

void update_daily(FILE **output,
                  Cell *cell, /* cell                   */
                  Config config, /* config              */
                  Real co2,   /* atmospheric CO2 (ppmv) */
                  Real temp,  /* temperature (deg C)    */
                  Real prec,  /* precipitation (mm)     */
                  Real sun,   /* sunshine (%)           */
                  Real swdown,/* LWSW                   */
                  Real lwdown,/* LWSW                   */ 
                  int day,    /* day (1..365)           */
                  int month,  /* month (0..11)          */
                  int year,   /* year (XXXX-XXXX)       */
                  const Pftpar pftpar[],
                  int npft,
                  int ncft,
                  int ntypes,
                  Real *landfrac,
                  Bool intercrop,
                  int cellid,
                  Real ozone, 
                  int ndaysmonth, 
                  Real mtemp, 
                  Real minund  /* monthly inundation fraction */
                 )
{
  int p,s,l,b;
  Pft *pft;
  Real melt,pet,par,daylength,evap,wet,rd,gpp;
  Real ci;
  Real gp_stand,gp_stand_leafon,cover_stand,soilmoist=0;
  Real intercep_stand,fpc_total_stand;
  Real aet_stand[NSOILLAYER];
  Real gtemp_air,gtemp_soil;
  Real flux_estab=0;
  Real npp,harvest,rharvest;
  Real *gp_pft,gc_pft,gcgp;
  Real wdf;                        //water deficit fraction
  Stand *stand;
  const Pftcroppar *croppar;
  Bool negbm,dummy;
  const Real prec1=prec;
  int index;
  Real fo3uptake;                  //OZONE
  Real aet_pft[NSOILLAYER];    
  Real molcv; 
  Real bgaresp;
  Real soiltemp_t,swdown_t;
  Real drh;
  #ifdef BVOC
  Real isopr=0.0, monopr=0.0;   //BVOC variables
  #endif

  #ifdef PERMAFROST
  Real w_pi[2];                 //soil moisture with ice fraction
  #endif

  #if defined(TOPMODEL) && !defined(SWETINPUT)
  Real ice_frac[2];
  #endif 

  #ifdef TOPMODEL
  Real wtd_t,wetfrac_t;
  #endif

  //initialize variables
  soiltemp_t=drh=0.0;

  //note: daily outputs are reset to zero in daily loop in iterateyear.c
  gp_pft=malloc(sizeof(Real)*(npft+2*(ncft+NGRASS)));


  //If no sw or lw input
  #ifndef SWRAD
    #ifndef SWLWRAD
      swdown_t=petpar(&daylength,&par,&pet,cell->coord.lat,day,temp,sun);
    #endif
  #endif
  
  #ifdef SWLWRAD
    swdown_t=petparRAD(&daylength,&par,&pet,cell->coord.lat,day,temp,swdown,lwdown);
  #endif
  
  #ifdef SWRAD
    swdown_t=petparRAD(&daylength,&par,&pet,cell->coord.lat,day,temp,swdown,lwdown);
  #endif

  //update monthly climate variables
  cell->output.mtair+=temp;
  cell->output.mppt+=prec;
  if(prec>0){
    cell->output.mwetdays+=1.0;
  }
  cell->output.mswdown+=swdown_t;
  cell->output.mlwdown+=lwdown;
  cell->output.mcld+=100-sun;
  cell->output.mco2conc+=co2;

  //update daily climate variables
  cell->output.dtair=temp;
  cell->output.dppt=prec;
  cell->output.dswdown=swdown_t;
  cell->output.dlwdown=lwdown;
  cell->output.dcld=100-sun;

  //update monthly par pet
  cell->output.mpar+=par;
  cell->output.mpet+=pet;

  #ifdef DEBUG3
    printf( "par= %.2f  pet= %.5f daylength= %.5f \n",par,pet,daylength);
    printf( "wet=%f,temp= %.2f prec= %.2f sun= %.2f sw= %.2f lw= %.2f \t co2=%f\n",wet,temp,prec,sun,swdown,lwdown,co2);
  #endif

  gtemp_air=temp_response(temp);
  daily_climbuf(&cell->climbuf,temp);

  if(existlandusetype(cell->standlist,&s,SETASIDE)){
    stand=getstand(cell->standlist,s);
    soilmoist=stand->soil.w[0];
  }

  calc_cropdates(pftpar,&cell->climbuf,&cell->cropdates,cell->coord.lat,day,npft,ncft,soilmoist);
  flux_estab=sowing(cell->standlist,&cell->cropdates,pftpar,&cell->climbuf,landfrac,cell->coord.lat,day,npft,ncft,ntypes,&cell->output);

  cell->discharge.drunoff=0.0;

  foreachstand(stand,s,cell->standlist)
  {
    //-----------------------------------------------------------------
    // NOTE: physical variables are output as area-weighted averages
    //      ..by correcting for stand fractional area in grid-cell
    //      flux variables are output as area-weighted sums
    //      ..also by correcting for stand fractional area
    //-----------------------------------------------------------------

    negbm=FALSE;
    /* allocate temporary arrays for phen and wet */
    foreachsoillayer(l)  aet_stand[l]=0.0;
    cover_stand=intercep_stand=0.0;
    #if defined(TOPMODEL) && !defined(SWETINPUT)
      foreachsoillayer(l) ice_frac[l]=0.0;
    #endif

    #ifdef PERMAFROST
      //For Testing set to peatland
      stand->permafrost.peathydr = FALSE;
        
      //daily snow depth calculation from LPJWHYME
      snownew(temp,prec,melt,&stand->permafrost,year,day);
        
      //original soiltemp
      soiltemp_t=soiltemp(&stand->soil,&cell->climbuf)/ndaysmonth;
      //soiltemp calculation from LPJWHYME
      soiltempnew(&stand->permafrost,&stand->soil,stand->pftlist,npft,temp,soiltemp_t*ndaysmonth,day,month,year);
         
      //temporaty vairables of ice fraction for topmodel calucaltion
      #if defined(TOPMODEL) && !defined(SWETINPUT)
        ice_frac[0] = stand->permafrost.ice_frac1*stand->frac;
        ice_frac[1] = stand->permafrost.ice_frac2*stand->frac;
      #endif
      gtemp_soil=temp_response(stand->permafrost.Tsoil_top);
      drh=littersom(&stand->soil,gtemp_soil)*stand->frac;
      
      cell->output.msoiltemp+=soiltemp_t*stand->frac;
      //Export soil temperature and soil moisture for the 8 soil layers
      cell->output.mtsoil_0+=stand->permafrost.Tsoil_0*stand->frac;
      cell->output.mtsoil_10+=stand->permafrost.Tsoil_10*stand->frac;
      cell->output.mtsoil_20+=stand->permafrost.Tsoil_20*stand->frac;
      cell->output.mtsoil_25+=stand->permafrost.Tsoil_25*stand->frac;
      cell->output.mtsoil_50+=stand->permafrost.Tsoil_50*stand->frac;
      cell->output.mtsoil_70+=stand->permafrost.Tsoil_70*stand->frac;
      cell->output.mtsoil_100+=stand->permafrost.Tsoil_100*stand->frac;
      cell->output.mtsoil_150+=stand->permafrost.Tsoil_150*stand->frac;
      cell->output.mtsoil_200+=stand->permafrost.Tsoil_200*stand->frac;
      //depth-weighted average temperture for top 4 layers
      cell->output.mtsoil_top+=stand->permafrost.Tsoil_top*stand->frac;  
      cell->output.mthaw_depth+=stand->permafrost.thawdepth*stand->frac;
      
      cell->output.mFwater_10+=stand->permafrost.Fwater[3]*stand->frac;
      cell->output.mFwater_20+=stand->permafrost.Fwater[4]*stand->frac;
      cell->output.mFwater_30+=stand->permafrost.Fwater[5]*stand->frac;
      cell->output.mFwater_50+=stand->permafrost.Fwater[6]*stand->frac;
      cell->output.mFwater_70+=stand->permafrost.Fwater[7]*stand->frac;
      cell->output.mFwater_100+=stand->permafrost.Fwater[8]*stand->frac;
      cell->output.mFwater_150+=stand->permafrost.Fwater[9]*stand->frac;
      cell->output.mFwater_200+=stand->permafrost.Fwater[10]*stand->frac;
      cell->output.mFice_10+=stand->permafrost.Fice[3]*stand->frac;
      cell->output.mFice_20+=stand->permafrost.Fice[4]*stand->frac;
      cell->output.mFice_30+=stand->permafrost.Fice[5]*stand->frac;
      cell->output.mFice_50+=stand->permafrost.Fice[6]*stand->frac;
      cell->output.mFice_70+=stand->permafrost.Fice[7]*stand->frac;
      cell->output.mFice_100+=stand->permafrost.Fice[8]*stand->frac;
      cell->output.mFice_150+=stand->permafrost.Fice[9]*stand->frac;
      cell->output.mFice_200+=stand->permafrost.Fice[10]*stand->frac;
      
      cell->output.msnowpack+=(stand->permafrost.snowdepth/ndaysmonth)*stand->frac;
      cell->output.drh+=drh;
      cell->output.mrh+=drh;

      //set set permafrost soiltemp to soiltemp_t for CH4E calculation
      soiltemp_t=stand->permafrost.Tsoil_top;
         
      if(stand->permafrost.frozen_day[3] && stand->permafrost.frozen_day[4] && stand->permafrost.frozen_day[5] && stand->permafrost.frozen_day[6]){   
        //all the 4 top soil layers
        cell->output.frozen_days+=1.0;
      }
    #else
      gtemp_soil=temp_response(soiltemp(&stand->soil,&cell->climbuf));
      //Calculate total aboveground litter for litter depth calculation
      drh=littersom(&stand->soil,gtemp_soil)*stand->frac;
      soiltemp_t=soiltemp(&stand->soil,&cell->climbuf)/ndaysmonth;
      cell->output.msoiltemp+=soiltemp_t*stand->frac;
      //Output mean monthly snowpack
      cell->output.msnowpack+=(stand->soil.snowpack/ndaysmonth)*stand->frac;
      cell->output.drh+=drh;
      cell->output.mrh+=drh;
    #endif //..end permafrost
     

    #if defined(OUTPUT_BYSTAND) || defined(OUTPUT_BYAGECLASS)
      //stand-level outputs
      if(stand->landusetype==PRIMARY){
        #ifdef OUTPUT_BYSTAND
          cell->output.mrh_primary+=drh;
        #endif

        #ifdef OUTPUT_BYAGECLASS
          //stored in stand structure
          cell->output.ageclass_pri_mrh[stand->ageclass_standid-1]+=drh;
        #endif
      }else if(stand->landusetype==SECFOREST){
        #ifdef OUTPUT_BYSTAND
          cell->output.mrh_secforest+=drh;
        #endif

        #ifdef OUTPUT_BYAGECLASS
          //stored in stand structure
          cell->output.ageclass_sec_mrh[stand->ageclass_standid-1]+=drh;
        #endif
      }
    #endif

    //Set precipitation to zero and add precipitation to snowpack if temperature < 0
    cell->discharge.drunoff+=snow(&stand->soil.snowpack,&prec,&melt,temp)*stand->frac;

    if(stand->landusetype==PRIMARY){
      cell->output.dFwater_10+=stand->permafrost.Fwater[3]*stand->frac;
      cell->output.dFwater_20+=stand->permafrost.Fwater[4]*stand->frac;
      cell->output.dFwater_30+=stand->permafrost.Fwater[5]*stand->frac;
      cell->output.dFwater_50+=stand->permafrost.Fwater[6]*stand->frac;
      cell->output.dFwater_70+=stand->permafrost.Fwater[7]*stand->frac;
      cell->output.dFwater_100+=stand->permafrost.Fwater[8]*stand->frac;
      cell->output.dFwater_150+=stand->permafrost.Fwater[9]*stand->frac;
      cell->output.dFwater_200+=stand->permafrost.Fwater[10]*stand->frac;
      cell->output.dFice_10+=stand->permafrost.Fice[3]*stand->frac;
      cell->output.dFice_20+=stand->permafrost.Fice[4]*stand->frac;
      cell->output.dFice_30+=stand->permafrost.Fice[5]*stand->frac;
      cell->output.dFice_50+=stand->permafrost.Fice[6]*stand->frac;
      cell->output.dFice_70+=stand->permafrost.Fice[7]*stand->frac;
      cell->output.dFice_100+=stand->permafrost.Fice[8]*stand->frac;
      cell->output.dFice_150+=stand->permafrost.Fice[9]*stand->frac;
      cell->output.dFice_200+=stand->permafrost.Fice[10]*stand->frac;
    }

    #ifdef TOPMODEL
      if(stand->landusetype==PRIMARY){
        wtd_t = wtd(stand->soil.w,&stand->soil);
        cell->output.wtd+=wtd_t;  //water table depth
              
        #ifdef SWETINPUT
          wetfrac_t=minund; //get wetarea from satellite products
          cell->output.wetfrac+=minund; //still export wetfrac output by using TOPMODEL
        #else
          wetfrac_t=wetdfrac(&cell->topmodel,&stand->soil,wtd_t,ice_frac); //wetland fraction
          cell->output.wetfrac+=wetfrac_t;
        #endif
              
        //wetfrac for methane emission calculation
        cell->output.dwet_frac=wetfrac_t;
        cell->output.dch4e+=ch4e(&cell->topmodel,&cell->coord,soiltemp_t,drh,wetfrac_t); // CH4 emission
        cell->output.mch4e+=cell->output.dch4e;
        //oxidated CH4 = CH4 production - CH4 emission
        cell->output.ch4o+=ch4o(&cell->topmodel,&cell->coord,soiltemp_t,drh,wetfrac_t);
      }
    #endif

    if(iffire && temp>0 && (stand->landusetype == PRIMARY || stand->landusetype == SECFOREST))
      stand->fire_sum+=fire_sum(&stand->soil.litter,stand->soil.w[0]);
    
    //stand-level GPP (max, non-water stressed)
    gp_stand=gp_sum(stand->pftlist,co2,temp,par,daylength,&gp_stand_leafon,gp_pft,&fpc_total_stand);

    foreachpft(pft,p,stand->pftlist)
    {
      foreachsoillayer(l)
        aet_pft[l]=0.;

      if(!leaf_phenology(pft,npft,temp,cell->coord.lat,cell->laimax,day,daylength,
                         stand->irrigation,stand->landusetype,mtemp,par*2/86400)) {//swrad as par x 2 (W/m^2)
        dummy=FALSE;
        if(stand->landusetype==GRASSLAND){
          allocation_today(stand,&dummy,ntypes);
          harvest=harvest_grass(stand,pft,&rharvest);
          index=ncft;
        }else{ 
          index=pft->par->id-npft;
          harvest=litter_update(&stand->soil.litter,pft,0.5,&rharvest,TRUE,TRUE);
        }

        cell->output.flux_harvest+=(harvest+rharvest)*stand->frac;

        if(stand->irrigation){
          cell->output.pft_harvest[index+ncft+NGRASS]+=harvest;   
          cell->output.pft_rharvest[index+ncft+NGRASS]+=rharvest; 
        }else{
          cell->output.pft_harvest[index]+=harvest;        
          cell->output.pft_rharvest[index]+=rharvest;           
        } 

        if(stand->landusetype==AGRICULTURE){
          croppar=pft->par->data;
          if(!stand->irrigation)
            cell->cropdates.fallow[pft->par->id-npft]=croppar->fallow_days;
          else
            cell->cropdates.fallow[pft->par->id-npft+ncft]=croppar->fallow_days;
          delpft(stand->pftlist,p);
          p--;
          stand->landusetype=KILL;
          continue;
        }
      }//..end if not leaf_phenology()

      cover_stand+=pft->fpc*pft->phen;

      intercep_stand+=interception(&wet,pft,pet,prec+stand->irrig_amount);
      //printf("%f\t%d\n",stand->soil.soildepth[0],pft->par->id);

      //-------------------------------------------------------------------------
      //                 E C O S Y S T E M   F U N C T I O N 
      //                      NEP  NPP  GPP  Ra  RH
      //
      //  Calculate net assimilation, gross primary production minus leaf
      //  respiration, including conversion from FPC to grid cell basis
      //
      //-------------------------------------------------------------------------
      wdf=-9;

      //.............................
      //   G P P   &   R a  (leaf)
      //.............................
      #ifdef PERMAFROST
      w_pi[0]=stand->soil.w[0]*(1-stand->permafrost.ice_frac1);
      w_pi[1]=stand->soil.w[1]*(1-stand->permafrost.ice_frac2);
      gpp=water_stressed(pft,aet_stand,w_pi,gp_stand,gp_stand_leafon,
                         gp_pft[getpftpar(pft,id)],&gc_pft,fpc_total_stand,&rd,&wet,pet,co2,temp,par,daylength,
                         stand->irrigation,stand->soil.whcs,
                         stand->irrig_amount,&wdf,aet_pft, ozone, &fo3uptake, cell->coord.lat,cell->coord.lon,cell->elev,&ci);
      #else
      gpp=water_stressed(pft,aet_stand,stand->soil.w,gp_stand,gp_stand_leafon,
                         gp_pft[getpftpar(pft,id)],&gc_pft,fpc_total_stand,&rd,&wet,pet,co2,temp,par,daylength,
                         stand->irrigation,stand->soil.whcs, 
                         stand->irrig_amount,&wdf,aet_pft, ozone, &fo3uptake, cell->coord.lat,cell->coord.lon,cell->elev,&ci);
      #endif

      //update monthly daily gpp
      cell->output.dgpp+=gpp*stand->frac;
      cell->output.mgpp+=gpp*stand->frac;

      #ifdef BVOC
        cell->output.misopr+=isopr*stand->frac; 
        cell->output.mmonopr+=monopr*stand->frac;
      #endif
   
      #if defined(OUTPUT_BYSTAND)
        //stand-level outputs
        if(stand->landusetype==PRIMARY){
          cell->output.mgpp_primary+=gpp*stand->frac;
        }else if(stand->landusetype==SECFOREST){
          cell->output.mgpp_secforest+=gpp*stand->frac;
        }
      #endif

      /*
         printf( "day=%d,stand=%d,pft=%d,sm[0]=%.4f,sm[1]=%.4f,gpp=%.4f,wet=%.4f,temp=%.4f,prec=%.4f,sun=%.4f,sw=%.4f,lw= %.4f,co2=%f,pet=%.4f,par=%.4f\n",
                 day,stand->landusetype,pft->par->id,stand->soil.w[0],stand->soil.w[1],gpp,wet,temp,prec,sun,swdown,lwdown,co2,pet,par);
      */

      //PHENDATE
      if(stand->landusetype==PRIMARY || stand->landusetype==SECFOREST){
        if(day == 1){
          pft->DailyFPAR=0.; //Macros know where in array to put value
        }else if(day > 1){
          //If fpar is greater than existing fpar, update fpar max and return the day
          //Use greater than to take first max fpar day
          if(fpar(pft) > pft->DailyFPAR){
            pft->DailyFPAR=fpar(pft);
             cell->output.pft_maxphenday[getpftpar(pft,id)]=day;
          }
        }
      }

      if(stand->landusetype==PRIMARY || stand->landusetype==SECFOREST){
        if(day % 7 == 1){
        b = (day-1)/7;
           cell->output.pft_bimonfpar[pft->par->id+(b*npft)]=fpar(pft);
        }
      }

      if(gp_pft[getpftpar(pft,id)]>0.0) {
       gcgp=gc_pft/gp_pft[getpftpar(pft,id)];
       if(stand->irrigation){
         if(stand->landusetype==GRASSLAND){
           cell->output.gcgp_count[npft+2*(ncft+NGRASS)-1]+=1;
           cell->output.pft_gcgp[npft+2*(ncft+NGRASS)-1]+=gcgp;
         }else{
           cell->output.gcgp_count[pft->par->id+ncft+NGRASS]+=1;
           cell->output.pft_gcgp[pft->par->id+ncft+NGRASS]+=gcgp;
         }
       }else{
         if(stand->landusetype==GRASSLAND){
           cell->output.gcgp_count[npft+ncft+NGRASS-1]+=1;
           cell->output.pft_gcgp[npft+ncft+NGRASS-1]+=gcgp;
         }else{
           cell->output.gcgp_count[pft->par->id]+=1;
           cell->output.pft_gcgp[pft->par->id]+=gcgp;
         }
       }
      }


      //...............................
      //    N P P   &   R a  (total)
      //...............................
      //net primary production; assimilation is gpp-leaf respiration (rd)
      npp=npp(pft,gtemp_air,gtemp_soil,(gpp-rd),&negbm,wdf,&bgaresp)*stand->frac;
      //update npp and ra daily (d),monthly (m)
      cell->output.dnpp+=npp;
      cell->output.mnpp+=npp;
      cell->output.mra+=bgaresp;

      #if defined(OUTPUT_BYSTAND) || defined(OUTPUT_BYAGECLASS)
        //stand-level outputs
        if(stand->landusetype==PRIMARY){
          #ifdef OUTPUT_BYSTAND
            cell->output.mnpp_primary+=npp;
          #endif

          #ifdef OUTPUT_BYAGECLASS
            //stored in stand structure
            cell->output.ageclass_pri_mnpp[stand->ageclass_standid-1]+=npp;
          #endif
        }else if(stand->landusetype==SECFOREST){
          #ifdef OUTPUT_BYSTAND
            cell->output.mnpp_secforest+=npp;
          #endif

          #ifdef OUTPUT_BYAGECLASS
            //stored in stand structure
            cell->output.ageclass_sec_mnpp[stand->ageclass_standid-1]+=npp;
          #endif
        }
      #endif

      //Ideal gas law (from Renato Spahni)
      molcv=8.314*(temp+273.15)/100000*1000;

      //...................................
      // U p d a t e   P F T  o u t p u t
      // ..internal fn
      //...................................
      update_pftOutputs_daily(cell,stand,pft,month,ndaysmonth,npft,ncft,molcv,gc_pft,daylength,npp,gpp,ci,aet_pft,fo3uptake);

      if(negbm){
        croppar=pft->par->data;
        if(!stand->irrigation)
          cell->cropdates.fallow[pft->par->id-npft]=croppar->fallow_days;
        else
          cell->cropdates.fallow[pft->par->id-npft+ncft]=croppar->fallow_days;
        harvest=litter_update(&stand->soil.litter,pft,pft->nind,&rharvest,FALSE,TRUE);
        cell->output.flux_harvest+=(rharvest+harvest)*stand->frac;
        if(stand->irrigation){
          cell->output.pft_harvest[pft->par->id-npft+ncft+NGRASS]+=harvest; 
          cell->output.pft_rharvest[pft->par->id-npft+ncft+NGRASS]+=rharvest; 
        }else{
          cell->output.pft_harvest[pft->par->id-npft]+=harvest;               
          cell->output.pft_rharvest[pft->par->id-npft]+=rharvest;            
        }
        delpft(stand->pftlist,p);
        stand->landusetype=KILL;
        p--;
        continue;
      }
    } /* of foreachpft */

    //----------------------------------
    //    W a t e r   B a l a n c e
    //----------------------------------
    #ifdef PERMAFROST
    cell->discharge.drunoff+=waterbalancenew(&stand->permafrost,&stand->soil,stand->pftlist,aet_stand,&evap,
                                        prec+melt+stand->irrig_amount-intercep_stand,
                                        pet,cover_stand,cell->coord.lat,
                                        cell->coord.lon,npft,year,day,stand->landusetype)*stand->frac;
    #else
    cell->discharge.drunoff+=waterbalance(&stand->soil,aet_stand,&evap,
                                        prec+melt+stand->irrig_amount-intercep_stand,
                                        pet,cover_stand,cell->coord.lat,
                                        cell->coord.lon,stand->landusetype)*stand->frac;
    #endif

    if((stand->landusetype==AGRICULTURE || stand->landusetype==GRASSLAND) && stand->irrigation)
      foreachpft(pft,p,stand->pftlist)
        irrigation(pft,stand->soil.w,gp_stand,&wet,pet,stand->soil.whcs,&stand->pot_irrig_amount);
    
    cell->output.mevap+=evap*stand->frac;
    foreachsoillayer(l){
      cell->output.mtransp+=aet_stand[l]*stand->frac;
      cell->output.mswc[l]+=stand->soil.w[l]*stand->frac;
    }
    cell->output.minterc+=intercep_stand*stand->frac;
    prec=prec1; /* reset prec to original value for next stand */
  } 
  // ..END OF STAND LOOP

  cell->output.mrunoff+=cell->discharge.drunoff;

  #if defined(RIVER_ROUTING)
    /* lake waterbalance */
    cell->discharge.dmass_lake+=prec*cell->coord.area*cell->lakefrac;
    if(cell->discharge.dmass_lake>cell->discharge.dmass_lake_max){
      cell->discharge.dmass_new+=cell->discharge.dmass_lake-cell->discharge.dmass_lake_max;
      cell->discharge.dmass_lake=cell->discharge.dmass_lake_max;
    }
  
    cell->output.aevap_lake+=min(cell->discharge.dmass_lake/cell->coord.area,pet*ALPHAM*cell->lakefrac);
    cell->discharge.dmass_lake=max(cell->discharge.dmass_lake-pet*ALPHAM*cell->coord.area*cell->lakefrac,0.0);
  
    /* extra standloop for setaside routine needed to ensure closed water balance */
    foreachstand(stand,s,cell->standlist)
      if(stand->landusetype==KILL){
        cell->discharge.dmass_new+=stand->irrig_sum*cell->coord.area*stand->frac;
        cell->discharge.mfin+=stand->irrig_sum*cell->coord.area*stand->frac;
      }
  #endif

  foreachstand(stand,s,cell->standlist)
    if(stand->landusetype==KILL)
      setaside(cell,&s,pftpar,intercrop,npft,TRUE); //TRUE to allow establishment
    
  check_stand_fracs(cell->standlist,cell->lakefrac);

  /* Establishment fluxes are weighted by stand fractional area in subroutines */
  //this establishment flux is from crop sowing
  cell->output.flux_estab+=flux_estab;

  #ifdef DAILYOUTPUT  
    if(config.firstdyear != 9999 & year>=config.firstdyear)
        fwriteoutput_daily(output,cell,day,month,year);
  #endif 

  free(gp_pft);
} /* of 'update_daily' */


