/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP连接类, 提供了建立RTP连接,关闭RTP连接,发送RTP数据,
 * 设置分片大小等操作

 * @file	RTPConnector.h
 * @short	RTP连接类
 * @author	chetang <chetang@webcon.com.cn>

 * @class	CRTPConnectorTpl
 * @short	RTP连接类
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
* 创建RTP连接
*/
template <class SOCKHEADER>
class CRTPConnectorTpl : public IThreadCallBack
{
public:
	/**
	* 构造函数
	* @param pCallBack 当RTP连接的事件触发时回调的接口
	* @see IRTPEventCallBack
	*/
	CRTPConnectorTpl(IRTPEventCallBack* pCallBack);
	~CRTPConnectorTpl();

	/**
	* 有SSRC加入时被回调的函数
	*/
	static void SSRCNewSource(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* 有SSRC离开时被回调的函数
	*/
	static void SSRCDeparture(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* 有SSRC超时时被回调的函数
	*/
	static void SSRCTimeout(int exceptiontype, void *exceptiondata,void *usrdata);

	/**
	* 创建绑定在指定的port上的RTP连接
	* 根据RFC规定指定的端口必须为偶数,否则会失败
	* @return 成功时为0
	*/
	int create(unsigned short port);

	/**
	* 创建绑定在指定的IP与port上的RTP连接
	* 根据RFC规定指定的端口必须为偶数,否则会失败
	* @return 成功时为0
	*/
	int create(const char* ipstr, unsigned short port);

	/**
	* 停止RTP接收数据
	*/
	int stop();

	/**
	* 关闭RTP连接
	*/
	int close();

	/**
	* 增加,删除,清除发送数据的目标地址
	* @return 成功时为0
	*/
	int addDestination(const char* ipstr , unsigned short port);
	int deleteDestination(const char* ipstr, unsigned short port);
	inline int addDestination(unsigned long ip, unsigned short port);
	inline int deleteDestination(unsigned long ip, unsigned short port);
	inline int clearDestinations();

	/**
	* 取得RTP的socket,若还没有创建则返回RTPSOCKERR
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为RTP socket
	*/
	inline RTPSOCKET getRTPSock();

	/**
	* 取得RTCP的socket,若还没有创建则返回RTPSOCKERR
	* (对于linux为-1;对于windows则为~0)
	* @return 成功时为RTCP socket
	inline RTPSOCKET getRTCPSock();
	*/

	/**
	* 设置数据分片的大小
	* @return 成功时为0
	*/
	inline int setSliceSize(unsigned short sliceSize);

	/**
	* 取得数据分片的大小
	* @return 返回分片大小
	*/
	inline unsigned short getSliceSize();

	/**
	* 发送RTP数据包,根据分片进行分包
	* @return 成功时为0
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
	* 存储一个SSRC中同一个数据包的相关信息
	* (包括其第一个分片的序号,最后一个分片到达的时间,
	* 分片的大小,分片总数,及一个数据包的所有分片)
	* @class	SliceInfo
	* @short	数据包分片的信息
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

			/*没有超出最大包的大小*/
			if(MAX_PACKET_LEN >= totalNum * packet->GetPayloadLength()) {
				/*只有一个分片 或 不止一个分片,并且不是最后一个分片先到*/
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
					_ERROR("/*此处过滤了所有最后一个分片先到的包*/");
				}
			}
			else {
				_ERROR("/*此处过滤了所有大于MAX_PACKET_LEN的包*/");
			}

			return ret;
		}

		int add(RTPPacket* packet, unsigned long sliceNo, unsigned long totalNum)
		{
			int		ret = 0;

			/*若是最后一个分片,可以小于分片大小*/
			if(sliceNo == totalNum - 1) {
				if(packet->GetPayloadLength() > size_) {
					_ERROR("此处过滤了大小不符的最后一个分片 size_(=%d) < packe->GetPayloadLength()(=%d)", size_, packet->GetPayloadLength());
					ret = -1;
				}
			}
			else {
				/*否则应该等于预期的分片大小*/
				if(packet->GetPayloadLength() != size_) {
					_ERROR("此处过滤了大小不符的非最后一个分片 size_(=%d), packet->GetPayloadLength()(=%d)", size_, packet->GetPayloadLength());
					ret = -2;
				}
			}
			/*以上过滤了所有大小不是预期的分片*/

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
	* 存储一个SSRC的相关信息
	* (包括其IP,PORT,及所有数据包的分片)
	* @class	SsrcInfo
	* @short	SSRC的信息
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
		* 由SSRC来比较大小
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
	* 存储SSRC主动离开或超时出错的SSRC及相关的IP与PORT
	* 其中IP与PORT都是以本地字节序存储的
	* @class	ErrSsrc
	* @short	错误SSRC的信息
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
	* 接收RTP数据包,根据分片进行组包
	* @see IThreadCallBack
	*/
	/* From IThreadCallBack */
	void* threadProc(void* arg);

	/**
	* 检查RTPPacket是否有效
	*/
	int checkPacketValid(RTPPacket* packet);

	/**
	* 从RTPPacket中取SliceNo与totalSliceNum
	*/
	int getSliceNoFromPacket(RTPPacket* packet,
							unsigned long& sliceNo, unsigned long& totalSliceNum);

	/**
	* 删除所有超时的Slice
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

