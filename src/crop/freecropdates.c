/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/crop/ f r e e c r o p d a t e s . c                      **/
/*******************************************************************/
#include "lpj.h"

void freecropdates(Cropdates *cropdates)
{
  free(cropdates->fall_sdate20);
  free(cropdates->fallow);
  free(cropdates->last_update_fall);
  free(cropdates->last_update_spring);
  free(cropdates->last_update_vern);
  free(cropdates->spring_sdate20);
  free(cropdates->vern_date20);
  /*free(cropdates);*/
} /* of 'freecropdates' */
