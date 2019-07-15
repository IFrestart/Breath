/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	SockHeader.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include "Logger.h"
#include "RECSockHeader.h"
#include "SockStream.h"

/*
* CSockHeader
*/
unsigned short CRECSockHeader::GetPacketHeaderSize()
{
	return 6;
}

int CRECSockHeader::GeneratePacketHeader(void* pHeader, unsigned short pduType, unsigned long bodyLen)
{
	int		ret = -1;

	if(bodyLen <= MAX_PACKET_SIZE) {
		CSockStream		sockStream(pHeader, GetPacketHeaderSize());

		sockStream << bodyLen
				<< pduType;
		ret = 0;
	}
	else {
	}

	return ret;
}

int CRECSockHeader::ParsePacketHeader(const void* pHeader, unsigned short& pduType, unsigned long& bodyLen)
{
	CSockStream		sockStream(pHeader, GetPacketHeaderSize());

	sockStream >> bodyLen
			>> pduType;

	int	ret = 0;
	if(bodyLen > MAX_PACKET_SIZE){
		ret = -1;
	}

	return ret;
}

