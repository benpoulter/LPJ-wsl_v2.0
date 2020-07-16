/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / l i s t . h                                 **/
/*******************************************************************/

#ifndef LIST_H /* Already included? */
#define LIST_H

/* Definition of datatypes */

typedef struct
{
  void **data;
  int n;  /* Length of list */
} List;

/* Declaration of functions */

extern List *newlist(void);
extern int addlistitem(List *,void *);
extern int dellistitem(List *,int);
extern void freelist(List *);

/* Definition of macros */

#define foreachlistitem(i,list) for(i=0;i<(list)->n;i++)
#define getlistitem(list,index) (list)->data[index]
#define isempty(list) ((list)->n==0)
#define getlistlen(list) (list)->n

#endif
