/***************************************************************************/
/**                                                                       **/
/**     i n i t m a n a g e .  c                                          **/
/**                                                                       **/
/**     C implementation of LPJ                                           **/
/**                                                                       **/                                  
/**     Potsdam Institute for Climate Impact Research                     **/
/**     PO Box 60 12 03                                                   **/
/**     14412 Potsdam/Germany                                             **/
/**                                                                       **/
/**     Last change: 03.05.2007, Marlies Gumpenberger                     **/
/**                                                                       **/
/***************************************************************************/

#include "lpj.h"

void initmanage(Manage *manage,const Countrypar *countrypar,const Regionpar *regionpar)
{
  manage->par=countrypar;    
  manage->regpar=regionpar;
} /* of 'initmanage' */
