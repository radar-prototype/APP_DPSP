#include "math.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "SimDataGen.h"
#include <QDebug>

using namespace std;

/*********************************************
描述：构造函数，根据参数构造类
输入参数：
    信号参数
返回值：
    无
***********************************************/
SimDataGen::SimDataGen(SIMRE_LIB_PARAM param)
//SimDataGen::SimDataGen(SIMRE_LIB_PARAM param, char * m_echo_path)
{
    // 参数初始化
    m_prt		= param.nPRT;		//雷达重复周期
    m_frqn		= param.nRF;		//雷达载频
    m_psWide	= param.nPW;		//脉宽;
    m_num		= param.nPulse;		//一组脉冲中的子脉冲个数;
    m_band		= param.nBand;		//调频带宽;
    m_deltaf	= param.nDeltaf;	//调频带宽步进频率
    m_sampling  = param.nSampling;	//采样率;

    m_count		= param.nGroup;		//脉冲组数
    m_ampt		= param.amplitude;	//目标回波幅度;
    m_range		= param.range;		//目标径向位置
    m_velcity   = param.velcity;	//目标径向速度
    m_omega		= param.omega;		//目标旋转角
    m_acc		= param.acc;		//目标径向加速度;
//    m_echo = m_echo_path;         //文件路径

    // 目标物数据定义 初始化为五个点
    pos = 5;
    px = (float*)malloc(sizeof(float) * pos);
    py = (float*)malloc(sizeof(float) * pos);
    px[0] = 0.0f;
    px[1] = 0.0f;
    px[2] = 0.0f;
    px[3] = -1.0f;
    px[4] = 0.0f;

    py[0] = 0.0f;
    py[1] = 1.0f;
    py[2] = -1.0f;
    py[3] = 0.0f;
    py[4] = 0.0f;
}
/*********************************************
描述：析构函数，释放px和py
输入参数：
    无
返回值：
    无
***********************************************/
SimDataGen::~SimDataGen()
{
    if(py)
    {
        free(py);
        py = nullptr;
    }
    if(px)
    {
        free(px);
        px = nullptr;
    }
}

/*********************************************
描述：求目标时延
输入参数：
	range：目标位置，单位Km
返回值：
	雷达回波延迟时间，单位us
***********************************************/
float SimDataGen::GetTimeDelay(float range)
{
    float delay;
    delay = 20 * range / 3;
    return delay;
}

/************************************************************************************************************
函数描述：仿真目标回波
输入参数：
    x，y：目标上 一点离目标中心的位置, 单位m
    w：目标自转角速度, 单位弧度rad/s [QUESTION]
	a，v：目标径向运动加速度和速度
    R:目标中心距离雷达的距离，单位Km
	t：子脉冲内的快速时间变量，单位us
    count：快速时间变量个数
    tm：慢时间变量, 单位us
输出函数：
	rge：输出目标径向距离信息，单位Km
************************************************************************************************************/
void SimDataGen::GetTgtRange(float x, float y, float w, float R, float v, float a,
                             float* t, int count, int tm, float *rge)
{
    float ts, fai;
    for(int i = 0; i < count; i++)
    {
        ts = (t[i] + tm) / 1000000.0f;
        fai = w * ts;
        rge[i] = static_cast<float>(R + (v * ts + 0.5f * a * ts * ts + y * cos(fai) - x * sin(fai)) / 1000.0f);
    }
}

/************************************************************************************************************
函数描述：仿真目标某个位置上的线性调频信号, 生成步进频调频信号
输入参数：
	psWide：脉冲宽度 us
	tbuf:   采样时间
	count：脉宽内的采样点数
	rge：目标上某点离雷达的径向距离 单位Km
	cf: 载频，单位MHz
	gama: 调频斜率，单位MHz/us
	tm	:慢时间
	num: 子脉冲数
	prt: 雷达重频：us
输出函数：
	real：输出回波缓冲区，实部
	imag: 输出回波缓冲区，虚部
************************************************************************************************************/
void SimDataGen::ChirpSig(int psWide, float*tbuf, int count, float* rge, float amg, float cf, float gama, int tm, float* real, float* imag)
{
    double fai, td, t, tp2, f0, beta;
    tp2 = psWide / 2.0f;

    f0 = 6.2831852f * cf;
    beta = gama * 3.1415926f;

    for(int i = 0; i < count; i++)
    {
        td = GetTimeDelay(rge[i]);
        t = tbuf[i] - td;		//us
        if((t >= -tp2) && (t < tp2))
        {
            fai = f0 * (t + tm) + beta * t * t;			//2*pi*(f0+n*B)*(t+prt*tm) + pi*gama*t*t
            real[i] = (float)(amg * cos(fai));
            imag[i] = (float)(amg * sin(fai));
        }
        else
        {
            real[i] = 0;
            imag[i] = 0;
        }
    }
}

