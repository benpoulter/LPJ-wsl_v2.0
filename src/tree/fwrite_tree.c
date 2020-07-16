/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**  src/tree/ f w r i t e _ t r e e . c                          **/
/*******************************************************************/
#include "lpj.h"
#include "tree.h"

Bool fwrite_tree(FILE *file,const Pft *pft)
{
  const Pfttree *tree;
  tree=pft->data;
  return fwrite(tree,sizeof(Pfttree),1,file)!=1;
} /* of 'fwrite_tree' */
