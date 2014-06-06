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
#include "MStdLib.h"
#include "MBuffer.h"
#include "MString.h"
#include "MFile.h"
#include "MPathNameList.h"
#include "MFileOps.h"
#include "MConfigFile.h"


//************************************************************
//** Module Elements
//************************************************************
static const int GMaxLineLength=512;

///////////////////////////////////////////////////////////
struct GStringData
	{
	char *Name;
	char *Value;
	GStringData *Next;
	};


///////////////////////////////////////////////////////////
static GStringData GEndMarker={0,0,0};


///////////////////////////////////////////////////////////
static bool GIsAlphaNumeric(char ch)
	{
	if(ch>='a' && ch<='z') { return true; }
	if(ch>='A' && ch<='Z') { return true; }
	if(ch>='0' && ch<='9') { return true; }
	return false;
	}


///////////////////////////////////////////////////////////
static bool GIsSpace(char ch)
	{
	if(ch==' ' || ch=='\t') { return true; }
	return false;
	}


///////////////////////////////////////////////////////////
static bool GIsNameValuePair(const char *name)
	{
	//Check if name contains a # marker
	int i;
	for(i=0;name[i]!=0;++i)
		{
		if(GIsSpace(name[i])==true) { continue; }
		if(name[i]=='#') { return false; }
		break;
		}

	for(i=0;name[i]!=0;++i)
		{
		if(name[i]=='=') { return true; }
		}

	return false;
	}


/////////////////////////////////////////////////////////////
static int GFindPosition(const char *name,char ch)
	{
	for(int i=0;name[i]!=0;++i)
		{
		if(name[i]==ch) { return i; }
		}

	return -1;
	}


/////////////////////////////////////////////////////////////
static bool GReplaceExeForIni(MBuffer &name)
	{
	MStdStrToUpper(name.GetBuffer() );

	int length=name.GetStringLength();	
	if(length<=4)
		{
		return false;
		}

	if(MStdStrICmp(name.GetBuffer()+length-4,".EXE")!=0)
		{
		return false;
		}

	MStdStrCpy(name.GetBuffer()+length-4,5,".ini");

	return true;
	}


/////////////////////////////////////////////////////
static bool GGetIniFilePath(MBuffer &inifilepath)
	{
	MFileOps fileops(true);
	MString exepath;
	if(fileops.GetAppPath(exepath)==false)
		{
		return false;
		}

	MPathNameList path;
	if(path.Create(exepath.Get() )==false)
		{
		return false;
		}

	MBuffer ininame;
	if(ininame.Create(2000)==false)
		{
		return false;
		}

	if(ininame.SetString(path.GetTop())==false)
		{
		return false;
		}

	if(GReplaceExeForIni(ininame)==false)
		{
		return false;
		}

	// Pop the exe name
	if(path.Pop()==false || path.Push(ininame.GetBuffer())==false)
		{
		return false;
		}

	if(path.GetFullPath(inifilepath)==false)
		{
		return false;
		}
	
	return true;
	}


/////////////////////////////////////////////////////
static bool GGetMasterIniFilePath(MBuffer &inifilepath)
	{
	MFileOps fileops(true);
	MString exepath;
	if(fileops.GetAppPath(exepath)==false)
		{
		return false;
		}

	MPathNameList path;
	if(path.Create(exepath.Get())==false)
		{
		return false;
		}

	MBuffer ininame;
	if(ininame.Create(2000)==false)
		{
		return false;
		}

	if(ininame.SetString(path.GetTop())==false)
		{
		return false;
		}

	if(GReplaceExeForIni(ininame)==false)
		{
		return false;
		}

	// Replace path as C:/etc
	if(path.Create("c:/etc")==false)
		{
		return false;
		}

	if(path.Push(ininame.GetBuffer())==false)
		{
		return false;
		}

	if(path.GetFullPath(inifilepath)==false)
		{
		return false;
		}
	
	return true;
	}



//************************************************************
//** MConfigFile Class
//************************************************************
void MConfigFile::ClearObject(void)
	{
	mList=NULL;
	}


//////////////////////////////////////////////
struct GStringData *MConfigFile::Find(const char *name)
	{
	if(name==NULL || *name==0)
		{
		return NULL;
		}

	GStringData *p;
	for(p=mList;p!=NULL;p=p->Next)
		{
		if(MStdStrICmp(name,p->Name)==0) { return p; }
		}
	
	return NULL;
	}


////////////////////////////////////////////
MConfigFile::MConfigFile(void)
	{  ClearObject();  }


