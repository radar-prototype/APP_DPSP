typedef struct tagRadarControlBlock
{
    unsigned short sRsv0; /*1*/
    unsigned short sFlag; //报文标识:0x7E55

    unsigned short sPrtNo;  //2,脉冲编号,0-1023
    unsigned short sGrpNum; //脉冲组数,1-1024
    unsigned short sMtdStCh;  //动目标起始通道
    unsigned short sMtdEdCh;  //动目标终止通道

    unsigned short sPrt;       //重复周期,0.2us
    unsigned char cArray;      //子阵合成模式,11H:每块集成模块分为左右两子阵,22H分为上下
    unsigned int cWorkPattern; //1/2/3/4 : RDA搜索/确认/跟踪/无源侦查

    unsigned short sPGLen;   //4 采样波门长度,30m
    unsigned short sPGFront; //采样波门起点 30m

    unsigned int uBeamNo; //5 波束编号

    unsigned short sRf;   //6 雷达工作频率,1MHz
    unsigned short sCode; // 码型

    unsigned char cACJMode;  //7 抗同频异步干扰模式
    unsigned char cSeaMode;  //对海处理模式 1
    unsigned int cCfarThld; //CFAR 门限
    unsigned int cCfarGain; //CFAR增益,单位1/16

    unsigned char cRfMode;   //8 射频模式
    unsigned char cPrtMode;  // 重频模式
    unsigned char cRsv0;     //保留(阵面号)
    unsigned char cProcMode; //处理模式   1/2/3:MTI/MTD/NCA

    unsigned short sAngCoef;   //9 跟踪角度系数,单位:0.001
    unsigned short sTargetPos; //跟踪波束目标位置,相对于采样波门起点,单位:15m

    unsigned short sRadialVol;

} RCB_DSP;
