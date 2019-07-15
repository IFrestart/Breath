#include "FileProc.h"
#include "Logger.h"

ArchiveFile::ArchiveFile()
{

}

/**Linux��ɨ���ļ��У� ����ļ����µ��ļ���*/
int ArchiveFile::scanFiles(StrArray &fileList, string inputDirectory)
{
    DIR *p_dir;
    const char* str = inputDirectory.c_str();
    p_dir = opendir(str);
    if( p_dir == NULL)
    {
        _ERROR("can't open:%s",  inputDirectory.c_str());
    }

    struct dirent *p_dirent;
    while ( p_dirent = readdir(p_dir))
    {
        string tmpFileName = p_dirent->d_name;
        if( tmpFileName.substr(0,1) == ".")
        {
            continue;
        }
        else
        {
            fileList.push_back(tmpFileName);
        }
    }
    closedir(p_dir);
    sort(fileList.begin(), fileList.end());
    return fileList.size();
}

//���������ļ�Ŀ¼
void ArchiveFile::Datamkdir(string dir)
{
    if (access(dir.c_str(), 0) == -1)
    {
        if (0 != mkdir(dir.c_str(), 0755))
        {
            _ERROR("make errorly");
        }
    }
}


StatFileProc::StatFileProc()
{
}

StatFileProc::~StatFileProc()
{}


//����״̬����1
int StatFileProc::StateAdjust1(statArr &astat)
{
    SArr_it   sit;
    uint16  Stat[5];
    int size = astat.size();
    if(size <= 2)
        return 0;

    for(sit = astat.begin()+2; sit < astat.end(); sit++)
    {
        if((sit+2) == astat.end())
            break;
        for(int i=0; i<5;i++)
        {
            Stat[i] = *(sit+i-2);
        }

        if(Stat[2] != Stat[1] && Stat[0]==Stat[1] && Stat[0]==Stat[3] && Stat[0]==Stat[4])
        {
            *sit = Stat[1];
            sit = sit + 2;
        }
    }

    return 0;
}

//����״̬����2
int StatFileProc::StateAdjust2(statArr &astat)
{
    SArr_it   sit;
    uint16  Stat[6];
    int size = astat.size();
    if(size <= 6)
        return 0;

    for(sit = astat.begin()+1; sit != astat.end(); sit++)
    {
        if(*(sit) != *(sit-1))
        {
           if(sit+4 < astat.end())
           {//һǰ�ĺ�������ͬ
               for(int i=0;i<6;i++)
               {
                   Stat[i] = *(sit+i-1);
               }
               if(Stat[2]==Stat[0] && Stat[2]==Stat[3] && Stat[2]==Stat[4] && Stat[2]==Stat[5])
               {
                   *sit = Stat[2];
                   continue;
               }
           }

           //��ǰһ��������ͬ
           for(int i=0;i<6;i++)
           {
               Stat[i] = *(sit+i-4);
           }
           if(Stat[3]==Stat[5] && Stat[3]==Stat[2] && Stat[3]==Stat[1] && Stat[3]==Stat[0])
           {
               *sit = Stat[3];
           }
        }
    }

    return 0;
}

//ƽ������תΪ״̬
int StatFileProc::pfcAnalysis(int32 pfc)
{
    int state;
    if(pfc <= 20)
    {
        state = 0;
    }
    else if(20 < pfc && pfc <= 40)
    {
        state = 1;
    }
    else if(40 < pfc && pfc <= 50)
    {
        state = 2;
    }
    else if(pfc > 50)
    {
        state = 3;
    }

    return state;
}

//ƽ����������Ϊ˯��״̬
int StatFileProc::SleepStateAnalysis(int32 para[], statArr &sArr)
{
    uint16 spara;
//    int count=0;

//    for(int i=0;i<5; i++)
//    {
//        if(zht[i] == -2323324)
//        {
//            count++;
//        }
//    }

//    if(count >= 4)
//    {
//        spara = 4;
//        sArr.push_back(spara);
//        spara = 4;
//        sArr.push_back(spara);
//    }
//    else
//    {
        spara = pfcAnalysis(para[0]);
        sArr.push_back(spara);
        spara = pfcAnalysis(para[1]);
        sArr.push_back(spara);
        _DEBUG("push_back ok!");
//    }

    return sArr.size();
}

//״̬�������뵽˯�߲���
int StatFileProc::inStatToSleep(SlStArr &astat, statArr &sstat)
{
    int size = astat.size();
    for(int i =0; i < size; i++)
    {
        astat[i].wSleepStat1 = sstat[i*2];
        astat[i].wSleepStat2 = sstat[i*2+1];
        //_DEBUG("AHIS:%d\t AHIO:%d",astat[i].dwAHIS, astat[i].dwAHIO);
        //_DEBUG("HeartRate:%d,RespRate:%d\n,", astat[i].dwRespRate, astat[i].dwHeartRate);
    }
    return size;
}

//����ƽ��������ȡ״̬��Ϣ
int StatFileProc::getStat_BaseSmoothPara(CCHAR *path, SlStArr &astat, statArr &sArr)
{
    int buffer[13]; //һ��������
    uint32  flagTime=0; //���ʱ��

    _DEBUG("pathf:%s", path);
    ifstream file(path, ios::in|ios::binary|ios::ate);
    int size = file.tellg();
    if(size < ONEMIN_LEN)
    {
        file.close();
        return -1;
    }
    file.seekg (0, ios::beg);

    for(int i=0; i < size; i+=ONEMIN_LEN)
    {//һ��������ͳ�Ʒ���

        file.read ((char*)&buffer, sizeof(buffer));
        //�ֽ���ת��
        for(int i=0; i<13;i++)
        {
            buffer[i]=ntohl(buffer[i]);
        }

        //�ļ�����ʱ��
        uint32 stattime = buffer[0];
        //���ظ�����
        if(flagTime >= stattime)
        {   break; }
        flagTime = stattime;
        uint32 BreathRate = buffer[1];
        uint32 AHIS = buffer[2];
        uint32 AHIO = buffer[3];
        //״̬����
        int zht[5]={0};
//        zht[0] = buffer[4];
//        zht[1] = buffer[5];
//        zht[2] = buffer[6];
//        zht[3] = buffer[7];
//        zht[4] = buffer[8];
        //ƽ������
        int para[2]={0};
        para[0] = buffer[9];
        para[1] = buffer[10];
        //_DEBUG("para1:%d\t para2:%d", para[0], para[1]);
        SleepStateAnalysis(para, sArr);

        //��������
        int hxpara = buffer[11];
        //��������
        int xtpara = buffer[12];
        //_DEBUG("hxpara:%d\t xtpara:%d", hxpara, xtpara);
        //˯�߲�����ջ
        SleepStat slst;
        memset(&slst,0,sizeof(slst));
        slst.dwTime = flagTime;
        slst.dwBreathRate = BreathRate;
        slst.dwAHIS = AHIS;
        slst.dwAHIO = AHIO;
        slst.dwHeartRate = hxpara;
        slst.dwRespRate = xtpara;
        astat.push_back(slst);
    }

    file.close();

    StateAdjust1(sArr);
    StateAdjust2(sArr);

    inStatToSleep(astat, sArr);

    return 0;
}
