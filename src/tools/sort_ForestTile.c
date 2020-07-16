/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ s o r t _ F o r e s t T i l e . c                 **/
/*******************************************************************/
#include "lpj.h"

void sort_ForestTile(Standlist standlist, int AGE_or_DBH, int MGLAND_or_FOREST, int PRIMARY_or_SECFOREST)
{
  //DEVQ needs a pass for Age/DBH into function as char*
  int i,j;
  int TMP_VAL=100;
  Stand *stand_swap,*stand_current,*stand_next;

  #ifdef DEBUGLC
    //test sorting Before
    foreachstand(stand_current,i,standlist){
      printf("BEFORE SORT: standID: %d, landuse#: %d, ageclass_standid: %d, standfrac: %f, soil_Cpool_slow: %f\n",
        i, stand_current->landusetype,stand_current->ageclass_standid, stand_current->frac,stand_current->soil.cpool.slow);
    }
    //end test print
  #endif

  //////////////////////////////////////////////////////////////////////////
  // sorting method is based on 'bubble' sort, common sorting algorithm
  //  ..less efficient than linked-lists, but more suitable for accessing
  //  ..list members by index than linked-list structure
  //  ..AND small array structs, bubble sort is sufficient
  //////////////////////////////////////////////////////////////////////////

  //------------------------------------------------------------
  // sorting preference: primary or secforest
  // ..(1) set landusetype to high number (100)
  // ..(2) sort all stands w/ landusetype==100 at front of list
  // ..(3) sort ageclass within same landusetype
  // ..(4) finally: set landusetype to original value
  //------------------------------------------------------------
  for(i=0; i< standlist->n; i++){
    stand_current=(Stand *)standlist->data[i];
    if(stand_current->landusetype==PRIMARY_or_SECFOREST){
      //set landusetype to tmp_val
      stand_current->landusetype=TMP_VAL;
    }
  }

  //------------------------------------------------------------
  // FIRST: sort by landusetype (all forest to front of list)
  //------------------------------------------------------------
  if(MGLAND_or_FOREST == FOREST){
    for(i=0;i< (standlist->n - 1);i++){
      for(j=0;j< (standlist->n - i - 1);j++){
        stand_current=(Stand *)standlist->data[j];
        stand_next=(Stand *)standlist->data[j+1];
        if(stand_current->landusetype < stand_next->landusetype){
          //use ">" comparison operator for increasing sort
          stand_swap=standlist->data[j];
          standlist->data[j]= standlist->data[j+1];
          standlist->data[j+1]= stand_swap;
        }
      }
    }
  }else if(MGLAND_or_FOREST == CROP){
    //////////////////
    /// DEVQ: doesn't work for now..
    /// ...bc stand-IDS use temp and dummy stands with negative landusetype values
    /// ... therefore, the temp and dummy stands would be first in list
    //////////////////////
    for(i=0;i< (standlist->n - 1);i++){
      for(j=0;j< (standlist->n - i - 1);j++){
        stand_current=(Stand *)standlist->data[j];
        stand_next=(Stand *)standlist->data[j+1];
        if(stand_current->landusetype > stand_next->landusetype){
          //use "<" comparison operator for decreasing sort
          stand_swap=standlist->data[j];
          standlist->data[j]= standlist->data[j+1];
          standlist->data[j+1]= stand_swap;
        }
      }
    }
  }

  //---------------------------------------------------------------------------
  // SECOND: sort by age (TSD)
  //  currently, the AGECLASS searches stands by standid --fixed ageClasses
  //---------------------------------------------------------------------------
  if(AGE_or_DBH == AGESTAND){
    for(i=0;i< (standlist->n - 1);i++){
      for(j=0;j< (standlist->n - i - 1);j++){
        stand_current=(Stand *)standlist->data[j];
        stand_next=(Stand *)standlist->data[j+1];
        if(stand_current->landusetype == stand_next->landusetype &&
            stand_current->ageclass_standid < stand_next->ageclass_standid){
          //non-secforests used ageclass_standids of zero
          //use ">" comparison operator for increasing sort (youngest first)
          //use "<" for decreasing sort (oldest first)
          stand_swap=standlist->data[j];
          standlist->data[j]= standlist->data[j+1];
          standlist->data[j+1]= stand_swap;
        }
      }
    }
  }else if(AGE_or_DBH == DBHSTAND){
    //Currently the not DBH, but reverse order as above -- youngest stands first, oldest last
    /////////////////////////////////////////////////////////////////
    // DEVQ: not working yetm needs comparison operators on total biomass, or avg.dbh
    //       ..currently not DBH, but reverse order as above, youngest stands first, oldest last
    ////////////////////////////////////////////////////////////////

    for(i=0;i< (standlist->n - 1);i++){
      for(j=0;j< (standlist->n - i - 1);j++){
        stand_current=(Stand *)standlist->data[j];
        stand_next=(Stand *)standlist->data[j+1];
        if(stand_current->landusetype == stand_next->landusetype &&
            stand_current->ageclass_standid > stand_next->ageclass_standid){
          //non-secforests used ageclass_standids of zero
          //use ">" comparison operator for increasing sort (youngest first)
          //use "<" for decreasing sort (oldest first)
          stand_swap=standlist->data[j];
          standlist->data[j]= standlist->data[j+1];
          standlist->data[j+1]= stand_swap;
        }
      }
    }
  }

  //---------------------------------------------------
  // reset stand landusetype to original value
  //---------------------------------------------------
  for(i=0; i< standlist->n; i++){
    stand_current=(Stand *)standlist->data[i];
    if(stand_current->landusetype==TMP_VAL){
      //reset landusetype to original value
      stand_current->landusetype=PRIMARY_or_SECFOREST;
    }
  }

  #ifdef DEBUGLC
    //test sorting After
    printf("Cell->standlist->: %d\n",standlist->n);
    foreachstand(stand_current,i,standlist){
      printf("AFTER SORT: standID: %d, landuse#: %d, ageclass_standid: %d, standfrac: %f, soil_Cpool_slow: %f\n",
        i, stand_current->landusetype,stand_current->ageclass_standid,stand_current->frac,stand_current->soil.cpool.slow);
    }
    //end test print
  #endif

  return;
}
