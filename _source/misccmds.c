/* misccmds:
 *  More monitor commands...
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
#include "config.h"
#include "genlib.h"
//#include "tfs.h"
//#include "tfsprivate.h"
//#include "ether.h"
//#include "devices.h"
#include "cli.h"

char        ApplicationInfo[512];
//int         (*extgetUsrLvl)();
//int         setUsrLvl(int, char *);
//static int  UserLevel;
//
//char *UlvlHelp[] = {
//    "Display or modify current user level.",
//    "-[c:hp] [new_level|min|max] [password]",
//    "Options:",
//    " -c{cmd,lvl}",
//    "      set command's user level",
//    " -h   dump system header",
//    " -p   build new password file",
//    0
//};
//
//int
//Ulvl(int argc,char *argv[])
//{
//    extern  char *getpass(char *,char *,int);
//    extern  int newPasswordFile(), setCmdUlvl();
//    char    passwd[32], *pwp;
//    int     level, opt, newulvl;
//
//    newulvl = 0;
//    pwp = (char *)0;
//    level = MINUSRLEVEL;
//    while((opt=getopt(argc,argv,"c:hp")) != -1) {
//        switch(opt) {
//        case 'c':
//            setCmdUlvl(optarg,1);
//            newulvl++;
//            break;
//        case 'h':
//            monHeader(0);
//            break;
//        case 'p':
//            newPasswordFile();
//            break;
//        default:
//            return(CMD_PARAM_ERROR);
//        }
//    }
//
//    /* At this point, the newulvl flag is used to indicate that the
//     * -c option was used.  If it was, then we return here.
//     */
//    if (newulvl)
//        return(CMD_SUCCESS);
//
//    /* If there is one or two arguments on the command line, then
//     * the user must want to modify the current user level.  If
//     * there are no arguments, then simply display the current
//     * user level.
//     *
//     * If the new user level is lower than the current user level,
//     * then the user can simply enter the new level (one argument).
//     * If the new user level is higher than the current user level,
//     * then the user must also enter a password.  The password is
//     * entered either as the second argument or interactively
//     * using getpass().
//     */
//    newulvl = 0;
//    if ((argc == optind+1) || (argc == optind+2)) {
//        if (!strcmp(argv[optind],"min"))
//            level = MINUSRLEVEL;
//        else if (!strcmp(argv[optind],"max"))
//            level = MAXUSRLEVEL;
//        else
//            level = atoi(argv[optind]);
//
//        if (argc == optind+1) {
//            if (level > UserLevel) {
//                getpass("Password: ",passwd,sizeof(passwd)-1);
//                pwp = passwd;
//            }
//        }
//        else {
//            pwp = argv[optind+1];
//        }
//        newulvl = 1;
//    }
//    else if (argc != optind) {
//        return(CMD_PARAM_ERROR);
//    }
//
//    /* At this point,the  newulvl flag is used to indicate that an
//     * adjustment to the current user level is to be made.
//     */
//    if (newulvl) {
//        if (level <= UserLevel)
//            UserLevel = level;
//        else
//            setUsrLvl(level,pwp);
//    }
//
//    if (extgetUsrLvl)
//        printf("User level controlled by application: %d\n",extgetUsrLvl());
//    else
//        printf("Current monitor user level: %d\n",UserLevel);
//    return(CMD_SUCCESS);
//}
//
//void
//initUsrLvl(int lvl)
//{
//    extgetUsrLvl = 0;
//    UserLevel = lvl;
//}
//
///* getUsrLvl():
// *  This is the ONLY point of access for retrieval of the user level.
// *  This allows the application to redefine how the monitor retrieves
// *  what it thinks of as the user level.
// */
//int
//getUsrLvl(void)
//{
//    if (extgetUsrLvl)
//        return(extgetUsrLvl());
//    return(UserLevel);
//}
//
//int
//setUsrLvl(int level, char *password)
//{
//    int olvl;
//
//    olvl = UserLevel;
//
//    /* If level is -1, then assume this is only a request for the current   */
//    /* user level.  If the incoming level is any other value outside the    */
//    /* range of MINUSRLEVEL and MAXUSRLEVEL, return -1.                     */
//    if (level == -1)
//        return(UserLevel);
//    if ((level > MAXUSRLEVEL) || (level < MINUSRLEVEL))
//        return(olvl);
//
//    /* If password pointer is NULL, then don't check for password, just set */
//    /* the level and be done.                                               */
//    if (!password) {
//        UserLevel = level;
//    }
//    else {
//        if (validPassword(password,level))
//            UserLevel = level;
//    }
//    return(olvl);
//}
//
///* returnMaxUsrLvl(), setTmpMaxUsrLvl() & clrTmpMaxUsrLvl():
// *  These three functions are used to allow a few places in the monitor
// *  to temporarily force the user level to MAXUSRLEVEL.  This is necessary
// *  for accessing the password file and the tfs log file.
// *  Call setTmpMaxUsrLvl() to enable MAX mode and then clrTmpMaxUsrLvl()
// *  with the value returned by setTmpMaxUsrLvl() when done.
// */
//int
//returnMaxUsrLvl(void)
//{
//    return(MAXUSRLEVEL);
//}
//
//void *
//setTmpMaxUsrLvl()
//{
//    void *fptr;
//
//    fptr = (void *)extgetUsrLvl;
//    extgetUsrLvl = returnMaxUsrLvl;
//    return(fptr);
//}
//
//void
//clrTmpMaxUsrLvl(int (*fptr)())
//{
//    extgetUsrLvl = fptr;
//}

