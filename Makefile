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
	@echo "  $(MAKE) win32-mingw       for bare MinGW on Windows"
	@echo "  $(MAKE) mingw-clean       to clean the GLFW library and header"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-msys        for MinGW/MSYS on Windows"
	@echo "  $(MAKE) msys-clean        to clean the GLFW library and header"
	@echo "  $(MAKE) msys-install      to install the GLFW library and header into MinGW/MSYS"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-cygwin      for Windows native (no Cygwin dependency) on Cygwin"
	@echo "  $(MAKE) cygwin-clean      to remove any compiled files for Cygwin/Windows"
	@echo "  $(MAKE) cygwin-install    to install the GLFW library and header into Cygwin"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) win32-lcc         for LCC-Win32 for Windows"
	@echo "  $(MAKE) win32-msvc        for MS Visual C++ 6.x for Windows"
	@echo "  $(MAKE) win32-ow          for OpenWatcom for Windows"
	@echo "  $(MAKE) win32-clean       to remove any compiled files for Windows"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) cross-mgw         for MinGW cross-compile to Windows from Unix" 
	@echo "  $(MAKE) cross-mgw-clean   to clean the GLFW library and header"
	@echo "  $(MAKE) cross-mgw-install to install the GLFW library and header"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) x11               for X11 on Unix-like systems (auto-configuring)"
	@echo "  $(MAKE) x11-clean         to remove any compiled files for Unix/X11"
	@echo "  $(MAKE) x11-install       to install the GLFW library and header"
	@echo "  $(MAKE) x11-dist-install  for a dist to install the GLFW libraries and header"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) cocoa             for Cocoa on Mac OS X"
	@echo "  $(MAKE) cocoa-clean       to remove any compiled files for Cocoa on Mac OS X"
	@echo "-----------------------------------------------------------------------------"


###########################################################################
# Bare MinGW on Windows
###########################################################################

win32-mingw:
	@.\\compile.bat $(MAKE) mingw

mingw-clean:
	@.\\compile.bat CLEAN


###########################################################################
# MinGW/MSYS on Windows
###########################################################################

msys-install: win32-msys
	cd lib/win32 && $(MAKE) -f Makefile.win32.msys install

win32-msys:
	cd lib/win32 && $(MAKE) -f Makefile.win32.msys
	cd examples  && $(MAKE) -f Makefile.win32.msys

msys-clean:
	cd lib/win32 && $(MAKE) -f Makefile.win32.msys clean
	cd examples  && $(MAKE) -f Makefile.win32.msys clean


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


###########################################################################
# Various compilers on Windows
###########################################################################

# Windows, LCC-Win32
win32-lcc:
	@.\\compile.bat $(MAKE) lcc

# Windows, Microsoft Visual C++
win32-msvc:
	@.\\compile.bat $(MAKE) msvc

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

# List of example programs (used by clean target)
EXAMPLES = boing gears listmodes mipmaps mtbench mthello particles \
           pong3d splitview triangle wave

# List of test programs (used by clean target)
TESTS = accuracy defaults events fsaa fullscreen joysticks peter reopen \
	tearing version

MAKEFILES_X11_IN = lib/x11/Makefile.x11.in examples/Makefile.x11.in \
		   tests/Makefile.x11.in

MAKEFILES_X11 = lib/x11/Makefile.x11 examples/Makefile.x11 tests/Makefile.x11

# Cleanup for X11 (must be here due to generated makefile)
x11-clean:
	rm -f $(MAKEFILES_X11)
	rm -f config.log
	rm -f lib/x11/*.o
	rm -f lib/x11/libglfw.a
	rm -f lib/x11/libglfw.so
	rm -f lib/x11/libglfw.pc
	rm -f lib/x11/libglfw.pc.in
	cd examples; rm -f $(EXAMPLES)
	cd tests; rm -f $(TESTS)

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

