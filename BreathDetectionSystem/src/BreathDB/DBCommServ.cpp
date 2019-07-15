/**
 * Copyright (c) 2015, HangZhou Webcon Corporation.

 * @file	DBCommServ.cpp      数据库通信服务器
 * @author	shizc <shizc@webcon.com.cn>

 * @short
 * @date    07/2015
 **/

#include <iostream>
#include "Common.h"
#include "DBCommServ.h"
#include "IdDef.h"
using namespace std;

#define HAS_PRINT_DEBUG

CDBCommServ g_dBCommServ;

Concurrent::Concurrent()
{
    m_sock=0;
    m_pdutype = 0;
    m_len = 0;
    m_buffer = NULL;
}

Concurrent::Concurrent(const Concurrent& that)
{
    m_sock=that.m_sock;
    m_pdutype=that.m_pdutype;
    m_len=that.m_len;
    m_buffer = NULL;
    if(that.m_len>0)
    {
        m_buffer = new char[m_len];
        assert(m_buffer);
        memcpy(m_buffer,that.m_buffer,m_len);
    }
}

Concurrent::~Concurrent()
{
    if(m_buffer)
    {
        delete[] m_buffer;
    }
    m_buffer = NULL;
    m_len = 0;
}

int Concurrent::set(int sock,short pdutype,CVOID *buf,int len)
{
    m_sock=sock;
    m_pdutype = pdutype;
    m_len = len;
    if(m_len == 0)
    {
        return -1;
    }
    m_buffer = new char[m_len];
    assert(m_buffer);
    printf("============Concurrent----set()====,sock=[%d],pdutype=[%d],len=[%d]\n",m_sock,m_pdutype,len);
    memcpy(m_buffer,(CCHAR*)buf,m_len);
    printf("============Concurrent----set()====,m_buffer============================================\n");
    //dumpHex(m_buffer,m_len);
    return 0;
}

int Concurrent::getSock()
{
    return m_sock;
}

short Concurrent::getPdutype()
{
    return m_pdutype;
}

int Concurrent::getLen()
{
    return m_len;
}

CCHAR *Concurrent::getBuffer()
{
    return m_buffer;
}

Concurrent& Concurrent::operator= (const Concurrent& that)
{
    m_sock = that.m_sock;
    m_pdutype = that.m_pdutype;
    if(that.m_len > 0)
    {
        m_len = that.m_len;
        m_buffer = new char[m_len];
        assert(m_buffer);
        memcpy(m_buffer, that.m_buffer, that.m_len);
    }
    return (*this);
}


/**
 * CDBCommSockEvent
 */
CDBCommSockEvent::CDBCommSockEvent (CDBCommServ * pDBCommServ):m_pDBCommServ
    (pDBCommServ)
{

}

CDBCommSockEvent::~CDBCommSockEvent ()
{

}


/**
 * 连接数据库服务器的回调函数
 */
int CDBCommSockEvent::OnNewConnection (int sock)
{
    return (m_pDBCommServ->OnNewConnection (sock));
}


/**
 * 数据到达后的回调函数
 * @param sock 连接的sock号
 * @param pdutype PDU类型
 * @param data 接收到的数据
 * @param len  接收到的数据长度
 */
int CDBCommSockEvent::OnTCPPacketReceived (int sock, unsigned short pdutype,
                                           CVOID *data, unsigned long len)
{
//#ifdef HAS_PRINT_DEBUG
    cout << "CDBCommSockEvent::OnTCPPacketReceived(" << sock << ")"
         << "pdutype = [" << pdutype << "]" << "len = [" << len
         << "]" << endl;        
         dumpHex (data, len);
//#endif
    m_pDBCommServ->dillWithPDU (sock, pdutype, data, len);
    return 0;
}

int CDBCommSockEvent::OnConnectionClosed (int sock)
{
    return (m_pDBCommServ->OnConnectionClosed (sock));
}

