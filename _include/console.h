/*
 * console.h
 *
 *  Created on: 2023. 4. 10.
 *      Author: LJH
 */

#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#define CMDLINESIZE 128

#define ARGCNT      24

#define CMD_SUCCESS         0
#define CMD_FAILURE         -1
#define CMD_PARAM_ERROR     -2
#define CMD_LINE_ERROR      -3
#define CMD_ULVL_DENIED     -4
#define CMD_NOT_FOUND       -5
#define CMD_PREFILL         -6

#define BOOT_MEMORY_BASE	0xf902c800

struct cliextra {
    int (*nextarg)();       /* Function that will print command's next arg. */
};


struct  monCommand {
    char    *name;          /* Name of command seen by user. */
    int     (*func)();      /* Function called when command is invoked. */
    char    **helptxt;      /* Help text (see notes below). */
    struct  cliextra *clix; /* Pointer to "extras" that may or may not be */
                            /* used by a specific command. */
};

typedef struct __GET_KEY_T__ {
	signed long		value;
	unsigned char	valid;
} GET_KEY_T;



extern void paramerr(struct monCommand *);
extern void showusage(struct monCommand *);
extern int showhelp(struct monCommand *list,int index,int verbose);
extern void shownextarg(struct monCommand *,char *,int,char **);
extern int expandshellvars(char *);
extern int tokenize(char *,char **);
extern int docommand(char *cmdline,int verbose);

extern void CommandLoop(void *arg);
extern void init_console(void);
extern int xatoi (char **str,	long *res);


extern  struct monCommand cmdlist[];
extern  char cmdUlvl[];

extern int		optind_u;
extern char    *optarg_u;    	/* pointer to argument associated with an option */

#endif /* SRC_CONSOLE_H_ */
