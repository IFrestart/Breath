/*

  This file is a part of JRTPLIB
  Copyright (c) 1999-2002 Jori Liesenborgs

  Contact: jori@lumumba.luc.ac.be

  This library (JRTPLIB) was partially developed for my thesis at the
  School for Knowledge Technology (Belgium/The Netherlands)

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE.

*/

#ifndef RTPCONNECTION_H

#define RTPCONNECTION_H

#include "rtpconfig.h"
#include "rtpdefines.h"
#include "rtpdebug.h"
#include "rtpstructs.h"
#include "rtpdestlist.h"
#include "rtpiptable.h"
#include "rtperror.h"
#include "rtpmcasttable.h"

#include "UdpSocks5.h"
//#include "../../socks5/jni/include/UdpSocks5.h"

#define RECEIVEMODE_ALL							0
#define RECEIVEMODE_IGNORESOME						1
#define RECEIVEMODE_ACCEPTSOME						2

class RTPContributingSources;
struct RawDataBlock;

class RTPConnection								RTPDEBUGBASE
{
public:
	RTPConnection();
	~RTPConnection();
	//Modify by chetang(2004-05-12)
	int Create(int pbase,unsigned long localip,
				 int proxyport = 0, unsigned long proxyip = 0,
				const char* username = 0, const char* passwd = 0);
	void Destroy();
	int GetPortBase();
	void SetMaximumPacketSize(int size)					{ maxpacksize = size; }
	int SetToS(int tos);

	int AddDestination(unsigned long ip,int portbase)			{ return destinations.Add(ip,portbase); }
	int DeleteDestination(unsigned long ip,int portbase)			{ return destinations.Delete(ip,portbase); }
	void ClearDestinations()						{ destinations.Clear(); }

	bool SupportsMulticasting();	
	int JoinMulticastGroup(unsigned long mcastIP);
	int LeaveMulticastGroup(unsigned long mcastIP);
	void LeaveAllMulticastGroups();
	int SetMulticastTTL(unsigned char ttl);
	
	int SendRTPData(void *data,int len,RTPContributingSources *srcs,unsigned char pt,bool mark,RTPuint16 seqnr,RTPuint32 timestamp);
	int SendRTPData(void *data,int len,RTPContributingSources *srcs,unsigned char pt,bool mark,RTPuint16 seqnr,RTPuint32 timestamp,unsigned short hdrextID,void *hdrextdata,int numhdrextwords);
//	int SendRTCPCompoundData(void *data,int len); //add by zjj 2010-12-24
	
	//Modify by zhangzl(2004-07-14)
	int SendUdpData(const char* lpData, unsigned long dwSize, const char* lpRemoteAddr, 
		unsigned short nRemotePort);

	int PollRTP()								{ return Poll(true); }
	//int PollRTCP()								{ return Poll(false); }
	bool RTPDataAvailable()							{ if (rtp_first == NULL) return false; return true; }
	//bool RTCPDataAvailable()						{ if (rtcp_first == NULL) return false; return true; }
	int ReceiveRTPData(unsigned char **data,int *len,unsigned long *ip,int *port);
	//int ReceiveRTCPData(unsigned char **data,int *len,unsigned long *ip,int *port);
	//struct timeval GetRTCPReceiveTime()					{ return rtcprecvtime; }

	int SetReceiveMode(int mode)						{ if (mode < 0 || mode > 2) return ERR_RTP_INVALIDRECEIVEMODE; receivemode = mode; return 0; }
                                                                        	
	int AddToIgnoreList(unsigned long ip,bool allports,int portbase)	{ return ignoreIPs.Add(ip,allports,portbase); }
	int DeleteFromIgnoreList(unsigned long ip,bool allports,int portbase)	{ return ignoreIPs.Delete(ip,allports,portbase); }
	void ClearIgnoreList()													{ ignoreIPs.Clear(); }
	int AddToAcceptList(unsigned long ip,bool allports,int portbase)	{ return acceptIPs.Add(ip,allports,portbase); }
	int DeleteFromAcceptList(unsigned long ip,bool allports,int portbase)	{ return acceptIPs.Delete(ip,allports,portbase); }
	void ClearAcceptList()													{ acceptIPs.Clear(); }

	int GetSendPort()							{ return portbase; }
	unsigned short GetProxyPort()				{ return udpsocks5rtp.GetProxyBindPort();}
	unsigned long GetLocalIP()						{ return localip; }
	
	RTPSOCKET GetRTPSocket()						{ return rtpsock; }
	//RTPSOCKET GetRTCPSocket()						{ return rtcpsock; }

private:
	int Poll(bool rtp);
	void FlushPackets();
	unsigned long CalcLocalIP();
	//Add by chetang(2004-05-12)
	int RecvFrom( int  s, char *buf, unsigned long len, unsigned long& ip, int& port );

	//RTPSOCKET rtpsock,rtcpsock;
	RTPSOCKET rtpsock;
	bool socketsopened;
	int portbase;
	unsigned long localip;

	RTPDestList destinations;
	int receivemode;
	RTPIPTable ignoreIPs,acceptIPs;
	RTPMCastTable mcasttable;
	//struct timeval rtcprecvtime;

	int maxpacksize;

	RawDataBlock *rtp_first,*rtp_last;
	//RawDataBlock *rtcp_first,*rtcp_last;
	
	unsigned char packetbuffer[RTP_MAXIMUMPACKETSIZE];
	unsigned char sendbuffer[RTP_MAXIMUMPACKETSIZE];
	
	//Add by chetang(2004-05-12)
	//CUdpSocks5	udpsocks5rtp, udpsocks5rtcp;
	CUdpSocks5	udpsocks5rtp;
	bool		isproxy;
};

#endif // RTPCONNECTION_H
