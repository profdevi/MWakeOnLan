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
#ifndef MWakeOnLanMgr_h
#define MWakeOnLanMgr_h

//******************************************
// Config File will be 
//
//   # Comment lines
//   air=34:12:EA:45:23:12
//   water=14:12:EA:45:23:12

///////////////////////////////////////////////////
#include "MStdLib.h"
#include "MConfigFile.h"
#include "MString.h"

//******************************************************
//**  MWakeOnLanMgr class
//******************************************************
class MWakeOnLanMgr
	{
	////////////////////////////////////////////////
	
	////////////////////////////////////////////////
	void ClearObject(void);
	MConfigFile mConfigFile;
	
	////////////////////////////////////////////////
	public:
	MWakeOnLanMgr(void);
	~MWakeOnLanMgr(void);
	bool Create(const char *configfile=NULL);
	bool Destroy(void);
	bool Wake(const char *machine,MString &targetether);
	bool Wake(const unsigned char ether[6]);
	};

#endif // MWakeOnLanMgr_h
