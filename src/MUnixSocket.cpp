/*
Copyright (C) 2011-2014, Comine.com ( comine.com@gmail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- The the names of the contributors of this project may not be used to 
  endorse or promote products derived from this software without specific 
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


//v1.8 copyright Comine.com 20130428U1600
#include "MStdLib.h"

/////////////////////////////////////////////////
#if (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) )
#include <winsock2.h>
#include <windows.h>

// Link to Windows Libraries
#pragma comment(lib,"WS2_32.lib")

///////////////////////////////////////
#elif defined(MSTDLIB_OS_MINGW)	
#include <Windows.h>
#include <WinSock.h>

///////////////////////////////////////
#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) ) 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>

//////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
#error Module Does not compile on MoSync

#endif  // MSTDLIB_OS_WINDOWS


////////////////////////////////////////////////////////
#include "MUnixSocket.h"


#if (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
/////////////////////////////////////////////////////
struct GSockInitObject
	{
	//////////////////////////////////////////////
	GSockInitObject(void)
		{
		// Init Socket Layer if necessary
		struct WSAData sockinfo;
		MStdMemSet(&sockinfo,0,sizeof(sockinfo) );

		if(::WSAStartup(MAKEWORD(2,2),&sockinfo)!=0)
			{
			return;
			}
		}

	//////////////////////////////////////////////
	~GSockInitObject(void)
		{
		// WSA Clean
		WSACleanup();
		}
	};


////////////////////////////////////////////////////
static GSockInitObject GSocketInitializeObject;

#endif // MSTDLIB_OS_WINDOWS

//******************************************************************************
//  MUnixSocketSockAddr
//******************************************************************************
void MUnixSocketSockAddr::ClearObject(void)
	{
	MStdMemSet(&mSockAddr,0,sizeof(mSockAddr) ); 
	}


////////////////////////////////////////////////////////
MUnixSocketSockAddr::MUnixSocketSockAddr(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////
MUnixSocketSockAddr::MUnixSocketSockAddr(struct sockaddr_in &sad)
	{
	ClearObject();
	MStdMemCpy(&mSockAddr,&sad,sizeof(mSockAddr) ); 
	}


//////////////////////////////////////////////////////////
MUnixSocketSockAddr::~MUnixSocketSockAddr(void)
	{  Destroy();  }


//////////////////////////////////////////////////////////
bool MUnixSocketSockAddr::Create(unsigned long ipaddress,int port,int family)
	{
	Destroy();
	mSockAddr.sin_family=family;
	mSockAddr.sin_addr.s_addr=htonl(ipaddress);
	mSockAddr.sin_port=htons(port);
	
	return true;
	}


//////////////////////////////////////////////////////////
bool MUnixSocketSockAddr::Create(const char *ipaddress,int port,int family)
	{
	Destroy();
	mSockAddr.sin_family=family;
	mSockAddr.sin_addr.s_addr=inet_addr(ipaddress);
	mSockAddr.sin_port=htons(port);

	return true;	
	}


//////////////////////////////////////////////////////////
bool MUnixSocketSockAddr::Create(struct sockaddr_in &sad)
	{
	Destroy();
	MStdMemCpy(&mSockAddr,&sad,sizeof(mSockAddr) );
	return true;
	}


//////////////////////////////////////////////////////////
bool MUnixSocketSockAddr::Destroy(void)
	{
	ClearObject();
	return true;
	}

	
//////////////////////////////////////////////////////////
int MUnixSocketSockAddr::GetPort(void)
	{
	return ntohs(mSockAddr.sin_port);
	}


//////////////////////////////////////////////////////////
unsigned long MUnixSocketSockAddr::GetAddress(void)
	{
	return ntohl(mSockAddr.sin_addr.s_addr);
	}


//////////////////////////////////////////////////////////
int MUnixSocketSockAddr::GetFamily(void)
	{
	return mSockAddr.sin_family;
	}


//////////////////////////////////////////////////////////
struct sockaddr_in &MUnixSocketSockAddr::Get(void)
	{
	return mSockAddr;
	}


//******************************************************************************
//  MUnixSocket
//******************************************************************************
void MUnixSocket::ClearObject(void)
	{
	mSocket=-1;
	}


///////////////////////////////////////////////////
MUnixSocket::MUnixSocket(void)
	{  ClearObject(); }


///////////////////////////////////////////////////
MUnixSocket::MUnixSocket(int sockfamily,int socktype,int protocol)
	{
	ClearObject();
	if(Create(sockfamily,socktype,protocol)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////////
MUnixSocket::MUnixSocket(int tsocket)
	{
	ClearObject();
	if(Create(tsocket)==false)
		{
		return;
		}
	}


///////////////////////////////////////////////////
MUnixSocket::~MUnixSocket(void)
	{  Destroy(); }


///////////////////////////////////////////////////
bool MUnixSocket::Create(int sockfamily,int socktype,int protocol)
	{
	Destroy();
	mSocket=socket(sockfamily,socktype,protocol);
	if(mSocket<0) 
		{
		return false;
		} 

	return true;
	}


///////////////////////////////////////////////////
bool MUnixSocket::Create(int tsocket)
	{
	Destroy();
	mSocket=tsocket;
	return true;
	}

///////////////////////////////////////////////////
bool MUnixSocket::Destroy(void)
	{
	if(mSocket!=-1)
		{
		/////////////////////////////////////////////
		#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
		closesocket(mSocket);

		/////////////////////////////////////////////
		#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
		close(mSocket);

		/////////////////////////////////////////////
		#endif 
		}
		
	ClearObject();
	return true;
	}


///////////////////////////////////////////////////
int MUnixSocket::Recv(void *Buffer,int len,int flag)  // return <0, on failure,
	{
	int ret;
	for(;;)
		{
		ret=recv(mSocket,(char *)Buffer,len,flag);
		if(ret<0)
			{
			////////////////////////////////
			#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
			if(ret==WSAETIMEDOUT) { Sleep(3*1000);  continue; }

			////////////////////////////////
			#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
			// Linux Specific code
			if(ret==EAGAIN) { sleep(3);  continue; }
			#endif // MSTDLIB_OS_WINDOWS
			
			return -1;
			}
		break;
		}

	return ret;
	}


//////////////////////////////////////////////////
int MUnixSocket::RecvFrom(void *buffer,int len,MUnixSocketSockAddr &sad,int flag)
	{
	int socklen=sizeof(struct sockaddr_in);
	struct sockaddr_in *ptr;  ptr=(struct sockaddr_in *)(void *)& (sad.Get());

	//////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	int ret=recvfrom(mSocket,(char *)buffer,len,flag,(struct sockaddr *)ptr
			,&socklen );

	//////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int ret=recvfrom(mSocket,(char *)buffer,len,flag,(struct sockaddr *)ptr
			,(socklen_t*)&socklen );

	#endif // MSTDLIB_OS_WINDOWS

	if(ret==-1)
		{
		return -1;
		}

	return ret;
	}


////////////////////////////////// flag - MSG_OOB, MSG_DONTROUTE
int MUnixSocket::Send(void *Buffer,int len,int flag) // =<0, on failure
	{
	int ret=send(mSocket,(const char *)Buffer,len,flag);
	if(ret<0)
		{
		return -1;
		}

	return ret;
	}


///////////////////////////////////
int MUnixSocket::SendTo(void *Buffer,int len,MUnixSocketSockAddr &addr,int flag)
	{
	struct sockaddr_in *ptr; ptr=(struct sockaddr_in *)(void *)&addr.Get();
	int ret=sendto(mSocket,(const char *)Buffer,len,flag
			,(struct sockaddr *)ptr,sizeof(struct sockaddr_in));
	if(ret<0)
		{
		return -1;
		}

	return ret;	
	}


////////////////////////////////////////////
#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
SOCKET MUnixSocket::GetSocket(void)
	{
	return mSocket;
	}

/////////////////////////////////////////////
#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
int MUnixSocket::GetSocket(void)
	{
	return mSocket;
	}

#endif // MSTDLIB_OS_WINDOWS


////////////////////////////////////////////
int MUnixSocket::SetSocketOption(int level,int optname,const char *optvalue,int optlen)
	{
	int ret=setsockopt(mSocket,level,optname,optvalue,optlen);	
	if(ret<0)
		{
		return -1;
		}

	return ret;
	}


////////////////////////////////////////////
bool MUnixSocket::Listen(int backlog)
	{
	if(listen(mSocket,backlog)<0)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////
bool MUnixSocket::Bind(MUnixSocketSockAddr &sad)
	{
	struct sockaddr_in *ptr=&sad.Get();

	if(bind(mSocket,(struct sockaddr *)ptr,sizeof(*ptr))<0)
		{
		return false;
		}
		
	return true;
	}

/////////////////////////////////////////////
bool MUnixSocket::IsReadable(int sec,int usec)
	{
	fd_set readset;  FD_ZERO(&readset);
	FD_SET(mSocket,&readset);

	int ret;
	if(sec==-1 && usec==-1)
		{  ret=select(mSocket+1,&readset,NULL,NULL,NULL);  } // Wait forever
	else
		{
		struct timeval tm={sec,usec};
		ret=select(mSocket+1,&readset,NULL,NULL,&tm);
		}

	if(ret==0) { return false; }
	if(ret==1) { return true; }

	return false;
	}


//////////////////////////////////////////////
bool MUnixSocket::IsWritable(int sec,int usec)
	{
	fd_set writeset;  FD_ZERO(&writeset);
	FD_SET(mSocket,&writeset);

	int ret;
	if(sec==-1 && usec==-1)
		{  ret=select(mSocket+1,NULL,&writeset,NULL,NULL);  } // Wait forever
	else
		{
		struct timeval tm={sec,usec};
		ret=select(mSocket+1,NULL,&writeset,NULL,&tm);
		}

	if(ret==0) { return false; }
	if(ret==1) { return true; }

	return false;
	}


///////////////////////////////////////////////
bool MUnixSocket::IOCtlSocket(long cmd,unsigned long *retvalue)
	{
	/////////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	return false;

	/////////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	if(ioctl(mSocket,cmd,retvalue)<0)
		{
		return false;
		}

	#endif // MSTDLIB_OS_WINDOWS

	return true;	
	}


////////////////////////////////////////////////
bool MUnixSocket::Accept(MUnixSocket &socket,MUnixSocketSockAddr &clientaddress)
	{
	struct sockaddr_in cliad; 
	MStdMemZero(&cliad,sizeof(cliad) );
	int cliadlen=sizeof(cliad);

	// Accept the socket
	///////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET clisock=accept(mSocket,(struct sockaddr*)&cliad,&cliadlen);
	if(clisock==INVALID_SOCKET)
		{
		return false;
		}

	///////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int clisock=accept(mSocket,(struct sockaddr*)&cliad,(socklen_t *)&cliadlen);
	if(clisock<0)
		{
		return false;		
		}

	#endif // MSTDLIB_OS_WINDOWS


	if(socket.Create(clisock)==false )
		{
		/////////////////////////////////////////
		#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
		closesocket(clisock);

		////////////////////////////////////////
		#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
		close(clisock);

		#endif // MSTDLIB_OS_WINDOWS
		return false;
		}


	if(clientaddress.Create(cliad)==false)
		{
		// Close Client Socket	
		//////////////////////////////////////////////////
		#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
		closesocket(clisock);

		//////////////////////////////////////////////////
		#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
		close(clisock);

		#endif	// MSTDLIB_OS_WINDOWS

		return false;
		}

	return true;	
	}


//////////////////////////////////////////////////////
bool MUnixSocket::ShutDown(void)
	{
	////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	if(shutdown(mSocket,SD_BOTH)==SOCKET_ERROR)
		{
		return false;
		}

	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	if(shutdown(mSocket,SHUT_RDWR)<0)
		{
		return false;
		}

	#endif // MSTDLIB_OS_WINDOWS

	return true;
	}


//*****************************************************************************************
//  MUnixSocketIP
//*****************************************************************************************
void MUnixSocketIP::ClearObject(void)
	{
	mIPAddress=0;
	MStdMemSet(mIPBuffer,0,sizeof(mIPBuffer) );
	}


///////////////////////////////////////////////
MUnixSocketIP::MUnixSocketIP()
	{  ClearObject();  }


//////////////////////////////////////////////////
MUnixSocketIP::MUnixSocketIP(unsigned long ipaddress)
	{
	ClearObject();
	if(Create(ipaddress)==false)
		{
		return;
		}
	}

///////////////////////////////////////////////////
MUnixSocketIP::MUnixSocketIP(const char *ipaddress)
	{
	ClearObject();
	if(Create(ipaddress)==false)
		{
		return;
		}
	}


//////////////////////////////////////////////////
MUnixSocketIP::~MUnixSocketIP() { Destroy();  }


//////////////////////////////////////////////////
bool MUnixSocketIP::Create(const char *addr)
	{
	ClearObject();
	if(addr==NULL)
		{ 
		return false; 
		}
	if(Create(inet_addr(addr) )==false)
		{ return false; }

	return true;
	}


///////////////////////////////////////////////////
bool MUnixSocketIP::Create(unsigned long ipaddress)
	{
	Destroy();
	mIPAddress=ipaddress;
	const char *address=(const char *)&mIPAddress;
	MStdSPrintf(mIPBuffer,sizeof(mIPBuffer),"%d.%d.%d.%d"
		,(int)(unsigned char)address[0]
		,(int)(unsigned char)address[1]
		,(int)(unsigned char)address[2]
		,(int)(unsigned char)address[3]);
		
	return true;
	}


///////////////////////////////////////////////////
bool MUnixSocketIP::Destroy(void)
	{
	ClearObject();
	return true;
	}


///////////////////////////////////////////////////
const char *MUnixSocketIP::ToString() 
	{
	return mIPBuffer;
	}


////////////////////////////////////////////////////
unsigned long MUnixSocketIP::GetAddress(void)
	{
	return mIPAddress;
	}

	
//*****************************************************************************************
//  MUnixSocketClient
//*****************************************************************************************
void MUnixSocketClient::ClearObject(void)
	{
	}


////////////////////////////////////////////////////////////////////////////////
MUnixSocketClient::MUnixSocketClient(void)
	{  ClearObject();  }


////////////////////////////////////////////////////////////////////////////////
MUnixSocketClient::MUnixSocketClient(const char *host,int port,int family
		,int type,int protocol)
	{
	ClearObject();
	if(Create(host,port,family,type,protocol)==false)
		{
		return;
		}
	}


////////////////////////////////////////////////////////////////////////////////
MUnixSocketClient::~MUnixSocketClient(void)
	{
	Destroy();
	}


////////////////////////////////////////////////////////////////////////////////
bool MUnixSocketClient::Create(const char *host,int port,int family
		,int type,int protocol)
	{
	Destroy();
	if(MUnixSocket::Create(family,type,protocol)==false)
		{
		Destroy();
		return false;
		}

	struct sockaddr_in addr;  
	MStdMemZero(&addr,sizeof(addr));

	addr.sin_port=htons(port);

	MUnixSocketNameResolver dns;
	if(dns.Create(host)==true)
		{
		addr.sin_family=dns.AddressType();
		MStdMemCpy(&(addr.sin_addr),dns.Address(0),dns.AddressLength());
		}
	else
		{
		addr.sin_addr.s_addr=inet_addr(host);
		addr.sin_family=AF_INET;
		}

	if(connect(GetSocket(),(struct sockaddr *)&addr,sizeof(addr))<0)
		{
		Destroy();  return false; 
		}

	return true;
	}


////////////////////////////////////////////////////////////////////////////////
bool MUnixSocketClient::Destroy(void)
	{
	MUnixSocket::Destroy();
	ClearObject();
	return true;
	}


//******************************************************************************
//  MUnixSocketServer
//******************************************************************************
void MUnixSocketServer::ClearObject(void)
	{
	mBreakServiceLoop=false;
	}

	
//////////////////////////////////////////////////////////////
MUnixSocketServer::MUnixSocketServer(void)
	{  ClearObject();  }


/////////////////////////////////////////////////////////////
MUnixSocketServer::~MUnixSocketServer(void)
	{  Destroy();  }


/////////////////////////////////////////////////////////////
bool MUnixSocketServer::Create(int port,int listensize,int family,int type,int protocol)
	{
	Destroy();
	
	mBreakServiceLoop=false;
	
	if(MUnixSocket::Create(family,type,protocol)==false)
		{
		Destroy();
		return false;
		}

	
	struct sockaddr_in addr;
	MStdMemZero(&addr,sizeof(addr));
	addr.sin_port=htons(port);
	addr.sin_family=family;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	// bind socket to address
	if(bind(GetSocket(),(struct sockaddr *)&addr,sizeof(addr))!=0 )
		{
		Destroy();
		return false;
		}

	// set listening size
	if(listen(GetSocket(),listensize)!=0)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////////////
bool MUnixSocketServer::Destroy(void)
	{  
	BreakServiceLoop();
	MUnixSocket::Destroy();  // Close Socket
	ClearObject();
	return true;
	}


////////////////////////////////////////////////////////////
bool MUnixSocketServer::OnInitialize(int socket)
	{  return true;  }


////////////////////////////////////////////////////////////
bool MUnixSocketServer::OnAccept(int socket,struct sockaddr_in *sad)
	{  return false;  }


///////////////////////////////////////////////////////////
bool MUnixSocketServer::OnAcceptError(int WSAErrorNo)
	{
	return true;
	}


////////////////////////////////////////////////////////////
bool MUnixSocketServer::StartService(void)  // Does not return
	{
	
	if(this->OnInitialize(GetSocket())==false) {  return false;  }

	while(mBreakServiceLoop==false) // Service Loop
		{
		struct sockaddr_in cliad;
		MStdMemZero(&cliad,sizeof(cliad) );
		int cliadlen=sizeof(cliad);

		// Accept Socket
		/////////////////////////////////////////////
		#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
		SOCKET clisock=accept(GetSocket(),(struct sockaddr*)&cliad
				,&cliadlen);
		if(clisock==INVALID_SOCKET)
			{  this->OnAcceptError(errno); continue; }

		/////////////////////////////////////////////
		#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
		int clisock=accept(GetSocket(),(struct sockaddr*)&cliad
				,(socklen_t *)&cliadlen);
		if(clisock==-1)
			{  this->OnAcceptError(errno); continue; }
		#endif // MSTDLIB_OS_WINDOWS

		if(this->OnAccept(clisock,&cliad)==true) { continue; }

		this->OnClientClose(&cliad);	// If a class overrides OnAccept, it should
										// call OnClientClose

		////////////////////////////////////////////
		#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
		shutdown(clisock,SD_BOTH);
		closesocket(clisock);  // Otherwise close socket

		////////////////////////////////////////////
		#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
		shutdown(clisock,SHUT_RDWR);
		close(clisock);  // Otherwise close socket

		#endif // MSTDLIB_OS_WINDOWS		
		}

	return true;
	}


//******************************************************************************
//  MUnixSocketUDPBroadcast
//******************************************************************************
void MUnixSocketUDPBroadcast::ClearObject(void)
	{
	}
	
////////////////////////////////////////////////////
MUnixSocketUDPBroadcast::MUnixSocketUDPBroadcast(void)
	{  ClearObject();  }


/////////////////////////////////////////////////
MUnixSocketUDPBroadcast::~MUnixSocketUDPBroadcast(void)
	{  Destroy();  }


/////////////////////////////////////////////////
bool MUnixSocketUDPBroadcast::Create(int broadcastport,unsigned long ipaddress)
	{
	Destroy();
	if(mSocket.Create(AF_INET,SOCK_DGRAM)==false)
		{
		Destroy();
		return false;
		}

	int enable=1;
	if(mSocket.SetSocketOption(SOL_SOCKET,SO_BROADCAST,(char *)&enable
			,sizeof(enable))<0)
		{
		Destroy();
		return false;
		}
	
	if(mSockAddress.Create(INADDR_BROADCAST,broadcastport)==false)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////
bool MUnixSocketUDPBroadcast::Destroy(void)
	{
	mSocket.Destroy();
	mSockAddress.Destroy();
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////
int MUnixSocketUDPBroadcast::Send(const char *message)
	{
	int ret=mSocket.SendTo((void *)message,MStdStrLen(message)+1,mSockAddress);
	if(ret<0)
		{
		return ret;
		}

	return ret;
	}


/////////////////////////////////////////////////
int MUnixSocketUDPBroadcast::Send(void *message,int size)
	{
	int ret=mSocket.SendTo(message,size,mSockAddress);
	if(ret<0)
		{
		return ret;
		}

	return ret;
	}


//*****************************************************************************
//  MUnixSocketNameResolver
//*****************************************************************************
void MUnixSocketNameResolver::ClearObject(void)
	{
	mEntry=NULL;
	}


///////////////////////////////////////////////////////////////////
MUnixSocketNameResolver::MUnixSocketNameResolver(void)
	{  ClearObject();  }


///////////////////////////////////////////////////////////////////
MUnixSocketNameResolver::~MUnixSocketNameResolver(void) 
	{ Destroy();  }


///////////////////////////////////////////////////////////////////
bool MUnixSocketNameResolver::Create(const char *name)
	{
	Destroy();
	if((mEntry=gethostbyname(name))==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////
bool MUnixSocketNameResolver::Destroy(void)
	{
	ClearObject();
	return true;
	}


/////////////////////////////////////////////////////
const char *MUnixSocketNameResolver::Name(void)
	{
	return mEntry->h_name;  
	}


///////////////////////////////////////////////////
int MUnixSocketNameResolver::AddressLength(void)
	{
	return mEntry->h_length;
	}


//////////////////////////////////////////////////
const char *MUnixSocketNameResolver::Alias(int index)
	{ 
	return mEntry->h_aliases[index];
	}


///////////////////////////////////////////////////
int MUnixSocketNameResolver::AddressType(void)
	{  
	return mEntry->h_addrtype;
	}


///////////////////////////////////////////////////
const char *MUnixSocketNameResolver::Address(int index)
	{
	return mEntry->h_addr_list[index];
	}


//******************************************************************************
//  MUnixSocketServiceResolver
//******************************************************************************
void MUnixSocketServiceResolver::ClearObject(void)
	{
	mEntry=NULL;
	}


/////////////////////////////////////////////////////
MUnixSocketServiceResolver::MUnixSocketServiceResolver(void)
	{  ClearObject();  }  


/////////////////////////////////////////////////////
MUnixSocketServiceResolver::~MUnixSocketServiceResolver()
	{  Destroy();  }


/////////////////////////////////////////////////////
bool MUnixSocketServiceResolver::Create(int port,const char *proto)
	{
	Destroy();
	if((mEntry=getservbyport(htons(port),proto))==NULL)
		{
		Destroy();
		return false;
		}

	return true;
	}


/////////////////////////////////////////////////////
bool MUnixSocketServiceResolver::Create(const char *service,const char *proto)
	{
	Destroy();
	if((mEntry=getservbyname(service,proto))==NULL)
		{
		Destroy();
		return false;
		}
		
	return true;	
	}


/////////////////////////////////////////////////////
bool MUnixSocketServiceResolver::Destroy(void)
	{
	ClearObject();
	return true;
	}


//////////////////////////////////////////////////////
const char *MUnixSocketServiceResolver::GetName(void) // return Service Name
	{
	return mEntry->s_name;
	}


//////////////////////////////////////////////////////
int MUnixSocketServiceResolver::GetPort(void) // Returns Port
	{
	return mEntry->s_port;
	}


/////////////////////////////////////////////////////
int MUnixSocketServiceResolver::GetPortHostOrder(void)
	{
	return ntohs(mEntry->s_port);
	}


/////////////////////////////////////////////////////
const char * MUnixSocketServiceResolver::GetProtocol(void)
	{
	return mEntry->s_proto;
	}


/////////////////////////////////////////////////////
const char * MUnixSocketServiceResolver::GetAlias(int index)
	{
	return mEntry->s_aliases[index];
	}


//******************************************************************************
//  MUnixSocketSelect
//******************************************************************************
void MUnixSocketSelect::ClearObject(void)
	{
	mReadHandles=NULL;
	mWriteHandles=NULL;
	mErrorHandles=NULL;
	mSocketSet=NULL;
	mSocketCount=0;
	}

///////////////////////////////////////////////////
MUnixSocketSelect::MUnixSocketSelect(void)
	{  ClearObject();  }


///////////////////////////////////////////////////////
MUnixSocketSelect::~MUnixSocketSelect(void)
	{  Destroy();  }


///////////////////////////////////////////////////////
bool MUnixSocketSelect::Create(bool readable
			,bool writeable,bool errors)
	{
	Destroy();

	if(readable==false && writeable==false && errors==false)
		{
		Destroy();
		return false;
		}

	if(readable==true)
		{
		mReadHandles=new(std::nothrow) fd_set;
		if(mReadHandles==NULL)
			{
			Destroy();
			return false;
			}

		FD_ZERO(mReadHandles);
		}

	if(writeable==true)
		{
		mWriteHandles=new(std::nothrow) fd_set; 
		if(mWriteHandles==NULL)
			{
			Destroy();
			return false;
			}

		FD_ZERO(mWriteHandles);
		}

	if(errors==true)
		{
		mErrorHandles=new(std::nothrow) fd_set;
		if(mErrorHandles==NULL)
			{
			Destroy();
			return false;
			}

		FD_ZERO(mErrorHandles);
		}

	// Init elements
	
	return true;
	}


///////////////////////////////////////////////////////
bool MUnixSocketSelect::Destroy(void)
	{
	if(mReadHandles!=NULL)
		{
		delete mReadHandles;
		mReadHandles=NULL;
		}

	if(mWriteHandles!=NULL)
		{
		delete mWriteHandles;
		mWriteHandles=NULL;
		}

	if(mErrorHandles!=NULL)
		{
		delete mErrorHandles;
		mErrorHandles=NULL;
		}

	if(mSocketSet!=NULL)
		{
		delete mSocketSet;
		mSocketSet=NULL;
		}

	ClearObject();
	return true;
	}


////////////////////////////////////////////////////////////////
int MUnixSocketSelect::GetSocketCount(void)
	{
	return mSocketCount;
	}


////////////////////////////////////////////////////////////////
int MUnixSocketSelect::Select(int msdelay)
	{
	
	timeval tv={0,0};
	timeval *tvptr=NULL;
	if(msdelay>=0)
		{
		tvptr=&tv;
		tvptr->tv_usec=msdelay*1000;
		tvptr->tv_sec=0;
		}

	//////////////////////////////////////////
	#if (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	// Hold the maximum socket value
	int maxreadsocket=-1;
	int maxwritesocket=-1;
	int maxerrorsocket=-1;
	#endif // MSTDLIB_OS_LINUX

	//*Set all descriptors
	int i;
	if(mReadHandles!=NULL)
		{
		FD_ZERO(mReadHandles);
		
		for(i=0;i<mSocketCount;++i)
			{
			FD_SET(mSocketSet[i],mReadHandles);

			//////////////////////////////////////////
			#if (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
			if(mSocketSet[i]>maxreadsocket)
				{ maxreadsocket=mSocketSet[i]; }
			#endif // MSTDLIB_OS_LINUX
			}
		}

	if(mWriteHandles!=NULL)
		{
		FD_ZERO(mWriteHandles);
		
		for(i=0;i<mSocketCount;++i)
			{
			FD_SET(mSocketSet[i],mWriteHandles);

			//////////////////////////////////////////
			#if (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
			if(mSocketSet[i]>maxwritesocket)
				{ maxwritesocket=mSocketSet[i]; }
			#endif // MSTDLIB_OS_LINUX
			}
		}
	
	if(mErrorHandles!=NULL)
		{
		FD_ZERO(mErrorHandles);

		for(i=0;i<mSocketCount;++i)
			{
			FD_SET(mSocketSet[i],mErrorHandles);

			//////////////////////////////////////////
			#if (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
			if(mSocketSet[i]>maxerrorsocket)
				{ maxerrorsocket=mSocketSet[i]; }
			#endif // MSTDLIB_OS_LINUX
			}
		}

	// Find max of all the sockets
	//////////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET maxsocket=0;

	//////////////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )

	int maxsocket=maxreadsocket;
	if(maxwritesocket>maxsocket) { maxsocket=maxwritesocket; }
	if(maxerrorsocket>maxsocket) { maxsocket=maxerrorsocket; }


	if(maxsocket<0)
		{
		return -1;
		}

	#endif // MSTDLIB_OS_LINUX

	// Call Select
	int ret=select(maxsocket+1,mReadHandles,mWriteHandles,mErrorHandles,tvptr);
	if(ret<0)
		{
		return -1;
		}

	return ret;
	}


////////////////////////////////////////////////////////////////
#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
SOCKET MUnixSocketSelect::GetSocket(int index)
	{
	return mSocketSet[index];
	}


////////////////////////////////////////////////////////////////
bool MUnixSocketSelect::AddSocket(SOCKET sock)
	{

	// Check if socket members are in array
	int i;
	for(i=0;i<mSocketCount;++i)
		{
		if(mSocketSet[i]==sock) { return true; }
		}

	//= Socket is not part of set
	
	// Increase the array
	mSocketCount = mSocketCount + 1;
	if(mSocketSet==NULL)
		{
		mSocketSet=new(std::nothrow) SOCKET[1];
		if(mSocketSet==NULL)
			{
			mSocketCount=0;
			return false;
			}
		}
	else
		{
		SOCKET *newarray;
		newarray=new(std::nothrow) SOCKET[mSocketCount];
		if(newarray==NULL)
			{
			mSocketCount=mSocketCount-1;
			return false;
			}

		// Copy old socket 
		int i;
		for(i=0;i<mSocketCount-1;++i)
			{
			newarray[i] = mSocketSet[i];
			}

		delete[] mSocketSet;
		
		// point to memory block
		mSocketSet=newarray;
		}

	// Add socket to end of array
	mSocketSet[mSocketCount-1]=sock;

	return true;
	}


////////////////////////////////////////////////////////////////
bool MUnixSocketSelect::DelSocket(SOCKET sock)
	{

	// Check if socket members are in array
	int index;
	for(index=0;index<mSocketCount;++index)
		{
		if(mSocketSet[index]==sock) { break; }
		}

	if(index>=mSocketCount)
		{
		return false;
		}

	//= We have the index of the socket
	SOCKET tmp=mSocketSet[index];
	mSocketSet[index]=mSocketSet[mSocketCount-1];
	mSocketSet[mSocketCount-1]=tmp;

	// Reduce the # of elements
	mSocketCount=mSocketCount-1;

	return true;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::IsReadable(SOCKET sock)
	{
	if(mReadHandles==NULL) { return false; }
	if(FD_ISSET(sock,mReadHandles)) { return true; }
	return false;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::IsWritable(SOCKET sock)
	{
	if(mWriteHandles==NULL) { return false; }
	if(FD_ISSET(sock,mWriteHandles)) { return true; }
	return false;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::HasError(SOCKET sock)
	{
	if(mErrorHandles==NULL) { return false; }
	if(FD_ISSET(sock,mErrorHandles)) { return true; }
	return false;
	}

////////////////////////////////////////////////
#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
int MUnixSocketSelect::GetSocket(int index)
	{
	return mSocketSet[index];
	}


////////////////////////////////////////////////////////////////
bool MUnixSocketSelect::AddSocket(int sock)
	{

	// Check if socket members are in array
	int i;
	for(i=0;i<mSocketCount;++i)
		{
		if(mSocketSet[i]==sock) { return true; }
		}

	//= Socket is not part of set
	
	// Increase the array
	mSocketCount = mSocketCount + 1;
	if(mSocketSet==NULL)
		{
		mSocketSet=new(std::nothrow) int[mSocketCount];
		if(mSocketSet==NULL)
			{
			mSocketCount=0;
			return false;
			}
		}
	else
		{
		int *newarray;
		newarray=new(std::nothrow) int[mSocketCount];
		if(newarray==NULL)
			{
			mSocketCount=mSocketCount-1;
			return false;
			}
		
		for(i=0;i<mSocketCount-1;++i)
			{
			newarray[i] = mSocketSet[i];
			}

		delete[] mSocketSet;

		// point to memory block
		mSocketSet=newarray;
		}

	// Add socket to end of array
	mSocketSet[mSocketCount-1]=sock;

	return true;
	}


////////////////////////////////////////////////////////////////
bool MUnixSocketSelect::DelSocket(int sock)
	{

	// Check if socket members are in array
	int index;
	for(index=0;index<mSocketCount;++index)
		{
		if(mSocketSet[index]==sock) { break; }
		}

	if(index>=mSocketCount)
		{
		return false;
		}

	//= We have the index of the socket
	int tmp=mSocketSet[index];
	mSocketSet[index]=mSocketSet[mSocketCount-1];
	mSocketSet[mSocketCount-1]=tmp;

	// Reduce the # of elements
	mSocketCount=mSocketCount-1;

	return true;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::IsReadable(int sock)
	{
	if(mReadHandles==NULL) { return false; }
	if(FD_ISSET(sock,mReadHandles)) { return true; }
	return false;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::IsWritable(int sock)
	{
	if(mWriteHandles==NULL) { return false; }
	if(FD_ISSET(sock,mWriteHandles)) { return true; }
	return false;
	}


////////////////////////////////////////////////////////
bool MUnixSocketSelect::HasError(int sock)
	{
	if(mErrorHandles==NULL) { return false; }
	if(FD_ISSET(sock,mErrorHandles)) { return true; }
	return false;
	}

#endif // MSTDLIB_OS_WINDOWS


