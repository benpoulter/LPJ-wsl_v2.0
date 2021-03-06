/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ l e f t m o s t z e r o . c                     **/
/*******************************************************************/
#include <stdio.h>
#include "types.h"
#include "numeric.h"

#define NSEG 20


Real leftmostzero(Real (*fcn)(Real,void *),
                  Real x1,
                  Real x2,
                  void *data,
                  Real xacc,
                  Real yacc,
                  int maxiter)
{
  Real dx,xmid,swap;
  if(x2<x1)
  {
    swap=x1;
    x1=x2;
    x2=swap;
  }

  dx=(x2-x1)/NSEG;
  if((*fcn)(x1,data)<0)
    for(xmid=x1+dx;(*fcn)(xmid,data)<0 && xmid<=x2-dx;xmid+=dx);  
  else
    for(xmid=x1+dx;(*fcn)(xmid,data)>0 && xmid<=x2-dx;xmid+=dx);
  return bisect(fcn,xmid-dx,xmid,data,xacc,yacc,maxiter);
}  /* of 'leftmostzero' */ 
