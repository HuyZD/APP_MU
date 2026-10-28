/*  stddefs.h:
 *
 *  General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes.  In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 *  Note1: the majority of this code was edited with 4-space tabs.
 *  Note2: as more and more contributions are accepted, the term "author"
 *         is becoming a mis-representation of credit.
 *
 *  Original author:    Ed Sutter
 *  Email:              esutter@lucent.com
 *  Phone:              908-582-2351
 */
#ifndef TJ_TYPES_H
#define TJ_TYPES_H

//typedef signed int		s32;
typedef signed short	s16;
//typedef signed char		s8;

//typedef unsigned int	u32;
typedef unsigned short	u16;
typedef	unsigned char	u8;

typedef unsigned char 	uchar;
typedef unsigned short 	ushort;
typedef unsigned int 	uint;
typedef unsigned long 	ulong;
typedef volatile unsigned char 	vuchar;
typedef volatile unsigned short vushort;
typedef volatile unsigned long 	vulong;
typedef volatile unsigned int 	vuint;
typedef volatile int vint;

typedef int                             bool;
#define true                    1
#define false                   0

typedef enum {
	VAR_LONG=32,
	VAR_SHORT=16,
	VAR_CHAR=8
} VAR_TYPE;

/* number of nibbles in a word */
#define NIBBLES_PER_WORD (8)

#ifndef NULL
#define NULL (void *)0
#endif

//   typedef char *va_list;
//#  define va_start(ap, p)       (ap = (char *) (&(p)+1))
//#  define va_arg(ap, type)      ((type *) (ap += sizeof(type)))[-1]
//#  define va_end(ap)

#endif

