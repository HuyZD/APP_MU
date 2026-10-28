/* docmd.c:
 *  This code supports the command line interface (CLI) portion of the
 *  monitor.  It is a table-driven CLI that uses the table in cmdtbl.c.
 *  A limited amount of "shell-like" capabilities are supported...
 *      shell variables, symbol-table lookup, command history,
 *      command line editiing, command output redirection, etc...
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
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "genlib.h"
#include "stddefs.h"
//#include "tfs.h"
//#include "tfsprivate.h"
#include "cli.h"

/* appCmdlist:
 *  This is a pointer to a list of commands that can be added to the
 *  monitor's list by the application using addcommand().
 */
//struct  monCommand *appCmdlist=(struct monCommand *)0;

void    paramerr(struct monCommand *);
void    showusage(struct monCommand *);
int     showhelp(struct monCommand *list,int index,int verbose);
void    shownextarg(struct monCommand *,char *,int,char **);
int     expandshellvars(char *);
int     tokenize(char *,char **);
extern  struct monCommand cmdlist[];
extern  char cmdUlvl[];

/*  docommand():
 *  Assume the incoming string is a null terminated character string
 *  that is made up of whitespace delimited tokens that will be parsed
 *  into command line arguments.  The first argument is the command name
 *  and all following arguments (if any) are specific to that command.
 *  Three levels of verbosity are supported:
 *  verbose == 0        no verbosity;
 *  verbose == 1        print the list of arguments after tokenization
 *  verbose == 2        print the incoming string prior to processing AND
 *                      print the list of arguments after tokenization;
 *                      the print of the unprocessed string is prefixed by
 *                      a "+ " string.
 *  The verbosity level is 0 by default, to change the level, the script
 *  should be run with "tfs -v run script" for verbose = 1 or
 *  "tfs -v -v run script" for verbose = 2.
 */
int
docommand(char *cmdline,int verbose)
{
    int ret, argc;
    struct  monCommand  *cmdptr;
    char    *ps, *argv[ARGCNT], cmdcpy[CMDLINESIZE];

    /* Just in case an upper-level interpreter has the same command as
     * this monitor, if the first character of the incoming command
     * string is an underscore, just throw it away.  This allows the
     * user to get to the monitor command that is duplicated in an
     * upper level interpreter by preceding it with an underscore.
     */
    if (cmdline[0] == '_')
        cmdline++;

    /* Copy the incoming command line to a local buffer so that we can
     * be sure that the line is writeable; plus we do not alter the
     * incoming buffer.
     */
    strcpy(cmdcpy,cmdline);

    /* Scan through the string, if '#' is found, terminate line if
     * it is not preceded by a backslash.  If it is preceded by a
     * backslash, then remove the backslash from the line and leave
     * the rest of the line as is.
     */
    ps = strchr(cmdcpy,'#');
    if (ps) {
        if (*(ps-1) != '\\')
            *ps = 0;
        else
            strcpy(ps-1,ps);
    }

    /* Build argc/argv structure based on incoming command line. */
    argc = tokenize(cmdcpy,argv);
    if (argc == 0)
        return(CMD_LINE_ERROR);
    if (argc < 0) {
#if DEBUG_PRINT
        printf("Command line error\n");
#endif
        return(CMD_LINE_ERROR);
    }

    /* Initialize static data used by getopt(). */
    getoptinit();

    /* Step through the command table looking for a match between
     * the first token of the command line and a command name.
     */
    cmdptr = cmdlist;
    while(cmdptr->name) {
        if (strcmp(argv[0],cmdptr->name) == 0)
            break;
        cmdptr++;
    }

    if (cmdptr->name) {
        //if (cmdUlvl[cmdptr-cmdlist] > getUsrLvl()) {
        //    return(CMD_ULVL_DENIED);
        //}
        if ((argc > 1) && (strcmp(argv[argc-1],"?") == 0)) {
            shownextarg(cmdptr,cmdline,argc,argv);
            return(CMD_PREFILL);
        }
        ret = cmdptr->func(argc,argv);

        /* If command returns parameter error, then print the second
         * string in that commands help text as the usage text.  If
         * the second string is a null pointer, then print a generic
         * "no arguments" string as the usage message.
         */
        if (ret == CMD_PARAM_ERROR)
            paramerr(cmdptr);

        //RedirectionCmdDone();
        return(ret);
    }

    /* If we get here, then the first token does not match on any of
     * the command names in ether of the command tables.  As a last
     * resort, we look to see if the first token matches an executable
     * file name in TFS.
     */
    ret = CMD_NOT_FOUND;

    if (ret == CMD_NOT_FOUND) {
#if DEBUG_PRINT
        printf("Command not found: %s\n",argv[0]);
#endif
    }

    return(ret);
}

