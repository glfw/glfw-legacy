##########################################################################
# This makefile builds the example programs using the Digital Mars D
# compiler. You need to copy the .lib files from support\d\lib to your
# Digital Mars C/C++ compiler/linker lib folder (dm\lib) before compiling
# the programs.
##########################################################################
# $Id: Makefile,v 1.1 2004-03-04 21:33:20 marcus256 Exp $
##########################################################################

DC     = dmd
DFLAGS = -O -I..\imports

LIBS = glfwdll.lib glu32.lib opengl32.lib


all: triangle.exe splitview.exe


clean:
	del *.exe *.obj *.map


triangle.exe: triangle.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)

splitview.exe: splitview.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)
