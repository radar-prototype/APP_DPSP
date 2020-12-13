#ifndef RSPL_HEADER_H
#define RSPL_HEADER_H

#include "string.h"


/*
#if defined(__cplusplus)
extern "C" {
#endif*/

#ifdef __cplusplus
extern "C" {
#endif
	
typedef unsigned long int  RSPL_ISO_ADDR;		//64λ����ϵͳ�趨
//typedef unsigned int  RSPL_ISO_ADDR;		//32λ����ϵͳ�趨

#define RSPL_BUF_RLBIT	 7
#define RSPL_OK		0
#define RSPL_ERR	-1

typedef signed int rspl_sta;

typedef enum 
{
	RLL_NONE,
	RLL_ERROR,
	RLL_WARNING,
	RLL_DEBUG,
	RLL_INFO
}RSPL_LOG_LEVEL;


typedef enum
{
	RLT_NONE	= 0x00,
	RLT_CONSOLE	= 0x01,
	RLT_FILE	= 0x10
}RSPL_LOG_TARGET;


////////////////////////////////////˫������������������������/////////////////////////////////////////////////////
typedef enum 
{
	RBS_EMPTY,
	RBS_OCCUPY,
	RBS_BUSY
}RSPL_BUF_STATUS;



void rspl_initialize(int iLogLevel, int iLogTarget, char* logfileName);

typedef struct RSPL_LINKING_ICOMP
{
	char						*pbuf;
	unsigned short			sPage;
	char						cReserved[2];
	struct RSPL_LINKING_ICOMP 	*prev;
	struct RSPL_LINKING_ICOMP 	*next;
}RSPL_LINKING_ICOMP;

RSPL_LINKING_ICOMP* rspl_comm_InitLinking(int num, int buflen);
void rspl_comm_uninitLinking(RSPL_LINKING_ICOMP* pLink);

char* rspl_comm_get_romate_datbuf(char* pbuf);
void rspl_comm_set_romate_flag(char* pbuf, RSPL_BUF_STATUS iBufStatus);
void rspl_comm_clear_romate_flag(char* pbuf);
void rspl_comm_set_romate_cnt(char* pbuf, char sel, char cnt);
void rspl_comm_count_romate_inv(char* pbuf, char sel);
void rspl_comm_count_romate_fwd(char* pbuf, char sel);
char rspl_comm_get_romate_cnt(char* pbuf, char sel);


char* rspl_comm_get_datbuf(RSPL_LINKING_ICOMP* pLink);
char rspl_comm_get_flag(RSPL_LINKING_ICOMP* pLink);
void rspl_comm_set_flag(RSPL_LINKING_ICOMP* pLink, RSPL_BUF_STATUS iBufStatus);
void rspl_comm_clear_flag(RSPL_LINKING_ICOMP* pLink);
char rspl_comm_get_cnt(RSPL_LINKING_ICOMP* pLink, char sel);
void rspl_comm_set_cnt(RSPL_LINKING_ICOMP* pLink, char sel, char cnt);
void rspl_comm_cnt_count(RSPL_LINKING_ICOMP* pLink, char sel);
RSPL_LINKING_ICOMP* rspl_comm_buff_overflow(RSPL_LINKING_ICOMP* pLink);

//////////////////////////////////simulate target/////////////////////////////////////////////
typedef struct rspl_simobject_io
{
	unsigned int 	num_thread;			//�����߳���
	unsigned int 	sMaxRgeCell;
	unsigned int	cMainBeamCnt;
	unsigned int	cSideBeamCnt;
//	unsigned int nFrameLen;
//	unsigned int ipPort;
//	char* ipLocalAddr;
//	char* ipMcastAddr;
//	char* ipSendAddr;
}RSPL_SIM_ICOMP;

struct	rspl_simobject_env;
typedef struct rspl_simobject_env RSPL_SIM_ENV;

RSPL_SIM_ENV* rspl_simulate_target_init(RSPL_SIM_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink);
rspl_sta rspl_simulate_target_uninit(RSPL_SIM_ENV* pEnv, RSPL_SIM_ICOMP* pComp, RSPL_LINKING_ICOMP* pLink);
unsigned int rspl_simulate_target_entry(RSPL_SIM_ENV* pEnv, int* nFrame, char* pOutLink);


//////////////////////////////////net reciver/////////////////////////////////////////////
typedef struct rspl_recvobject_io
{
	char* 			pMalBuf;			//ʵ�ֿ��ٺõĴ�Ķ�̬�ڴ棬 Ҫ��������ʱ����ɣ����ٲ��ɹ�����NULL;
	unsigned int 	nMalLen;			//�ڴ��С 

	unsigned int 	num_thread;	//�����߳���
	unsigned int 	len_block;
	
	unsigned int 	ipPort;
	char* 			ipLocalAddr;
	char* 			ipMcastAddr;
	
}RSPL_RECV_ICOMP;

struct	rspl_recvobject_env;
typedef struct rspl_recvobject_env RSPL_RECV_ENV;

RSPL_RECV_ENV* rspl_dbf_recv_init(RSPL_RECV_ICOMP* pComp);
rspl_sta rsp_dbf_recv_entry(RSPL_RECV_ENV* pEnv, char* buff, char* pOutLink);


//////////////////////////digital pulse compress/////////////////////////////////////////
typedef struct rspl_dpcobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;
	unsigned int	cMainBeamCnt;
	unsigned int	cSideBeamCnt;
	
	unsigned int 	num_filter;	//ƥ���˲�������
	char** 			filename;
}RSPL_DPC_ICOMP;

