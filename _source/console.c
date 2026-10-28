/*
 * console.c
 *
 *  Created on: 2023. 4. 10.
 *      Author: LJH
 */

#include <stdlib.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "stddefs.h"
#include "config.h"
#include "genlib.h"
#include "console.h"
#include "uart.h"
#include "main.h"
#include "task_config.h"
#include "chario.h"
#include "protocol.h"

static int _getline(char *buf,int max,int ledit, int timeout,char *prefill)
{
    static  unsigned char  crlf=0;
    int     tmt, tot, idx;
    char    *base;

    tot = idx = 0;
    base = buf;
    max -= 1;       /* Make sure there is space for the null terminator. */

    if (prefill) {
        strcpy(base,prefill);
        tot = strlen(prefill);
        puts(prefill);
        buf += tot;
        idx = tot;
    }

    for(;idx<max;idx++) {
//    	printf("Debug 2\n\r");
        if (timeout > 0) {
            for(tmt=timeout;tmt>0;tmt--) {
//                if (gotachar())
//                    break;
            }
            if (tmt == 0) {
                *buf = 0;
                return(0);
            }

        }
        *buf = (char)uartRead(UART_CLI);
        if (!*buf) {
            idx--;
            continue;
        }
#if 0
        if ((*buf == 0x1b) && (ledit)) {
            (void)line_edit(base);
            break;
        }
        else
#endif
        {
            if ((*buf == '\r') || (*buf == '\n')) {
                if ((crlf) && (*buf != crlf)) {
                    crlf = 0;
                    continue;
                }
                puts("\r\n");
                crlf = *buf;
                *buf = 0;
                break;
            }
            if (*buf == '\b') {
                if (tot) {
                    idx -= 2;
                    buf--;
                    tot--;
                    puts("\b \b");
                }
            }
            else if (*buf == CTLC) {
                puts("^C\n");
                *base = 0;
                return(0);
            }
            else {
                putchar(*buf);
                tot++;
                buf++;
            }
            crlf = 0;
        }
    }
    if (idx == max) {

        printf("\nInput too long (exceeds %d bytes).\n",max+1);

        *buf = 0;
        return(0);
    }
    return(strlen(base));
}

#include "xuartps_hw.h"


int getline_p(char *buf, int max, int ledit, char *prefill)
{
    return(_getline(buf,max,ledit,0,prefill));
}

/* tokenize():
*  Take the incoming string and create an argv[] array from that.  The
*  incoming string is assumed to be writeable.  The argv[] array is simple
*  a set of pointers into that string, where the whitespace delimited
*  character sets are each NULL terminated.
*/

/****************************************************************************************************/

void init_console(){
    xTaskCreate(vConsoleTask, "Console Task", OS_TASK_CONSOLE_STK_SIZE, NULL, OS_TASK_CONSOLE_PRIO, NULL);

#if DEBUG_PRINT
    printf("Console Task  loc\n");
#endif
}


/* appVersion():
 * Return a pointer to a string that is a concatenation of the
 * build date and build time found in info.h.
 */
char *appVersion(void)
{
    return(__DATE__ " @ " __TIME__);
}
/* ShowVersion():
 *  Display the build time and date of the running monitor.
 */
void
ShowVersion(void)
{
#if DEBUG_PRINT
    printf("Monitor built: %s\n", appVersion());
#endif
    if (ApplicationInfo[0]) {
		puts(ApplicationInfo);
//        printf("%s\n", ApplicationInfo);
    }
}
/* appHeader():
 *  Dump the common stuff that is displayed at the console port when
 *  the monitor first starts up.  If called with 'center' set, then
 *  the printed output is centered on an 80-character screen width.
 */
void appHeader(int center)
{
    char    *ip, *mac;
    int (*pfunc)();
#if DEBUG_PRINT
    if (center) {
        printf("\n\n");
        cprintf("TJ INNOVATION\n");
        pfunc = cprintf;
    }
    else
        pfunc = printf;
    pfunc("CPU: %s\n",CPU_NAME);
    pfunc("Platform: %s\n",PLATFORM_NAME);
    pfunc("Version: %s\n",VERSION_NAME);
//    pfunc("Built: %s @ %s\n",__DATE__, __TIME__);
    //pfunc("Monitor RAM: 0x%06x-0x%06x\n",&bss_start,&bss_end);
    //pfunc("Application RAM Base: 0x%08x\n",APPLICATION_RAMSTART);
//    mac = getenv("ETHERADD");
//    if (mac)
//        pfunc("MAC: %s\n",mac);
//    ip = getenv("IPADD");
//    if (ip)
//        pfunc("IP: %s\n",ip);
#endif
    /* Optionally support (via config.h) up to four lines of user-specified
     * header text...
     */

#ifdef USR_HEADER1
    pfunc("%s\n",USR_HEADER1);
#endif
#ifdef USR_HEADER2
    pfunc("%s\n",USR_HEADER2);
#endif
#ifdef USR_HEADER3
    pfunc("%s\n",USR_HEADER3);
#endif
#ifdef USR_HEADER4
    pfunc("%s\n",USR_HEADER4);
#endif
}

