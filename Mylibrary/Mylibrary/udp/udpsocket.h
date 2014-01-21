// udpsocket.h: interface for the udpsocket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __UDPSOCKET_H__
#define __UDPSOCKET_H__

#ifdef WIN32
#include <winsock2.h>
#include <WS2TCPIP.H>
#include <string>
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <fcntl.h>


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <net/if_arp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/un.h>
#include <semaphore.h>
#include <iconv.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <stdarg.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/vfs.h>


typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in  *LPSOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr  *LPSOCKADDR;
typedef const struct sockaddr* LPCSOCKADDR;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr  *LPIN_ADDR;

typedef  int    SOCKET;
typedef  unsigned short USHORT;
typedef  unsigned long  ULONG;
typedef  const char*    LPCSTR;
typedef  char*			LPSTR;
typedef  const char*    LPCTSTR;
typedef  unsigned int   UINT;
typedef  char* 	LPTSTR;
typedef  unsigned long HANDLE;

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#endif

class UDPSocket  
{
public:
	UDPSocket();
	~UDPSocket();
public:
	bool Create(unsigned short usPort,const char*cszLocalIPAddress=NULL);
	bool CreateBlock(unsigned short usPort,const char*cszLocalIPAddress=NULL);
	void Close(void);
	bool Connect(unsigned long ulIPAddress,unsigned short usPort);
	int ReceiveFrom(char*szDataBuffer,int nBufferLen,char *pulIPAddress,unsigned short*pusPort);
	int ReceiveFromBlock(char*szDataBuffer,int nBufferLen,unsigned long*pulIPAddress,unsigned short*pusPort);
	void SendTo(const char*pPacketData,int nLen,unsigned long ulIPAddress,unsigned short usPort);
	void SendTo(const char*pPacketData,int nLen,char*szIPAddress,unsigned short usPort);
	std::string GetLocalIPAddress(void);
	unsigned short GetLocalPort(void);
	static unsigned long GetIPAddressByName(const char*cszName);
	static char* GetNameByIPAddress(unsigned long ulIPAddress);
public:
#ifdef WIN32
	SOCKET	m_hSocket;
#else
	int		m_hSocket;
#endif
};

#endif 