/* shownextarg():
 */
void
shownextarg(struct monCommand *cmdptr, char *cmdline, int argc, char **argv)
{
    char *eol;

    /* Remove the question mark: */
    eol = cmdline + strlen(cmdline);
    while(eol > cmdline) {
        if (*eol == '?') {
            *eol = 0;
            break;
        }
        else
            *eol = 0;
        eol--;
    }
    if (cmdptr->clix) {
        if (cmdptr->clix->nextarg(cmdline,argc,argv) == -1)
            showusage(cmdptr);
    }
    else
        showusage(cmdptr);
}

void
showusage(struct monCommand *cmdptr)
{
    char    *usage;

    usage = cmdptr->helptxt[1];
#if DEBUG_PRINT
    printf("Usage: %s %s\n",
        cmdptr->name,*usage ? usage : "(no args/opts)");
#endif
}

void
paramerr(struct monCommand *cmdptr)
{
#if DEBUG_PRINT
    printf("Command line error...\n");
#endif
    showusage(cmdptr);
}

/* tokenize():
 *  Take the incoming string and create an argv[] array from that.  The
 *  incoming string is assumed to be writeable.  The argv[] array is simple
 *  a set of pointers into that string, where the whitespace delimited
 *  character sets are each NULL terminated.
 */
int
tokenize(char *string,char *argv[])
{
    int argc, done;

    /* Null out the incoming argv array. */
    for(argc=0;argc<ARGCNT;argc++)
        argv[argc] = (char *)0;

    argc = 0;
    while(1) {
        while ((*string == ' ') || (*string == '\t'))
            string++;
        if (*string == 0)
            break;
        argv[argc] = string;
        while ((*string != ' ') && (*string != '\t')) {
            if ((*string == '\\') && (*(string+1) == '"')) {
                strcpy(string,string+1);
            }
            else if (*string == '"') {
                strcpy(string,string+1);
                while(*string != '"') {
                    if ((*string == '\\') && (*(string+1) == '"'))
                        strcpy(string,string+1);
                    if (*string == 0)
                        return(-1);
                    string++;
                }
                strcpy(string,string+1);
                continue;
            }
            if (*string == 0)
                break;
            string++;
        }
        if (*string == 0)
            done = 1;
        else {
            done = 0;
            *string++ = 0;
        }
        argc++;
        if (done)
            break;
        if (argc >= ARGCNT) {
            argc = -1;
            break;
        }
    }
    return(argc);
}

/* Help():
 * This command displays each commands help text.
 * The help text is assumed to be formatted as an array of strings
 * where...
 *  the first string is the command description;
 *  the second string is command usage;
 *  and all remaining strings up to the NULL are just printable text.
 */

char *HelpHelp[] = {
        "Display command set",
        "-[d] [commandname]",
        "Options:",
        " -d   list commands and descriptions",
        0,
};

