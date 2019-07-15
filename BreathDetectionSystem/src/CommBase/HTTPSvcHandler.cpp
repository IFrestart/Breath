/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 
 * HTTP连接数据接收与发送处理类,提供了HTTP连接的打开,关闭与发送数据等操作
 * 并在有一个完整报文到达时与出错时执行回调.

 * @file	HTTPSvcHandler.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short	HTTP连接数据接收与发送处理类
 **/

#include <iostream>

#include "Logger.h"
#include "HTTPSvcHandler.h"

template<>
int CTCPSvcHandlerTpl<CHTTPHeader>::handle_input(ACE_HANDLE h)
{
	static char	token[4] = {'\r', '\n', '\r', '\n'};
	const int	n = sizeof(token);
	int		ret = 0;
	if(! isBegin_) {
		if(0 == mbRecv_) {
			mbRecv_ = new CSockBlock(n);
		}
		size_t	wantLen = 0;
		bool	end = false;
		while(! end) {
			wantLen = n - (mbRecv_->wr_ptr() - mbRecv_->base());
			
			ret = peer().recv(mbRecv_->wr_ptr(), wantLen);
			if(0 == ret) {
				//_ERROR("HTTP Recv() header: Peer(sock=%4d) is Closed!!!", get_handle());
				return -1;
			}
			else
			if(-1 == ret) {
				if(EAGAIN == errno || EINTR == errno) {
					//_ERROR("HTTP Recv() header: Peer(sock=%4d) is Error(=%d),do next.", get_handle(), errno);
					return 0;
				}
				else {
					//_ERROR("HTTP Recv() header: Peer(sock=%4d) is Error(=%d)!!!", get_handle(), errno);
					return -1;
				}
			}
			else {
				mbRecv_->wr_ptr(ret);
				wantLen -= ret;
			}

			if(mbRecv_->wr_ptr() - mbRecv_->base() == n) {
				int	i, j, k;
				for(i = 0; i < n; i ++) {
					k = 0;
					for(j = i; j < n; j ++, k ++) {
						if(*((char*)mbRecv_->base() + j) != token[k]) {
							break;
						}
					}

					if(n == j) {
						break;
					}
				}

				if(j == n) {
					if(0 == i) {
						end = true;
					}
					else {
						memmove(mbRecv_->base(), mbRecv_->base() + i, k);
						mbRecv_->wr_ptr(mbRecv_->base() + k);
					}
				}
				else {
					mbRecv_->wr_ptr(mbRecv_->base());
				}
			}
		}

		isBegin_ = true;

		isHeader_ = true;
		if (mbRecv_)		//add 2011-7-8
		{
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;
		}		
		wantLen_ = sockHeaderCallBack_.GetPacketHeaderSize() - sockHeaderCallBack_.GetHttpHeaderSize();
		mbRecv_ = new CSockBlock(wantLen_);
	}

	while(wantLen_) {
		ret = peer().recv(mbRecv_->wr_ptr(), wantLen_);
		if(0 == ret) {
			//_ERROR("HTTP Recv() body: Peer(sock=%4d) is Closed!!!", get_handle());
			return -1;
		}
		else
		if(-1 == ret) {
			if(EAGAIN == errno || EINTR == errno) {
				//_ERROR("HTTP Recv() body: Peer(sock=%4d) is Error(=%d),do next.", get_handle(), errno);
				return 0;
			}
			else {
				//_ERROR("HTTP Recv() body: Peer(sock=%4d) is Error(=%d)!!!", get_handle(), errno);
				return -1;
			}
		}
		else {
			mbRecv_->wr_ptr(ret);
			wantLen_ -= ret;
		}
	}

	ret = 0;
	if(isHeader_) {
		isHeader_ = false;

		if(0 != sockHeaderCallBack_.ParsePacketHeader(mbRecv_->base(), pduType_, wantLen_)) {
			//_ERROR("HTTP ParsePacketHeader(): sock=%4d Error!!!", get_handle());
			return -1;
		}

		if (mbRecv_)		//2011-7-8
		{
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;
		}	
		if(0 != wantLen_) {
			mbRecv_ = new CSockBlock(wantLen_);
		}
	}

	if(0 == wantLen_) {
		if(pSvcManager_) {
			//_INFO("HTTP OnTCPPacketReceived(): sock=%4d pdu=%5d len=%5d...", get_handle(), pduType_, mbRecv_ ? mbRecv_->size() : 0);
			ret = pSvcManager_->OnPacketReceived(this, pduType_,
							mbRecv_ ? mbRecv_->base() : 0, 
							mbRecv_ ? mbRecv_->size() : 0);
		}
		if(mbRecv_) {
			mbRecv_->release();
			delete mbRecv_;
			mbRecv_ = 0;
		}
		isBegin_ = false;
	}

	return ret == 0 ? 0 : -1;
}