/************************************************************************************************************
函数描述：仿真某个距离上的基准信号，是用于相干检波的，因此不带线性调频部分数据
输入参数：
	psWide：脉冲宽度 us
	tbuf:   采样时间
	Count：脉宽内的采样点数
	rge：目标上某点离雷达的径向距离 单位Km
	cf: 载频，单位MHz
	gama: 调频斜率，单位MHz/us
	tm	:慢时间
	num: 子脉冲数
	prt: 雷达重频：us
输出函数：
	real：输出回波缓冲区，实部
	imag: 输出回波缓冲区，虚部
************************************************************************************************************/
void SimDataGen::ChirpRef(int psWide, float*tbuf, int count, float rge, float amg, float cf, float gama, int tm, float* real, float* imag)
{
    double fai, td, t, tp2, f0, beta;

    tp2 = psWide / 2.0;

    f0 = 6.2831852 * cf;
    beta = gama * 3.1415926;
    td = GetTimeDelay(rge);

    for(int i = 0; i < count; i++)
    {
        t = tbuf[i] - td;		//us
        if((t >= -tp2) && (t < tp2))
        {
            fai = f0 * (t + tm)/* + beta*t*t*/;			//2*pi*(f0+n*B)*(t+prt*tm) + pi*gama*t*t
            real[i] = (float)(amg * cos(fai));
            imag[i] = (float)(amg * sin(fai));

        }
        else
        {
            real[i] = 0;
            imag[i] = 0;
        }
    }
}

/************************************************************************************************************
函数描述：共轭相乘累加 相干检波，就是相乘，去掉载频
输入参数：
	count：脉冲宽度 us
	Irl:   采样时间
	Iig：脉宽内的采样点数
	frl：目标上某点离雷达的径向距离 单位Km
	fig: 载频，单位MHz
输出函数：
	prl：输出回波缓冲区，实部
	pig: 输出回波缓冲区，虚部
************************************************************************************************************/

void SimDataGen::ComplexConjMult(int count, float* Irl, float*Iig, float* frl, float* fig, float *prl, float*pig)
{
    for(int i = 0; i < count; i++)
    {
        prl[i] += Irl[i] * frl[i] + Iig[i] * fig[i];
        pig[i] += Iig[i] * frl[i] - Irl[i] * fig[i];
    }
}

