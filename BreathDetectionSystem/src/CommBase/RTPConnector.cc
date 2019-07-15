/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * RTP连接类, 提供了建立RTP连接,关闭RTP连接,发送RTP数据,
 * 设置分片大小等操作

 * @file	RTPConnector.cc
 * @author	chetang <chetang@webcon.com.cn>

 * @short	RTP连接类
 **/

#include "Logger.h"

/*
* CRTPConnectorTpl<SOCKHEADER>
*/
template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::CRTPConnectorTpl(IRTPEventCallBack* pCallBack)
	:
	pCallBack_(pCallBack),
	cancelThread_(false)
{
	init();
	m_pSendBuf = new char[DEFAULT_SLICE_SIZE]; //add by zjj 2008-1-6
	m_MaxSendBufLen = DEFAULT_SLICE_SIZE;
}

template <class SOCKHEADER>
CRTPConnectorTpl<SOCKHEADER>::~CRTPConnectorTpl<SOCKHEADER>()
{
	close();
	delete[] m_pSendBuf ; //add by zjj 2008-1-6
	m_pSendBuf = NULL ; //add by zjj 2008-1-6
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::create(unsigned short port)
{
	_INFO("RTPConnector::create(%u)", port);

	return create(0, port);
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::create(const char* ipstr, unsigned short port)
{
	_INFO("RTPConnector::create('%s', %u)", ipstr, port);

	if(true == initialized_) {
		_ERROR("RTPConnector::create() Already initialized!!!");
		return -1;
	}

	if(0 == pCallBack_) {
		_ERROR("RTPConnector::create() 0 == pCallBack!!!");
		return -1;
	}

	unsigned long	ip = 0;
	if(0 != ipstr) {
		if(0 == (ip = ip_to_long(ipstr))) {
			_ERROR("RTPConnector::create() 0 == ip_to_long(%s)!!!", ipstr);
			//return -1; //modify 2013-4-2  by zjj
		}
	}

	int		ret = sess_.Create(port, ip);
	if(0 == ret) {
		if(0 != sendThread_.start(this, 0)) {
			_ERROR("RTPConnector::create() sendThread_.start() Error!!!");
			return -1;
		}
		sess_.SetMaxPacketSize(20 * 1024); //add by zjj 2008-4-22
		sess_.SetDefaultMark(false);
		sess_.SetDefaultPayloadType(DEFAULT_PAYLOAD_TYPE);
		sess_.SetDefaultTimeStampIncrement(DEFAULT_TIMESTAMP_INC);

		sess_.SetNewSourceHandler(CRTPConnectorTpl<SOCKHEADER>::SSRCNewSource, (void*)this);
		sess_.SetSSRCDepartureHandler(CRTPConnectorTpl<SOCKHEADER>::SSRCDeparture, (void*)this);
		sess_.SetSSRCTimeoutHandler(CRTPConnectorTpl<SOCKHEADER>::SSRCTimeout, (void*)this);

		if(0 != sess_.GetRTPSocket(&rtpSock_)) {
			_ERROR("RTPConnector::create() sess_.GetRTPSocket() Error!!!");
			return -1;
		}

		/*
		if(0 != sess_.GetRTCPSocket(&rtcpSock_)) {
			_ERROR("RTPConnector::create() sess_.GetRTCPSocket() Error!!!");
			return -1;
		}
		*/

		initialized_ = true;
		cancelThread_ = false;
	}
	else
	if(ERR_RTP_PORTBASENOTEVEN == ret) {
		_ERROR("RTPConnector::create() port(=%u) ERR_RTP_PORTBASENOTEVEN!!!", port);
		ret = -1;
	}
	else
	if(ERR_RTP_CANTBINDSOCKET == ret) {
		_ERROR("RTPConnector::create() ERR_RTP_CANTBINDSOCKET!!!", port);
		ret = -1;
	}

	return ret;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::stop()
{
	cancelThread_ = true;
	return 0;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::close()
{
	_INFO("RTPConnector::close()");

	if(false == initialized_) {
		_ERROR("RTP close():Not initialized!!!");
		return -1;
	}

	stop();
	sendThread_.kill();

	ssrcInfoList_.clear();
	errSsrcList_.clear();

	sess_.Destroy();

	init();

	return 0;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::sendPacket(unsigned short pduType, const void* dataPtr, unsigned long len)
{
	//_INFO("CRTPConnector::sendPacket(pdutype = %u, dataPtr = %lx, len = %lu)", pduType, dataPtr, len);


	if(false == initialized_) {
		_ERROR("RTP sendPacket():Not initialized!!!");
		return -1;
	}

	if(0 == sliceSize_) {
		_ERROR("RTP sendPacket():0 == sliceSize_!!!");
		return -1;
	}

	unsigned short	headerLen = sockHeaderCallBack_.GetPacketHeaderSize();
	unsigned long	dataLen = headerLen + len;

	unsigned long	sliceNum = dataLen / sliceSize_;
	if(dataLen % sliceSize_) {
		++ sliceNum;
	}

	//modified by zjj 2008-1-6
	//char*	pdata = new char[ dataLen ];
	char*	pdata = NULL;
	if(m_MaxSendBufLen < dataLen)
	{
		m_MaxSendBufLen = dataLen + 1024;
		delete[] m_pSendBuf;
		m_pSendBuf = new char[ m_MaxSendBufLen ];
	}
	pdata = m_pSendBuf;
	//end modified 

	ACE_ASSERT(0 != pdata);

	int		ret = 0;
	if(0 != sockHeaderCallBack_.GeneratePacketHeader((void*)pdata, pduType, len)) {
		_ERROR("RTP sendPacket():GeneratePacketHeader() Error!!!");
	//	delete[] pdata; //modified by zjj 2008-1-6
	//	pdata = NULL; //modified by zjj 2008-1-6
		ret = -1;
	}
	else {
		memcpy(pdata + headerLen, dataPtr, len);

		unsigned long	extensionData[2] = { htonl(sliceNum), 0 };
		for(unsigned long i = 0; i < sliceNum; ++ i) {
			extensionData[1] = htonl(i);
			unsigned long	size = sliceSize_;

			if(sliceNum - 1 == i) {
				size = dataLen - i * sliceSize_;
			}

			if(0 != (ret = sess_.SendPacket(pdata + i * sliceSize_,
										size,
										0,	/*externID*/
										extensionData,
										sizeof(extensionData)/4))) {
				_ERROR("RTP sendPacket():sess_.SendPacket() Error(=%d)!!!", ret);
				ret = -1;
			//	delete[] pdata; //modified by zjj 2008-1-6
			//	pdata = NULL; //modified by zjj 2008-1-6
				break;
			}
			/*
			if(i && (i % 50  == 0)) {
				ACE_OS::sleep(ACE_Time_Value(0, 5));
			}
			*/
		}
	}

	//delete[] pdata; //modified by zjj 2008-1-6
	//pdata = NULL; //modified by zjj 2008-1-6

	return ret;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::removeSsrcInfo(unsigned long ip,unsigned short port)
{
	ssrcinfo_it		itSsrcInfo ;
	CMutexKeeper	mutexKeeper(errMutex_);
	for(itSsrcInfo = ssrcInfoList_.begin(); itSsrcInfo != ssrcInfoList_.end(); itSsrcInfo++)
	{
		if((itSsrcInfo->ip_ == ip) && (itSsrcInfo->port_ == port))
		{
			errSsrcList_.push_back(ErrSsrc(itSsrcInfo->ssrc_,ip,port, SSRCLEAVE));
		}
	}
	return 0;
}

template <class SOCKHEADER>
void* CRTPConnectorTpl<SOCKHEADER>::threadProc(void* arg)
{
	_INFO("RTPConnector::threadProc() start thread %d", ACE_OS::thr_self());

	RTPPacket*		packet;
	RTPSourceData*	sourceData;
	SliceInfo*		pSliceInfo;
	unsigned long	sliceNo, totalNum;

	while(! isStopped()) {
		/* 接收数据包 */
		int ret = sess_.PollData();
		
		{
			CMutexKeeper	mutexKeeper(errMutex_);
			/*检查是否有异常的SSRC*/
			while(! errSsrcList_.empty()) {
				ErrSsrc	errSsrc = errSsrcList_.front();

				/*若异常是有新的SSRC到达,则回调有新源到达,否则报错*/
				if(CRTPConnectorTpl<SOCKHEADER>::SSRCNEWSOURCE == errSsrc.errCode_) {
					pCallBack_->OnNewRTPSource(
											errSsrc.ip_,
											errSsrc.port_);
				}
				else {
					if(CRTPConnectorTpl<SOCKHEADER>::SSRCLEAVE != errSsrc.errCode_) { //add by zjj 2006-8-19
						pCallBack_->OnErrorRTPSource(errSsrc.errCode_,
											errSsrc.ip_,
											errSsrc.port_);
					}
				}

				/*将异常的SSRC相应的信息删除,并删除此异常*/
				ssrcInfoList_.remove(SsrcInfo(errSsrc.ssrc_,errSsrc.ip_,errSsrc.port_)); //modified by zjj 2006-7-25
				errSsrcList_.pop_front();

			}
		}

		bool	hasData = false;
		/*若有SSRC有数据*/
		if(sess_.GotoFirstSourceWithData() && ! isStopped()) {

			hasData = true;
			do {
				/*则检查此SSRC的信息*/
				sourceData = sess_.GetCurrentSourceInfo();
				if(0 == sourceData) {
					continue;
				}

				SsrcInfo	ssrcInfo(sourceData->GetSSRC(),
											sourceData->GetIP(),
											sourceData->GetRTPPort());

				ssrcinfo_it		itSsrcInfo = std::lower_bound(
												ssrcInfoList_.begin(),
												ssrcInfoList_.end(),
												ssrcInfo 
												);

				/*若是一个新的SSRC,则加到列表中*/
//modified by zjj 2006-7-25
/*
				if(ssrcInfoList_.end() == itSsrcInfo || !(*itSsrcInfo == ssrcInfo)) {
					ssrcInfoList_.insert(itSsrcInfo, ssrcInfo);
					itSsrcInfo --;
				}
*/
				if(ssrcInfoList_.end() == itSsrcInfo || !(itSsrcInfo->ssrc_ == ssrcInfo.ssrc_)) {
					ssrcInfoList_.insert(itSsrcInfo, ssrcInfo);
					itSsrcInfo --;
				}
//add by zjj 2006-7-25
				else
				{
					bool hasIn = false;
					while((itSsrcInfo != ssrcInfoList_.end()) && (itSsrcInfo->ssrc_ == ssrcInfo.ssrc_))
					{
						if(*itSsrcInfo == ssrcInfo)
						{
							hasIn = true;
							break;
						}
						itSsrcInfo++;
					}
					if(!hasIn)
					{
						ssrcInfoList_.insert(itSsrcInfo, ssrcInfo);
						itSsrcInfo --;
					}
				}
//end add
//add  by zjj 2006-8-19
				itSsrcInfo->setTime();

				pSliceInfo = &itSsrcInfo->sliceInfo_;

				/*取出此SSRC的每一个数据包,即Slice*/
				while(0 != (packet = sess_.GetNextPacket()) && ! isStopped()) {

					/*校验Slice的格式*/
					if(0 != checkPacketValid(packet)) {
						delete packet;
						continue;
					}

					/*取出Slice的总片数与片号*/
					if(0 != getSliceNoFromPacket(packet, sliceNo, totalNum)) {
						delete packet;
						continue;
					}

					/*第一个分片或是不同一个包的分片*/
					if(pSliceInfo->empty() 
						|| packet->GetExtendedSequenceNumber() - sliceNo != pSliceInfo->firstSeqNo_) {
						pSliceInfo->set(packet, sliceNo, totalNum);
					}
					else {
						pSliceInfo->add(packet, sliceNo, totalNum);
					}

					/*所有的分片都能已到,即已经收到一个完整的包*/
					if(pSliceInfo->full()) {
						unsigned short	pduType = 0;
						unsigned long	bodyLen = 0;
						if(0 == sockHeaderCallBack_.ParsePacketHeader(pSliceInfo->packet_, pduType, bodyLen)
							&& pSliceInfo->hasSize_ >= bodyLen) {
							unsigned short		headerSize = sockHeaderCallBack_.GetPacketHeaderSize();
							if(pSliceInfo->hasSize_ == bodyLen + headerSize) {
								//_INFO("OnRTPPacketReceived(pduType = %d, bodyLen = %d)", pduType, bodyLen);

							//add by zjj 2006-8-14
								if(1 == pduType)
								{
									clearDestinations();
									addDestination( itSsrcInfo->ip_, itSsrcInfo->port_);
									sendPacket(19999,0,0);
								}
								else
								{
							//end add
								/*回调,通知有一个数据包到达*/
								pCallBack_->OnRTPPacketReceived(rtpSock_, 
														pduType,
														pSliceInfo->packet_ + headerSize, 
														pSliceInfo->hasSize_ - headerSize,
														itSsrcInfo->ip_,
														itSsrcInfo->port_);								
								}//add 2006-8-14
							}
							else {
								_ERROR("Not OnRTPPacketReceived: hasSize_ = %d, bodyLen = %d!!!", pSliceInfo->hasSize_, bodyLen);
							}
						}
						else {
							_ERROR("ParsePacketHeader() error!!!");
						}

						pSliceInfo->clear();
					}

					/*将packet删除*/
					delete packet;

				}/* while(0 != (packet = sess_.GetNextPacket())) */

			} while(! isStopped() && sess_.GotoNextSourceWithData());
		}/* if(sess_.GotoFirstSourceWithData()) */

		if(false == hasData && ! isStopped()) {
			ACE_OS::sleep(ACE_Time_Value(0, 1000));
		}
//add by zjj 2006-8-19
		{
			unsigned long nowtmpTime = time(NULL);
			ssrcinfo_it		it_ssrc, tmpit_ssrc ;
			for(it_ssrc = ssrcInfoList_.begin(); it_ssrc != ssrcInfoList_.end(); )
			{
				tmpit_ssrc = it_ssrc ;
				it_ssrc++;
				if((nowtmpTime - tmpit_ssrc->lasttime_) > 60)
				{
					ssrcInfoList_.erase(tmpit_ssrc);
				}
			}
		}
//end add by zjj 2006-8-19

	}/* while */

	//initialized_ = false;
	_ERROR("CRTPConnectorTpl<SOCKHEADER>::threadProc(=%d) thread exit!!!", ACE_OS::thr_self());

	return 0;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::getSliceNoFromPacket(RTPPacket* packet,
							unsigned long& sliceNo, unsigned long& totalNum)
{

	unsigned char*	pExternsionData = packet->GetHeaderExtensionData();
	ACE_ASSERT(pExternsionData);
	unsigned long*	p = (unsigned long*)pExternsionData;
	totalNum = ntohl(*p);
	++ p;
	sliceNo = ntohl(*p);

	int		ret = 0;
	/*illegal sliceNo >= totalNum*/
	if(sliceNo >= totalNum) {
		ret = -2;
	}

	if(totalNum == 0) {
		ret = -3;
	}

	return ret;
}

template <class SOCKHEADER>
int CRTPConnectorTpl<SOCKHEADER>::checkPacketValid(RTPPacket*	packet)
{
	int		ret = 0;

	/*No Extension Header*/
	if(false == packet->HasHeaderExtension()) {
		ret = -1;
	}
	else {
		int	externsionLen = packet->GetHeaderExtensionLength();
		if(externsionLen != 2 * sizeof(unsigned long)) {
			ret = -2;
		}
	}

	return ret;
}

template <class SOCKHEADER>
void CRTPConnectorTpl<SOCKHEADER>::SSRCNewSource(int exceptiontype, 
									void *exceptiondata,void *usrdata)
{
	RTPExcepSSRC*	exceptionSSRC = (RTPExcepSSRC*)exceptiondata;
	unsigned long	ssrc = exceptionSSRC->ssrc;
	CRTPConnectorTpl<SOCKHEADER>*	RTPConnector = (CRTPConnectorTpl<SOCKHEADER>*)usrdata;
	
	RTPSourceData*	sourceData = RTPConnector->sess_.GetSourceInfo(ssrc);

	if(sourceData) {
		CMutexKeeper	mutexKeeper(RTPConnector->errMutex_);
		RTPConnector->errSsrcList_.push_back(ErrSsrc(ssrc, 
												sourceData->GetIP(),
												sourceData->GetRTPPort(),
												SSRCNEWSOURCE));
	}

	return;
}

template <class SOCKHEADER>
void CRTPConnectorTpl<SOCKHEADER>::SSRCDeparture(int exceptiontype,
									void *exceptiondata,void *usrdata)
{
	RTPExcepSSRC*	exceptionSSRC = (RTPExcepSSRC*)exceptiondata;
	unsigned long	ssrc = exceptionSSRC->ssrc;
	CRTPConnectorTpl<SOCKHEADER>*	RTPConnector = (CRTPConnectorTpl<SOCKHEADER>*)usrdata;
	
	RTPSourceData*	sourceData = RTPConnector->sess_.GetSourceInfo(ssrc);

	if(sourceData) {
		CMutexKeeper	mutexKeeper(RTPConnector->errMutex_);
		RTPConnector->errSsrcList_.push_back(ErrSsrc(ssrc, 
												sourceData->GetIP(),
												sourceData->GetRTPPort(),
												SSRCDEPARTURE));
	}

	return;
}

template <class SOCKHEADER>
void CRTPConnectorTpl<SOCKHEADER>::SSRCTimeout(int exceptiontype,
									void *exceptiondata,void *usrdata)
{
	RTPExcepSSRC*	exceptionSSRC = (RTPExcepSSRC*)exceptiondata;
	unsigned long	ssrc = exceptionSSRC->ssrc;
	CRTPConnectorTpl<SOCKHEADER>*	RTPConnector = (CRTPConnectorTpl<SOCKHEADER>*)usrdata;
	
	RTPSourceData*	sourceData = RTPConnector->sess_.GetSourceInfo(ssrc);

	if(sourceData) {
		CMutexKeeper	mutexKeeper(RTPConnector->errMutex_);
		RTPConnector->errSsrcList_.push_back(ErrSsrc(ssrc, 
												sourceData->GetIP(),
												sourceData->GetRTPPort(),
												SSRCTIMEOUT));
	}

	return;
}

template <class SOCKHEADER>
void CRTPConnectorTpl<SOCKHEADER>::dump()
{
	_INFO("====================dump begin====================");
	int	i = 0, j = 0;
	for(ssrcinfo_it itSsrcInfo = ssrcInfoList_.begin();
		itSsrcInfo != ssrcInfoList_.end();
		++ itSsrcInfo, ++ i) {

		_INFO("ssrcInfoList_[%d]:", i);
		itSsrcInfo->dump("    ");

		j = 0;
		for(sliceinfo_it itSliceInfo = itSsrcInfo->sliceInfoList_.begin();
			itSliceInfo != itSsrcInfo->sliceInfoList_.end();
			++ itSliceInfo, ++ j) {

			_INFO("    sliceInfoList_[%d]:", j);
			itSliceInfo->dump("        ");
		}
	}
	_INFO("--------------------dump   end--------------------");

	return;
}

