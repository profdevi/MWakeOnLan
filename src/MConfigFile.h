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


//v3.13 copyright Comine.com 20140421M1236
#ifndef MConfigFile_h
#define MConfigFile_h

/*
	Sample Config File

	#
	#	Pound Markers are comments
	#

	Name=Value
	Hello=World	

*/

#include <stdio.h>

//*******************************************************
//*  Simple File Configuration Reader
//*******************************************************
class MConfigFile
	{
	struct GStringData *mList;		// List of Name/Value Pairs

	////////////////////////////////////////////////////////////
	void ClearObject(void);
	struct GStringData *Find(const char *name);
	
	////////////////////////////////////////////////////////////
	public:
	MConfigFile(void);
	~MConfigFile(void);
	bool Create(void);
	bool Destroy(void);
	bool Clear(void);									// Clear Name Value pairs
	bool ReadFile(const char *filename);				// Read from a config file
	bool ReadFileExeHome(const char *filename);			// Reads Config file from exe home
	bool WriteFile(const char *filename);				// Write Values to a file
	bool WriteFileExeHome(const char *filename);		// Write Config File to exe home
	bool Set(const char *name,const char *value=NULL);	// Set Name/Value Pair
	const char *Get(const char *name);					//="", if name is empty
	bool Sort(void);									// Sort All the items
	
	// Reader Code 
	bool Read(void *&handle);
	const char *ReadName(void *handle);
	const char *ReadValue(void *handle);

	// Reads/Writes/Deletes INI Files
	bool ReadIniFile(void);					// Reads ExeFileName.ini from exe home or in c:/etc
	bool WriteIniFile(void);				// Writes ExeFileName.ini File
	bool DeleteIniFile(void);				// Deletes INI File
	};

#endif // MConfigFile_h