/*******************************************************************************
函数描述：模拟步进频ISAR成像的目标回波数据
具体步骤：   1. 计算运动目标上每一个成像点距离雷达的径向距离
		  2. 计算每个子脉冲对应目标径向位置上的线性调频载波信号
		  3. 计算载波基准信号
		  4. 用基准信号混频目标信号，去掉基带载频
		  5. 在某个子脉冲内将目标上所有成像点的回波叠加，产生目标的雷达回波
		  6. 兼容两种模式，m_count=1，表示每次只仿真1组数据，按nFrame流水号处理，m_count>1，满足成像，一次仿真1幅图。
输入参数：
	nCell：
	nFrame:
输出函数：
	pRl：输出回波缓冲区，实部
	pIg: 输出回波缓冲区，虚部
********************************************************************************/
unsigned int SimDataGen::SimIsarEchoes(int nCell, unsigned int nFrame, float *pRl, float *pIg)	//nCnt 距离单元
{
    float *prge, *pErl, *pEig, *pYrl, *pYig;
    float tst, delay;
    float cf, gama, all_B;
    int tm, addr;

    //计算采样时间
    float ts = 1.0f / m_sampling;
    int sample = m_psWide * m_sampling;		//Tp*Fs 子脉冲内的采样点数
    if(sample > nCell)
        sample = nCell;

    float* tbuf = (float*)malloc(sizeof(float) * sample);
    delay = GetTimeDelay((float)m_range);
    tst = delay - m_psWide / 2;		//2Rs/c-Tp/2,单位us
    for(int n = 0; n < sample; n++)
    {
        tbuf[n] = tst + ts * n;
    }

    //开辟回波缓冲区
    prge = (float*)malloc(sizeof(float) * sample);
    pErl = (float*)malloc(sizeof(float) * sample);
    pEig = (float*)malloc(sizeof(float) * sample);
    pYrl = (float*)malloc(sizeof(float) * sample);
    pYig = (float*)malloc(sizeof(float) * sample);

    gama = m_band * 1.0f / m_psWide;
    all_B = (float)(m_deltaf * (m_num - 1) + m_band) / 2;
    m_frqn = m_frqn * 1000 - all_B;

    for(int i = 0; i < m_count; i++)		//仿真XX个脉冲的数据
    {
        if(m_count > 1)		//兼容两种模式，nCnt=1，表示每次只仿真1组数据，
            nFrame = i;		//nCnt>1，满足成像，一次仿真1幅图。

        for(int j = 0; j < m_num; j++)		//仿真子脉冲数据
        {
            cf = m_frqn + j * m_deltaf; //计算雷达发射频率
            tm = (nFrame * m_num + j) * m_prt;

            addr = (i * m_num + j) * nCell;
            memset(&pRl[addr], 0, sizeof(float)*nCell);
            memset(&pIg[addr], 0, sizeof(float)*nCell);

            for(int k = 0; k < pos; k++)
            {
                GetTgtRange(px[k], py[k], m_omega, m_range, (float)m_velcity, m_acc, tbuf, sample, tm, prge);

                ChirpSig(m_psWide, tbuf, sample, prge, (float)m_ampt, cf, gama, tm, pErl, pEig);  //生成步进频调频信号
                ChirpRef(m_psWide, tbuf, sample, (float)m_range, 1, cf, 0, 0, pYrl, pYig);        //产生基准信号
                ComplexConjMult(sample, pErl, pEig, pYrl, pYig, &pRl[addr], &pIg[addr]);          //相干检波，就是相乘，去掉载频

//                ChirpSig(m_psWide, tbuf, sample, prge, m_ampt, 0, gama, tm,  &pRl[addr], &pIg[addr]);	//仿真没有射频值的信号
            }
        }
    }
    free(pYig);
    free(pYrl);
    free(pEig);
    free(pErl);
    free(prge);
    free(tbuf);

    return m_count * m_num * nCell;
}

/*******************************************************************************
函数描述：生成数据主函数
具体步骤：1. 计算运动目标上每一个成像点距离雷达的径向距离
		  2. 计算每个子脉冲对应目标径向位置上的线性调频载波信号
		  3. 计算载波基准信号
		  4. 用基准信号混频目标信号，去掉基带载频
		  5. 在某个子脉冲内将目标上所有成像点的回波叠加，产生目标的雷达回波
		  6. 兼容两种模式，m_count=1，表示每次只仿真1组数据，按nFrame流水号处理，m_count>1，满足成像，一次仿真1幅图。
输入参数：
	nCell：
	nFrame:
输出函数：
	pRl：输出回波缓冲区，实部
	pIg: 输出回波缓冲区，虚部
********************************************************************************/
void SimDataGen::GenData(char *m_echo)
{
    float amg;
    int sample = m_psWide * m_sampling;		//Tp*Fs 子脉冲内的采样点数 10*60=600
    float* pRl = (float*)malloc(sizeof(float) * sample * m_count * m_num);
    float* pIg = (float*)malloc(sizeof(float) * sample * m_count * m_num);

    SimIsarEchoes(sample, 0, pRl, pIg);

    // 打开要写的文件
//    m_echo = "/home/a913/Qt_project/20200719/build-prototype_v0501-Desktop_Qt_5_9_8_GCC_64bit-Debug/radar/radar5/room/waveData/INPUT060808e6-ff7a-4541-af4b-1387b4008bfb/data.txt";
    FILE* fec = fopen(m_echo, "w");
    qDebug() << "file open";
//    printf("%s", m_echo);
    qDebug() << "file writing...";
    int all = m_count * m_num * sample;
    qDebug("total %d rows data", all);

    for(int s = 0; s < all; s++) //
    {
//        emit progress_to_sdgw(s*100/m_count*m_num*sample);
        fprintf(fec, "%10.3f\t", (float)pRl[s]);
        fprintf(fec, "%10.3f\t", (float)pIg[s]);
        amg = sqrt(pRl[s] * pRl[s] + pIg[s] * pIg[s]);
        fprintf(fec, "%10.3f\n", (float)amg);
//        if (s % 9600 == 0)
//        {
//            qDebug("%d行", s);
//        }
    }

    fclose(fec);
    qDebug() << "file close";
    fec = nullptr;
}
