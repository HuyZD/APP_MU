/* cmdtbl.c:
 *  This is the command table used by the monitor.
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
//#include <stdlib.h>
//#include <string.h>

#include "config.h"
#include "stddefs.h"
#include "genlib.h"
//#include "psa.h"
#include "cli.h"
#include <stdint.h>
#include "xbasic_types.h"

#define ULVLCMD  "ulvl"

extern  int Set(), Read();
extern  char *SetHelp[], *ReadHelp[];

extern  int Help();
extern  char *HelpHelp[];

#if INCLUDE_REPEATER
extern 	int Rpt();
extern 	char *RptHelp[];
#endif

extern 	int Tbl();
extern 	char *TblHelp[];

#if INCLUDE_HW_ATT
extern 	int Att();
extern 	char *AttHelp[];
#endif
#if INCLUDE_HW_PLL
extern 	int Pll();
extern 	char *PllHelp[];
#endif

extern 	int Hpa();
extern 	char *HpaHelp[];
//extern 	int AdcCmd();
//extern 	char *AdcHelp[];
//extern 	int DacCmd();
//extern 	char *DacHelp[];
//extern 	int ClkCmd();
//extern 	char *ClkHelp[];
#if INCLUDE_HW_TEMP
extern 	int Temp();
extern 	char *TempHelp[];
#endif

extern 	int Led();
extern 	char *LedHelp[];

#if INCLUDE_HW_ANT
//extern 	int Ant();
//extern 	char *AntHelp[];
#endif

extern 	int Reboot();
extern 	char *RebootHelp[];

#if INCLUDE_ICS
extern 	int Ics();
extern 	char *IcsHelp[];
extern 	int Lpc();
extern 	char *LpcHelp[];
extern 	int Pre();
extern 	char *PreHelp[];
extern 	int Psa();
extern 	char *PsaHelp[];

#endif


extern 	int Nms();
extern 	char *NmsHelp[];

extern 	int Xadc();
extern 	char *XadcHelp[];

extern 	int Br();
extern 	char *BrHelp[];

extern 	int Uart();
extern 	char *UartHelp[];

extern 	int At();
extern 	char *AtHelp[];

extern 	int Alarm();
extern 	char *AlarmHelp[];

extern  int Pwm();
extern  char *PwmHelp[];

extern int Nms(int argc, char *argv[]);
extern char *NmsHelp[];
extern int At(int argc, char *argv[]);
extern char *AtHelp[];

extern int dn_ser(int argc, char *argv[]);

extern int RuCmd(int argc, char *argv[]);
extern char *RuHelp[];

struct monCommand cmdlist[21] = {
    { "help",       Help,       HelpHelp,       0 },
    { "?",          Help,       HelpHelp,       0 },
    { "reset",		Reboot,		RebootHelp,		0 },
//	{ "reboot", 	Reboot, 	RebootHelp, 	0 },
    { "rpt",  		Rpt,		RptHelp,		0 },
    { "tbl",  		Tbl,		TblHelp,		0 },
	{ "att",		Att,		AttHelp,		0 },
	{ "pll",		Pll,		PllHelp,		0 },
	{ "temp",		Temp,		TempHelp,		0 },
	{ "led",		Led,		LedHelp,		0 },
	{ "hpa",		Hpa,		HpaHelp,		0 },
	{ "ics",		Ics,		IcsHelp,		0 },
	{ "pre",		Pre,		PreHelp,		0 },
	{ "lpc",		Lpc,		LpcHelp,		0 },
	{ "psa",		Psa,		PsaHelp,		0 },
	{ "pwm",		Pwm,		PwmHelp,		0 },
	{ "xmodem",		dn_ser,		HelpHelp,		0 },
	{ "uart",		Uart,		UartHelp,		0 },
	{ "xadc",       Xadc,       XadcHelp,       0 },
	{ "br",	       	Br,       	BrHelp,       	0 },
    { "toRu", 		RuCmd,		RuHelp, 		0 },

    { 0,0,0,0 }
};

/* cmdUlvl[]:
 *  This table stores one char per command that contains that command's
 *  user level.  The default user level of all commands is 0, but can
 *  be re-defined by the ulvl -c command.
 */
