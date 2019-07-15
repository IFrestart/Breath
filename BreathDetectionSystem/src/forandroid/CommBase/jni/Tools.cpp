/**
 * Copyright (c) 2004, HangZhou Webcon Corporation. 

 * @file	Tools.cpp
 * @author	chetang <chetang@webcon.com.cn>

 * @short 
 **/

#include <iostream>
#include <iomanip>


#include "Tools.h"
#include <sys/ioctl.h>
#include <net/if.h>

//#include "ace/INET_Addr.h"

#define SOCKET_ERROR            (-1)

unsigned long ip_to_long(const char* ipstr)
{
	unsigned long	ip = 0;

	struct sockaddr_in	peer_addr;
	peer_addr.sin_addr.s_addr = inet_addr(ipstr);
	ip = ntohl(peer_addr.sin_addr.s_addr) ;
	return ip;
}

const char* ip_to_string(unsigned long ip)
{
	const char*	ipstr = 0;
	struct in_addr addr;
	//addr.s_addr = ip;
	addr.s_addr = htonl(ip);
	ipstr = inet_ntoa(addr);
	return ipstr;
}

unsigned long getSockIP(ACE_HANDLE sock)
{
	struct sockaddr_in	peer_addr;
	//int		length;
	socklen_t	length;
	unsigned long	ip = 0;
	///int		ret = ACE_OS::getpeername(sock, (struct sockaddr *)&peer_addr, &length);
	int		ret = getpeername(sock, (struct sockaddr *)&peer_addr, &length);

	if(0 == ret) {
//		ip = ACE_NTOHL(peer_addr.sin_addr.s_addr) ;
		ip = ntohl(peer_addr.sin_addr.s_addr) ;
	}

	return ip;
}

unsigned short getSockPort(ACE_HANDLE sock)
{
	struct sockaddr_in	peer_addr;
	//int		length;
	socklen_t	length;
	unsigned short	port = 0;
	//int		ret = ACE_OS::getpeername(sock, (struct sockaddr *)&peer_addr, &length);
	int		ret = getpeername(sock, (struct sockaddr *)&peer_addr, &length);

	if(0 == ret) {
	//	port = ACE_NTOHS(peer_addr.sin_port) ;
		port = ntohs(peer_addr.sin_port) ;
	}

	return port;
}

int getSockName(ACE_HANDLE hSocket, char* lpLocalHost, unsigned short* lpLocalPort, char* lpLocalMask)
{
	struct sockaddr_in saAddr;
	unsigned long lip = 0;
	socklen_t nAddrLen = sizeof(saAddr);
	if (SOCKET_ERROR == getsockname(hSocket, (struct sockaddr*)&saAddr, &nAddrLen))
	{
		return -1;
	}
	if (lpLocalHost != NULL)
	{
		lip = saAddr.sin_addr.s_addr;
		lip = ntohl(saAddr.sin_addr.s_addr);
		strcpy(lpLocalHost,ip_to_string(lip));
	}

	if (lpLocalPort != NULL)
	{
		*lpLocalPort = ntohs(saAddr.sin_port); 
	}

	if (lpLocalMask != NULL)
	{

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