int
Help(int argc,char *argv[])
{
    struct  monCommand *cptr, *acptr;
    int     i, foundit, opt, descriptions;

    descriptions = 0;
    while((opt=getopt(argc,argv,"d")) != -1) {
        switch(opt) {
        case 'd':
            descriptions = 1;
            break;
        default:
            return(CMD_PARAM_ERROR);
        }
    }
    cptr = cmdlist;
    if (argc == optind) {
        foundit = 1;
        if (descriptions) {
            while(cptr->name) {
                showhelp(cmdlist,cptr-cmdlist,1);
                cptr++;
            }
            //if (appCmdlist) {
            //    acptr = appCmdlist;
            //    for(i=0;acptr->name;acptr++,i++)
            //        showhelp(appCmdlist,i,1);
            //}
            putchar('\n');
        }
        else {
            i = 0;
            while(1) {
                if (showhelp(cmdlist,cptr-cmdlist,0)) {
                    if ((++i%6) == 0)
                        putchar('\n');
                }
                cptr++;
                if (cptr->name == 0L)
                    break;
            }
            //if (appCmdlist) {
            //    acptr = appCmdlist;
            //    for(j=0;acptr->name;acptr++,j++) {
            //        if (showhelp(appCmdlist,j,0)) {
            //            if ((++i%6) == 0)
            //                putchar('\n');
            //        }
            //    }
            //}
            putchar('\n');
        }
    }
    else {
        foundit = 0;
        while(cptr->name) {
            if (strcmp(cptr->name,argv[1]) == 0) {
                foundit = showhelp(cmdlist,cptr-cmdlist,2);
                break;
            }
            cptr++;
        }
        if (!cptr->name) {
            //if (appCmdlist) {
            //    acptr = appCmdlist;
            //    for(i=0;acptr->name;acptr++,i++) {
            //        if (strcmp(acptr->name,argv[1]) == 0) {
            //            foundit = showhelp(appCmdlist,i,2);
            //            break;
            //        }
            //    }
            //}
        }
    }
    if (!foundit) {
#if DEBUG_PRINT
        printf("\"%s\" not found\n",argv[1]);
#endif
        return(CMD_FAILURE);
    }
    return(CMD_SUCCESS);
}

/* showhelp():
 *  Called by Help() when it is time to print out some verbosity level of
 *  a command's help text.
 *  if...
 *      verbose == 2, then print all the help text;
 *      verbose == 1, then print the command name and abstract;
 *      verbose == 0, then print only the command name;
 */
int
showhelp(struct monCommand *list,int index,int verbose)
{
    char **hp;
    struct monCommand *cptr;

    cptr = &list[index];

    /* If the incoming command list is that of the monitor, then check
     * for user level...
     */
    //if ((list == cmdlist) && (cmdUlvl[index] > getUsrLvl()))
    //if ((cptr == cmdlist))
    //    return(0);

    if (verbose == 2) {
#if DEBUG_PRINT
        printf("%s\n", cptr->helptxt[0]);
#endif
        showusage(cptr);

        hp = &cptr->helptxt[2];
        while(*hp) {
#if DEBUG_PRINT
            printf("%s\n",*hp++);
#endif
			//if (++lcnt == 18) {
            //    if (!More())
            //        break;
            //    lcnt = 0;
            //}
        }
#if DEBUG_PRINT
        printf("\nRequired user level: ");
#endif
        if (list == cmdlist) {
#if DEBUG_PRINT
            printf("%d\n\n",cmdUlvl[index]);
#endif
		}
        else {
#if DEBUG_PRINT
            printf("NA\n\n");
#endif
		}
    }
    else if (verbose == 1) {
        if (list == cmdlist) {
#if DEBUG_PRINT
            printf(" %-12s %d %s\n", cptr->name, cmdUlvl[index],
                cptr->helptxt[0]);
#endif
		}
        else {
#if DEBUG_PRINT
            printf(" %s %s\n", cptr->name,cptr->helptxt[0]);
#endif
		}
    }
    else {
#if DEBUG_PRINT
        printf("%-12s",cptr->name);
#endif
    }
    return(1);
}