ConcurrenReadThread::ConcurrenReadThread()
{

}

ConcurrenReadThread::~ConcurrenReadThread()
{

}

void *ConcurrenReadThread ::threadProc(void* arg)
{
    CDBCommServ *p_Ser = (CDBCommServ*)arg;
    if(p_Ser)
    {
        p_Ser->ReadData(arg);
    }
}

ConcurrenWriteThread::ConcurrenWriteThread()
{

}

ConcurrenWriteThread::~ConcurrenWriteThread()
{

}

void *ConcurrenWriteThread ::threadProc(void* arg)
{
    CDBCommServ *p_Ser= (CDBCommServ*)arg;
    if(p_Ser)
    {
        p_Ser->WriteData(arg);
    }
}


/*
* CDBCommServ
*/
CDBCommServ::CDBCommServ ():m_tcpSockEvent (this), m_tcpAcceptor (&m_tcpSockEvent), m_port (0)
{
    senddata = new char[MAX_OUTBUF_SIZE];
    memset (senddata, 0, MAX_OUTBUF_SIZE);

    senddata2 = new char[MAX_OUTBUF_SIZE];
    memset (senddata2, 0, MAX_OUTBUF_SIZE);
}

CDBCommServ::~CDBCommServ ()
{
    if (senddata)
        delete[](char *) senddata;
    senddata = NULL;

    if (senddata2)
        delete[](char *) senddata2;
    senddata2 = NULL;

}

int CDBCommServ::OnNewConnection (int sock)
{
#ifdef HAS_PRINT_DEBUG
    std::cerr << "CDBCommServ::OnNewConnection(" << sock << ")" << std::endl;
#endif
}

int CDBCommServ::OnConnectionClosed (int sock)
{
#ifdef HAS_PRINT_DEBUG
    std::cerr << "CDBCommServ::OnConnectionClosed(" << sock << ")" <<
                 std::endl;
#endif
    std::list < int >::iterator it;
    if (m_sockList.end () !=
            (it = std::find (m_sockList.begin (), m_sockList.end (), sock)))
    {
        m_sockList.erase (it);
    }
    return 0;
}


/*
 *  @返回队列中首元素
 *  @param type :读、写 队列的类型
 */
Concurrent* CDBCommServ::getQueueTop(int type)
{
    if(type == READ_BUFFER)
    {
        return &(read_queue.front());
    }
    else if(type == WRITE_BUFFER)
    {
        return &(write_queue.front());
    }
}

/*
 *  @检测队列是否为空队列
 *  @param type :读、写 队列的类型
 */
bool CDBCommServ::checkQueueEmpty(int type)
{
    if(type == WRITE_BUFFER)
    {
        return write_queue.empty();
    }
    else if(type == READ_BUFFER)
    {
        return read_queue.empty();
    }
}

/*
 * 获取队列的大小
 *  @param type :读、写 队列的类型
 */
int CDBCommServ::getQueueSize(int type)
{
    if(type == WRITE_BUFFER)
    {
        return write_queue.size();
    }
    else if(type == READ_BUFFER)
    {
        return read_queue.size();
    }
}


/**
 *  @删除队列中首元素，在线程中操作完成后调用
 *  @param type :读、写 队列的类型
 */
void CDBCommServ::delQueueTop(int type)
{
    if(type == WRITE_BUFFER)
    {
        write_queue.pop();//自动调用队列元素的析构函数
    }
    else if(type == READ_BUFFER)
    {
        read_queue.pop();
    }
}

/**
 *  @将接受的数据插入到队列尾
 *  @param type :读、写 队列的类型
 *  @param Concurrent 回调的数据封装后的类
 */
void CDBCommServ::insertQueue(int type,Concurrent &concurrent)
{
    if(type == WRITE_BUFFER)
    {
        write_queue.push(concurrent);//会调用Concurrent类的拷贝构造函数,所以要重写拷贝构造
    }
    else if(type == READ_BUFFER)
    {
        read_queue.push(concurrent);
    }
}



