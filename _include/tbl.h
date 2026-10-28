#ifndef _TBL_H_
#define _TBL_H_	

#define	NUMB_TBL		4


#define	TMP_TBL_SIZE	(19)  //(22+1)
#define	ATT_TBL_SIZE	(60+1)


struct tbl_temp_struct{
	short idx[TMP_TBL_SIZE];
	short val[TMP_TBL_SIZE];
};

struct tbl_att_struct{
	short idx[ATT_TBL_SIZE];
	short val[ATT_TBL_SIZE];
};

struct tbl_temp_info_struct{
	struct tbl_temp_struct tbl_dl_temp_struct_B3;
	struct tbl_temp_struct tbl_ul_temp_struct_B3;
	struct tbl_temp_struct tbl_dl_temp_struct_B28;
	struct tbl_temp_struct tbl_ul_temp_struct_B28;
};


struct hpa_info {
	u8  hpa_onoff;               //1: on 0: off
	u8  alc_onoff;
	u8	asd_onoff;
};


struct hpa_info_struct{
	struct hpa_info tbl_dl_hpa_B3;
	struct hpa_info tbl_ul_hpa_B3;
	struct hpa_info tbl_dl_hpa_B28;
	struct hpa_info tbl_ul_hpa_B28;
};



struct tbl_att_info_struct{
   struct tbl_att_struct tbl_dl_dnc_struct_B3;
   struct tbl_att_struct tbl_ul_dnc_struct_B3;
   struct tbl_att_struct tbl_dl_upc_struct_B3;
   struct tbl_att_struct tbl_ul_upc_struct_B3;

   struct tbl_att_struct tbl_dl_dnc_struct_N78;
   struct tbl_att_struct tbl_ul_dnc_struct_N78;
   struct tbl_att_struct tbl_dl_upc_struct_N78;
   struct tbl_att_struct tbl_ul_upc_struct_N78;
};


extern int init_tbl(void);


#define	DL_OUT_MAX		(short)160		/* DL:+10dBm ����(10dBm 10mW)+6dB */
#define	UL_OUT_MAX		(short)190		/* UL:+13dBm ����(13dBm 20mW)+6dB */

                                                                               
struct tbl_info {                                                                               
	char				tbl_id;                                                
	short 				step;                                                  
	short 				start;                                                 
	short 				end;                                                   
	char 				size;                                                  
};                                                                             
                                                                               
struct tbl_list {                                                              
	struct tbl_info		header;             
	short				idx[TMP_TBL_SIZE];                                     
	short				val[TMP_TBL_SIZE];	                                     
                                              
};                                                                             
                                                                                                                                                            
struct tbl_desc {                                                              
	char 				*fname;                                                
	struct tbl_list 	*table;                                                
	int 				size;                                                  
	int   				(*Load)(char *fname, struct tbl_list *ptbl);           
	void   				(*Save)(char *fname, struct tbl_list *ptbl);           
	void   				(*SaveDef)(char *fname, struct tbl_list *ptbl);        
	void   				(*Cal)(char *fname, char index, struct tbl_list *ptbl);
                                                                               
	int 				startVal;                                              
};               


//static short hi[32] = {	 0, 10848,	   0,	  0, -4427,-32766,-32763,  -924,	 0, 	6, 27490,	  0,	 2,    -9,	  -1,-11169,	-4, 	1,	   2,	  1,  9570, 	6,	   3,	 -3, -8513,   -10,	   4,	  0,	 8, 10061,	   2, -7382};
//static short hq[32] = {-4225,    -3,	   6,	  0, 32746,    14,	   5,	 -7, -7228,   -11,	   0,	  0,	21,  2579,	   5,	  0,	-6, -2080,	 -21,	 -4,	 2, 	5,	3550,	  5,	 2,    -3, -3619,	  0,	 0, 	0,	2296,	  0};



#endif
