#ifndef GENERATEDATA_H
#define GENERATEDATA_H


class GenerateData
{
public:
    GenerateData();
    static void *generateData(void *arg);
    static void *ddsData(void *arg);
    void run();
};

#endif // GENERATEDATA_H
