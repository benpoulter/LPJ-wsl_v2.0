/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / s t a n d . h                               **/
/*******************************************************************/

#ifndef STAND_H  /* Already included? */
#define STAND_H

#include "permafrost.h"
/* Definition of land use types */
//stand names & numbering necessary for sorting tiles
//    .. NATURAL: PRIMARY and SECFOREST (secondary)
//    .. temp states for LUC
//    .. global variable for age-classes
//    .. PASTURE class (CROP exists ==2 in pftpar.h) so that future LUC can make distinction
#define GRASSLAND 0
#define GRASSLAND_TEMP 1
#define SETASIDE 2
#define AGRICULTURE 3
#define MANAGEDFOREST 4
#define KILL 5
#define PRIMARY 6 
#define SECFOREST 7
#define SECFOREST_TEMP -1
#define PRIMARY_TEMP -2
#define STAND_DUMMY -3

//for AGECLASS
#define AGESTAND 0
#define DBHSTAND 1
#define MGLAND 0
#define FOREST 1

//for LUC distinguishing between crop and pasture
#define PASTURE 1 //for LUC deforestation rules

#if defined(AGECLASS_PRIMARY) || defined(AGECLASS_SECFOREST)
  #ifdef UNEQUAL_AGEWIDTH //not default
    //maximum within-stand transitions (AGECLASS)
    #define MAX_WITHINSTAND_TRANS 25

    #define AGECLASS_FYR_1 1
    #define AGECLASS_FYR_2 3
    #define AGECLASS_FYR_3 5
    #define AGECLASS_FYR_4 7
    #define AGECLASS_FYR_5 9
    #define AGECLASS_FYR_6 11
    #define AGECLASS_FYR_7 16
    #define AGECLASS_FYR_8 21
    #define AGECLASS_FYR_9 26
    #define AGECLASS_FYR_10 51
    #define AGECLASS_FYR_11 76
    #define AGECLASS_FYR_12 101

    #define AGECLASS_WIDTH_1 1
    #define AGECLASS_WIDTH_2 2
    #define AGECLASS_WIDTH_3 2
    #define AGECLASS_WIDTH_4 2
    #define AGECLASS_WIDTH_5 2
    #define AGECLASS_WIDTH_6 5
    #define AGECLASS_WIDTH_7 5
    #define AGECLASS_WIDTH_8 5
    #define AGECLASS_WIDTH_9 25
    #define AGECLASS_WIDTH_10 25
    #define AGECLASS_WIDTH_11 25
    #define AGECLASS_WIDTH_12 1
  #else //EQUAL_AGEWIDTH is default
    //maximum within-stand transitions (AGECLASS)
    #define MAX_WITHINSTAND_TRANS 50

    #define AGECLASS_FYR_1 1
    #define AGECLASS_FYR_2 11
    #define AGECLASS_FYR_3 21
    #define AGECLASS_FYR_4 31
    #define AGECLASS_FYR_5 41
    #define AGECLASS_FYR_6 51
    #define AGECLASS_FYR_7 61
    #define AGECLASS_FYR_8 71
    #define AGECLASS_FYR_9 81
    #define AGECLASS_FYR_10 91
    #define AGECLASS_FYR_11 101
    #define AGECLASS_FYR_12 151

    #define AGECLASS_WIDTH_1 10
    #define AGECLASS_WIDTH_2 10
    #define AGECLASS_WIDTH_3 10
    #define AGECLASS_WIDTH_4 10
    #define AGECLASS_WIDTH_5 10
    #define AGECLASS_WIDTH_6 10
    #define AGECLASS_WIDTH_7 10
    #define AGECLASS_WIDTH_8 10
    #define AGECLASS_WIDTH_9 10
    #define AGECLASS_WIDTH_10 10
    #define AGECLASS_WIDTH_11 50
    #define AGECLASS_WIDTH_12 1
  #endif

#else //NO ageclases
  //reqd, but not used
  #define MAX_WITHINSTAND_TRANS 1
#endif

/* Definition of datatypes */

typedef struct
{
  Pftlist pftlist;
  Soil soil;
  Permafrost permafrost;
  Real fire_sum;
  Real frac;
  int landusetype;
  Bool irrigation;
  Real pot_irrig_amount;
  Real irrig_amount;
  Real irrig_sum;
  int count;
  
  //for keeping track of crop-use land history
  int TimeSinceDist;
  int CropYrs;
  
  //for AGECLASS
  int  ageclass_standid;                       //for tracking age-classes
  int  len_frac_transition;                    //length of the vector for fractional transitions
  Real frac_transition[MAX_WITHINSTAND_TRANS]; //tracking within-stand fractional transitions
}Stand;

typedef List *Standlist;

/* Declaration of functions */

extern Bool fwritestand(FILE *,const Stand *,Bool);
extern void fprintstand(FILE *,const Stand *);
extern int fwritestandlist(FILE *,const Standlist,Bool);
extern void fprintstandlist(FILE *,const Standlist);
extern Stand *freadstand(FILE *,const Pftpar[],const Soilpar *,Bool);
extern Standlist freadstandlist(FILE *,const Pftpar [],const Soilpar *,Bool);
extern int addstand(List *);
extern void initstand (Stand *);
extern void delstand(List *,int);
extern int freestand(List *,int);
extern void reclaim_land(Stand *, Stand *,Real *,Real *);
extern Bool existlandusetype (List *, int *, Bool);
extern void mixsoil(Stand *,Stand *);
extern Real cultivate(List *,const Pftpar *,int,Real *,int,Bool,Real,int,Bool,int);
extern Bool check_lu(List *,Real *,int,Bool);
extern Real sowing(List *,Cropdates *,const Pftpar *,Climbuf *,Real *,Real,int,int,int,int,Output *);
extern void check_stand_fracs(List *,Real);
extern Bool findstand(List *,int *, Bool, Bool);
extern void allocation_today(Stand *, Bool *, int);
extern Real harvest_grass(Stand *,Pft *,Real *);
extern void copysoil(Stand *, Stand *);
extern int copy_pft(Pft *, Stand *, const Pftpar[], int);
extern Bool exist_pft(int , Stand *,int *);
extern void mixingRule_pft(Pft *, Real , Pft *, Real , const Pftpar [], int );
extern Bool ageclass_createORmix(List *, int *, Bool);
extern Bool find_exist_ageclass (List *, int , int*, int);
extern Bool ageclass_youngest(List *, int *, Bool);
extern Bool ageclass_oldest(List *, int *, Bool);
extern void sort_ForestTile(const Standlist, int, int, int);

/* Definition of macros */

#define getstand(list,index) ((Stand *)getlistitem(list,index))
#define foreachstand(stand,i,list) for(i=0;i<getlistlen(list) && (stand=getstand(list,i));i++)
#define printstandlist(standlist) fprintstandlist(stdout,standlist)

#endif
