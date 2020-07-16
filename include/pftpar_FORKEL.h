/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / p f t p a r _ F O R K E L . h               **/
/*******************************************************************/

#ifndef PFTPAR_H /* Already included? */
#define PFTPAR_H

/* Definition of constant for pft parameter */
#define EVERGREEN 0
#define RAINGREEN 1
#define SUMMERGREEN 2
#define ANY 3
#define CROPGREEN 4
#define NOPATHWAY 0
#define C3 1
#define C4 2
#define BROADLEAVED 0
#define NEEDLELEAVED 1
#define ANYLEAVED 2
#define GRASS 0
#define TREE 1
#define CROP 2
/* CROP must always last id because of ntypes, see also lpj.c */
#define TROPICAL_BROADLEAVED_EVERGREEN_TREE 0
#define TROPICAL_BROADLEAVED_RAINGREEN_TREE 1
#define TEMPERATE_NEEDLELEAVED_EVERGREEN_TREE 2
#define TEMPERATE_BROADLEAVED_EVERGREEN_TREE 3
#define TEMPERATE_BROADLEAVED_SUMMERGREEN_TREE 4
#define BOREAL_NEEDLELEAVED_EVERGREEN_TREE 5
#define BOREAL_BROADLEAVED_SUMMERGREEN_TREE 6
#define BOREAL_NEEDLELEAVED_SUMMERGREEN_TREE 7
#define C3_PERENNIAL_GRASS 8
#define C3_POLAR_GRASS 9
#define C4_PERENNIAL_GRASS 10
#define TEMPERATE_CEREALS 11
#define RICE 12
#define MAIZE 13
#define TROPICAL_CEREALS 14
#define PULSES 15
#define TEMPERATE_ROOTS 16
#define TROPICAL_ROOTS 17
#define OIL_CROPS_SUNFLOWER 18
#define OIL_CROPS_SOYBEAN 19
#define OIL_CROPS_GROUNDNUT 20
#define OIL_CROPS_RAPESEED 21
#define NO_CALC_SDATE 0
#define PREC_CALC_SDATE 1
#define TEMP_WTYP_CALC_SDATE 2
#define TEMP_STYP_CALC_SDATE 3
#define TEMP_PREC_CALC_SDATE 4
#define MULTICROP 5

#define NGRASS 1

#define NPFT 11    //number of PFT must match npft at top of par/pft.par file
#endif