struct rspl_dpcobject_env;
typedef struct rspl_dpcobject_env RSPL_DPC_ENV;

RSPL_DPC_ENV* rspl_pulse_compress_init(RSPL_DPC_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink);
rspl_sta rspl_pulse_compress_uninit(RSPL_DPC_ENV* pEnv, RSPL_DPC_ICOMP* pComp, RSPL_LINKING_ICOMP* pLink);
unsigned int rspl_pulse_compress_dgn_entry(char* buff, char* pOut);
unsigned int rspl_pulse_compress(RSPL_DPC_ENV* pEnv, char* pdat, unsigned int thread_id);



//////////////////////////sidelobe canceller/////////////////////////////////////////
typedef struct rspl_slcobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;
	unsigned int	sCPINum;
	unsigned int	cMainBeamCnt;
	
	unsigned int	cSideBeamCnt;
	unsigned int 	num_sRgeCell;	//���괦����뵥Ԫ��
}RSPL_SLC_ICOMP;


struct rspl_slcobject_env;
typedef struct rspl_slcobject_env RSPL_SLC_ENV;

RSPL_SLC_ENV* rspl_sidelobe_canceller_init(RSPL_SLC_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink);
rspl_sta rspl_sidelobe_canceller_uninit(RSPL_SLC_ENV* pEnv, RSPL_SLC_ICOMP* pComp, RSPL_LINKING_ICOMP* pLink);
float rspl_sidelobe_canceller(RSPL_SLC_ENV* pEnv, char* buff, char* pOut, unsigned int thread_id);
unsigned int rspl_sidelobe_canceller_entry(char* buff, RSPL_LINKING_ICOMP* pLink, RSPL_LINKING_ICOMP** pRecv);


////////////////////////////////////target detetion////////////////////////////////////////////////////

typedef struct rspl_mtdobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;
	unsigned int	sCPINum;
	unsigned int	cMainBeamCnt;
}RSPL_MTD_ICOMP;

struct rspl_mtdobject_env;
typedef struct rspl_mtdobject_env RSPL_MTD_ENV;

RSPL_MTD_ENV* rspl_target_detection_init(RSPL_MTD_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink, RSPL_LINKING_ICOMP** pIQLink);
int rspl_target_detection_entry(char* buff, unsigned int * pLen);
rspl_sta rspl_target_detection_uninit(RSPL_MTD_ENV* pEnv, RSPL_MTD_ICOMP* pComp, RSPL_LINKING_ICOMP* pLink, RSPL_LINKING_ICOMP* pLink2);
unsigned int rspl_target_detection(RSPL_MTD_ENV* pEnv, char* buff, char* pOut, char* pOut2, unsigned int* pOut2Len, unsigned int thread_id);



