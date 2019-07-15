/**
  * @file     FileProc.h
  * @author   wudaozc
  * @brief
  * @date     2018-09-18
  **/
#ifndef FILEPROC_H
#define FILEPROC_H

//共有
#include <vector>
#include <string>
#include <dirent.h>
#include <list>

//class ArchiveFile
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>

//class StatFileProc
#include "TypeDef.h"
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

typedef std::vector<string> StrArray;
typedef StrArray::iterator StrArr_it;

typedef std::vector<char* > CharArray;
typedef CharArray::iterator CharArr_it;

class ArchiveFile
{
public:
    ArchiveFile();

    /**Linux下扫描文件夹， 获得文件夹下的文件名*/
    int scanFiles(StrArray &fileList, string inputDirectory);

    //创建数据文件目录
    void Datamkdir(string dir);
};


//状态信息
typedef struct tagSleepStat
{
    DWORD  dwTime;         //状态所在时刻
    DWORD  dwBreathRate;
    DWORD  dwAHIS;
    DWORD  dwAHIO;
    uint16  wSleepStat1;    //状态
    uint16  wSleepStat2;    //状态
    DWORD dwRespRate;       //呼吸率
    DWORD dwHeartRate;      //心跳率
}SleepStat, *SLEEPStat;
typedef vector<SleepStat> SlStArr;
typedef SlStArr::iterator SlArr_it;

typedef vector<uint16> statArr;
//typedef list<uint16> statArr;
typedef statArr::iterator SArr_it;

//状态文件处理
class StatFileProc
{
public:
    StatFileProc();
    ~StatFileProc();

    //修整状态规则1
    int StateAdjust1(statArr &astat);
    //修整状态规则2
    int StateAdjust2(statArr &astat);
    //平滑参数转为状态
    int pfcAnalysis(int32 pfc);
    //平缓参数换算为睡眠状态
    int SleepStateAnalysis(int32 para[], statArr &sArr);

    //状态参数插入到睡眠参数
    int inStatToSleep(SlStArr &astat, statArr &sstat);

    //根据平滑参数获取状态信息
    int getStat_BaseSmoothPara(CCHAR *path, SlStArr &astat, statArr &sArr);

    enum
    {
        ONEMIN_LEN		=52      //一分钟内容长度
    };
};
#endif // FILEPROC_H