/*
*写线程：从写操作队列中读取首元素，在进行处理
*处理结束后，释放写操作队列首元素
*/
void *CDBCommServ::WriteData(void *arg)
{
    while(1)
    {
        if( !checkQueueEmpty(WRITE_BUFFER) )
        {
            DWORD sendlen = MAX_OUTBUF_SIZE;
            memset (senddata, 0, MAX_OUTBUF_SIZE);

            //此处不能用引用,pop时候先释放队列,是引用作用域找不到地址
            Concurrent *m_concurrent = getQueueTop(WRITE_BUFFER);
            int sock = 	m_concurrent->getSock();
            short pdutype = m_concurrent->getPdutype();
            int len = m_concurrent->getLen();
            CCHAR *buffer = m_concurrent->getBuffer();
            CSockStream ar ((BYTE *) buffer, len);
            switch (pdutype)
            {
                //客户端跟数据库之间的通用通道PDU
                case ROUTER0_REQ_TODBCOMM:
                {
                    short   shSocket = 0;
                    DWORD   ulPmid = 0;
                    WORD  ushSubPDUType = 0;
                    ar >> shSocket;
                    ar >> ulPmid;
                    ar >> ushSubPDUType;
#ifdef HAS_PRINT_DEBUG
                    printf("\ndebug: WriteData() ROUTER0_REQ_TODBCOMM- Line:[%d] - pdutype=[%d], ushSubPDUType=[%d]\n", __LINE__, pdutype, ushSubPDUType);
#endif
                    switch (ushSubPDUType)
                    {
                        default:
                        {
                            int i = 0;
                            for (i = 0; WriteSubPduList[i].wReqPduType != 0; i++)
                            {
                                if (WriteSubPduList[i].wReqPduType == ushSubPDUType)
                                {                                     
                                    (m_pEDUDBComm->*(WriteSubPduList[i].pFun))((char *) buffer +2 * sizeof (short) + sizeof(long),len - 2 * sizeof (short)- sizeof(long),senddata, sendlen);
                                    if (WriteSubPduList[i].wRspPduType != 0)
                                    {
                                        CSockStream ostream (sendlen + 64);
                                        ostream << shSocket;
                                        ostream << ulPmid;
                                        ostream << WriteSubPduList[i].wRspPduType;
                                        ostream.writeBytes (senddata,sendlen);
                                        sendPacket (sock,ROUTER0_RSP_FROMDBCOMM,ostream.getData (),ostream.length ());
#ifdef HAS_PRINT_DEBUG
                                        printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default send after shSocket = [%hd], ulPmid= [%ld]"
                                                ", wRspPduType = [%hu], sendlen = [%lu]\n", shSocket, ulPmid, WriteSubPduList[i].wRspPduType, sendlen + 8);
#endif
                                    }
                                    break;
                                }
                            }
                            if (WriteSubPduList[i].wReqPduType == 0)
                            {
#ifdef HAS_PRINT_DEBUG
                                printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default error ********pdusubtype \n");
#endif
                            }
                            break;
                        }
                    }
                    break;
                }

                case MAINCERT_REQ_TODBCOMM:
                case inStat_BaseSmoothPara_REQ_TODBCOMM:
                {
                    //printf("\ndebug: WriteData() - Line:[%d] - pdutype=[%d]\n", __LINE__, pdutype);
                    int i = 0;
                    for (i = 0; WritePubPduList[i].wReqPduType != 0; i++)
                    {
                        if (WritePubPduList[i].wReqPduType == pdutype)
                        {
                            printf("====================Into EDUDBComm==============\n");
                            int p=(m_pEDUDBComm->*(WritePubPduList[i].pFun)) (buffer,len,senddata,sendlen);
                            if (WritePubPduList[i].wRspPduType != 0)
                            {
                                sendPacket (sock, WritePubPduList[i].wRspPduType,senddata, sendlen);
                            }
                            break;
                        }
                    }

                    if (WritePubPduList[i].wReqPduType == 0)
                    {
#ifdef HAS_PRINT_DEBUG
                        printf ("error ********pdusubtype \n");
#endif
                    }
                    break;
                }

                case NHSystemDBROUTER0_REQ_FromCenter:
                {
                    WORD    wNHSystemCenterSock = 0;
                    WORD    wRetSubPDUType = 0;
                    ar >> wNHSystemCenterSock;
                    ar >> wRetSubPDUType;
#ifdef HAS_PRINT_DEBUG
                    printf("\ndebug: WriteData() NHSystemDBROUTER0_REQ_FromCenter- Line:[%d] - pdutype=[%d], ushSubPDUType=[%d]\n", __LINE__, pdutype, wRetSubPDUType);
#endif
                    switch (wRetSubPDUType)
                    {
                    default:
                    {
                        int i = 0;
                        for (i = 0; WriteSubPduList[i].wReqPduType != 0; i++)
                        {
                            if (WriteSubPduList[i].wReqPduType == wRetSubPDUType)
                            {
                                (m_pEDUDBComm->*(WriteSubPduList[i].pFun))((char *) buffer +2 * sizeof (short),len - 2 * sizeof (short),senddata, sendlen);
                                if (WriteSubPduList[i].wRspPduType != 0)
                                {
                                    CSockStream ostream (sendlen + 64);
                                    ostream << wNHSystemCenterSock;
                                    ostream << WriteSubPduList[i].wRspPduType;
#ifdef HAS_PRINT_DEBUG
                                    printf("WriteDate()=========[%d]\n",WriteSubPduList[i].wRspPduType);
#endif
                                    ostream.writeBytes (senddata,sendlen);
                                    sendPacket (sock,NHSystemDBROUTER0_RSP_ToCenter,ostream.getData (),ostream.length ());
#ifdef HAS_PRINT_DEBUG
                                    printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default wRspPduType = [%d] datalen = [%d]\n", WriteSubPduList[i].wRspPduType, sendlen + 4);
#endif
                                }
                                break;
                            }
                        }
                        if (WriteSubPduList[i].wReqPduType == 0)
                        {
#ifdef HAS_PRINT_DEBUG
                            printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default error ********pdusubtype \n");
#endif
                        }
                        break;
                    }
                    }
                    break;
                }

                default:
#ifdef HAS_PRINT_DEBUG
                    printf("error: Unknown PDU type\n");
#endif
                    break;
            }

            m_concurrent = NULL;
            delQueueTop(WRITE_BUFFER);
        }
        else
        {
            usleep(1000);
        }
    }
}

