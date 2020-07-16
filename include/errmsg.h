/*******************************************************************/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                  |  L P J  -  w s l  |                        **/
/**                  +-+-+-+-+-+-+-+-+-+-+                        **/
/**                                                               **/
/**   i n c l u d e / e r r m s g . h                             **/
/*******************************************************************/

#ifndef ERRMSG_H /* Already included? */
#define ERRMSG_H

/* Definitio of macros */

#define printfopenerr(fcn,filename) fprintf(stderr,"Error opening file '%s' in '%s': %s\n",filename,fcn,strerror(errno))
#define printallocerr(fcn,ptr) fprintf(stderr,"Error allocating '%s' in '%s': %s\n",ptr,fcn,strerror(errno))
#define readinterr(fcn,var) fprintf(stderr,"Error reading int '%s' in '%s'.\n",var,fcn)
#define readrealerr(fcn,var) fprintf(stderr,"Error reading real '%s' in '%s'.\n",var,fcn)
#define readstringerr(fcn,var) fprintf(stderr,"Error reading string '%s' in '%s'.\n",var,fcn)

#define fscanname(file,var,fcn,name) if(fscanf(file,"%s",var)!=1){readstringerr(fcn,name); return TRUE;}
#define fscanstring2(file,var,fcn,name) if(fscanstring(file,var))readstringerr(fcn,name)

#endif /* of ERRMSG_H */
