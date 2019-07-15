/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP������, �ṩ�˽���RTP����,�ر�RTP����,����RTP����,
 * ���÷�Ƭ��С�Ȳ���

 * @file	RTPConnector.h
 * @short	RTP������
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CRTPConnectorTpl
 * @short	RTP������
 **/

#ifndef	__RTPCONNECTOR_H__
#define	__RTPCONNECTOR_H__

#include <list>
#include <algorithm>
#include <string.h>

#include "rtpsession.h"
#include "rtppacket.h"

//#include "BaseServ.h"
#include "Tools.h"
#include "Mutex.h"
#include "Thread.h"
#include "RTPEventCallBack.h"
//#include "SockHeaderCallBack.h"
#include "SockHeader.h"

/**
* ����RTP����
*/
template <class SOCKHEADER>
class CRTPConnectorTpl : public IThreadCallBack
{
public:
	/**
	* ���캯��
	* @param pCallBack ��RTP���ӵ��¼�����ʱ�ص��Ľӿ�
	* @see IRTPEventCallBack
	*/
	CRTPConnectorTpl(IRTPEventCallBack* pCallBack);
	~CRTPConnectorTpl();

	/**
	* ��SSRC����ʱ���ص��ĺ���
	*/
	static void SSRCNewSource(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* ��SSRC�뿪ʱ���ص��ĺ���
	*/
	static void SSRCDeparture(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* ��SSRC��ʱʱ���ص��ĺ���
	*/
	static void SSRCTimeout(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* ��������ָ����port�ϵ�RTP����
	* ����RFC�涨ָ���Ķ˿ڱ���Ϊż��,�����ʧ��
	* @return �ɹ�ʱΪ0
	*/
	int create(unsigned short port);

	/**
	* ��������ָ����IP��port�ϵ�RTP����
	* ����RFC�涨ָ���Ķ˿ڱ���Ϊż��,�����ʧ��
	* @return �ɹ�ʱΪ0
	*/
	int create(const char* ipstr, unsigned short port);

	/**
	* ֹͣRTP��������
	*/
	int stop();

	/**
	* �ر�RTP����
	*/
	int close();

	/**
	* ����,ɾ��,����������ݵ�Ŀ���ַ
	* @return �ɹ�ʱΪ0
	*/
	int addDestination(const char* ipstr , unsigned short port);
	int deleteDestination(const char* ipstr, unsigned short port);
	inline int addDestination(unsigned long ip, unsigned short port);
	inline int deleteDestination(unsigned long ip, unsigned short port);
	inline int clearDestinations();

	/**
	* ȡ��RTP��socket,����û�д����򷵻�RTPSOCKERR
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪRTP socket
	*/
	inline RTPSOCKET getRTPSock();

	/**
	* ȡ��RTCP��socket,����û�д����򷵻�RTPSOCKERR
	* (����linuxΪ-1;����windows��Ϊ~0)
	* @return �ɹ�ʱΪRTCP socket
	inline RTPSOCKET getRTCPSock();
	*/

	/**
	* �������ݷ�Ƭ�Ĵ�С
	* @return �ɹ�ʱΪ0
	*/
	inline int setSliceSize(unsigned short sliceSize);

	/**
	* ȡ�����ݷ�Ƭ�Ĵ�С
	* @return ���ط�Ƭ��С
	*/
	inline unsigned short getSliceSize();

	/**
	* ����RTP���ݰ�,���ݷ�Ƭ���зְ�
	* @return �ɹ�ʱΪ0
	*/
	int sendPacket(unsigned short pdutype, const void* dataPtr, unsigned long len);
	int removeSsrcInfo(unsigned long ip, unsigned short port); //add by zjj 2006-8-19

public:
	enum{ SSRCLOSEDATA = -1, SSRCDEPARTURE = -2, SSRCTIMEOUT = -3, SSRCLEAVE = -4, //add by zjj 2006-8-19
			SSRCNEWSOURCE = 0 };

	enum{ DEFAULT_TIMESTAMP_INC = 10, DEFAULT_PAYLOAD_TYPE = 11 };

	enum{ SLICETIMEOUT = 1 };

	enum{ DEFAULT_SLICE_SIZE = 9 * 1024 };

private:
	inline void init();
	void dump();
	inline bool isStopped() const;

	typedef typename std::list<int>::iterator		slice_it;

	/**
	* �洢һ��SSRC��ͬһ�����ݰ��������Ϣ
	* (�������һ����Ƭ�����,���һ����Ƭ�����ʱ��,
	* ��Ƭ�Ĵ�С,��Ƭ����,��һ�����ݰ������з�Ƭ)
	* @class	SliceInfo
	* @short	���ݰ���Ƭ����Ϣ
	*/
	class SliceInfo
	{
	public:
		enum {MAX_PACKET_LEN = 200 * 1024};

		unsigned long	firstSeqNo_;
		unsigned long	totalNum_;
		unsigned long	size_;
		unsigned long	hasSize_;
		std::list<int>	list_;
		unsigned char	packet_[MAX_PACKET_LEN];

		void clear()
		{
			firstSeqNo_ = 0;
			totalNum_ = 0;
			size_ = 0;
			hasSize_ = 0;
			list_.clear();
		}

		bool full() const
		{
			return !empty() && totalNum_ == list_.size();
		}

		bool empty() const
		{
			return 0 == list_.size();
		}

		int set(RTPPacket* packet, unsigned long sliceNo, unsigned long totalNum)
		{
			int		ret = -1;

			/*û�г��������Ĵ�С*/
			if(MAX_PACKET_LEN >= totalNum * packet->GetPayloadLength()) {
				/*ֻ��һ����Ƭ �� ��ֹһ����Ƭ,���Ҳ������һ����Ƭ�ȵ�*/
				if(totalNum == 1 || (totalNum != 1 && sliceNo != totalNum - 1)) {
					list_.clear();
					firstSeqNo_ = packet->GetExtendedSequenceNumber() - sliceNo;
					totalNum_ = totalNum;
					size_ = packet->GetPayloadLength();
					list_.push_back(sliceNo);
					memcpy(packet_ + sliceNo * size_,
						packet->GetPayload(),
						size_);
					hasSize_ += packet->GetPayloadLength();

					//dump("set()  ");
					ret = 0;
				}
				else {
					_ERROR("/*�˴��������������һ����Ƭ�ȵ��İ�*/");
				}
			}
			else {
				_ERROR("/*�˴����������д���MAX_PACKET_LEN�İ�*/");
			}

			return ret;
		}

		int add(RTPPacket* packet, unsigned long sliceNo, unsigned long totalNum)
		{
			int		ret = 0;

			/*�������һ����Ƭ,����С�ڷ�Ƭ��С*/
			if(sliceNo == totalNum - 1) {
				if(packet->GetPayloadLength() > size_) {
					_ERROR("�˴������˴�С���������һ����Ƭ size_(=%d) < packe->GetPayloadLength()(=%d)", size_, packet->GetPayloadLength());
					ret = -1;
				}
			}
			else {
				/*����Ӧ�õ���Ԥ�ڵķ�Ƭ��С*/
				if(packet->GetPayloadLength() != size_) {
					_ERROR("�˴������˴�С�����ķ����һ����Ƭ size_(=%d), packet->GetPayloadLength()(=%d)", size_, packet->GetPayloadLength());
					ret = -2;
				}
			}
			/*���Ϲ��������д�С����Ԥ�ڵķ�Ƭ*/

			if(0 == ret) {
				list_.push_back(sliceNo);
				memcpy(packet_ + sliceNo * size_,
					packet->GetPayload(),
					packet->GetPayloadLength());
				hasSize_ += packet->GetPayloadLength();

				//dump("set()  ");
			}

			return ret;
		}

		void dump(const char* prefix)
		{
			_INFO("%sfirstSeqNo_   = %lu", prefix, firstSeqNo_);
			_INFO("%stotalNum_     = %lu", prefix, totalNum_);
			_INFO("%ssize_         = %lu", prefix, size_);
			_INFO("%shasSize_      = %d", prefix, hasSize_);
			_INFO("%sSIZE               = %d", prefix, list_.size());
			int		i = 0;
			for(std::list<int>::iterator it = list_.begin(); it != list_.end(); ++ it, ++ i) {
				_INFO("%dth  = %d", i, *it);
			}
		}
	};

	/**
	* �洢һ��SSRC�������Ϣ
	* (������IP,PORT,���������ݰ��ķ�Ƭ)
	* @class	SsrcInfo
	* @short	SSRC����Ϣ
	*/
	class SsrcInfo
	{
	public:
		explicit SsrcInfo(unsigned long ssrc = 0,
					unsigned long ip = 0,
					unsigned short port = 0);
		~SsrcInfo();

		unsigned long	ssrc_;
		unsigned long	ip_;
		unsigned short	port_;
		unsigned long	lasttime_;//add by zjj 2006-8-19;
		void setTime(){lasttime_ = time(NULL);}//add by zjj 2006-8-19
		SliceInfo 		sliceInfo_;

		/**
		* ��SSRC���Ƚϴ�С
		*/
		bool operator< (const SsrcInfo& ssrcInfo) const;
		bool operator== (const SsrcInfo& ssrcInfo) const;

		void dump(const char* prefix)
		{
			_INFO("%sssrc_ = %lu", prefix, ssrc_);
			_INFO("%sip_   = %lu", prefix, ip_);
			_INFO("%sport_ = %u", prefix, port_);
		}
	};

	/**
	* �洢SSRC�����뿪��ʱ�����SSRC����ص�IP��PORT
	* ����IP��PORT�����Ա����ֽ���洢��
	* @class	ErrSsrc
	* @short	����SSRC����Ϣ
	*/
	class ErrSsrc
	{
	public:
		ErrSsrc(unsigned long ssrc,
					unsigned long ip,
					unsigned short port,
					int errCode);

		~ErrSsrc();

		unsigned long	ssrc_;
		unsigned long	ip_;
		unsigned short	port_;
		int				errCode_;
	};

	typedef typename std::list<SsrcInfo>::iterator	ssrcinfo_it;
	typedef typename std::list<SliceInfo>::iterator	sliceinfo_it;

private:
	/**
	* ����RTP���ݰ�,���ݷ�Ƭ�������
	* @see IThreadCallBack
	*/
	/* From IThreadCallBack */
	void* threadProc(void* arg);

	/**
	* ���RTPPacket�Ƿ���Ч
	*/
	int checkPacketValid(RTPPacket* packet);

	/**
	* ��RTPPacket��ȡSliceNo��totalSliceNum
	*/
	int getSliceNoFromPacket(RTPPacket* packet,
							unsigned long& sliceNo, unsigned long& totalSliceNum);

	/**
	* ɾ�����г�ʱ��Slice
	*/
	int recombinePacket(SliceInfo& sliceInfo, unsigned long ip, unsigned short port);

private:
	IRTPEventCallBack*	pCallBack_;
	SOCKHEADER 			sockHeaderCallBack_;

	RTPSession			sess_;
	CMutex				destinationMutex_;

	bool				initialized_;
	unsigned short		sliceSize_;
	RTPSOCKET			rtpSock_, rtcpSock_;
	std::list<SsrcInfo>	ssrcInfoList_;

	std::list<ErrSsrc>	errSsrcList_;
	CMutex				errMutex_;

	CThread				sendThread_;
	bool				cancelThread_;
	char 				*m_pSendBuf;//add by zjj 2008-1-6
	unsigned long 			m_MaxSendBufLen;//add by zjj 2008-1-6
};

/*
* CRTPConnectorTpl<SOCKHEADER>::SsrcInfo
*/
template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::SsrcInfo::SsrcInfo(unsigned long ssrc, unsigned long ip, unsigned short port)
		: ssrc_(ssrc), ip_(ip), port_(port)
{
}

template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::SsrcInfo::~SsrcInfo()
{
	sliceInfo_.clear() ; //add by zjj 2006-8-19
}

template <class SOCKHEADER>
bool CRTPConnectorTpl<SOCKHEADER>::SsrcInfo::operator< (const SsrcInfo& ssrcInfo) const
{
	return ssrc_ < ssrcInfo.ssrc_;
}

template <class SOCKHEADER>
bool CRTPConnectorTpl<SOCKHEADER>::SsrcInfo::operator== (const SsrcInfo& ssrcInfo) const
{
//	return ssrc_ == ssrcInfo.ssrc_; //modified by zjj 2006-7-25
	return ((ssrc_ == ssrcInfo.ssrc_) && (ip_ == ssrcInfo.ip_) && (port_ == ssrcInfo.port_));
}

/*
* CRTPConnectorTpl<SOCKHEADER>::ErrSsrc
*/
template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::ErrSsrc::ErrSsrc(unsigned long ssrc,
			unsigned long ip,
			unsigned short port,
			int errCode)
		: ssrc_(ssrc), ip_(ip), port_(port), errCode_(errCode)
{
}

template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::ErrSsrc::~ErrSsrc()
{
}


template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::addDestination(unsigned long ip, unsigned short port)
{
	CMutexKeeper	mutexKeeper(destinationMutex_);

	return sess_.AddDestination(ip, port);
}

template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::addDestination(const char* ipstr, unsigned short port)
{
	/* Why ?
	if(true == initialized_) {
		return -1;
	}
	*/

	unsigned long	ip = ip_to_long(ipstr);
	if(0 == ip) {
		return -1;
	}

	return addDestination(ip, port);
}

template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::deleteDestination(unsigned long ip, unsigned short port)
{
	CMutexKeeper	mutexKeeper(destinationMutex_);

	return sess_.DeleteDestination(ip, port);
}

template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::deleteDestination(const char* ipstr, unsigned short port)
{
	/* Why ?
	if(true == initialized_) {
		return -1;
	}
	*/

	unsigned long	ip = ip_to_long(ipstr);
	if(0 == ip) {
		return -1;
	}

	return deleteDestination(ip, port);
}

template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::clearDestinations()
{
	/* Why ?
	if(true == initialized_) {
		return -1;
	}
	*/

	CMutexKeeper	mutexKeeper(destinationMutex_);
	sess_.ClearDestinations();

	return 0;
}

template <class SOCKHEADER>
inline RTPSOCKET CRTPConnectorTpl<SOCKHEADER>::getRTPSock()
{
	return rtpSock_;
}

/*
template <class SOCKHEADER>
inline RTPSOCKET CRTPConnectorTpl<SOCKHEADER>::getRTCPSock()
{
	return rtcpSock_;
}
*/

template <class SOCKHEADER>
inline int CRTPConnectorTpl<SOCKHEADER>::setSliceSize(unsigned short sliceSize)
{
	/*
	if(false == initialized_) {
		return -1;
	}
	*/

	if(0 == sliceSize) {
		return -1;
	}

	sliceSize_ = sliceSize;

	return 0;
}

template <class SOCKHEADER>
inline unsigned short CRTPConnectorTpl<SOCKHEADER>::getSliceSize()
{
	return sliceSize_;
}

template <class SOCKHEADER>
inline void CRTPConnectorTpl<SOCKHEADER>::init()
{
	initialized_ = false;
	sliceSize_ = DEFAULT_SLICE_SIZE;
	rtpSock_ = RTPSOCKERR;
	rtcpSock_ = RTPSOCKERR;

	return;
}

template <class SOCKHEADER>
inline bool CRTPConnectorTpl<SOCKHEADER>::isStopped() const
{
	//return cancelThread_ || CThread::testCancel();
	return cancelThread_ || sendThread_.testCancel();
}

#include "../RTPConnector.cc"
typedef CRTPConnectorTpl<CSockHeader>	CRTPConnector;

#endif	/*__RTPCONNECTOR_H__*/

