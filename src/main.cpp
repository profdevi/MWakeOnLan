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


//v1.3 copyright Comine.com 20140606F0011
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MWakeOnLanMgr.h"
#include "MFileOps.h"
#include "MConfigFile.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="MWakeOnLan";	// Used in Help
static const char *GApplicationVersion="1.3";	// Used in Help

////////////////////////////////////////////////////
static void GDisplayHelp(void);
static bool GWakeOnLan(const unsigned char etheraddress[]);

////////////////////////////////////////////////////
int main(int argn,const char *argv[])
	{
	MCommandArg args(argn,argv);

	///////////////////////////////////////////////
	if(args.GetArgCount()<2)
		{
		GDisplayHelp();
		return 0;
		}

	// Check for configuration file
	MFileOps fileops(true);
	MString configfilename;
	fileops.GetAppConfigFile(configfilename);


	if(args.IsArgument("-l")==true)
		{
		MConfigFile configfile;
		if(configfile.Create()==false)
			{
			MStdPrintf("**System Failed\n");
			return 1;
			}

		if(configfile.ReadFile(configfilename.Get())==false)
			{
			MStdPrintf("**No Config File contents to list \n");
			return 2;
			}

		void *handle=NULL;
		while(configfile.Read(handle)==true)
			{
			const char *name=configfile.ReadName(handle);
			const char *value=configfile.ReadValue(handle);

			MStdPrintf("%-20s : %s\n",name,value);
			}

		return 0;
		}

	MWakeOnLanMgr wakonlanmgr;
	if(wakonlanmgr.Create(configfilename.Get())==false)
		{
		MStdPrintf("**Failed startup\n");
		return 1;
		}

	const int argcount=args.GetArgCount();

	int i;
	for(i=1;i<argcount;++i)
		{
		const char *machinename=args.GetArg(i);
		MString ether;
		if(wakonlanmgr.Wake(machinename,ether)==false)
			{
			printf("%-20s : Failed \n",machinename);
			continue;
			}

		MStdPrintf("%-20s : %-15s - Sent Packet\n",machinename,ether.Get() );
		}

	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s <etheraddress|computer name>+ [-l|-?]\n"
				"           v%s copyright Comine.com\n"
				"\n"
				"   Program will send wake on lan packet to machine on ethernet \n"
				"   using the ethernet address.  A configuratin file MWakeOnLan.ini\n"
				"   can be used to hold name/ethernet values for easy command line \n"
				"   execution.\n"
				"\n"
				"   The configuration file consists of name=etheraddress pairs.  \n"
				"   A # marker is used for comment lines\n"
				"\n"
				"   Flags:\n"
				"            -l | List All Name/Value Pairs\n"
				"\n"
				"   Example 1: \n"
				"\n"
				"        C:>  MWakeOnLan  00:2E:27:4B:20:34\n"
				"\n"
				"   Example 2: \n"
				"\n"
				"        C:>  MWakeOnLan server1 server2\n"
				"\n"
				,GApplicationName,GApplicationVersion);
	}




