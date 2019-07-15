//#pragma once
#ifndef __ACTCPCLIENTPROC_H__
#define __ACTCPCLIENTPROC_H__

#include "ACBaseMainProcClass.h"

class CACSocket;
class CACTCPClientProc :public CACBaseMainProc
{
public:
	CACTCPClientProc(void);
	~CACTCPClientProc(void);

	void Initialize(short nCommType, short nThreadNum, short nMaxConn,
		ISockEventCallBack* lpDataCallBack, ISockHeaderCallBack* lpHeadProc);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort,
		LPCTSTR lpRemoteHost, USHORT nRemotePort,BOOL bMulPoint);

	int BeginWork(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, LPCTSTR pProxyHost, USHORT nProxyPort,
		LPCTSTR pProxyUserName, LPCTSTR pProxyPassword);

	int BeginWorkHttp(LPCTSTR lpLocalHost, USHORT nLocalPort, LPCTSTR lpRemoteHost, 
		USHORT nRemotePort, LPCTSTR pProxyHost, USHORT nProxyPort,
		LPCTSTR pProxyUserName, LPCTSTR pProxyPassword);

	int CloseConnection(SOCKET hSocket = INVALID_SOCKET);
	int SendCommTCPData(WORD wPduType, const BYTE* lpBuf, DWORD dwSize, BOOL bSync,
		SOCKET hSocket = INVALID_SOCKET);
	int StartMainProc(void);
	int StopMainProc(void);
	void SetHttpFlag(void);

private:
	enum {MaxReciveBufferLen = 1024}; //add by zjj 2009-5-25

private:
	int SessionMainLoop(void);
	int HttpSessionMainLoop(void);
	int ConnectBySock5(LPCTSTR lpDestHost, USHORT nDestPort, LPCTSTR lpProxyHost, 
		USHORT nProxyPort, LPCTSTR pProxyUserName, LPCTSTR pProxyPassword, 
		ULONG lTimeOut);
	int ConnectByHttp(LPCTSTR lpDestHost, USHORT nDestPort, LPCTSTR lpProxyHost, 
		USHORT nProxyPort, LPCTSTR pProxyUserName, LPCTSTR pProxyPassword, 
		ULONG lTimeOut);
	int ConsultProcess(const BYTE* lpSendBuf, DWORD dwSendLen, BYTE* lpRecvBuf, 
		DWORD* lpRecvLen, UINT nTimeOut);

#ifdef _WIN32
	static DWORD WINAPI RecvThreadHelper(LPVOID lpParameter);
#else
	static void* RecvThreadHelper(LPVOID lpParameter);
	long GetCurrentStopFlag();
#endif
	static void BuildHttpHead(DWORD dwDataSize, BYTE* lpHeadBuf);
	static DWORD GetHttpHeadLength(void);
	char* make_base64_string(const char *str);

#ifdef _WIN32
	HANDLE m_hRecvThread;
#else
	pthread_t m_hRecvThread;
	pthread_mutex_t m_mutexStopFlag;
#endif


	long m_nStopFlag;
	CACSocket* m_pSocket;
	BYTE* m_pPackHead;
	int m_dwRHeadBytes;
	BYTE* m_pPackData;
	int m_dwRDataBytes;
	//int m_dwDataSize;
	unsigned long m_dwDataSize;
	BOOL m_bHeadFlag;

	char m_szSvrAddr[16];
	USHORT m_nSvrPort;

	BOOL m_bHttpFlag; //使用HTTP协议收发数据(暂时不支持代理)

	int m_dwMaxBufLen; //add by zjj 2009-5-25
};
#endif
