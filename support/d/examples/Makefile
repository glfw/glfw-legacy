##########################################################################
# This makefile builds the example programs using the Digital Mars D
# compiler. You need to copy the .lib files from support\d\lib to your
# Digital Mars C/C++ compiler/linker lib folder (dm\lib) before compiling
# the programs.
##########################################################################
# $Id: Makefile,v 1.2 2004-04-05 19:19:07 marcus256 Exp $
##########################################################################

DC     = dmd
DFLAGS = -O -I..\imports

LIBS = glfwdll.lib glu32.lib opengl32.lib


all: boing.exe gears.exe particles.exe splitview.exe triangle.exe


clean:
	del *.exe *.obj *.map


boing.exe: boing.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)

gears.exe: gears.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)

particles.exe: particles.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)

splitview.exe: splitview.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)

triangle.exe: triangle.d
	$(DC) $(DFLAGS) -of$@ $<d $(LIBS)
