##########################################################################
# Makefile for the GLFW documentation.
#-------------------------------------------------------------------------
# $Id: Makefile,v 1.2 2003-11-04 23:01:32 marcus256 Exp $
##########################################################################

PDFDOCS = glfwrm.pdf glfwug.pdf
DVIDOCS = glfwrm.dvi glfwug.dvi


##########################################################################
# Build macros
##########################################################################
default: pdf
pdf:     $(PDFDOCS)
dvi:     $(DVIDOCS)


##########################################################################
# Clean macros
##########################################################################
clean:
	rm -f glfwrm.dvi glfwrm.aux glfwrm.log glfwrm.out glfwrm.pdf glfwrm.toc
	rm -f glfwug.dvi glfwug.aux glfwug.log glfwug.out glfwug.pdf glfwug.toc

clean-win:
	@.\\cleanup.bat


##########################################################################
# Rules for building the GLFW Reference Manual
##########################################################################

glfwrm.pdf: glfwrm.tex glfwrm.toc glfwdoc.sty
	pdflatex glfwrm.tex

glfwrm.dvi: glfwrm.tex glfwrm.toc glfwdoc.sty
	latex glfwrm.tex

glfwrm.toc: glfwrm.tex glfwdoc.sty
	latex glfwrm.tex


##########################################################################
# Rules for building the GLFW Users Guide
##########################################################################

glfwug.pdf: glfwug.tex glfwug.toc glfwdoc.sty
	pdflatex glfwug.tex

glfwug.dvi: glfwug.tex glfwug.toc glfwdoc.sty
	latex glfwug.tex

glfwug.toc: glfwug.tex glfwdoc.sty
	latex glfwug.tex
