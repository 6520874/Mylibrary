// udpsocket.cpp: implementation of the udpsocket class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "udpsocket.h"

#ifdef WIN32
#endif
#ifdef UNIX
#include <signal.h>
#include <sys/types.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UDPSocket::UDPSocket()
{
	WORD wVersionRequired;
	WSAData wsaData;
	wVersionRequired = MAKEWORD(2,2);

	int ierr = WSAStartup(wVersionRequired,&wsaData);

	if ( ierr != 0 ) 
	{
		printf("\n WSAStartup error \n");
	}
	m_hSocket=INVALID_SOCKET;
}


UDPSocket::~UDPSocket()
{
    WSACleanup();

}

bool UDPSocket::Create(unsigned short usPort,const char*cszLocalIPAddress)
{
	sockaddr_in local;
	local.sin_family=AF_INET;
	if (cszLocalIPAddress && strlen(cszLocalIPAddress)>0)
		local.sin_addr.s_addr=inet_addr(cszLocalIPAddress);
	else
		local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(usPort);
		
	if((m_hSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))==INVALID_SOCKET)
	{
		m_hSocket=INVALID_SOCKET;
		return false;
	}
	
	if(bind(m_hSocket,(SOCKADDR*)&local,sizeof(sockaddr_in))==SOCKET_ERROR)
	{   
		local.sin_addr.s_addr=htonl(INADDR_ANY);

		if(bind(m_hSocket,(SOCKADDR*)&local,sizeof(sockaddr_in))==SOCKET_ERROR)
		{   
			return false;
		}
	}

	return true;
}

bool UDPSocket::CreateBlock(unsigned short usPort,const char*cszLocalIPAddress)
{
	sockaddr_in local;
	local.sin_family=AF_INET;
	if (cszLocalIPAddress && strlen(cszLocalIPAddress)>0)
		local.sin_addr.s_addr=inet_addr(cszLocalIPAddress);
	else
		local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(usPort);
		
	if((m_hSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))==INVALID_SOCKET)
	{
		m_hSocket=INVALID_SOCKET;
		return false;
	}
	
	if(bind(m_hSocket,(SOCKADDR*)&local,sizeof(sockaddr_in))==SOCKET_ERROR)
	{   
		local.sin_addr.s_addr=htonl(INADDR_ANY);

		if(bind(m_hSocket,(SOCKADDR*)&local,sizeof(sockaddr_in))==SOCKET_ERROR)
		{   
			return false;
		}
	}
		
	int err;
#ifndef WIN32
#else
	{
		int timeout = 0;

		// set receive buffer size to 64K
		int	nBufferSize = 64 * 1024;
		err=setsockopt(
					m_hSocket,
					SOL_SOCKET,
					SO_RCVBUF,
					(char*)&nBufferSize,
					sizeof(nBufferSize));
		if (err != 0)
		{
			return false;
		}
		err=setsockopt(
					m_hSocket,
					SOL_SOCKET,
					SO_SNDBUF,
					(char*)&nBufferSize,
					sizeof(nBufferSize));
		if (err != 0)
		{
			return false;
		}
	}
#endif

	int nValue=160;
	err=setsockopt(
				m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}

	return true;
}

void UDPSocket::Close(void)
{
	if (INVALID_SOCKET == m_hSocket) 
		return;
	
#ifdef WIN32	
	//shutdown(m_hSocket,SD_BOTH);
	closesocket (m_hSocket);
#else
	shutdown(m_hSocket,SHUT_RDWR);
	close(m_hSocket);
#endif
	m_hSocket = INVALID_SOCKET;
}

int UDPSocket::ReceiveFrom(char*szDataBuffer,int nBufferLen,char *pulIPAddress,unsigned short*pusPort)
{
#ifdef WIN32	
	int nAddrLen=sizeof(sockaddr_in);
#else
	socklen_t nAddrLen=sizeof(sockaddr_in);
#endif
	sockaddr_in RemoteAddr;

	int nRet=recvfrom(m_hSocket,szDataBuffer,nBufferLen,0,(sockaddr*)&RemoteAddr,&nAddrLen);
	
	if (pusPort)
		*pusPort=ntohs (RemoteAddr.sin_port);

	strcpy(pulIPAddress,inet_ntoa(RemoteAddr.sin_addr));
	return nRet;
}

int UDPSocket::ReceiveFromBlock(char*szDataBuffer,int nBufferLen,unsigned long*pulIPAddress,unsigned short*pusPort)
{
#ifdef WIN32	
	int nAddrLen=sizeof(sockaddr_in);
#else
	socklen_t nAddrLen=sizeof(sockaddr_in);
#endif
	sockaddr_in RemoteAddr;

	int nRet=recvfrom(m_hSocket,szDataBuffer,nBufferLen,0,(sockaddr*)&RemoteAddr,&nAddrLen);
	
	if (pusPort)
		*pusPort=ntohs (RemoteAddr.sin_port);

	if (pulIPAddress)
		*pulIPAddress=ntohl (RemoteAddr.sin_addr.s_addr);

	return nRet;
}

