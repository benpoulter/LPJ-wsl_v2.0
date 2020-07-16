/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/soil/ f s c a n s o i l p a r _ n c d f . c              **/
/*******************************************************************/
#include "lpj.h"

void fscansoilpar_ncdf(Soilpar **soilpar,   /* Pointer to Soilpar array */
    Real usda_top,
    Real oc_top,
    Real sand_top,
    Real clay_top,
    Real silt_top,
    Real oc_bot,
    Real sand_bot,
    Real clay_bot,
    Real silt_bot)
{
  //Saxton variables
  Real theta_1500t_top, theta_1500_top, theta_33t_top, theta_33_top;
  //Real theta_1500t_bot, theta_33t_bot, theta_1500_bot, theta_33_bot;
  Real theta_s_min_33t, theta_s_min_33, theta_s, B, lambda, ks;
  Real oc, sand, clay; //silt
  Real om, om_top; 
  Soilpar *soil;
  
  //COSBY variables
  Real soil_b_top, vwc_sat_top, psi_sat_top, psi_log_top, vwc_fc_top, vwc_wp_top, hold_top;
  Real soil_b_bot, vwc_sat_bot, psi_sat_bot, psi_log_bot, vwc_fc_bot, vwc_wp_bot, hold_bot;
  //Set up soil par structure
  *soilpar=newvec(Soilpar,1);
  check(*soilpar);

  //Calculate average for top and bottom layer
  oc=(oc_top+oc_bot)/2;
  sand=(sand_top+sand_bot)/2;
  clay=(clay_top+clay_bot)/2;
  //silt=(silt_top+silt_bot)/2;
  
  //Sept 2012 - bug fixed where OM should be used in Saxton, not OC
  //Convert OC to OM to work with Saxton equations
  om=oc*2;
  om_top=oc_top*2;
  //om_bot=oc_bot*2;

  //Calculate Hmax
  theta_1500t_top= -0.024*sand_top + 0.487*clay_top + 0.006*om_top + 0.005*(sand_top*om_top) - 0.013*(clay_top * om_top) + 0.068*(sand_top*clay_top) + 0.031;
  theta_1500_top= theta_1500t_top + (0.14*theta_1500t_top - 0.02);
  theta_33t_top=-0.251*sand_top + 0.195*clay_top + 0.011*om_top + 0.006*(sand_top*om_top) - 0.027*(clay_top*om_top) + 0.452*(sand_top*clay_top) + 0.299;
  theta_33_top=theta_33t_top + (1.283*pow(theta_33t_top,2) - 0.374*theta_33t_top - 0.015);

  //theta_1500t_bot= -0.024*sand_bot + 0.487*clay_bot + 0.006*om_bot + 0.005*(sand_bot*om_bot) - 0.013*(clay_bot * om_bot) + 0.068*(sand_bot*clay_bot) + 0.031;
  //theta_1500_bot= theta_1500t_bot + (0.14*theta_1500t_bot - 0.02);
  //theta_33t_bot=-0.251*sand_bot + 0.195*clay_bot + 0.011*om_bot + 0.006*(sand_bot*om_bot) - 0.027*(clay_bot*om_bot) + 0.452*(sand_bot*clay_bot) + 0.299;
  //theta_33_bot=theta_33t_bot + (1.283*pow(theta_33t_bot,2) - 0.374*theta_33t_bot - 0.015);

  //Calculate kperc for top layer
  theta_s_min_33t=0.278*sand+0.034*clay+0.022*om-0.018*(sand*om)-0.027*(clay*om)-0.584*(sand*clay)+0.078;
  theta_s_min_33=theta_s_min_33t+(0.636*theta_s_min_33t-0.107);
  theta_s=theta_33_top+theta_s_min_33-0.097*sand+0.043;
  B=(log(1500)-log(33))/(log(theta_33_top)-log(theta_1500_top));
  lambda=1/B;
  ks=1930*pow((theta_s - theta_33_top),(3-lambda));
  
  //COSBY WHC approach
  //1. Calculate the slope of the retention curve, b
  //2. Calculate saturated moisture content, theta_s, divide by 100. to have volumetric soil water concentration at saturation
  //3. Calculate saturated matric potential, psi_s
  //4. Convert to MPa (ln(10) = 2.3025851, 1 Pa = 1/9810 m, 1 cm = MPa *9.81e-5)
  //5. Calculate field capacity, theta
  //For many herbaceous at field capacity, psi = -1 to -3 m (or ~0.015 MPa )
  //Should use 0.033 MPa instead
  //6. Calculate wilting point, theta
  //For many herbaceous at wilting point, psi = -150 m (or 1.5 MPa
  //7. Water holding capacity = field capacity - wilting point
  soil_b_top = (3.10 + 0.157*clay_top*100 - 0.003*sand_top*100);
  vwc_sat_top = (50.5 - 0.142*sand_top*100 - 0.037*clay_top*100)/100.;
  psi_sat_top = 1.54 - 0.0095*sand_top*100 + 0.0063*silt_top*100;
  psi_log_top = -(exp(psi_sat_top*2.3035851))*9.8e-5;
  vwc_fc_top = vwc_sat_top*pow((-0.015/psi_log_top),(-1.0/soil_b_top))*1000.;
  vwc_wp_top = vwc_sat_top*pow((-1.5/psi_log_top),(-1.0/soil_b_top))*1000.;
  hold_top = (vwc_fc_top - vwc_wp_top)/1000;
  if(hold_top < 0)
    hold_top=0;

  soil_b_bot = (3.10 + 0.157*clay_bot*100 - 0.003*sand_bot*100);
  vwc_sat_bot = (50.5 - 0.142*sand_bot*100 - 0.037*clay_bot*100)/100.;
  psi_sat_bot = 1.54 - 0.0095*sand_bot*100 + 0.0063*silt_bot*100;
  psi_log_bot = -(exp(psi_sat_bot*2.3035851))*9.8e-5;
  vwc_fc_bot = vwc_sat_bot*pow((-0.015/psi_log_bot),(-1.0/soil_b_bot))*1000.;
  vwc_wp_bot = vwc_sat_bot*pow((-1.5/psi_log_bot),(-1.0/soil_b_bot))*1000.;
  hold_bot = (vwc_fc_bot - vwc_wp_bot)/1000;
  if(hold_bot < 0)
    hold_bot=0;
  //End COSBY approach
                            
  //Put data in the first location (0) of soil par structure
  soil=(*soilpar)+0;
  soil->name="USDA";
  soil->type=0;
  soil->k2=2;  //the percolation rate in Sitch 2003 is non-linear/exponential to the power of 2
  soil->k1=ks; //values in Sitch 2003 are mm/day, but seems incorrect and should be mm/hr
	if(usda_top == 1){
	  soil->k1=1.0;  //empirical parameter in percolation equation (K1) (mm/day)
	  soil->tdiff_15=0.5;  //thermal diffusivity (mm^2/s) at 15% whc
	  soil->f=3.2;  //soil hydaulic parameter for TOPMODEL
      soil->wat_hld=0.066;  //water held below wilting point, important for heat conductance
	  soil->porosity=0.468;  //porosity from AGRMET Handbook, 2002
	}	//clay heavy
	else if(usda_top == 2){
	  soil->k1=3.0;
	  soil->tdiff_15=0.5;
	  soil->f=3.1;
      soil->wat_hld=0.119;
	  soil->porosity=0.468;
	} //silty clay
	else if(usda_top == 3){
	  soil->k1=3.0;
	  soil->tdiff_15=0.5;
	  soil->f=2.8;
      soil->wat_hld=0.139;
	  soil->porosity=0.468;
	} //clay
	else if(usda_top == 4){
	  soil->k1=4.0;
	  soil->tdiff_15=0.575;
	  soil->f=2.9;
      soil->wat_hld=0.119;
	  soil->porosity=0.464;
	} //silty clay loam
	else if(usda_top == 5){
	  soil->k1=4.0;
	  soil->tdiff_15=0.575;
	  soil->f=2.7;
      soil->wat_hld=0.103;
	  soil->porosity=0.465;
	} //clay loam
	else if(usda_top == 6){
	  soil->k1=4.0;
	  soil->tdiff_15=0.575;
	  soil->f=3.4;
      soil->wat_hld=0.119;
	  soil->porosity=0.476;
	} //silt
	else if(usda_top == 7){
	  soil->k1=4.0;
	  soil->tdiff_15=0.65;
	  soil->f=2.6;
      soil->wat_hld=0.119;
	  soil->porosity=0.476;
	} //silt loam
	else if(usda_top == 8){
	  soil->k1=4.0;
	  soil->tdiff_15=0.725;
	  soil->f=2.5;
      soil->wat_hld=0.020;
	  soil->porosity=0.406;
	} //sandy clay
	else if(usda_top == 9){
	  soil->k1=4.5;
	  soil->tdiff_15=0.65;
	  soil->f=2.5;
      soil->wat_hld=0.047;
	  soil->porosity=0.439;
	} //loam
	else if(usda_top == 10){
	  soil->k1=4.5;
	  soil->tdiff_15=0.65;
	  soil->f=2.4;
      soil->wat_hld=0.069;
	  soil->porosity=0.404;
	} //sandy clay loam
	else if(usda_top == 11){
	  soil->k1=4.5;
	  soil->tdiff_15=0.65;
	  soil->f=2.3;
      soil->wat_hld=0.069;
	  soil->porosity=0.434;
	} //sandy loam
	else if(usda_top == 12){
	  soil->k1=5.0;
	  soil->tdiff_15=0.65;
	  soil->f=2.2;
      soil->wat_hld=0.029;
	  soil->porosity=0.421;
	} //loamy sand
	else if(usda_top == 13){
	  soil->k1=9.0;
	  soil->tdiff_15=0.8;
	  soil->f=2.1;
      soil->wat_hld=0.02;
	  soil->porosity=0.339;
	} //sand
  
  soil->whc[0]=hold_top;
  soil->whc[1]=hold_bot;
        
  //Matched USDA soil texture class with LPJ look up table
  soil->tdiff_0=0.2; //IGNORES EFFECT OF ORGANIC MATTER - SHOULD SET A Hmax THRESHOLD
  soil->tdiff_100=0.4;//IGNORES EFFECT OF ORGANIC MATTER - SHOULD SET A Hmax THRESHOLD
  soil->min_con=1-oc - soil->porosity;
  soil->org_con=oc;

} /* of 'fscansoilpar' */