/////////////////////////////////////////////
MConfigFile::~MConfigFile(void)
	{ Destroy();  }


////////////////////////////////////////////
bool MConfigFile::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////
bool MConfigFile::Destroy(void)
	{
	Clear();
	ClearObject();
	return true;
	}


////////////////////////////////////////////
bool MConfigFile::Clear(void)						// Clear Name Value pairs
	{
	// Dont Check if object is initialized
	GStringData *current;
	for(current=mList;current!=NULL;)
		{
		delete [] current->Name;

		delete [] current->Value;

		GStringData *tmp=current;
		current = current->Next;

		delete tmp;
		}

	mList=NULL;
	return true;
	}


////////////////////////////////////////////
bool MConfigFile::ReadFile(const char *filename)
	{
	MFileInput fileinput;
	if(fileinput.Create(filename)==false)
		{
		return false;
		}

	// Clear the current list
	if(Clear()==false) { return false; }

	MBuffer buffer,namebuffer,valuebuffer;
	if(buffer.Create(GMaxLineLength)==false	
			|| namebuffer.Create(GMaxLineLength)==false
			|| valuebuffer.Create(GMaxLineLength)==false)
		{
		return false;
		}

	while(fileinput.ReadLine(buffer.GetBuffer(),buffer.GetSize())==true)
		{
		if(GIsNameValuePair(buffer.GetBuffer())==false) { continue; }
		
		//=We have a name/value
		int equalindex=GFindPosition(buffer.GetBuffer(),'=');
		if(equalindex<0)
			{
			return false;
			}

		buffer[equalindex]=0;
		namebuffer.SetString(buffer.GetBuffer());
		valuebuffer.SetString(buffer.GetBuffer()+equalindex+1);
		MStdStrTrim(namebuffer.GetBuffer());
		MStdStrTrimLeft(valuebuffer.GetBuffer());
		MStdStrTrimRight(valuebuffer.GetBuffer());

		// Add Name/Value Pair to list
		if(Set(namebuffer.GetBuffer(),valuebuffer.GetBuffer())==false)
			{
			return false;
			}
		}

	fileinput.Destroy();
	return true;
	}


/////////////////////////////////////////////
bool MConfigFile::ReadFileExeHome(const char *filename)	// Reads Config file from exe home
	{
	MFileOps fileops(true);
	MString exepath;
	if(fileops.GetAppPath(exepath)==false)
		{
		return false;
		}

	MPathNameList path;
	if(path.Create(exepath.Get() )==false)
		{
		return false;
		}

	// Pop the exe name
	if(path.Pop()==false || path.Push(filename)==false)
		{
		return false;
		}

	MBuffer fullpath;
	if(path.GetFullPath(fullpath)==false)
		{
		return false;
		}
	
	return ReadFile(fullpath.GetBuffer() );
	}


////////////////////////////////////////////
bool MConfigFile::WriteFile(const char *filename)
	{
	MFileOutput fileoutput;
	if(fileoutput.Create(filename)==false)
		{
		return false;
		}

	GStringData *p;
	for(p=mList;p!=NULL;p=p->Next)
		{
		if(fileoutput.WriteChars(p->Name)==false
				|| fileoutput.WriteChars("=")==false
				|| fileoutput.WriteChars(p->Value)==false
				|| fileoutput.WriteChars("\r\n")==false )
			{
			return false;
			}
		}

	fileoutput.Destroy();

	return true;
	}


////////////////////////////////////////////
bool MConfigFile::WriteFileExeHome(const char *filename)	// Write File to exe home
	{
	MFileOps fileops(true);
	MString exepath;
	if(fileops.GetAppPath(exepath)==false)
		{	
		return false;
		}

	MPathNameList path;
	if(path.Create(exepath.Get() )==false)
		{
		return false;
		}

	// Pop the exe name
	if(path.Pop()==false || path.Push(filename)==false)
		{
		return false;
		}

	MBuffer fullpath;
	if(path.GetFullPath(fullpath)==false)
		{
		return false;
		}
		
	return WriteFile(fullpath.GetBuffer());
	}


