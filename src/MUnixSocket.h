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
#ifndef MUnixSocket_h
#define MUnixSocket_h

//////////////////////////////////////////////////////
#include "MStdLib.h"

///////////////////////////////////////
#if (defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) )
#include <winsock2.h>

///////////////////////////////////////
#elif defined(MSTDLIB_OS_MINGW)	
#include <WinSock.h>

//////////////////////////////////////
#elif ( defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/////////////////////////////////////
#elif defined(MSTDLIB_OS_MOSYNC)
#error This Module is not implemented on MoSync
#endif  // MSTDLIB_OS_WINDOWS

/////////////////////////////////////////////////////////////////////////////
// Byte Ordering is always in Host Ordering
class MUnixSocketSockAddr
	{
	struct sockaddr_in mSockAddr;

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocketSockAddr(void);
	MUnixSocketSockAddr(struct sockaddr_in &sad);
	~MUnixSocketSockAddr(void);

		// ipaddress= INADDR_ANY, INADDR_BROADCAST
	bool Create(unsigned long ipaddress,int port,int family=AF_INET);
	bool Create(const char *ipaddress,int port,int family=AF_INET);
	bool Create(struct sockaddr_in &sad);
	bool Destroy(void);
	int GetPort(void);    				// =Port in host order form
	unsigned long GetAddress(void); 	// = address in host order form
	int GetFamily(void);
	struct sockaddr_in &Get(void);
	};


/////////////////////////////////////////////////////////////////////////////
class MUnixSocket
	{
	/////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET mSocket;

	/////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int mSocket;

	#endif // MSTDLIB_OS_WINDOWS

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocket(void);

	//sockfamily = AF_INET    ,socktype=SOCK_STREAM,SOCK_DGRAM
	MUnixSocket(int sockfamily,int socktype,int protocol=0);
	MUnixSocket(int tsocket);
	~MUnixSocket(void);

	//sockfamily = AF_INET    ,socktype=SOCK_STREAM,SOCK_DGRAM
	bool Create(int sockfamily,int socktype,int protocol=0);
	bool Create(int tsocket);
	bool Destroy(void);
	int Recv(void *Buffer,int len,int flag=0);  // return <0, on failure, # of bytes read
	int RecvFrom(void *buffer,int len,MUnixSocketSockAddr &sad,int flag=0);

					// flag - MSG_OOB, MSG_DONTROUTE
	int Send(void *Buffer,int len,int flag=0); // return <0, on failure, #of bytes written
	int SendTo(void *buffer,int len,MUnixSocketSockAddr &sad,int flag=0);

	////////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET GetSocket(void);

	///////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int GetSocket(void);

	#endif // MSTDLIB_OS_WINDOWS

	int SetSocketOption(int level,int optname,const char *optvalue,int optlen);
	bool Listen(int backlog=10);
	bool Bind(MUnixSocketSockAddr &sad);  // Bind local address to socket

	bool IsReadable(int sec=-1,int usec=-1);
	bool IsWritable(int sec=-1,int usec=-1);

	bool IOCtlSocket(long cmd,unsigned long *retvalue);
	bool Accept(MUnixSocket &socket,MUnixSocketSockAddr &sockaddrs);
	bool ShutDown(void);
	};


//////////////////////////////////////////////////////////////////////////////
class MUnixSocketIP
	{
	unsigned long mIPAddress;  char mIPBuffer[4*4+1];

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocketIP(void);
	MUnixSocketIP(unsigned long ipaddress); // Must be in network order
	MUnixSocketIP(const char *ipaddress);
	~MUnixSocketIP(void);
	bool Create(const char *ipaddress); // ie "148.4.41.1"
	bool Create(unsigned long ipaddress);
	bool Destroy(void);
	const char *ToString(void);
	unsigned long GetAddress(void);			// Returns Network ordered address
	};


//////////////////////////////////////////////////////////////////////////////
class MUnixSocketClient:public MUnixSocket
	{
	////////////////////////////////////////////////////
	void ClearObject(void);
	
	// Hide Base Class Functions
	bool Create(int sockfamily,int socktype,int protocol=0);
	bool Create(int tsocket);
	
	////////////////////////////////////////////////////
	public:
	MUnixSocketClient(void);
	MUnixSocketClient(const char *host,int port,int family=AF_INET
		,int type=SOCK_STREAM,int protocol=0);
	~MUnixSocketClient(void);
	bool Create(const char *host,int port,int family=AF_INET
		,int type=SOCK_STREAM,int protocol=0);
	bool Destroy(void);
	};


