/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Tools.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>
#include <iomanip>

#include "ace/INET_Addr.h"

#include "Tools.h"

#ifndef  __LINUX__
#include <Winsock2.h>
#include <Ws2tcpip.h>
#else

#define SOCKET_ERROR            (-1)
#endif

unsigned long ip_to_long(const char* ipstr)
{
	unsigned long	ip = 0;

	ACE_INET_Addr	addr;
	if(-1 != addr.set((unsigned short)0, ipstr)) {
		ip = addr.get_ip_address();
	}

	return ip;
}

const char* ip_to_string(unsigned long ip)
{
	ACE_INET_Addr	addr;

	const char*	ipstr = 0;
	if(-1 != addr.set((unsigned short)0, ip)) {
		ipstr = addr.get_host_addr();
	}

	return ipstr;
}

unsigned long getSockIP(ACE_HANDLE sock)
{
	struct sockaddr_in	peer_addr;
	int		length;
	unsigned long	ip = 0;
	int		ret = ACE_OS::getpeername(sock, (struct sockaddr *)&peer_addr, &length);

	if(0 == ret) {
		ip = ACE_NTOHL(peer_addr.sin_addr.s_addr) ;
	}

	return ip;
}

unsigned short getSockPort(ACE_HANDLE sock)
{
	struct sockaddr_in	peer_addr;
	int		length;
	unsigned short	port = 0;
	int		ret = ACE_OS::getpeername(sock, (struct sockaddr *)&peer_addr, &length);

	if(0 == ret) {
		port = ACE_NTOHS(peer_addr.sin_port) ;
	}

	return port;
}

int getSockIpPort(ACE_HANDLE sock,unsigned long& remoteIp,unsigned short& remotePort)
{	
	struct sockaddr_in	peer_addr;
	int		length;
	unsigned short	port = 0;
	int		ret = ACE_OS::getpeername(sock, (struct sockaddr *)&peer_addr, &length);

	if(0 == ret) {
		remoteIp = ACE_NTOHL(peer_addr.sin_addr.s_addr) ;
		remotePort = ACE_NTOHS(peer_addr.sin_port) ;
	}
	return ret;
}

int getSockName(ACE_HANDLE hSocket, char* lpLocalHost, unsigned short* lpLocalPort, char* lpLocalMask)
{
	struct sockaddr_in saAddr;
	unsigned long lip = 0;
	#ifdef __LINUX__
	socklen_t nAddrLen = sizeof(saAddr);
	if (SOCKET_ERROR == getsockname(hSocket, (struct sockaddr*)&saAddr, &nAddrLen))
	{
		return -1;
	}
	#else
	int nAddrLen = sizeof(saAddr);
	if (SOCKET_ERROR == ACE_OS::getsockname(hSocket, (struct sockaddr*)&saAddr, &nAddrLen))
	{
		return -1;
	}
	#endif

	if (lpLocalHost != NULL)
	{
		#ifdef __LINUX__
		lip = ACE_NTOHL(saAddr.sin_addr.s_addr);
		#else
		lip = ACE_NTOHL(saAddr.sin_addr.S_un.S_addr);
		#endif
		strcpy(lpLocalHost,ip_to_string(lip));
	}

	if (lpLocalPort != NULL)
	{
		*lpLocalPort = ACE_NTOHS(saAddr.sin_port); 
	}

	if (lpLocalMask != NULL)
	{

		#ifdef __LINUX__
		int					 len;
		char				*ptr, buf[2048];
		struct ifconf		ifc;
		struct ifreq		*ifr;
		struct sockaddr_in	*sinptr,*addr;
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_req = (struct ifreq *) buf;

		ioctl(hSocket, SIOCGIFCONF, &ifc);

		for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
			ifr = (struct ifreq *) ptr;
			len = sizeof(struct sockaddr);
			ptr += sizeof(ifr->ifr_name) + len;	/* for next one in buffer */

			if (AF_INET == ifr->ifr_addr.sa_family) {
				sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
				if(saAddr.sin_addr.s_addr == sinptr->sin_addr.s_addr)
				{
					if(ioctl(hSocket,SIOCGIFNETMASK,ifr) == -1)
					{
						//printf("GetSockName ioctl SIOCGIFNETMASK  errno[%d] device name[%s] \n",errno,ifr->ifr_name);
						return 1;
					}
					addr = (struct sockaddr_in *)&ifr->ifr_addr;
        				strcpy(lpLocalMask , inet_ntoa(addr->sin_addr));
					break;
				}
			}
		}

		#else

		INTERFACE_INFO infBuf[10];
		DWORD dwInfSize = 0;

		if (::WSAIoctl((SOCKET)hSocket, SIO_GET_INTERFACE_LIST, NULL, 0, infBuf, 
			sizeof(infBuf), &dwInfSize, NULL, NULL) != SOCKET_ERROR)
		{
			DWORD dwCount = dwInfSize/sizeof(INTERFACE_INFO);
			if (dwCount > 10)
			{
				dwCount = 10;
			}

			for (DWORD i=0; i<dwCount; i++)
			{
				if (infBuf[i].iiAddress.AddressIn.sin_addr.S_un.S_addr 
					== saAddr.sin_addr.S_un.S_addr)
				{
					lip = ACE_NTOHL(infBuf[i].iiNetmask.AddressIn.sin_addr.S_un.S_addr);
					strcpy(lpLocalMask,ip_to_string(lip));
					break;
				}
			}
		}
	
		#endif
		//printf("GetSockName lpLocalHost[%s] lpLocalPort[%d] localport[%d] lpLocalMask[%s]\n",lpLocalHost,*lpLocalPort,ntohs(saAddr.sin_port),lpLocalMask);
	}
	return 0;
}

void dumpHex(const void* data, unsigned long len, unsigned short numperline)
{
	unsigned long	lines = len / numperline;
	unsigned char*	p = (unsigned char*)data;

	if(len % numperline) {
		++ lines;
	}

	char	fillc = std::cerr.fill();
	std::cerr << std::setfill('-') << std::setw((numperline + 2) * 3) << ' ' << std::endl;
	std::cerr << std::hex << std::setfill('0') << std::setw(2);

	for(unsigned long i = 0; i < lines; ++ i) {
		std::cerr << std::setw(2) << i << ": ";

		unsigned long	lineLen = len - i * numperline;
		if(lineLen > numperline) {
			lineLen = numperline;
		}
		for(unsigned long j = 0; j < lineLen; ++ j) {
			std::cerr << std::setw(2) << (unsigned short)p[ i * numperline + j ] << ' ';
			if(j == (numperline / 2) - 1) {
				std::cerr << "- ";
			}
		}

		std::cerr << std::endl;
	}

	std::cerr << std::setfill('-') << std::setw((numperline + 2) * 3) << ' ' << std::endl;
	std::cerr << std::dec;
	std::cerr.fill(fillc);

	return;
}

