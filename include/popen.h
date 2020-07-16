/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / p o p e n . h                               **/
/*******************************************************************/

#ifndef POPEN_H
#define POPEN_H

#ifdef _WIN32
#define cpp_cmd "cl /EP "
#else
#define cpp_cmd "cpp -P "
#endif

extern FILE* pt_popen(const char *,const char *);
extern void pt_pclose(FILE *);

#endif
