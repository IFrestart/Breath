/**
  * @file     FileProc.h
  * @author   wudaozc
  * @brief
  * @date     2018-09-18
  **/
#ifndef FILEPROC_H
#define FILEPROC_H

//����
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

    /**Linux��ɨ���ļ��У� ����ļ����µ��ļ���*/
    int scanFiles(StrArray &fileList, string inputDirectory);

    //���������ļ�Ŀ¼
    void Datamkdir(string dir);
};


//״̬��Ϣ
typedef struct tagSleepStat
{
    DWORD  dwTime;         //״̬����ʱ��
    DWORD  dwBreathRate;
    DWORD  dwAHIS;
    DWORD  dwAHIO;
    uint16  wSleepStat1;    //״̬
    uint16  wSleepStat2;    //״̬
    DWORD dwRespRate;       //������
    DWORD dwHeartRate;      //������
}SleepStat, *SLEEPStat;
typedef vector<SleepStat> SlStArr;
typedef SlStArr::iterator SlArr_it;

typedef vector<uint16> statArr;
//typedef list<uint16> statArr;
typedef statArr::iterator SArr_it;

//״̬�ļ�����
class StatFileProc
{
public:
    StatFileProc();
    ~StatFileProc();

    //����״̬����1
    int StateAdjust1(statArr &astat);
    //����״̬����2
    int StateAdjust2(statArr &astat);
    //ƽ������תΪ״̬
    int pfcAnalysis(int32 pfc);
    //ƽ����������Ϊ˯��״̬
    int SleepStateAnalysis(int32 para[], statArr &sArr);

    //״̬�������뵽˯�߲���
    int inStatToSleep(SlStArr &astat, statArr &sstat);

    //����ƽ��������ȡ״̬��Ϣ
    int getStat_BaseSmoothPara(CCHAR *path, SlStArr &astat, statArr &sArr);

    enum
    {
        ONEMIN_LEN		=52      //һ�������ݳ���
    };
};
#endif // FILEPROC_H