/*
*读线程：从读操作队列中读取首元素，在进行处理
*处理结束后，释放读操作队列首元素
*/
void *CDBCommServ::ReadData(void *arg)
{
    while(1)
    {
        if(!checkQueueEmpty(READ_BUFFER))
        {
            unsigned long sendlen = MAX_OUTBUF_SIZE;
            unsigned long sendlen2 = MAX_OUTBUF_SIZE;
            memset (senddata, 0, MAX_OUTBUF_SIZE);
            memset (senddata2, 0, MAX_OUTBUF_SIZE);

            Concurrent *m_concurrent = getQueueTop(READ_BUFFER);
            int sock = m_concurrent->getSock();
            short pdutype = m_concurrent->getPdutype();
            int len = m_concurrent->getLen();
            CCHAR *buffer = m_concurrent->getBuffer();
            //strncpy(buffer,m_concurrent->getBuffer(),len);
            CSockStream ar ((BYTE *) buffer, len);
            switch (pdutype)
            {
                //通用通道
                case ROUTER0_REQ_TODBCOMM:
                {
                    WORD repdu= pdutype+1;
                    short   shSocket = 0;
                    DWORD   ulPmid = 0;
                    WORD  ushSubPDUType = 0;
                    ar >> shSocket;
                    ar >> ulPmid;
                    ar >> ushSubPDUType;
#ifdef HAS_PRINT_DEBUG
                    printf ("PDU:ROUTER0_REQ_TODBCOMM**********shSocketx=[%d],ushSubPDUType = [%d]\n", shSocket, ushSubPDUType);
#endif
                    switch (ushSubPDUType)
                    {
                        default:
                        {
                            int i = 0;
                            for (i = 0; ReadSubPduList[i].wReqPduType != 0; i++)
                            {
                                if (ReadSubPduList[i].wReqPduType == ushSubPDUType)
                                {
                                    (m_pEDUDBComm->*(ReadSubPduList[i].pFun))(buffer+2 * sizeof (short) + sizeof(DWORD),len - 2 * sizeof (short) - sizeof(DWORD),senddata, sendlen);
                                    if (ReadSubPduList[i].wRspPduType != 0)
                                    {
                                        CSockStream ostream (sendlen +64);
                                        ostream << shSocket;
                                        ostream << ulPmid;
                                        ostream << ReadSubPduList[i].wRspPduType;
#ifdef HAS_PRINT_DEBUG
                                        printf("ReadDate()=========[%d]\n",ReadSubPduList[i].wRspPduType);
#endif
                                        ostream.writeBytes (senddata,sendlen);
                                        sendPacket (sock,repdu,ostream.getData (),ostream.length ());
#ifdef HAS_PRINT_DEBUG
                                        printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default wRspPduType = [%d] datalen = [%d]\n", ReadSubPduList[i].wRspPduType, sendlen + 4);
#endif
                                    }
                                    break;
                                }
                            }
                            if (ReadSubPduList[i].wReqPduType == 0)
                            {
#ifdef HAS_PRINT_DEBUG
                                printf ("PDU:ROUTER0_REQ_TODBCOMM/SubPDUType:default error ********pdusubtype \n");
#endif
                            }
                            break;
                        }
                    }
                    break;
                }
                case getUnprocDir_REQ_TODBCOMM:
                case getStatFilelist_REQ_TODBCOMM:
                {
                    //printf("\ndebug: WriteData() - Line:[%d] - pdutype=[%d]\n", __LINE__, pdutype);
                    int i = 0;
                    for (i = 0; ReadPubPduList[i].wReqPduType != 0; i++)
                    {
                        if (ReadPubPduList[i].wReqPduType == pdutype)
                        {
                            printf("====================Into EDUDBComm==============\n");
                            int p=(m_pEDUDBComm->*(ReadPubPduList[i].pFun)) (buffer,len,senddata,sendlen);
                            if (ReadPubPduList[i].wRspPduType != 0)
                            {
                                sendPacket (sock, ReadPubPduList[i].wRspPduType,senddata, sendlen);
                            }
                            break;
                        }
                    }

                    if (ReadPubPduList[i].wReqPduType == 0)
                    {
#ifdef HAS_PRINT_DEBUG
                        printf ("error ********pdusubtype \n");
#endif
                    }
                    break;
                }
            }

            m_concurrent = NULL;

            /*删除队列首元素*/
            delQueueTop(READ_BUFFER);
        }
        else
        {
            usleep(1000);
        }
    }
}

