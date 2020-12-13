#pragma once
#include <iostream>
#include <string>

using namespace std;
typedef struct SIMRE_LIB_PARAM
{
    //信号参数-数据结构
    // 每秒种产生的触发脉冲数目，称为脉冲重复频率。以PRF(Pulse-Recurrence-Frequency) 表示
    // 两个相邻脉冲之间的时间间隔，称为脉冲重复周期，用PRT表示，它等于脉冲重复频率的倒数
    int		nPRT;		//雷达重复周期					脉冲重复频率取4K, 重复周期为其倒数
    int		nPW;		//脉宽   						脉宽10us
    int		nPulse;		//一组脉冲中的子脉冲个数		16
    int		nBand;		//调频带宽						调频带宽50M
    int		nDeltaf;	//调频带宽步进频率				??
    int		nSampling;	//采样率						采样率60MHz

    int		nGroup;		//脉冲组数						脉冲组数512
    float	nRF;		//雷达载频						发射频率10G=10000
    float	amplitude;	//目标回波幅度
    float	range;		//目标径向位置
    float	velcity;	//目标径向速度
    float	omega;		//目标旋转角
    float	acc;		//目标径向加速度
} SIMRE_LIB_PARAM;

class SimDataGen
{
public:
    // 信号参数相关
    int m_band;
    int m_count;
    int m_deltaf;
    int m_num;
    float m_omega;
    int m_prt;
    int m_psWide;
    float m_range;
    int m_sampling;
    float m_acc;
    int m_frqn;
    float m_ampt;
    float m_velcity;
//    string m_file;    // 输入模型文件路径
//    char* m_echo;    // 输出仿真数据路径

    // 数据存储相关
//    FILE* fp;
    float *px;
    float *py;
    int	pos;

public:

    SimDataGen(SIMRE_LIB_PARAM param);
    ~SimDataGen();
    float GetTimeDelay(float range);
    void GetTgtRange(float x, float y, float w, float R, float v, float a, float* t, int count, int tm, float *rge);
    void ChirpSig(int psWide, float*tbuf, int count, float* rge, float amg, float cf, float gama, int tm, float* real, float* imag);
    void ChirpRef(int psWide, float*tbuf, int count, float rge, float amg, float cf, float gama, int tm, float* real, float* imag);
    void ComplexConjMult(int count, float* Irl, float*Iig, float* frl, float* fig, float *prl, float*pig);
    unsigned int SimIsarEchoes(int nCell, unsigned int nFrame, float *pRl, float *pIg);
    void GenData(char *m_echo);

};
