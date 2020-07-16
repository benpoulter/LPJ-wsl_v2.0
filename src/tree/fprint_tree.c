/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ f p r i n t _ t r e e . c                          **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"


void fprint_tree(FILE *file,const Pft *pft)
{
  Pfttree *tree;
  tree=pft->data;
  fprintf(file,"Cmass: ");
  fprinttreephys2(file,tree->ind,pft->nind);
  fprintf(file,"\n");
} /* of 'fprint_tree' */
