#include <math.h>
#include "config.h"
#include "genlib.h"
#include "fpga.h"
#include "ics.h"
#include "tbl.h"
#include "repeater.h"
#include "tfs.h"
#include "tbl.h"
#include "flash.h"

/*Temp Atten*/
/*0.*/ struct tbl_list dlTempAtt_B28;
/*1.*/ struct tbl_list ulTempAtt_B28;
/*2.*/ struct tbl_list dlTempAtt_B3;
/*3.*/ struct tbl_list ulTempAtt_B3;



struct tbl_temp_info_struct			tinfo;





static int loadFwdTempAtt(char *fileName, struct tbl_list *ptbl);
static void saveFwdTempAtt(char *fileName, struct tbl_list *ptbl);
static void saveTempAttDef(char *fileName, struct tbl_list *ptbl);
static void calNull(char *fileName, char index, struct tbl_list *ptbl);
#if 1
const int DL_B28_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-150,-150,-150,-150,-150,-150,-150,-150,-150,-150,
	-150,-150,-150,-150,-50,0,0,50,150
};

const int UL_B28_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-150,-150,-150,-150,-150,-150,-150,-150,-150,-100,
	-100,-100,-50,-50,0,0,50,100,200
};


const int DL_B3_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-200,-200,-200,-200,-200,-200,-200,-200,-200,-150,
	-150,-150,-100,-50,0,0,50,100,200
};

const int UL_B3_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-350,-350,-350,-350,-350,-350,-350,-350,-350,-300,
	-250,-250,-200,-150,-50,0,50,100,200
};

#endif

#if 0
const int DL_B28_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-250,-250,-250,-250,-250,-250,-250,-150,-100,-100,
	-50,-50,0,0,0,0,50,100,150
};

const int UL_B28_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-250,-250,-250,-250,-250,-250,-250,-150,-100,-100,
	-50,-50,0,0,0,0,50,100,150
};


const int DL_B3_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-250,-250,-250,-250,-250,-250,-250,-150,-100,-100,
	-50,-50,0,0,0,0,50,100,150
};

const int UL_B3_Temp_Tbl[TMP_TBL_SIZE] = 
{
	-250,-250,-250,-250,-250,-250,-250,-150,-100,-100,
	-50,-50,0,0,0,0,50,100,150
};

#endif

static struct tbl_desc table_desc[NUMB_TBL] = {
	{
		"dlTemp_B28",			//  Table B28 DL Temp : 0
		&dlTempAtt_B28,
		TMP_TBL_SIZE,
		loadFwdTempAtt,
		saveFwdTempAtt,
		saveTempAttDef,
		calNull
	},
	{
		"ulTemp_B28",			//  Table B28 UL Temp : 1
		&ulTempAtt_B28,
		TMP_TBL_SIZE,
		loadFwdTempAtt,
		saveFwdTempAtt,
		saveTempAttDef,
		calNull
	},
	
	{
		"dlTemp_B3", 			//	Table B3 DL Temp : 2
		&dlTempAtt_B3,
		TMP_TBL_SIZE,
		loadFwdTempAtt,
		saveFwdTempAtt,
		saveTempAttDef,
		calNull
	},
	{
		"ulTemp_B3", 			//	Table B3 UL Temp : 3
		&ulTempAtt_B3,
		TMP_TBL_SIZE,
		loadFwdTempAtt,
		saveFwdTempAtt,
		saveTempAttDef,
		calNull
	},
};

/*	
	Band 0: B28
	     1: B3
	Type: 6 DL temp
	      7 UL temp
*/
struct tbl_desc *getTblDesc(unsigned char band, unsigned char tblType)
{
	short tblIndex = (band * 2) + (tblType - 6);

	return (&table_desc[tblIndex]);
	//return &table[0];
}

static int loadFwdTempAtt(char *fileName, struct tbl_list *ptbl)
{
	int		fd;	

	// fd = mon_tfsopen(fileName, TFS_RDONLY, NULL);
	// if (fd < 0) {
	// 	printf("%s:%s\n",(char *)mon_tfsctrl(TFS_ERRMSG,fd,0),fileName); 
	// 	return (-1);
	// }
	// mon_tfsread(fd, (char *)(ptbl), sizeof(struct tbl_list));
	// mon_tfsclose(fd,NULL);

    return (0);
}

static void saveFwdTempAtt(char *fileName, struct tbl_list *ptbl)
{
	int	    fd;	

	// fd = mon_tfsopen(fileName, TFS_CREATERM, NULL);
	// if (fd < 0) {
	// 	printf("%s:%s\n",(char *)mon_tfsctrl(TFS_ERRMSG,fd,0),fileName); 
	// 	return (-1);
	// }
	// mon_tfswrite(fd, (char *)(ptbl), sizeof(struct tbl_list));
	// mon_tfsclose(fd,NULL);	
}