char *VersionHelp[] = {
        "Version information",
        "[application_info]",
        0,
};

int
Version(int argc, char *argv[])
{
    extern  void ShowVersion(void);

    if (argc == 1)
        ShowVersion();
    else {
        strncpy(ApplicationInfo,argv[1],80);
        ApplicationInfo[80] = 0;
    }
    return(CMD_SUCCESS);
}

char *EchoHelp[] = {
    "Print string to local terminal",
    "[arg1] ... [argn]",
    " Special meaning: \\b \\c \\r \\n \\t \\x",
    0,
};

int
Echo(int argc,char *argv[])
{
    int     i, done;
    char    *cp, c, hex[3];

    for(i=optind;i<argc;i++) {
        cp = argv[i];
        done = 0;
        while(!done && *cp) {
            if (*cp == '\\') {
                cp++;
                switch(*cp) {
                    case 'b':           /* Backspace */
                        c = '\b';
                        break;
                    case 'c':           /* No newline, just end here */
                        return(CMD_SUCCESS);
                    case 'n':           /* Newline */
                        c = '\n';
                        break;
                    case 'r':           /* Carriage Return */
                        c = '\r';
                        break;
                    case 't':           /* Tab */
                        c = '\t';
                        break;
                    case 'x':           /* Hex conversion */
                        cp++;
                        hex[0] = *cp++;
                        hex[1] = *cp;
                        hex[2] = 0;
                        c = strtol(hex,0,16);
                        break;
                    case '\\':          /* Backslash */
                        c = '\\';
                        break;
                    default:            /* Ignore backslash */
                        c = *cp;
                        break;
                }
                putchar(c);
            }
            else {
                putchar(*cp);
            }
            if (cp)
                cp++;
        }
        if (i != argc-1) {
            putchar(' ');
        }
    }
    putchar('\n');
    return(CMD_SUCCESS);
}

/* Sleep():
 *  Simple delay loop accessible by the command line.
 *  This loop count is dependent on the underlying hardware.
 *  The LoopsPerSecond count is loaded with a default at startup, or it can
 *  be calibrated by the user via the -c option.
 *  Note that this LoopsPerSecond value is used in a few other places in
 *  the monitor also for time-dependent stuff.
 *  NOTES:
 *  - This is obviously not real accurate (not intended to be), but allows the
 *    monitor to be independent of the underlying hardware.
 *  - The delay time is very dependent on ethernet activity, since the call
 *    to pollethernet() part of the loop.
 */

