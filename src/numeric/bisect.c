/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/numeric/ b i s e c t . c                                 **/
/**                                                               **/ 
/**  finds a zero of a function using bisection algorithm         **/
/*******************************************************************/
#include <stdio.h>
#include <math.h>
#include "types.h"   /* Definition of datatype Real  */
#include "numeric.h"

Real bisect(Real (*fcn)(Real,void *), /* function */
            Real xlow,  /* lower bound of interval */
            Real xhigh, /* upper bound of interval */
            void *data, /* pointer to additional data for function */
            Real xacc,  /* accuracy in x */
            Real yacc,  /* accuracy in y */
            int maxit   /* maximum number of iterations */
           )            /* returns position of zero of function */
{
  int i;
  Real ylow,ymid,xmid;
  xmid=0.0;
  ylow=(*fcn)(xlow,data); 
  //yhigh=(*fcn)(xhigh,data);

  for(i=0;i<maxit;i++)
  {
    xmid=(xlow+xhigh)*0.5;
    if(xhigh-xlow<xacc)
      return xmid;
    ymid=(*fcn)(xmid,data);
    if(fabs(ymid)<yacc)
      return xmid;
    if(ylow*ymid<=0)
    {
      xhigh=xmid;
      //yhigh=ymid;
    }
    else
    {
      xlow=xmid;
      ylow=ymid;
    } 
  } /* of for */
  return xmid;
} /* of 'bisect' */