/**
 * 初始化工作
 */
int CDBCommServ::initialize (int argc, char **argv)
{

    m_pEDUDBComm = new CEDUDBComm ();
    m_pEDUDBComm->SetDBConfigClass (m_pDBConfigClass);
    if (m_pEDUDBComm->Initialize() != DB_ERR_SUCCESS)
    {
        printf ("EDUDBComm Initialize() error \n");
        exit (0);
    }
    std::cout << "Create a TCP Acceptor on('"
              << m_ip << "'," << m_port << ")..." << std::endl;

    if (0 != m_tcpAcceptor.create (m_ip.c_str (), m_port))
    {
        std::cerr << "Failed!" << std::endl;
        exit (0);
    }

#ifdef HAS_PRINT_DEBUG
    std::cout << "m_tcpAcceptor.create" << std::endl;
#endif

    /*启动读写线程*/
    m_readThread.start(&readThread,(void *)this);

    m_writeThread.start(&writeThread,(void *)this);

#ifdef HAS_PRINT_DEBUG
    std::cerr << "OK!" << std::endl;
#endif

    return 0;
}


/**
 * 从配置文件里导入配置信息
 * @param pFile 配置文件名
 * @return 零 表正常 ;非零表出错
 */
int CDBCommServ::loadConfig (CCHAR *pFile)
{
    CFileGetPara ini;
    int ret = 0;
    if (0 != (ret = ini.InitConfig (pFile)))
    {
        std::cerr << "InitConfig() == " << ret << std::endl;
        return -1;
    }
    m_ip = ini.GetParaString ("Acceptor", "IP", "localhost");

    m_port = ini.GetParaInt ("Acceptor", "Port", 0);
    if (0 == m_port)
    {
        std::cerr << "Port(=" << m_port << ") Error!" << std::endl;
        return -2;
    }
    serverType_ = ini.GetParaInt ("SelfInfo", "ServerType", 0);
    if (0 == serverType_)

    {
        std::cerr << "ServerType(=" << serverType_ << ") Error!" <<
                     std::endl;
        ini.UnloadConfig ();
        return -1;
    }
    serverIndex_ = ini.GetParaInt ("SelfInfo", "ServerIndex", 0);
    if (0 == serverIndex_)

    {
        std::cerr << "ServerIndex(=" << serverIndex_ << ") Error!" <<
                     std::endl;
        ini.UnloadConfig ();
        return -1;
    }
    //modify by zjj 2006-7-7
    m_pDBConfigClass = new CDBConfigClass (pFile);
    return 0;
}

