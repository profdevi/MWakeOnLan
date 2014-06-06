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


//v1.0 copyright Comine.com 20120122U0030
#include "MStdLib.h"
#include "MConfigFile.h"
#include "MUnixSocket.h"
#include "MStringSplitter.h"
#include "MString.h"
#include "MWakeOnLanMgr.h"


//******************************************************
//**  MWakeOnLanMgr class
//******************************************************
void MWakeOnLanMgr::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MWakeOnLanMgr::MWakeOnLanMgr(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MWakeOnLanMgr::~MWakeOnLanMgr(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MWakeOnLanMgr::Create(const char *configfile)
	{
	Destroy();

	if(mConfigFile.Create()==false)
		{
		Destroy();
		return false;
		}

	if(configfile!=NULL && *configfile!=0)
		{
		if(mConfigFile.ReadFile(configfile)==false )
			{
			// Continue anyway
			}
		}

	return true;
	}


////////////////////////////////////////////////
bool MWakeOnLanMgr::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MWakeOnLanMgr::Wake(const char *machine,MString &targetether)
	{
	// Fist Check if machine exists
	const char *value=mConfigFile.Get(machine);
	if(value==NULL || *value==0)
		{
		value=machine;
		}

	// Try Splitting up the value
	MStringSplitter splitter;
	if(splitter.Create(value,"-:")==false || splitter.GetCount()!=6 )
		{
		return false;
		}

	unsigned char etheraddress[6];
	int i;
	for(i=0;i<6;++i)
		{
		etheraddress[i] = (unsigned char) MStdHToI(splitter.Get(i) );
		}

	// Build up ether net
	char outputether[100];
	MStdSPrintf(outputether,sizeof(outputether)-1
			,"%02x:%02x:%02x:%02x:%02x:%02x"
			,(int)etheraddress[0]
			,(int)etheraddress[1]
			,(int)etheraddress[2]
			,(int)etheraddress[3]
			,(int)etheraddress[4]
			,(int)etheraddress[5] );

	MStdStrToUpper(outputether);
	targetether.Create(outputether);

	if(Wake(etheraddress)==false)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////
bool MWakeOnLanMgr::Wake(const unsigned char ether[6])
	{
	const int broadcastpost=40000;
	MUnixSocketUDPBroadcast sender;
	if(sender.Create(40000)==false)
		{
		return false;
		}

	
	const int packetsize=6+16*6;
	unsigned char packet[packetsize];

	int i;
	for(i=0;i<6;++i)
		{
		packet[i]=0xFF;
		}

	for(i=6;i<packetsize;++i)
		{
		packet[i]=ether[i%6];
		}
	
	if(sender.Send(packet,packetsize)==false)
		{
		return false;
		}

	return true;
	}