//////////////////////////////////////////////////////////////////////////////
class MUnixSocketServer:public MUnixSocket
	{
	volatile bool mBreakServiceLoop;  //volatile for thread access

	////////////////////////////////////////////////////
	void ClearObject(void);

	//////////////////////////////////////////////////////////////
	protected:
	virtual bool OnInitialize(int socket);
	virtual bool OnAccept(int socket,struct sockaddr_in *sockaddr);
	virtual bool OnAcceptError(int WSAErrorNo);
	virtual void OnClientClose(struct sockaddr_in *sockaddr) { }

	//////////////////////////////////////////////////////////////
	void BreakServiceLoop(void) { mBreakServiceLoop=true; }

	// Hide Base Class Functions
	bool Create(int sockfamily,int socktype,int protocol);
	bool Create(int tsocket);

	//////////////////////////////////////////////////////////////
	public:
	MUnixSocketServer(void);
	~MUnixSocketServer(void);
	bool Create(int port,int listensize,int family=AF_INET
			,int type=SOCK_STREAM,int protocol=0);
	bool Destroy(void);
	bool StartService(void);  // Does not return accept if someone calls Destroy()
	};


//////////////////////////////////////////////////////////////////
class MUnixSocketUDPBroadcast
	{
	MUnixSocket mSocket;
	MUnixSocketSockAddr mSockAddress;

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocketUDPBroadcast(void);
	~MUnixSocketUDPBroadcast(void);
	bool Create(int broadcastport,unsigned long ipaddress=INADDR_BROADCAST);
	bool Destroy(void);
	int Send(const char *message);  //=-1 on fail
	int Send(void *message,int size);//=-1 on fail
	};


/////////////////////////////////////////////////////////////////////////////
class MUnixSocketNameResolver
	{
	struct hostent *mEntry;

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocketNameResolver(void);  ~MUnixSocketNameResolver(void);
	bool Create(const char *name);  // Perform a Search, =true on success
	bool Destroy(void);
	const char *Name(void);
	int AddressLength(void);
	const char *Alias(int index);  // index={0...};  return is NULL 
	const char *Address(int index); // index={0...};  return is NULL //
	int AddressType(void);	
	};


////////////////////////////////////////////////////////////////////////////
class MUnixSocketServiceResolver
	{
	struct servent *mEntry;

	////////////////////////////////////////////////////
	void ClearObject(void);

	////////////////////////////////////////////////////
	public:
	MUnixSocketServiceResolver(void);
	~MUnixSocketServiceResolver(void);
	bool Create(int port,const char *proto="tcp");
	bool Create(const char *service,const char *proto="tcp");
	bool Destroy(void);
	const char *GetName(void); 		// return Service Name
	int GetPort(void);				// Returns Port in NetWork Order
	int GetPortHostOrder(void);
	const char * GetProtocol(void);
	const char * GetAlias(int index);
	};


///////////////////////////////////////////////////////////////////////////
class MUnixSocketSelect
	{
	fd_set *mReadHandles;
	fd_set *mWriteHandles;
	fd_set *mErrorHandles;

	///////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET *mSocketSet;			// List of Sockets for watching

	///////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int *mSocketSet;			// List of Sockets for watching

	#endif // MSTDLIB_OS_WINDOWS
	
	int mSocketCount;			// # of items in socket

	///////////////////////////////////////////////////
	void ClearObject(void);

	///////////////////////////////////////////////////
	public:
	MUnixSocketSelect(void);
	~MUnixSocketSelect(void);
	bool Create(bool readable=true,bool writeable=false,bool errors=false);
	bool Destroy(void);
	int GetSocketCount(void);
	int Select(int msdelay=-1);				//= # of sockets availible for read

	///////////////////////////////////
	#if ( defined(MSTDLIB_OS_WINDOWS) || defined(MSTDLIB_OS_WINDOWSOLD) || defined(MSTDLIB_OS_MINGW) )
	SOCKET GetSocket(int index);
	bool AddSocket(SOCKET sock);
	bool DelSocket(SOCKET sock);
	bool IsReadable(SOCKET sock);
	bool IsWritable(SOCKET sock);
	bool HasError(SOCKET sock);

	///////////////////////////////////
	#elif (defined(MSTDLIB_OS_LINUX) || defined(MSTDLIB_OS_OTHER) )
	int GetSocket(int index);
	bool AddSocket(int sock);
	bool DelSocket(int sock);
	bool IsReadable(int sock);
	bool IsWritable(int sock);
	bool HasError(int sock);

	#endif // MSTDLIB_OS_WINDOWS
	};

#endif // MUnixSocket_h