static void initTempAttDef(char *fileName, struct tbl_list *ptbl)
{
	int 	i;
	if (strcmp(fileName, "dlTemp_B28") == 0)
	{
		ptbl->header.step	= 5;	
		ptbl->header.start	= -20;
		ptbl->header.end	= 70;
		ptbl->header.size  	= TMP_TBL_SIZE;
		ptbl->header.tbl_id = 0; 

		for(i=0;i<ptbl->header.size;i++) {
			ptbl->idx[i] = ptbl->header.start + ptbl->header.step*i;	
			ptbl->val[i] = tinfo.tbl_dl_temp_struct_B28.val[i];
//			ptbl->val[i] = DL_B28_Temp_Tbl[i];
		}

		for( i = 0; i< TMP_TBL_SIZE;i++)
		{
//			printf("dlTemp_B28[%d]:[%d][%d]\n",i,ptbl->idx[i],ptbl->val[i]);
		}
	}
	else if (strcmp(fileName, "ulTemp_B28") == 0)
	{
		ptbl->header.step	= 5;	
		ptbl->header.start	= -20;
		ptbl->header.end	= 70;
		ptbl->header.size  	= TMP_TBL_SIZE;
		ptbl->header.tbl_id = 1; 

		for(i=0;i<ptbl->header.size;i++) {
			ptbl->idx[i] = ptbl->header.start + ptbl->header.step*i;	
//			ptbl->val[i] = UL_B28_Temp_Tbl[i];
			ptbl->val[i] = tinfo.tbl_ul_temp_struct_B28.val[i];
		}
		for( i = 0; i< TMP_TBL_SIZE;i++)
		{
//			printf("ulTemp_B28[%d]:[%d][%d]\n",i,ptbl->idx[i],ptbl->val[i]);
		}

	}
	else if (strcmp(fileName, "dlTemp_B3") ==0)
	{
		ptbl->header.step	= 5;	
		ptbl->header.start	= -20;
		ptbl->header.end	= 70;
		ptbl->header.size  	= TMP_TBL_SIZE;
		ptbl->header.tbl_id = 2; 

		for(i=0;i<ptbl->header.size;i++) {
			ptbl->idx[i] = ptbl->header.start + ptbl->header.step*i;	
//			ptbl->val[i] = DL_B3_Temp_Tbl[i];
			ptbl->val[i] = tinfo.tbl_dl_temp_struct_B3.val[i];
		}
		for( i = 0; i< TMP_TBL_SIZE;i++)
		{
//			printf("dlTemp_B3[%d]:[%d][%d]\n",i,ptbl->idx[i],ptbl->val[i]);
		}
	}
	else if (strcmp(fileName, "ulTemp_B3") == 0)
	{
		ptbl->header.step	= 5;	
		ptbl->header.start	= -20;
		ptbl->header.end	= 70;
		ptbl->header.size  	= TMP_TBL_SIZE;
		ptbl->header.tbl_id = 3; 

		for(i=0;i<ptbl->header.size;i++) {
			ptbl->idx[i] = ptbl->header.start + ptbl->header.step*i;	
//			ptbl->val[i] = UL_B3_Temp_Tbl[i];
			ptbl->val[i] = tinfo.tbl_ul_temp_struct_B3.val[i];
		}
		for( i = 0; i< TMP_TBL_SIZE;i++)
		{
//			printf("ulTemp_B3[%d]:[%d][%d]\n",i,ptbl->idx[i],ptbl->val[i]);
		}		
	}
}


static void saveTempAttDef(char *fileName, struct tbl_list *ptbl)
{
	saveFwdTempAtt(fileName, ptbl);
}


static void calNull(char *fileName, char index, struct tbl_list *ptbl)
{	
	return;
}

static struct tbl_list * getTbl(char *fileName)
{
	if (strcmp(fileName, "dlTemp_B28") == 0)
	{
		return &dlTempAtt_B28;
	}
	else if (strcmp(fileName, "ulTemp_B28") == 0)
	{
		return &ulTempAtt_B28;
	}
	else if(strcmp(fileName, "dlTemp_B3") == 0)
	{
		return &dlTempAtt_B3;
	}
	else if(strcmp(fileName, "ulTemp_B3") == 0)
	{
		return &ulTempAtt_B3;
	}

	return NULL;
}

static void saveTempAtt(char *fileName, struct tbl_list *ptbl)
{
	int 	i;
	struct tbl_list  *my_tbl = getTbl(fileName);

	for (i = 0; i < ptbl->header.size; i++) 
	{
		my_tbl->idx[i] = ptbl->idx[i];	
		my_tbl->val[i] = ptbl->val[i];
	}

	saveFwdTempAtt(fileName, my_tbl);
}

static char *showfloat(char *buf, short val)
{
	int 	ival,fval;
	char 	isNeg;
	char	sign[2]={'+','-'};
	char 	*str;
	
	str = buf;
	
	isNeg = (val&0x8000) ? 1 : 0;
	
	ival = (int)val;
	fval = abs(ival)%10;
	ival = abs(ival)/10;
	sprintf(buf, " %c%2d.%d",sign[isNeg],ival,fval);
	
	return (str);
}

