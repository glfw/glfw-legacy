###########################################################################
# GLFW top level Makefile
# $Date: 2003-05-23 22:00:15 $
# $Revision: 1.5 $
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
	@echo "  $(MAKE) win32-mgw       for MinGW32 for Windows"
	@echo "  $(MAKE) win32-cygwin    for Cygwin for Windows"
	@echo "  $(MAKE) win32-lcc       for LCC-Win32 for Windows"
	@echo "  $(MAKE) win32-bcc       for Borland C++ Builder 5.x for Windows"
	@echo "  $(MAKE) win32-msvc      for MS Visual C++ 6.x for Windows"
	@echo "  $(MAKE) win32-ow        for OpenWatcom for Windows"
	@echo "  $(MAKE) win32-clean     to remove any compiled files for Windows"
	@echo "  $(MAKE) cygwin-clean    to remove any compiled files for Cygwin/Windows"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) x11             for Unix/X11 (auto-configuring)"
	@echo "  $(MAKE) x11-gcc         for Unix/X11 (auto-configuring, force using GCC)"
	@echo "  $(MAKE) x11-clean       to remove any compiled files for Unix/X11"
	@echo "-----------------------------------------------------------------------------"
	@echo "  $(MAKE) amigaos-vbcc    for AmigaOS for VBCC"
	@echo "  $(MAKE) amigaos-clean   to remove any compiled files for AmigaOS"
	@echo "-----------------------------------------------------------------------------"
#	@echo "  $(MAKE) macosx-gcc      for GCC for Mac OS X (free Apple SDK)"
#	@echo "  $(MAKE) macosx-clean    to remove any compiled files for Mac OS X"
#	@echo "-----------------------------------------------------------------------------"


###########################################################################
# List of example programs (used for cleanup)
###########################################################################
EXAMPLES = boing \
           gears \
           keytest \
           listmodes \
           mipmaps \
           mtbench \
           mthello \
           particles \
           pong3d \
           splitview \
           triangle \
           wave


###########################################################################
# Windows
###########################################################################

# Cleanup for Windows
win32-clean:
	@.\\compile.bat CLEAN

# Cleanup for Cygwin (Cygwin Make does not support local BAT-files)
cygwin-clean:
	@rm -f lib/win32/*.o
	@rm -f lib/win32/*.obj
	@rm -f lib/win32/*.tds
	@rm -f lib/win32/lib*.a
	@rm -f lib/win32/glfw.dll
	@rm -f examples/*.o
	@rm -f examples/*.obj
	@rm -f examples/*.tds
	@rm -f examples/*.exe


# Windows, MinGW32
win32-mgw:
	@.\\compile.bat $(MAKE) mgw


# Windows, Cygwin (Cygwin Make does not support local BAT-files)
win32-cygwin:
	@cd lib/win32 && $(MAKE) -f Makefile.win32.cygwin
	@cd examples  && $(MAKE) -f Makefile.win32.cygwin


# Windows, LCC-Win32
win32-lcc:
	@.\\compile.bat $(MAKE) lcc


# Windows, Borland C++ Builder Compiler 5.x
win32-bcc:
	@.\\compile.bat $(MAKE) bcc


# Windows, Microsoft Visual C++
win32-msvc:
	@.\\compile.bat $(MAKE) msvc


# Windows, OpenWatcom
win32-ow:
	@.\\compile.bat $(MAKE) ow



###########################################################################
# X11 (Unix and Unix-like systems)
###########################################################################

MAKEFILES_X11_IN  = lib/x11/Makefile.x11.in  examples/Makefile.x11.in
MAKEFILES_X11     = lib/x11/Makefile.x11     examples/Makefile.x11
MAKEFILES_X11_GCC = lib/x11/Makefile.x11.gcc examples/Makefile.x11.gcc

# Cleanup for X11
x11-clean:
	@rm -f $(MAKEFILES_X11)
	@rm -f $(MAKEFILES_X11_GCC)
	@rm -f config.log
	@rm -f lib/x11/*.o
	@rm -f lib/x11/libglfw.a
	@cd examples; rm -f $(EXAMPLES)


# Auto configuration for X11
$(MAKEFILES_X11) : compile.sh $(MAKEFILES_X11_IN)
	@sh ./compile.sh

# Auto configuration for X11 (with forced GCC)
$(MAKEFILES_X11_GCC) : compile.sh $(MAKEFILES_X11_IN)
	@sh ./compile.sh -gcc


# X11
x11: $(MAKEFILES_X11)
	@cd lib/x11;  $(MAKE) -f Makefile.x11
	@cd examples; $(MAKE) -f Makefile.x11


# X11, GCC
x11-gcc: $(MAKEFILES_X11_GCC)
	@cd lib/x11;  $(MAKE) -f Makefile.x11.gcc
	@cd examples; $(MAKE) -f Makefile.x11.gcc


###########################################################################
# Mac OS X (not supported yet)
###########################################################################

# Cleanup for Mac OS X
macosx-clean:
	@rm -f lib/macosx/*.o
	@rm -f lib/macosx/libglfw.a
	@cd examples; $(MAKE) -f Makefile.macosx.gcc clean


# Mac OS X, GCC
macosx-gcc:
	@cd lib/macosx; $(MAKE) -f Makefile.macosx.gcc
	@cd examples;   $(MAKE) -f Makefile.macosx.gcc


###########################################################################
# AmigaOS
###########################################################################

# Cleanup for AmigaOS
amigaos-clean:
	@execute compile.ami CLEAN


# AmigaOS, VBCC
amigaos-vbcc:
	@execute compile.ami $(MAKE) vbcc