char cmdUlvl[(sizeof(cmdlist)/sizeof(struct monCommand))];
//
///* setCmdUlvl():
// *  The incoming string is a command name followed by a comma and a user
// *  level (ranging from 0 thru 4).
// *  Return 0 if pass, 1 if new level was user-level rejected, -1 if error.
// */
//
//int
//setCmdUlvl(char *cmdandlevel, int verbose)
//{
//    struct monCommand *cptr;
//    int newlevel, idx;
//    char *comma;
//
//    /* First verify that the comma is in the string... */
//    comma = strchr(cmdandlevel,',');
//    if (!comma)
//        goto showerr;
//
//    /* Retrieve and verify the new level to be assigned... */
//    newlevel = atoi(comma+1);
//    if ((newlevel < MINUSRLEVEL) || (newlevel > MAXUSRLEVEL))
//        goto showerr;
//
//    *comma = 0;
//
//    /* Don't allow adjustment of the ulvl command itself.  It must be
//     * able to run as user level 0 all the time...
//     */
//    if (!strcmp(cmdandlevel,ULVLCMD)) {
//        printf("Can't adjust '%s' user level.\n",ULVLCMD);
//        return(-1);
//    }
//
//    /* Find the command in the table that is to be adjusted... */
//    for(idx=0,cptr=cmdlist;cptr->name;cptr++,idx++) {
//        if (!strcmp(cmdandlevel,cptr->name)) {
//            /* If the command's user level is to be lowered, then the
//             * current monitor userlevel must be at least as high as the
//             * command's current user level...
//             */
//            if ((newlevel < cmdUlvl[idx]) && (getUsrLvl() < cmdUlvl[idx])) {
//                if (verbose)
//                    printf("User-level access denied: %s\n",cmdandlevel);
//                return(1);
//            }
//            cmdUlvl[idx] = newlevel;
//            return(0);
//        }
//    }
//showerr:
//    if (verbose)
//        printf("Input error: %s\n",cmdandlevel);
//    return(-1);
//}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
char *RebootHelp[] = {
    "reboot",
    0
};

int
_Reboot(int argc, char *argv[])
{
	*(volatile u32 *)0xF8000008 = 0xDF0D;
	*(volatile u32 *)0xF8000200 = 0x1;
	while(1);
}

int Reboot(int argc, char *argv[])
{
    // 1. SLCR Unlock (Ă‚Â¹Ä‚ï¿½Ă‚ÂµÄ‚Â¥Ă‚Â½Ä‚Æ’ Ă‚Â¼Ä‚Â¶Ä‚â€¡Ä‚Â )
    *(volatile u32 *)0xF8000008 = 0xDF0D; 
    
    // 2. Ä‚Æ’Ä‚Â¦Ă‚ÂºÄ‚ï¿½Ä‚â€¡Ä‚â€˜ Ă‚Â´Ä‚Â«Ă‚Â±Ä‚Â¢ Ă‚Â½Ä‚Æ’Ă‚Â°Ă‚Â£ (Ă‚Â¼Ă‚Â±Ä‚â€¦Ä‚Æ’ Ă‚Â»Ä‚Â§Ä‚â€¡Ä‚â€”)
    for(volatile int i=0; i<100; i++); 

    // 3. PSS_RST_CTRL Ă‚Â¼Ă‚Â³Ä‚ï¿½Ă‚Â¤
    // Ă‚ÂºÄ‚Â±Ä‚â€ Ă‚Â® 0: Soft Reset Ä‚â€ Ă‚Â®Ă‚Â¸Ă‚Â®Ă‚Â°Ä‚â€¦
    *(volatile u32 *)0xF8000200 = 0x1;

    // 4. Ă‚Â¸Ä‚ï¿½Ă‚Â¸Ä‚Â°Ă‚Â¸Ă‚Â® Ă‚Â¹Ä‚Â¨Ă‚Â¸Ă‚Â®Ă‚Â¾Ä‚Â® (Ä‚â€�Ä‚â€�Ä‚â€ Ä‚â€�Ä‚â‚¬Ä‚ï¿½Ă‚Â·Ă‚Â¯ Ä‚Æ’Ä‚â€“Ä‚â‚¬Ä‚Â»Ä‚Ë†Ă‚Â­ Ă‚Â¹Ä‚Â¦Ä‚ï¿½Ä‚Â¶ Ă‚Â¹Ä‚â€” Ä‚ï¿½Ä‚Â¯Ă‚Â½Ä‚Æ’ Ă‚Â¹Ä‚ï¿½Ă‚Â¿Ă‚Âµ)
    __asm__ __volatile__ ("dsb sy");
    __asm__ __volatile__ ("isb");

    // 5. Ă‚Â¸Ă‚Â®Ă‚Â¼Ä‚â€� Ă‚Â´Ä‚Â«Ă‚Â±Ä‚Â¢
    while(1); 
}



void cmdReset()
{
	*(volatile u32 *)0xF8000008 = 0xDF0D;
	*(volatile u32 *)0xF8000200 = 0x1;
	while(1);
}



#define UMON_BASE_ADDRESS 0x100000

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
