/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / n u m e r i c . h                           **/
/*******************************************************************/

#ifndef NUMERIC_H /* Already included? */
#define NUMERIC_H

/* Definition of datatypes */

typedef Real (*Bisectfcn)(Real,void *);

/* Declaration of functions */

extern Real bisect(Real (*)(Real,void *),Real,Real,void *,Real,Real,int); /* find zero */
extern Real leftmostzero(Real (*)(Real,void *),Real,Real,void *,Real,Real,int); /* find leftmost zero */
extern void linreg(Real *,Real *,const Real[],int); /* linear regression */
extern void setseed(int); /* set seed of random number generator */
extern int getseed(void); /* get seed of random number generator */
extern Real randfrac(void); /* random number generator */
extern Real petpar(Real *,Real *,Real *,Real,Real,Real,Real);
extern Real petparRAD(Real *,Real *,Real *,Real,Real,Real,Real,Real); //new function for radiation

#endif
