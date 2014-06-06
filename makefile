#v1.0 copyright Comine.com 20121217M0013
#
#	make file
#

####################################
CODEID=1754
PROGNAME=MWakeOnLan.exe

####################################
all:
	@mbanner "usage: make [update|test|clean|cleanall|buildbin] "
	@mecho -n
	@mecho "      check    : Builds and places excutable into bin " -n
	@mecho "      build    : Builds and places excutable into bin " -n
	@mecho "      update   : Updated Source Code from CodeMgr" -n
	@mecho "      test     : Builds a cleaned src code" -n
	@mecho "      clean    : Removes Test Build Directory" -n
	@mecho "      cleanall : Removes all unnecessary code" -n


####################################
check:
	cl
	@mecho "Check is ok.  Continue with build "


####################################
update:
	-mkdir src
	(cd src; rm -rf * ; codedump . $(CODEID) ; codewipe -all -w *.h *.cpp )
	(cd src; rm -f MMemTrack.* )
	(cd src; rm -f MModuleInfo.* )
	(cd src; rm -f codecheck )
	(cd src; HeadIns ../License.txt *.h *.cpp)
	(cd src; mclean -f *.h *.cpp )
	mbanner "Updated Files with new source code"


####################################
test: update
	cl
	(mkdir testbuild)
	(cd testbuild; rm -rf * ; cp -R ../src/* . ; nmake -f makefile.windows )
	(testbuild/$(PROGNAME) )


####################################
build: test
	-rm -fr bin
	mkdir bin
	(cp testbuild/$(PROGNAME) bin/$(PROGNAME) )
	(testbuild/$(PROGNAME) ) > Version.txt
	md5sum bin/$(PROGNAME) > bin/$(PROGNAME).md5


###################################
clean:
	(rm -rf testbuild)
	@mbanner "Cleaned Test Build Directory"


###################################
cleanall:
	(rm -rf testbuild)
	(rm -rf src)
	(rm -rf bin)
	@mbanner "Cleaned Out all the the Directories"