char *SleepHelp[] = {
    "Second or msec delay (not precise)",
    "-[clm] {count}",
    " -c  calibrate new LPS count",
    " -l  store LPS count",
    " -m  millisecond",
    " -v {LPSvarname}",
    0,
};

//int
//Sleep(argc,argv)
//int argc;
//char    *argv[];
//{
//    int i, opt, calibrate, count, loopvalue;
//
//    calibrate = 0;
//    loopvalue = LoopsPerSecond;
//    while ((opt=getopt(argc,argv,"clmv:")) != -1) {
//        switch(opt) {
//        case 'c':
//            calibrate = 2;
//            break;
//        case 'l':
//            calibrate = 1;
//            break;
//        case 'm':
//            loopvalue /= 1000;
//            break;
//        case 'v':
//            shell_sprintf(optarg,"%d",LoopsPerSecond);
//            return(CMD_SUCCESS);
//        default:
//            return(CMD_PARAM_ERROR);
//        }
//    }
//
//    /* If no args, just print the current LPS value and return... */
//    if (argc == 1) {
//        printf("Current LPS = %d\n",LoopsPerSecond);
//        return(CMD_SUCCESS);
//    }
//
//    /* For calibration, take in the count on the command line, then use
//     * it to put out 5 dots dot at the rate of the loop to allow the user
//     * to adjust it to be about 1 second.
//     */
//    if (calibrate) {
//        if (argc != optind+1)
//            return(CMD_PARAM_ERROR);
//        printf("Current LPS: %d\n",LoopsPerSecond);
//        LoopsPerSecond = strtol(argv[optind],(char **)0,0);
//        printf("New LPS: %d\n",LoopsPerSecond);
//        if (calibrate == 2) {
//            count = 10;
//            while(count-- > 0) {
//                for(i=0;i<LoopsPerSecond;i++) {
//#if INCLUDE_ETHERNET
//                    pollethernet();
//#endif
//                }
//                puts(".\007");
//            }
//            putchar('\n');
//        }
//        return(CMD_SUCCESS);
//    }
//
//    if (argc == optind)
//        count = 1;
//    else
//        count = strtol(argv[optind],(char **)0,0);
//
//    while(count-- > 0) {
//        for(i=0;i<loopvalue;i++) {
//#if INCLUDE_ETHERNET
//            pollethernet();
//#endif
//        }
//    }
//
//    return(CMD_SUCCESS);
//}
//
//#if INCLUDE_EXCTEST
//char *EtestHelp[] = {
//    "Tests to verify operation of exception handling.",
//    "-[v] {align|dbz|float}",
//    "Options:",
//    " -v   verbose",
//    "Exception Types:",
//    " align dbz float",
//    0
//};
//
//int
//Etest(int argc,char *argv[])
//{
//    int     opt, verbose;
//    volatile int    *ip, i, j, k;
//    volatile float  y, z;
//    volatile char   *cp;
//
//    verbose = 0;
//    while((opt=getopt(argc,argv,"v")) != -1) {
//        switch(opt) {
//        case 'v':
//            verbose = 1;
//            break;
//        default:
//            return(CMD_PARAM_ERROR);
//        }
//    }
//    if (argc <= optind)
//        return(CMD_PARAM_ERROR);
//    if (!strcmp(argv[optind],"align")) {
//        printf("Alignment error test...\n");
//        cp = (char *)&i;
//
//        for(j=1;j<=4;j++) {
//            printf("%d...\n",j);
//            cp++;
//            ip = (int *)cp;
//            *ip = 12345678;
//        }
//    }
//    else if (!strcmp(argv[optind],"dbz")) {
//        printf("Divide by zero test...\n");
//        i = 45;
//        j = 0;
//        k = i/j;
//        printf("45/0=%d\n",k);
//    }
//    else if (!strcmp(argv[optind],"float")) {
//        printf("Floating point test ");
//        y = 1.123;
//        z = 3.0;
//        if ((y * z) == 3.369)
//            printf("passed\n");
//        else
//            printf("failed\n");
//    }
//    else
//        return(CMD_PARAM_ERROR);
//
//    return(CMD_SUCCESS);
//}
//#endif