////////////////////////////////sum and difference angle estimation///////////////////////////
typedef struct rspl_sdaeobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;	//���������뵥Ԫ��
	unsigned int	sCPINum;
	unsigned int	cMainBeamCnt;
}RSPL_SDAE_ICOMP;

struct rspl_sdaeobject_env;
typedef struct rspl_sdaeobject_env RSPL_SDAE_ENV;

RSPL_SDAE_ENV* rspl_angle_estimate_init(RSPL_SDAE_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink, RSPL_LINKING_ICOMP** pIntLink);
int rspl_angle_estimate_entry2(char* buff, char* out);
rspl_sta rspl_angle_estimate_uninit(RSPL_SDAE_ENV* pEnv, 
									RSPL_SDAE_ICOMP* pComp, 
									RSPL_LINKING_ICOMP* pLink,
									RSPL_LINKING_ICOMP* pIntLink);

unsigned int rspl_angle_estimate(RSPL_SDAE_ENV* pEnv, 
								 RSPL_LINKING_ICOMP* pIntLink, 
								 char* buff, 
								 char* pOut, 
								 unsigned int thread_id);
////////////////////////////////////CFAR////////////////////////////////////////////////////

typedef struct rspl_cfarobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;	//���������뵥Ԫ��
	unsigned int	sCPINum;
	unsigned int	cMainBeamCnt;
}RSPL_CFAR_ICOMP;

struct rspl_cfarobject_env;
typedef struct rspl_cfarobject_env RSPL_CFAR_ENV;

int rspl_mtd_cfar_entry(char* buff, RSPL_LINKING_ICOMP* pLink, RSPL_LINKING_ICOMP** pRecv);
unsigned int rspl_mtd_cfar(RSPL_CFAR_ENV* pEnv, char* buff, char* pOut, unsigned int thread_id);
RSPL_CFAR_ENV* rspl_mtd_cfar_init(RSPL_CFAR_ICOMP* pComp, RSPL_LINKING_ICOMP** pLink);
rspl_sta rspl_mtd_cfar_uninit(RSPL_CFAR_ENV* pEnv, RSPL_CFAR_ICOMP* pComp, RSPL_LINKING_ICOMP* pLink);


///////////////Amplitude comparison and angle measurement////////////////////////////////////////

typedef struct rspl_acamobject_io
{
	unsigned int 	num_thread;	//�����߳���
	unsigned int 	sMaxRgeCell;	//���������뵥Ԫ��
	unsigned int	cMainBeamCnt;
}RSPL_ACAM_ICOMP;

struct rspl_acamobject_env;
typedef struct rspl_acamobject_env RSPL_ACAM_ENV;


///////////////////////////////////////////////////////////////////////////////////////////////////
#define RSPL_MAX_BEAM		32

// ��log
int log_writeLog(RSPL_LOG_LEVEL loglevel,         // Log����
				 const char* date,
				 const char* time,
				 const char* fileName,   // ���������ļ���
				 const char* function,   // ������
				 int lineNumber,            // �к�
				 const char* format,              // ��ʽ��
				 ...);                      // ����

#define LOG_INFO(...)		log_writeLog(RLL_INFO,__DATE__, __TIME__, __FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)		log_writeLog(RLL_DEBUG, __DATE__, __TIME__, __FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)
#define LOG_WARNING(...)	log_writeLog(RLL_WARNING, __DATE__, __TIME__,__FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)
#define LOG_ERROR(...)		log_writeLog(RLL_ERROR, __DATE__, __TIME__, __FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)
#define LOG_NONE(...)		log_writeLog(RLL_NONE, __DATE__, __TIME__, __FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)
/*
#if defined(__cplusplus)
}
#endif*/
#ifdef __cplusplus
}
#endif


typedef struct rspl_thread_param
{
	char* pEnv;
	char* pIn;
	char* pOut;
	char* pOut2;
	char* pOut3;
	unsigned int offset;
}rspl_thread_param;
unsigned int rspl_comm_getime(struct timeval tv_e, struct timeval tv_b);




#endif
