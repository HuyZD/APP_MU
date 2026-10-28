/* genlib.c:
 *  This file has some of the stuff that is typically pulled in by the c
 *  library.  It is provided as local source so that the monitor can be
 *  built without the need for a library.  Some of this code was taken from
 *  the Berkley source code (see notice below).
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
/*
 * Copyright (c) 1988, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdlib.h>
#include <string.h>
#include "ctype.h"
#include "stddefs.h"
#include "genlib.h"

/* If an application is using this source code, then the printf() calls
 * might want to be redirected to printf...
 */
#ifdef MONPRINTF
extern int printf();
#define printf printf
#endif

//#define DIGIT(x)    (isdigit(x) ? (x) - '0' :  islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
//#define MBASE   ('z' - 'a' + 1 + 10)
//
//
//long int
//strtol(char *str,char **ptr, int base)
//{
//    register int val;
//    register int c;
//    int xx, neg = 0;
//
//    if (ptr != (char **)0)
//        *ptr = str; /* in case no number is formed */
//    if (base < 0 || base > MBASE)
//        return (0); /* base is invalid -- should be a fatal error */
//    if (!isalnum((c = *str))) {
//        while (isspace(c))
//            c = *++str;
//        switch (c) {
//        case '-':
//            neg++;
//        case '+': /* fall-through */
//            c = *++str;
//        }
//    }
//    if (base == 0) {
//        if (c != '0')
//            base = 10;
//        else if (str[1] == 'x' || str[1] == 'X')
//            base = 16;
//        else
//            base = 8;
//    }
//    /*
//     * for any base > 10, the digits incrementally following
//     *  9 are assumed to be "abc...z" or "ABC...Z"
//     */
//    if (!isalnum(c) || (xx = DIGIT(c)) >= base)
//        return (0); /* no number formed */
//    if (base == 16 && c == '0' && isxdigit(str[2]) &&
//        (str[1] == 'x' || str[1] == 'X'))
//        c = *(str += 2); /* skip over leading "0x" or "0X" */
//    for (val = -DIGIT(c); isalnum((c = *++str)) && (xx = DIGIT(c)) < base; )
//        /* accumulate neg avoids surprises near MAXLONG */
//        val = base * val - xx;
//    if (ptr != (char **)0)
//        *ptr = str;
//    return (neg ? val : -val);
//}
//
//unsigned long int
//strtoul(char *str, char **ptr, int base)
//{
//    int    val;
//
//    val = strtol(str, ptr, base);
//    return((unsigned int)val);
//}
//
unsigned short swap2(unsigned short sval_in)
{
    unsigned char *cp_in, *cp_out;
    unsigned short sval_out;

    cp_in = (uchar *)&sval_in;
    cp_out = (uchar *)&sval_out;
    cp_out[0] = cp_in[1];
    cp_out[1] = cp_in[0];
    return (sval_out);
}

unsigned int swap4(unsigned int sval_in)
{
    unsigned char *cp_in, *cp_out;
    unsigned int sval_out;

    cp_in = (unsigned char *)&sval_in;
    cp_out = (unsigned char *)&sval_out;
    cp_out[0] = cp_in[3];
    cp_out[1] = cp_in[2];
    cp_out[2] = cp_in[1];
    cp_out[3] = cp_in[0];
    return (sval_out);
}
unsigned short swap2mul(float sval_in)
{
	return swap2((unsigned short)(sval_in * 100));
}
float swap2dev(unsigned short sval_in) {
	ushort sval = swap2(sval_in);
	return (float)sval/100;

}

/* Variables used by getopt: */
int optind;         /* argv index to first cmd line arg that is not part
                     * of the option list.
                     */
char    *optarg;    /* pointer to argument associated with an option */
int sp;

int getopt(int argc,char *argv[],char *opts)
{
    register int c;
    register char *cp;

    if(sp == 1) {
        if (optind >= argc)
            return(-1);
        else if (argv[optind][0] != '-')
            return(-1);
        else if (argv[optind][1] == '\0')
            return(-1);
        else if(strcmp(argv[optind], "--") == 0) {
            optind++;
            return(-1);
        }
    }
    c = argv[optind][sp];
    if(c == ':' || (cp=strchr(opts, c)) == 0) {
#if DEFINE_PRINTF
        printf("Illegal option: %c\n",c);
#endif
        if(argv[optind][++sp] == '\0') {
            optind++;
            sp = 1;
        }
        return('?');
    }
    if(*++cp == ':') {
        if(argv[optind][sp+1] != '\0')
            optarg = &argv[optind++][sp+1];
        else if(++optind >= argc) {
#if DEFINE_PRINTF
            printf("Option requires argument: %c\n",c);
#endif
            sp = 1;
            return('?');
        } else
            optarg = argv[optind++];
        sp = 1;
    } else {
        if(argv[optind][++sp] == '\0') {
            sp = 1;
            optind++;
        }
        optarg = 0;
    }
    return(c);
}

/* getoptinit():
 * Since getopt() can be used by every command in the monitor
 * it's variables must be reinitialized prior to each command
 * executed through docommand()...
 */
void getoptinit(void)
{
    sp = 1;
    optind = 1;
}

