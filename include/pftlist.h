/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / p f t l i s t . h                           **/
/*******************************************************************/

#ifndef PFTLIST_H /* Already included? */
#define PFTLIST_H

/* Definition of datatypes */

typedef List *Pftlist;

/* Declaration of functions */

extern int delpft(Pftlist ,int);
extern void freepftlist(Pftlist);
extern void light(Litter *,Pftlist,int,const Real[],Bool);
extern Real gp_sum(const Pftlist,Real,Real,Real,Real,Real *,Real *,Real *);
extern Real fpc_sum(Real [],int,const Pftlist);
extern Real establishmentpft(Litter *,Pftlist,const Pftpar[],int,int,const Real[], const Climbuf*, int, Bool);
extern Real moistfactor(const Pftlist);
extern int fwritepftlist(FILE *,const Pftlist,Bool);
extern void fprintpftlist(FILE *,const Pftlist);
extern Pftlist freadpftlist(FILE *,const Pftpar[],Bool);

/* Definitions of macros */

#define newpftlist() newlist()
#define addpft(list,pftpar) addlistitem(list,newpft(pftpar))
#define getpft(pftlist,i) ((Pft *)getlistitem(pftlist,i))
#define foreachpft(pft,i,pftlist) for(i=0;i<getlistlen(pftlist) && (pft=getpft(pftlist,i));i++)
#define getnpft(pftlist) getlistlen(pftlist)
#define printpftlist(pftlist) fprintpftlist(stdout,pftlist)

#endif
