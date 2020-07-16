/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tools/ s w a p . c                                       **/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "swap.h"

typedef struct
{
  int lo,hi;
}Num;

static void swap(char *a,char *b)
{
  char h;
  h=*a;
  *a=*b;
  *b=h;
} /* of 'swap' */

short int swapshort(short int x)
{
  swap((char *)&x,(char *)(&x)+1);
  return x;
} /* of 'swapshort' */

int swapint(int x)
{
  swap((char *)&x,(char *)(&x)+3);
  swap((char *)&x+1,(char *)(&x)+2);
  return x;
} /* of 'swapint' */

static double swapdouble(Num num)
{
  double ret;
  Num x;
  x.hi=swapint(num.lo);
  x.lo=swapint(num.hi);
  memcpy(&ret,&x,sizeof(Num));
  return ret;
} /* of 'swapdouble' */

int freadreal(Real *data,int n,Bool swap,FILE *file)
{
  Num *num;
  int i,rc;
  if(swap)
  {
    num=(Num *)malloc(sizeof(Num)*n);
    rc=fread(num,sizeof(Num),n,file); 
    for(i=0;i<rc;i++)
      data[i]=swapdouble(num[i]);
    free(num);
  }
  else
    rc=fread(data,sizeof(double),n,file);
  return rc;
} /* of 'freadreal' */

int freadint(int *data,int n,Bool swap,FILE *file)
{
  int i,rc;
  rc=fread(data,sizeof(int),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapint(data[i]);
  return rc;
} /* of 'freadint' */

int freadshort(short *data,int n,Bool swap,FILE *file)
{
  int i,rc;
  rc=fread(data,sizeof(short),n,file);
  if(swap)
    for(i=0;i<rc;i++)
      data[i]=swapshort(data[i]);
  return rc;
} /* of 'freadshort' */
