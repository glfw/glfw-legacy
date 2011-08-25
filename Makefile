###########################################################################
# GLFW top level Makefile
# $Date: 2007-10-26 14:33:43 $
# $Revision: 1.11 $
#
# This file works as a top level makefile for all supported systems and
# compilers. It builds both the GLFW link library and the supplied example
# programs.
###########################################################################

###########################################################################
# If your make program does not set the $(MAKE) variable correctly,
# uncomment the following line and make sure that the name of the make
# tool is correct.
###########################################################################
# MAKE = make


###########################################################################
# If no system/compiler is specified, display a list of available options
###########################################################################
default:
	@echo "This is the makefile for the GLFW link library and example programs."
	@echo "Type one of the following:"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-mingw       to compile GLFW on MinGW on Windows"
	@echo "  $(MAKE) mingw-clean       to remove any files generated by MinGW on Windows"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-cygwin      to compile GLFW on Windows native on Cygwin/MSYS"
	@echo "  $(MAKE) cygwin-clean      to remove any files generated by Cygwin/MSYS"
	@echo "  $(MAKE) cygwin-install    to install the GLFW library and header"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-lcc         to compile GLFW on LCC-Win32 for Windows"
	@echo "  $(MAKE) win32-ow          to compile GLFW on OpenWatcom for Windows"
	@echo "  $(MAKE) win32-clean       to remove any files generated by above compilers"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) cross-mgw         for Unix-hosted MinGW cross-compile to Windows" 
	@echo "  $(MAKE) cross-mgw-clean   to remove any files generated by MinGW for Unix"
	@echo "  $(MAKE) cross-mgw-install to install the GLFW library and header"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) x11               to compile GLFW for X11 on Unix-like systems"
	@echo "  $(MAKE) x11-clean         to remove any files generated for Unix/X11"
	@echo "  $(MAKE) x11-dist-clean    to also removed the generated build files"
	@echo "  $(MAKE) x11-install       to install the GLFW library and header"
	@echo "  $(MAKE) x11-dist-install  to also install the GLFW shared library"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) cocoa             to compile GLFW with GCC for Cocoa on Mac OS X"
	@echo "  $(MAKE) cocoa-clean       to remove any files generated for Cocoa on Mac OS X"
	@echo "  $(MAKE) cocoa-install     to install the GLFW library and header"
	@echo "-----------------------------------------------------------------------------"
	@echo " There are also Visual Studio 2008 and 2010 project files available in the"
	@echo " support/msvc90 and support/msvc100 directories, respectively"
	@echo "-----------------------------------------------------------------------------"


###########################################################################
# Bare MinGW on Windows
###########################################################################

win32-mingw:
	@.\\compile.bat $(MAKE) mingw

mingw-clean:
	@.\\compile.bat CLEAN


###########################################################################
# Cygwin on Windows
###########################################################################

cygwin-install: win32-cygwin
	cd lib/win32 && $(MAKE) -f Makefile.win32.cygwin install

win32-cygwin:
	cd lib/win32 && $(MAKE) -f Makefile.win32.cygwin
	cd examples  && $(MAKE) -f Makefile.win32.cygwin
	cd tests     && $(MAKE) -f Makefile.win32.cygwin

cygwin-clean:
	cd lib/win32 && $(MAKE) -f Makefile.win32.cygwin clean
	cd examples  && $(MAKE) -f Makefile.win32.cygwin clean
	cd tests     && $(MAKE) -f Makefile.win32.cygwin clean


###########################################################################
# Various compilers on Windows
###########################################################################

# Windows, LCC-Win32
win32-lcc:
	@.\\compile.bat $(MAKE) lcc

# Windows, OpenWatcom
win32-ow:
	@.\\compile.bat $(MAKE) ow

# Cleanup for Windows
win32-clean:
	@.\\compile.bat CLEAN


###########################################################################
# MinGW cross-compile to Windows from Unix 
###########################################################################

cross-mgw-install: cross-mgw 
	cd lib/win32 && $(MAKE) -f Makefile.win32.cross-mgw install 

cross-mgw:
	cd lib/win32 && $(MAKE) -f Makefile.win32.cross-mgw
	cd examples  && $(MAKE) -f Makefile.win32.cross-mgw 
	cd tests     && $(MAKE) -f Makefile.win32.cross-mgw

cross-mgw-clean:
	cd lib/win32 && $(MAKE) -f Makefile.win32.cross-mgw clean 
	cd examples  && $(MAKE) -f Makefile.win32.cross-mgw clean
	cd tests     && $(MAKE) -f Makefile.win32.cross-mgw clean


###########################################################################
# X11 on Unix-like systems
###########################################################################

MAKEFILES_X11_IN = lib/x11/Makefile.x11.in examples/Makefile.x11.in \
		   tests/Makefile.x11.in

MAKEFILES_X11 = lib/x11/Makefile.x11 examples/Makefile.x11 tests/Makefile.x11

# Cleanup for X11 (must be here due to generated makefile)
x11-clean: $(MAKEFILES_X11)
	cd lib/x11;  $(MAKE) -f Makefile.x11 clean
	cd examples; $(MAKE) -f Makefile.x11 clean
	cd tests;    $(MAKE) -f Makefile.x11 clean

x11-dist-clean: x11-clean
	rm -f config.log
	rm -f $(MAKEFILES_X11)
	rm -f lib/x11/libglfw.pc.in

# Auto configuration for X11
$(MAKEFILES_X11) : compile.sh $(MAKEFILES_X11_IN)
	@sh ./compile.sh

# X11
x11: $(MAKEFILES_X11)
	cd lib/x11;  $(MAKE) -f Makefile.x11
	cd examples; $(MAKE) -f Makefile.x11
	cd tests;    $(MAKE) -f Makefile.x11

# X11 install
x11-install: x11
	cd lib/x11;  $(MAKE) -f Makefile.x11 install

# X11 dist install
x11-dist-install: x11
	cd lib/x11;  $(MAKE) -f Makefile.x11 dist-install


###########################################################################
# Cocoa on Mac OS X
###########################################################################

cocoa:
	cd lib/cocoa; $(MAKE) -f Makefile.cocoa
	cd examples;  $(MAKE) -f Makefile.cocoa
	cd tests;     $(MAKE) -f Makefile.cocoa

cocoa-clean:
	cd lib/cocoa; $(MAKE) -f Makefile.cocoa clean
	cd examples;  $(MAKE) -f Makefile.cocoa clean
	cd tests;     $(MAKE) -f Makefile.cocoa clean

cocoa-install:
	cd lib/cocoa; $(MAKE) -f Makefile.cocoa install