void UDPSocket::SendTo(const char*pPacketData,int nLen,char*szIPAddress,unsigned short usPort)
{
	SendTo(pPacketData,nLen,UDPSocket::GetIPAddressByName(szIPAddress),usPort);
}

void UDPSocket::SendTo(const char*pPacketData,int nLen,unsigned long ulIPAddress,unsigned short usPort)
{
	if (m_hSocket!=INVALID_SOCKET)
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(ulIPAddress);
		addr.sin_port = htons(usPort);
#ifdef UNIX
		struct sigaction act,oact;
		act.sa_handler=SIG_IGN;
		sigemptyset(&act.sa_mask);
		act.sa_flags=0;
		sigaction(SIGPIPE,&act,&oact);
#endif
		int nSent=0;
		nSent=sendto(m_hSocket,pPacketData,nLen,
#ifdef WIN32
						0
#else
#ifdef UNIX
						0
#else
						MSG_NOSIGNAL
#endif
#endif
						,(sockaddr *) &addr, sizeof(addr));
#ifdef UNIX
		sigaction(SIGPIPE,&oact,NULL);
#endif

		if(nSent==SOCKET_ERROR)
		{
			printf("\nSendTO error %d\n",WSAGetLastError());
		}
	}
		
}

bool UDPSocket::Connect(unsigned long ulIPAddress,unsigned short usPort)
{
	bool bRet=false;

	if((m_hSocket=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP))==INVALID_SOCKET)
	{
		m_hSocket=INVALID_SOCKET;
		return bRet;
	}
	

	int err;
	// set the socket to never block on recv() calls */
#ifndef WIN32
	if (0 != fcntl (m_hSocket, F_SETFL, O_NONBLOCK))
    {
		return false;
    }
#else
	{
		int timeout = 0;

		err = setsockopt (m_hSocket,
				  SOL_SOCKET,
				  SO_RCVTIMEO, (char *) &timeout, sizeof (timeout));
		if (err != 0)
		{
			return false;
		}
		// set receive buffer size to 64K
		int	nBufferSize = 64 * 1024;
		err=setsockopt(
					m_hSocket,
					SOL_SOCKET,
					SO_RCVBUF,
					(char*)&nBufferSize,
					sizeof(nBufferSize));
		if (err != 0)
		{
			return false;
		}
	}
#endif

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ulIPAddress);
	addr.sin_port = htons(usPort);
	int nError=connect(m_hSocket,(sockaddr *) &addr, sizeof(addr));
	if (nError==0)
		bRet=true;

	int nValue=160;
	err=setsockopt(
				m_hSocket,
				IPPROTO_IP,
				IP_TOS,
				(char*)&nValue,
				sizeof(nValue));
	if (err != 0)
	{
		nValue=0;
	}

	return bRet;
}

string  UDPSocket::GetLocalIPAddress(void)
{
	char hostname[256] = {0};
	if(gethostname(hostname,256)==0) 
	{
		//printf("%s\n",hostname);//¼ÆËã»úÃû×Ö
	} 

	struct hostent *pHost = gethostbyname(hostname);
	string strIP;

	for (int i = 0; pHost != NULL && pHost->h_addr_list[i] != NULL; i++) 
	{
		strIP =  inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]); 
	} 

	return strIP;
}

unsigned short UDPSocket::GetLocalPort(void)
{
	if (m_hSocket == INVALID_SOCKET)
		return 0;
	
	sockaddr_in addr;
#ifdef WIN32	
	int nLengthAddr = sizeof (sockaddr);
#else
	socklen_t nLengthAddr = sizeof (sockaddr);
#endif
	if (getsockname (m_hSocket, (sockaddr*)&addr, &nLengthAddr) == SOCKET_ERROR)
		return 0;

	return ntohs (addr.sin_port);
}

unsigned long UDPSocket::GetIPAddressByName(const char*cszName)
{
	unsigned long ulIPAddress=0;
	struct in_addr addr;
#ifdef WIN32
	addr.s_addr = inet_addr(cszName);
	if (addr.s_addr==0 || addr.s_addr==INADDR_NONE)
#else
	if (inet_aton(cszName,&addr) == 0) 
#endif
	{
		struct hostent *hostentry;
		hostentry=gethostbyname(cszName);

		if (hostentry==NULL) 
		{
			ulIPAddress=0;
		}
		else
		{
			memcpy(&addr, hostentry->h_addr, sizeof(struct in_addr));
			ulIPAddress=ntohl (addr.s_addr);			
		}
	}
	else
	{
		ulIPAddress=ntohl(addr.s_addr);			
	}

	return ulIPAddress;
}

char* UDPSocket::GetNameByIPAddress(unsigned long ulIPAddress)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons (0);
	addr.sin_addr.s_addr = htonl (ulIPAddress);

	return inet_ntoa (addr.sin_addr);
}