////////////////////////////////////////////
bool MConfigFile::Set(const char *name,const char *value)
	{
	GStringData *search=Find(name);
	if(search!=NULL)
		{
		//= we have found the value
		// Allocate space for fresh string
		const int valuelen=MStdStrLen(value)+1;
		char *newvalue=new(std::nothrow) char[valuelen];
		if(newvalue==NULL)
			{
			return false;
			}

		// Copy String
		MStdStrCpy(newvalue,valuelen,value);

		// Trim Both Sides
		MStdStrTrimLeft(newvalue);
		MStdStrTrimRight(newvalue);

		//=We now have fresh string
		
		// Free old value
		delete[] search->Value;

		search->Value=newvalue;
		return true;
		}

	//=Name/Value is not in list
	GStringData *newnode=new(std::nothrow) GStringData;
	if(newnode==NULL)
		{
		return false;
		}

	// Alloc Space for name
	const int namelen=MStdStrLen(name)+1;
	char *newname=new(std::nothrow) char[namelen];
	if(newname==NULL)
		{
		delete newnode;
		return false;
		}

	// Copy String
	MStdStrCpy(newname,namelen,name);

	// Alloc Space for value
	char *newvalue=NULL;
	if(value==NULL) { value="";  }

	const int valuelen=MStdStrLen(value)+1;
	newvalue=new(std::nothrow) char[valuelen];
	if(newvalue==NULL)
		{
		delete newnode;

		delete[] newname;
		return false;
		}

	// Copy Value
	MStdStrCpy(newvalue,valuelen,value);

	//=Name/Value strings have now been allocated

	// Trim Name and Value
	MStdStrTrim(newname);
	MStdStrTrimLeft(newvalue);
	MStdStrTrimRight(newvalue);
	
	// Add Fresh Node and data
	newnode->Next = mList;
	newnode->Name = newname;
	newnode->Value = newvalue;
	mList = newnode;

	return true;
	}


////////////////////////////////////////////
const char *MConfigFile::Get(const char *item)
	{
	GStringData *search=Find(item);
	if(search==NULL){  return "";  }

	return search->Value;
	}


/////////////////////////////////////////////
bool MConfigFile::Sort(void)
	{
	if(mList==NULL || mList->Next==NULL)
		{ return true; }

	GStringData *px;
	for(px=mList;px->Next!=NULL;px=px->Next)
		{
		GStringData *py;
		for(py=mList->Next;py!=NULL;py=py->Next)
			{
			if(MStdStrICmp(px->Name,py->Name)>0)
				{
				char *tmp=px->Name;
				px->Name=py->Name;
				py->Name=tmp;

				tmp=px->Value;
				px->Value=py->Value;
				py->Value=tmp;
				}
			}
		}
	
	return true;
	}


////////////////////////////////////////////////////////////
bool MConfigFile::Read(void *&handle)
	{
	if(handle==&GEndMarker)
		{
		//=we have reached the last item
		return false;
		}
	else if(handle==NULL)
		{
		if(mList==NULL)
			{
			handle=&GEndMarker;
			return false;
			}

		handle=mList;
		return true;
		}

	// Now go to last item
	handle = ((GStringData *)handle)->Next;
	if(handle==NULL) { handle=&GEndMarker;  return false; }

	return true;
	}


////////////////////////////////////////////////////////////
const char *MConfigFile::ReadName(void *handle)
	{
	return ((GStringData *)handle)->Name;
	}


////////////////////////////////////////////////////////////
const char *MConfigFile::ReadValue(void *handle)
	{
	return ((GStringData *)handle)->Value;
	}


////////////////////////////////////////////
bool MConfigFile::ReadIniFile(void)
	{
	MBuffer inifilepath;
	if(GGetIniFilePath(inifilepath)==false)
		{
		return false;
		}

	// Check if file exists
	MFileOps fileops;
	if(fileops.Create()==false)
		{
		return false;
		}

	if(fileops.Exists(inifilepath.GetBuffer())==true)
		{
		//=Found INI File in exe home directory
		return ReadFile(inifilepath.GetBuffer());
		}

	//=Now Check for master file int c:/etc
	if(GGetMasterIniFilePath(inifilepath)==false)
		{
		return false;
		}

	//=We Have found the master file
	if(fileops.Exists(inifilepath.GetBuffer())==true)
		{
		//=Found INI File in exe home directory
		return ReadFile(inifilepath.GetBuffer());
		}
	
	return false;
	}


/////////////////////////////////////////////
bool MConfigFile::WriteIniFile(void)
	{
	MBuffer inifilepath;
	if(GGetIniFilePath(inifilepath)==false)
		{
		return false;
		}

	return WriteFile(inifilepath.GetBuffer());
	}


///////////////////////////////////////////////////////////
bool MConfigFile::DeleteIniFile(void)							// Deletes INI File
	{
	MBuffer inifilepath;
	if(GGetIniFilePath(inifilepath)==false)
		{
		return false;
		}

	MStdFileRemove(inifilepath.GetBuffer());	

	return true;
	}

