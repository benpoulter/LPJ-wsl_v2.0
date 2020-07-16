/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / c r o p d a t e s . h                       **/
/*******************************************************************/

#ifndef CROPDATES_H /* Already included? */
#define CROPDATES_H

/* Definition of constants */

/* Declaration of datatypes */

typedef struct
{
  int *fall_sdate20;
  /* Definition: Day when temperature drops below a cft-specific threshold in autumn
   * Saved as 20-year running mean
   * Used for temperate cereals, rapeseeds
   * ABUSED for rice, maize, tropical cereals, pulses, temperate roots, tropical roots, 
   * soybean, groundnuts to store current year's sowing date */
  int *last_update_fall;
  /* Counts years since last non-zero value of fallsdate20 */
  
  int *spring_sdate20;
  /* Definition: Day when spring temperature rises above cft-specific threshold in spring
   * Saved as 20-year running mean
   * Used for temperate cereals, rapeseeds, sunflowers, maize, rice (multicropping) */
  int *last_update_spring;
  /* Counts years since last non-zero value of springsdate20 */
  
  int *vern_date20;
  /* Definition: Day when temperature rises above a cft-specific threshold
   * Saved as 20-year running mean
   * Used for temperate cereals and rapeseeds to determine end of vernalization */
  int *last_update_vern;
  /* Counts years since last non-zero value of verndate20 */
  
  int *fallow;
  /* counts days to wait before next crop cycle of each cft to control multicropping */
  
}Cropdates; 

/* Declaration of functions */
extern void init_cropdates(const Pftpar *,Cropdates *,int,int,Real);
extern void calc_cropdates(const Pftpar *,const Climbuf *,Cropdates *,Real,int,int,int,Real);
extern void freecropdates(Cropdates *);
extern Bool fwritecropdates(FILE *,const Cropdates *,int);
extern Bool freadcropdates(FILE *,Cropdates *,int,Bool);

/* Definitions of macros */

#endif
