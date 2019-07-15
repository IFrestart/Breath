/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	SockHeader.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include "Logger.h"
#include "SockHeader.h"
#include "SockStream.h"

/*
* CSockHeader
*/
unsigned short CSockHeader::GetPacketHeaderSize()
{
	return 16;
}

int CSockHeader::GeneratePacketHeader(void* pHeader, unsigned short pduType, unsigned long bodyLen)
{
	int		ret = -1;

	if(bodyLen <= MAX_PACKET_SIZE) {
		CSockStream		sockStream(pHeader, GetPacketHeaderSize());

		unsigned short	version = PACKET_VERSION_NO;
		unsigned short  dataType = rand();
		unsigned long	verify = (bodyLen + dataType + version) - pduType;

		sockStream << bodyLen
				<< dataType
				<< verify
				<< version
				<< pduType;
		ret = 0;
	}
	else {
		_ERROR("CSockHeader::GeneratePacketHeader(pduType = %u, bodyLen = %lu", pduType, bodyLen);
	}

	return ret;
}

int CSockHeader::ParsePacketHeader(const void* pHeader, unsigned short& pduType, unsigned long& bodyLen)
{
	CSockStream		sockStream(pHeader, GetPacketHeaderSize());
	unsigned short	version, dataType;
	unsigned long	verify;

	sockStream >> bodyLen
			>> dataType
			>> verify
			>> version
			>> pduType;

	int	ret = 0;
	if(bodyLen > MAX_PACKET_SIZE
		 || version != PACKET_VERSION_NO
		 || verify != (bodyLen + dataType + version) - pduType) {
		_ERROR("CSockHeader::ParsePacketHeader():pduType = %u, bodyLen = %lu, dataType = %u, version = %u, verify = %lu", 
				pduType, bodyLen, pduType, version, verify);
		ret = -1;
	}

	return ret;
}

