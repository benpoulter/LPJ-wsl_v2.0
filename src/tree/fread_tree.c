/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ f r e a d _ t r e e . c                            **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

void fread_tree(FILE *file,Pft *pft,Bool swap)
{
  Pfttree *tree;
  tree=new(Pfttree);
  pft->data=tree;
/*  freadint1(&tree->leafondays,swap,file);
  freadint1(&tree->leafoffdays,swap,file);
  freadint1(&tree->leafon,swap,file);*/
  freadreal1(&tree->height,swap,file);
  freadreal1(&tree->crownarea,swap,file);
  freadreal1(&tree->gddtw,swap,file);
  freadreal1(&tree->aphen_raingreen,swap,file);
  freadreal((Real *)&tree->ind,sizeof(Treephys2)/sizeof(Real),swap,file);
  fpc_tree(pft);
} /* of 'fread_tree' */
