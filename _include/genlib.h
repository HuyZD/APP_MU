#ifndef UMON_UTIL_H
#define UMON_UTIL_H

//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
#include "stddefs.h"

/* Some compilers consider sizeof() to be unsigned... */
#define sizeof (int)sizeof

extern int optind;
extern char *optarg;

extern int abs(int);
//extern int atoi(char *);
extern int getopt(int,char **,char *);

//extern int memcmp(char *, char *, int);
//extern char *memccpy(char *, char *, int, int);
//extern char *memchr(char *, char, int);
//extern char *memcpy(char *, char *, int);
//extern char *memset(char *, char, int);
//extern int strspn(char *, char *);
//extern int strlen(char *);
extern int strcmp(char const*, char const*);
//extern int strncmp(char *, char *, int);
//extern char *strcat(char *, char *);
//extern char *strchr(char *, char);
//extern char *strstr(char *, char *);
//extern char *strcpy(char *, char *);
//extern char *strncat(char *, char *, int);
//extern char *strncpy(char *, char *, int);
//extern char *strpbrk(char *, char *);
//extern char *strrchr(char *, char);
//extern char *strtok(char *, char *);
//extern char *strtolower(char *string);
//extern char *strtoupper(char *string);
//extern int strtol(char *, char **, int);
extern unsigned short swap2(unsigned short);
extern unsigned int swap4(unsigned int);
//extern unsigned int strtoul(char *, char **, int);

extern void getoptinit(void);
//char *strlcpy(char *dest, const char *src, size_t n);		/* same as strncpy(), but also null terminates the string */

//extern char *malloc(int);
//extern void free(char *);
//extern char *realloc(char *,int);

/* Included here, but not in genlib.c: */
extern void rputchar(char);
extern int getchar(void);
extern int putchar(int);
extern int puts(const char *);

extern int AddrToSym(int,unsigned int,char *,unsigned int *);

extern int printf();
extern int sprintf();
extern int cprintf();

extern int gotachar(void);
extern int getUsrLvl(void);
extern int shell_sprintf();
extern int getline(char *,int,int);
extern int getline_t(char *,int,int);
extern int getline_p(char *,int,int,char *);
extern int stkchk(char *);
extern int inRange(char *,int);
extern int More(void);
extern int validPassword(char *,int);
extern int askuser(char *);
extern int hitakey(void);
extern int getreg(char *,unsigned int *);
extern int putargv(int,char *);
extern int addrtosector(unsigned char *,int *,int *,unsigned char **);
extern int AppFlashWrite(unsigned int *,unsigned int *,int);
extern int AppFlashErase(int);
extern int flushDcache(char *,int);
extern int invalidateIcache(char *,int);
extern int pollConsole(char *);
extern int sectortoaddr(int,int *,unsigned char **);
extern int sectorProtect(char *,int);
extern int FlashInit(void);
extern int cacheInit(void);
extern int pioget(char,int);
extern int extendHeap(char *,int);
extern int decompress(char *,int,char *);
extern int RedirectionCheck(char *);
extern int docommand(char *, int);
extern int SymFileFd(int);
extern unsigned short xcrc16(unsigned char *buffer,unsigned int nbytes);
extern unsigned int crc32(unsigned char *, unsigned int);
extern unsigned int intsoff(void);
extern unsigned int getAppRamStart(void);
extern char *line_edit(char *);

extern char *getpass(char *,char *,int);
extern char *getsym(char *,char *,int);
extern char *monVersion(void);
extern char *ExceptionType2String(int);
extern void Mtrace();
extern void MtraceInit(char *,int);
extern void monrestart(int);
extern void historylog(char *);
extern void MonitorBuiltEnvSet(void);
extern void writeprompt(void);
extern void intsrestore(unsigned int);
extern void prascii(unsigned char *,int);
extern void cacheInitForTarget(void);
extern void exceptionAutoRestart(int);
extern void clrTmpMaxUsrLvl(int (*)());
extern void rawon(), rawoff();
extern void monHeader(int);
extern void mstatshowcom(void);
extern void CommandLoop(void *arg);
extern void showregs(void), reginit(void);
extern void initUsrLvl(int);
extern void warmstart(int);
extern void coldstart(void);
extern void InitRemoteIO(void);
extern void appexit(int);
extern void pioset(char,int);
extern void pioclr(char,int);
extern void getargv(int *argc, char ***argv);
extern void init1(int), init2(void), init3(void);
extern void EnableBreakInterrupt(void);
extern void DisableBreakInterrupt(void);
extern void ctxMON(void), ctxAPP(void);
#if INCLUDE_REDIRECT
extern void RedirectCharacter(char);
extern void RedirectionCmdDone(void);
#else
#define RedirectCharacter(c)
#define RedirectionCmdDone()
#endif

extern unsigned short xcrc16tab[];
extern unsigned int crc32tab[];
extern char *Mtracebuf;
extern char ApplicationInfo[];
extern unsigned int ExceptionAddr;
extern unsigned int APPLICATION_RAMSTART, BOOTROM_BASE;
extern int ConsoleDevice;
extern int ConsoleBaudRate, LoopsPerSecond;
extern int StateOfMonitor, AppExitStatus, ExceptionType;
extern int  moncomptr;
extern int  bss_start, bss_end, boot_base;
extern int  (*remoterawon)(), (*remoterawoff)();
extern int  (*remoteputchar)(), (*remotegetchar)(), (*remotegotachar)();
extern int  (*dcacheFlush)(), (*icacheInvalidate)();
extern int  (*extgetUsrLvl)();

#endif
