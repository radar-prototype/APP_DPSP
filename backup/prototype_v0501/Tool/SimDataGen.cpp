#include "math.h"
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "SimDataGen.h"
#include <QDebug>

using namespace std;

/*********************************************
���������캯�������ݲ���������
���������
    �źŲ���
����ֵ��
    ��
***********************************************/
SimDataGen::SimDataGen(SIMRE_LIB_PARAM param)
//SimDataGen::SimDataGen(SIMRE_LIB_PARAM param, char * m_echo_path)
{
    // ������ʼ��
    m_prt		= param.nPRT;		//�״��ظ�����
    m_frqn		= param.nRF;		//�״���Ƶ
    m_psWide	= param.nPW;		//����;
    m_num		= param.nPulse;		//һ�������е����������;
    m_band		= param.nBand;		//��Ƶ����;
    m_deltaf	= param.nDeltaf;	//��Ƶ������Ƶ��
    m_sampling  = param.nSampling;	//������;

    m_count		= param.nGroup;		//��������
    m_ampt		= param.amplitude;	//Ŀ��ز�����;
    m_range		= param.range;		//Ŀ�꾶��λ��
    m_velcity   = param.velcity;	//Ŀ�꾶���ٶ�
    m_omega		= param.omega;		//Ŀ����ת��
    m_acc		= param.acc;		//Ŀ�꾶����ٶ�;
//    m_echo = m_echo_path;         //�ļ�·��

    // Ŀ�������ݶ��� ��ʼ��Ϊ�����
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
�����������������ͷ�px��py
���������
    ��
����ֵ��
    ��
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
��������Ŀ��ʱ��
���������
	range��Ŀ��λ�ã���λKm
����ֵ��
	�״�ز��ӳ�ʱ�䣬��λus
***********************************************/
float SimDataGen::GetTimeDelay(float range)
{
    float delay;
    delay = 20 * range / 3;
    return delay;
}

/************************************************************************************************************
��������������Ŀ��ز�
���������
    x��y��Ŀ���� һ����Ŀ�����ĵ�λ��, ��λm
    w��Ŀ����ת���ٶ�, ��λ����rad/s [QUESTION]
	a��v��Ŀ�꾶���˶����ٶȺ��ٶ�
    R:Ŀ�����ľ����״�ľ��룬��λKm
	t���������ڵĿ���ʱ���������λus
    count������ʱ���������
    tm����ʱ�����, ��λus
���������
	rge�����Ŀ�꾶�������Ϣ����λKm
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
��������������Ŀ��ĳ��λ���ϵ����Ե�Ƶ�ź�, ���ɲ���Ƶ��Ƶ�ź�
���������
	psWide�������� us
	tbuf:   ����ʱ��
	count�������ڵĲ�������
	rge��Ŀ����ĳ�����״�ľ������ ��λKm
	cf: ��Ƶ����λMHz
	gama: ��Ƶб�ʣ���λMHz/us
	tm	:��ʱ��
	num: ��������
	prt: �״���Ƶ��us
���������
	real������ز���������ʵ��
	imag: ����ز����������鲿
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
��������������ĳ�������ϵĻ�׼�źţ���������ɼ첨�ģ���˲������Ե�Ƶ��������
���������
	psWide�������� us
	tbuf:   ����ʱ��
	Count�������ڵĲ�������
	rge��Ŀ����ĳ�����״�ľ������ ��λKm
	cf: ��Ƶ����λMHz
	gama: ��Ƶб�ʣ���λMHz/us
	tm	:��ʱ��
	num: ��������
	prt: �״���Ƶ��us
���������
	real������ز���������ʵ��
	imag: ����ز����������鲿
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
������������������ۼ� ��ɼ첨��������ˣ�ȥ����Ƶ
���������
	count�������� us
	Irl:   ����ʱ��
	Iig�������ڵĲ�������
	frl��Ŀ����ĳ�����״�ľ������ ��λKm
	fig: ��Ƶ����λMHz
���������
	prl������ز���������ʵ��
	pig: ����ز����������鲿
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
����������ģ�ⲽ��ƵISAR�����Ŀ��ز�����
���岽�裺   1. �����˶�Ŀ����ÿһ�����������״�ľ������
		  2. ����ÿ���������ӦĿ�꾶��λ���ϵ����Ե�Ƶ�ز��ź�
		  3. �����ز���׼�ź�
		  4. �û�׼�źŻ�ƵĿ���źţ�ȥ��������Ƶ
		  5. ��ĳ���������ڽ�Ŀ�������г����Ļز����ӣ�����Ŀ����״�ز�
		  6. ��������ģʽ��m_count=1����ʾÿ��ֻ����1�����ݣ���nFrame��ˮ�Ŵ���m_count>1���������һ�η���1��ͼ��
���������
	nCell��
	nFrame:
���������
	pRl������ز���������ʵ��
	pIg: ����ز����������鲿
********************************************************************************/
unsigned int SimDataGen::SimIsarEchoes(int nCell, unsigned int nFrame, float *pRl, float *pIg)	//nCnt ���뵥Ԫ
{
    float *prge, *pErl, *pEig, *pYrl, *pYig;
    float tst, delay;
    float cf, gama, all_B;
    int tm, addr;

    //�������ʱ��
    float ts = 1.0f / m_sampling;
    int sample = m_psWide * m_sampling;		//Tp*Fs �������ڵĲ�������
    if(sample > nCell)
        sample = nCell;

    float* tbuf = (float*)malloc(sizeof(float) * sample);
    delay = GetTimeDelay((float)m_range);
    tst = delay - m_psWide / 2;		//2Rs/c-Tp/2,��λus
    for(int n = 0; n < sample; n++)
    {
        tbuf[n] = tst + ts * n;
    }

    //���ٻز�������
    prge = (float*)malloc(sizeof(float) * sample);
    pErl = (float*)malloc(sizeof(float) * sample);
    pEig = (float*)malloc(sizeof(float) * sample);
    pYrl = (float*)malloc(sizeof(float) * sample);
    pYig = (float*)malloc(sizeof(float) * sample);

    gama = m_band * 1.0f / m_psWide;
    all_B = (float)(m_deltaf * (m_num - 1) + m_band) / 2;
    m_frqn = m_frqn * 1000 - all_B;

    for(int i = 0; i < m_count; i++)		//����XX�����������
    {
        if(m_count > 1)		//��������ģʽ��nCnt=1����ʾÿ��ֻ����1�����ݣ�
            nFrame = i;		//nCnt>1���������һ�η���1��ͼ��

        for(int j = 0; j < m_num; j++)		//��������������
        {
            cf = m_frqn + j * m_deltaf; //�����״﷢��Ƶ��
            tm = (nFrame * m_num + j) * m_prt;

            addr = (i * m_num + j) * nCell;
            memset(&pRl[addr], 0, sizeof(float)*nCell);
            memset(&pIg[addr], 0, sizeof(float)*nCell);

            for(int k = 0; k < pos; k++)
            {
                GetTgtRange(px[k], py[k], m_omega, m_range, (float)m_velcity, m_acc, tbuf, sample, tm, prge);

                ChirpSig(m_psWide, tbuf, sample, prge, (float)m_ampt, cf, gama, tm, pErl, pEig);  //���ɲ���Ƶ��Ƶ�ź�
                ChirpRef(m_psWide, tbuf, sample, (float)m_range, 1, cf, 0, 0, pYrl, pYig);        //������׼�ź�
                ComplexConjMult(sample, pErl, pEig, pYrl, pYig, &pRl[addr], &pIg[addr]);          //��ɼ첨��������ˣ�ȥ����Ƶ

//                ChirpSig(m_psWide, tbuf, sample, prge, m_ampt, 0, gama, tm,  &pRl[addr], &pIg[addr]);	//����û����Ƶֵ���ź�
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
������������������������
���岽�裺1. �����˶�Ŀ����ÿһ�����������״�ľ������
		  2. ����ÿ���������ӦĿ�꾶��λ���ϵ����Ե�Ƶ�ز��ź�
		  3. �����ز���׼�ź�
		  4. �û�׼�źŻ�ƵĿ���źţ�ȥ��������Ƶ
		  5. ��ĳ���������ڽ�Ŀ�������г����Ļز����ӣ�����Ŀ����״�ز�
		  6. ��������ģʽ��m_count=1����ʾÿ��ֻ����1�����ݣ���nFrame��ˮ�Ŵ���m_count>1���������һ�η���1��ͼ��
���������
	nCell��
	nFrame:
���������
	pRl������ز���������ʵ��
	pIg: ����ز����������鲿
********************************************************************************/
void SimDataGen::GenData(char *m_echo)
{
    float amg;
    int sample = m_psWide * m_sampling;		//Tp*Fs �������ڵĲ������� 10*60=600
    float* pRl = (float*)malloc(sizeof(float) * sample * m_count * m_num);
    float* pIg = (float*)malloc(sizeof(float) * sample * m_count * m_num);

    SimIsarEchoes(sample, 0, pRl, pIg);

    // ��Ҫд���ļ�
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
//            qDebug("%d��", s);
//        }
    }

    fclose(fec);
    qDebug() << "file close";
    fec = nullptr;
}