/* FirmwareBuiltEnvSet():
 *  Called by ShellVarInit() to load a shell variable with the build
 *  date/time information of the monitor.  This will support the ability
 *  to automatically decide if a monitor is to be updated.
 */
void FirmwareBuiltEnvSet(void)
{
    char buf[64];

    sprintf(buf,"%s@%s",__DATE__,__TIME__);
    //setenv("MONITORBUILT", buf);
}

/* writeprompt():
 *  Called to print out the monitor prompt.  If the shell variable PROMPT
 *  exists, then use the content of that variable as a prompt; otherwise,
 *  just print uMON>.
 */
void writeprompt(void)
{
#if DEBUG_PRINT
    char *prompt=0;

//    prompt = mon_getenv("APPPROMPT");
    if (!prompt)
    prompt = "AD3T_MU> ";
    puts(prompt);
#endif
}

/* init3():
 *  As each target boots up, it calls init1() and init2() to do two phases
 *  of hardware initialization.  This third initialization phase is common
 *  to all targets...
 */
void
init3(void)
{
//    extern  void historyinit();
//    extern  int atinit();
//    extern  int ShellVarInit();
//
//#if INCLUDE_DEBUG
//    /* Init AT data structures. */
//    atinit();
//#endif
//
//#if INCLUDE_LINEEDIT
//    /* Initialize command line history table. */
//    historyinit();
//#endif
//    /* Init shell variable table. */
//    ShellVarInit();
//
//#ifdef MONCOMPTR
//    /* If MONCOMPTR is defined, then verify that the definition matches
//     * the location of the actual moncomptr value.  The definition in
//     * config.h is provided so that outside applications can include that
//     * header file so that the value passed into monConnect() is defined
//     * formally. The REAL value of moncomptr is based on the location of
//     * the pointer in monitor memory space, determined only after the final
//     * link has been done.  As a result, this check is used to simply verify
//     * that the definition matches the actual value.
//     */
//    {
//    if (MONCOMPTR != (unsigned int)&moncomptr)
//        printf("\nMONCOMPTR WARNING: runtime != definition\n");
//    }
//#endif
//
//#if INCLUDE_TFS
//    /* After basic initialization, if the monitor's run-control file
//     * exists, run it prior to EthernetStartup() so that the
//     * MAC/IP addresses can be configured based on shell variables
//     * that would be loaded by the rc file.
//     */
//	tfsrunrcfile();
//#endif

//    /* After MONRC is run, establish monitor flags... */
//    InitMonitorFlags();

    /* Now that all has been initialized, display the monitor header. */
    //if (!MFLAGS_NOMONHEADER())
        appHeader(1);

}

void
init4(void)
{ 
//#if INCLUDE_ETHERNET
//    EthernetStartup(0,1);
//    EthernetStartup(0,1);
//#endif
//#if INCLUDE_TFS
//    tfsrunboot();
//#endif
}

extern void *protocol_Aid(void);

void vConsoleTask(void *pvParameters)
{											//need to add a protocol
	unsigned char cmdline[CMDLINESIZE];
    while (1) {
//    	protocol_Aid();
    	protocol();

console:
        writeprompt();                  			/* Issue prompt */
        if (getline_p((char *)cmdline,CMDLINESIZE,0,0) == 0) {
			goto console;
//        	continue;
		}

		docommand((char *)cmdline,0);

        if (strcmp((char *)cmdline, "exit") == 0){
        	continue;
        }

		goto console;
    }
}

/* inRange():
 * This function is handed a range string and a value.
 * If the value is within the range of the string specified, then
 * return 1; else return 0.
 * The incoming string can be a mix of ranges and values with each
 * range and/or value separated by a comma and a range is specified 
 * by 2 numbers separated by a dash.
 */
int
inRange(char *range, int value)
{
    int     lo, hi;
    char    rcopy[32], *rsp, *comma, *dash;

    /* If the range pointer is NULL, then return 1.
     */
    if (!range)
        return(1);

    /* Scan the range string for valid characters:
     */
    rsp = range;
    while(*rsp) {
        if ((*rsp == ',') || (*rsp == '-') ||
            ((*rsp >= '0') && (*rsp <= '9')))
            rsp++;
        else
            break;
    }
    if (*rsp)
        return(0);

    /* If incoming range string exceeds size of copy buffer, return 0.
     */
    if (strlen(range) > sizeof(rcopy)-1)
        return(0);

    strcpy(rcopy,range);
    rsp = rcopy;
    do {
        comma = strchr(rsp,(int)(','));
        if (comma)
            *comma = 0;
        dash = strchr(rsp,'-');
        if (dash) {
            lo = atoi(rsp);
            hi = atoi(dash+1);
            if ((value >= lo) && (value <= hi))
                return(1);
        }
        else {
            if (value == atoi(rsp))
                return(1);
        }
        rsp = comma+1;
    } while (comma);
    return(0);
}


/**********************************************************************************************************/

