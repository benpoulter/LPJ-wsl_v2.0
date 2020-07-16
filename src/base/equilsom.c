/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/base/ e q u i l s o m . c                                **/
/*******************************************************************/

#include "lpj.h"

/*
 *  DESCRIPTION
 *
 *  Analytically solves differential flux equations for fast and slow SOM 
 *  pools assuming annual litter inputs close to long term equilibrium
 *
 */

void equilsom(Cell *cell) 
{
  int s;
  Stand *stand;
  
  foreachstand(stand,s,cell->standlist)
    equilsoil(&stand->soil);
} /* of 'equilsom' */