int CDBCommServ::OnTimer (unsigned long timerID, void *arg)
{
    switch (timerID)
    {
        defalut:break;
    }
    return 0;
}

int CDBCommServ::destroy ()
{
    m_tcpAcceptor.close ();
    if (m_pEDUDBComm)
        delete m_pEDUDBComm;
    m_pEDUDBComm = NULL;
    if (m_pDBConfigClass)
        delete m_pDBConfigClass;
    m_pDBConfigClass = NULL;

    /*杀死读、写线程*/

    m_readThread.kill();

    m_writeThread.kill();

    return 0;
}


int CDBCommServ::sendPacket (ACE_HANDLE sock, unsigned short pdutype,
                             CVOID *buf, unsigned long len)
{
    return m_tcpAcceptor.sendPacket (sock, pdutype, buf, len);
}


/**
 * 数据到达后的处理函数 在OnTCPPacketReceived里调用
 * @param sock 连接的sock号
 * @param pdutype PDU类型
 * @param data 接收到的数据
 * @param len  接收到的数据长度
 */
int CDBCommServ::dillWithPDU (int sock, unsigned short pdutype, CVOID *data,
                              unsigned long len)
{
    if (!m_pEDUDBComm)
        return -1;

    //printf ("in dillWithPDU pdutype= [%d] len = [%d]\n", pdutype, len);
    switch (pdutype)
    {
        default:
        {
            if (len == 0)
                return -1;
            break;
        }
    }

    Concurrent m_concurrent;
    m_concurrent.set(sock,pdutype,data,len);

    switch (pdutype)
    {
        //客户端跟数据库之间的通用通道PDU
        case ROUTER0_REQ_TODBCOMM:
        {
            CSockStream ar ((BYTE *) data, len);
            short   shSocket = 0;
            DWORD   ulPmid = 0;
            WORD  ushSubPDUType = 0;
            ar >> shSocket;
            ar >> ulPmid;
            ar >> ushSubPDUType;
            printf ("PDU:ROUTER0_REQ_TODBCOMM**********nResAllocateIndex=[%d],ushSubPDUType = [%d]\n", shSocket, ushSubPDUType);

            switch (ushSubPDUType)
            {
                case AddOpenId_Req_WebToDB:
                case BindOpenid_Req_WebToDB:
                case DelBindOpenid_Req_WebToDB:
                case AddTerminalInfo_REQ_FromWeb:
                case DelTerminalInfo_REQ_FromWeb:
                case SetIpeTime_REQ_FromWeb:
                {
                    //printf("RoutPDU DBCommServ->dillWithPDU() insertQueue WriteQueue\n");
                    insertQueue(WRITE_BUFFER,m_concurrent);
                    break;
                }
                case GetBandZDList_Req_WebToDB:
                case GetBandZhDList_Req_WebToDB:
                case GetTerminalList_REQ_FromWeb:
                case GetAlarmInfoListPage_Req_FromCli:
                case GetDataInfoListPage_Req_FromCli:
                case GetAlarmInfoData_Req_FromCli:
                case Abnormal_statistics_Req_FromCLI:
                case tryBreathInfo_Req_WebToDB:
                case GetIpeTime_REQ_FromCli:
                case GetAlarmInfoListPage2_Req_FromCli:
                case GetSleepInfo_REQ_FromCli:
                case GetBRInfo_REQ_FromCli:
                case GetInsomInfo_REQ_FromCli:
                case getHX_Req_WebToDB:
                {
                    printf("RoutPDU DBCommServ-->dillWithPdu() insertQueue ReadQueue,ushPDUType=[%d]\n",ushSubPDUType);
                    insertQueue(READ_BUFFER,m_concurrent);
                    break;
                }
                default:
                    break;
            }
            break;
        }
	
        //终端服务进程跟中心数据库之间的通用通道
        case NHSystemDBROUTER0_REQ_FromCenter:
        {
            CSockStream ar ((BYTE *) data, len);
            WORD    wNHSystemCenterSock = 0;
            WORD    wRetSubPDUType = 0;
            ar >> wNHSystemCenterSock;
            ar >> wRetSubPDUType;
            printf ("PDU:ROUTER0_REQ_FromCenter**********nResAllocateIndex=[%d], ushSubPDUType = [%d]\n", wNHSystemCenterSock, wRetSubPDUType);
            switch (wRetSubPDUType)
            {
                case DBSUB_AddAlarmInfo_Req_FromNHSystem:
                case DBSUB_AddDataInfo_Req_FromNHSystem:
                case AddAlarmInfo2_Req_FromNHSystem:
                case AddHX_Req_WebToDB:
                {
                    printf("RoutPDU DBCommServ->dillWithPDU() insertQueue WriteQueue \n");
                    insertQueue(WRITE_BUFFER,m_concurrent);
                    break;
                }

                case Reserve:
                {
                    printf("RoutPDU DBCommServ-->dillWithPdu() insertQueue ReadQueue,ushPDUType=[%d]\n",wNHSystemCenterSock);
                    insertQueue(READ_BUFFER,m_concurrent);
                    break;
                }
                default:
                    break;
            }
            break;
        }

        //非通道PDU
        case MAINCERT_REQ_TODBCOMM:
        case inStat_BaseSmoothPara_REQ_TODBCOMM:
        {
            //printf("=====not PDU==DBCommServ->dillWithPDU() insertQueue WriteQueue\n");
            insertQueue(WRITE_BUFFER,m_concurrent);	
            break;
        }

        case getUnprocDir_REQ_TODBCOMM:
        case getStatFilelist_REQ_TODBCOMM:
        {
            insertQueue(READ_BUFFER,m_concurrent);
            break;
        }
        default :
            break;
    }
    return 0;
}