void viewTable(struct tbl_desc *pdesc)
{
	int 	i;
	char 	buf1[32],buf2[32];
	
	printf("Table %s, size %d\n",pdesc->fname, pdesc->table->header.size);
	for (i=0;i<pdesc->table->header.size;i++) {
		printf("[%d]>  %d : %d\n", i, pdesc->table->idx[i], pdesc->table->val[i]);
	}
}

int init_tbl(void)
{	
	loadTemp(0);

	initTempAttDef("dlTemp_B28", &dlTempAtt_B28);
	initTempAttDef("ulTemp_B28", &ulTempAtt_B28);
	initTempAttDef("dlTemp_B3", &dlTempAtt_B3);
	initTempAttDef("ulTemp_B3", &ulTempAtt_B3);

	return 0;	
}

void saveTemp(void)
{
	for(int i = 0; i< TMP_TBL_SIZE;i++)
	{
		tinfo.tbl_dl_temp_struct_B3.val[i]= DL_B3_Temp_Tbl[i];		
		tinfo.tbl_ul_temp_struct_B3.val[i]= UL_B3_Temp_Tbl[i];		

		tinfo.tbl_dl_temp_struct_B28.val[i]= DL_B28_Temp_Tbl[i];		
		tinfo.tbl_ul_temp_struct_B28.val[i]= UL_B28_Temp_Tbl[i];		
	}

	saveTempTbl(&tinfo);
}

void loadTemp(int _prt)
{
	loadTempTbl(&tinfo);

	if(_prt)
	{
		for(int i = 0; i< TMP_TBL_SIZE;i++)
		{
			printf("tbl_dl_temp_struct_B3[%d]:[%d][%d]\n",i,tinfo.tbl_dl_temp_struct_B3.idx[i],tinfo.tbl_dl_temp_struct_B3.val[i]);
		}
		for(int i = 0; i< TMP_TBL_SIZE;i++)
		{
			printf("tbl_ul_temp_struct_B3[%d]:[%d][%d]\n",i,tinfo.tbl_ul_temp_struct_B3.idx[i],tinfo.tbl_ul_temp_struct_B3.val[i]);
		}
		for(int i = 0; i< TMP_TBL_SIZE;i++)
		{
			printf("tbl_dl_temp_struct_B28[%d]:[%d][%d]\n",i,tinfo.tbl_dl_temp_struct_B28.idx[i],tinfo.tbl_dl_temp_struct_B28.val[i]);
		}
		for(int i = 0; i< TMP_TBL_SIZE;i++)
		{
			printf("tbl_ul_temp_struct_B28[%d]:[%d][%d]\n",i,tinfo.tbl_ul_temp_struct_B28.idx[i],tinfo.tbl_ul_temp_struct_B28.val[i]);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
static int TableType = 0;			/* 0 ~ 6 */

char *TblHelp[] = {
    "Table",
    "-[i:]-[p:]-[v:]",
    "",
    "Options:",
    "-i index",
    "-p dBm",
    "-v value",
    "",
    "Operations:",
    "type, adc, info, set, save",
    "0->FWD_DNC_ATT,1->FWD_UPC_ATT,2->RVS_DNC_ATT,3->RVS_UPC_ATT,4->TEMP",
    "NOTE: none",
    0
};

int 
Tbl(int argc, char *argv[])
{
    char 				*arg1;
    int 				opt;
	int 				i;
	int 				val;
	int 				idx,dBm;
	struct tbl_desc 	*pdesc; 


    while((opt=getopt(argc,argv,"v:p:i:")) != -1) {
        switch (opt) {
            case 'v':
            	val = (int)strtoul(optarg,0,0);
                break;
            case 'p':
            	dBm = (unsigned int)strtoul(optarg,0,0);
                break;
            case 'i':
            	idx = (unsigned int)strtoul(optarg,0,0);
				pdesc = &table_desc[idx];
                break;
            default:
                return(0);
        }
    }

    arg1 = argv[optind];

	if (strcmp(arg1, "type") == 0) {
        int tmptype;
        if (argc == 3) {
            tmptype = atoi(argv[2]);
            if (tmptype < 12) {

			}
        }
        else {
        }
	}
    else if (strcmp(arg1, "adc") == 0) {

	}
	else if (strcmp(arg1, "info") == 0) {
		viewTable(pdesc);
	}
	else if (strcmp(arg1, "set") == 0) {
    
        if (argc == 8) {

		}
	}
	else if (strcmp(arg1, "def") == 0) {
		printf("Temp Tbl def \n");

		init_tbl();
	}
	else if (strcmp(arg1, "save") == 0) {
		printf("Temp Tbl Save \n");

		saveTemp();
	}
	else if (strcmp(arg1, "load") == 0) {
		printf("Temp Tbl load \n");

		loadTemp(1);
	}	
	else if (strcmp(arg1, "save") == 0) {

	}
	else {

	}
	
	return 0;
}


