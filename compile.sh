#!/bin/sh

##########################################################################
# compile.sh - Unix/X11 configuration script
# $Date: 2003-06-13 22:18:34 $
# $Revision: 1.8 $
#
# This is a minimalist configuration script for GLFW, which is used to
# determine the availability of certain features.
#
# This script is not very nice at all (especially the Makefile generation
# is very ugly and hardcoded). Hopefully it will be cleaned up in the
# future, but for now it does a pretty good job (much better than the
# three separate Makefiles in older distributions anyway).
##########################################################################

##########################################################################
# Check arguments
##########################################################################
force_gcc=no
silent=no
for arg in "$@"; do
{
  case "$arg" in
    # Force gcc?
    -gcc | -force-gcc | --gcc | --force-gcc | -gnuc | -gnu-c)
      force_gcc=yes ;;

    # Silent?
    -q | -quiet | --quiet | --quie | --qui | --qu | --q \
    | -silent | --silent | --silen | --sile | --sil)
      silent=yes ;;
  esac;
}
done;


##########################################################################
# Misc.
##########################################################################

config_script=$0

# File descriptor usage:
# 0 standard input
# 1 file creation
# 2 errors and warnings
# 3 some systems may open it to /dev/tty
# 4 used on the Kubota Titan
# 5 compiler messages saved in config.log
# 6 checking for... messages and results
exec 5>./config.log
if [ "x$silent" = xyes ]; then
  exec 6>/dev/null
else
  exec 6>&1
fi

echo "\
This file contains any messages produced by compilers while
running $config_script, to aid debugging if $config_script makes a mistake.
" 1>&5

# Add .gcc to the file name of the generated makefiles if GCC was forced
makefile_ext=
if [ "x$force_gcc" = "xyes" ]; then
  makefile_ext=".gcc"
fi


##########################################################################
# Default compiler settings
##########################################################################
if [ "x$force_gcc" = xyes ]; then
  CC=gcc
else
  CC=${CC-cc}
fi
CFLAGS=
LFLAGS=
LIBS="-lGL -lX11"


##########################################################################
# Compilation commands
##########################################################################
compile='$CC -c $CFLAGS conftest.c 1>&5'
link='$CC -o conftest $CFLAGS $LFLAGS conftest.c $LIBS 1>&5'


##########################################################################
# Check for X11 libs directory
##########################################################################
echo "Checking for X11 libraries location... " 1>&6

# X11R6 in /usr/X11R6/lib ?
if [ -r "/usr/X11R6/lib/libX11.so" ]; then

 LFLAGS="$LFLAGS -L/usr/X11R6/lib"
 CFLAGS="$CFLAGS -I/usr/X11R6/include"
 echo " X11 libraries location: /usr/X11R6/lib" 1>&6

# X11R5 in /usr/X11R5/lib ?
elif [ -r "/usr/X11R5/lib/libX11.so" ]; then

 LFLAGS="$LFLAGS -L/usr/X11R5/lib"
 CFLAGS="$CFLAGS -I/usr/X11R5/include"
 echo " X11 libraries location: /usr/X11R5/lib" 1>&6

# Mac OS X: X11R6 in /usr/X11R6/lib, uses .dylib instead of .so
elif [ -r "/usr/X11R6/lib/libX11.dylib" ]; then

 LFLAGS="$LFLAGS -L/usr/X11R6/lib"
 CFLAGS="$CFLAGS -I/usr/X11R6/include"
 echo " X11 libraries location: /usr/X11R6/lib" 1>&6

# QNX: X11R6 in /opt/X11R6/lib ?
elif [ -r "/opt/X11R6/lib/libX11.so" ]; then

 LFLAGS="$LFLAGS -L/opt/X11R6/lib"
 CFLAGS="$CFLAGS -I/opt/X11R6/include"
 echo " X11 libraries location: /opt/X11R6/lib" 1>&6

elif [ -r "/opt/X11R6/lib/libX11.so.6" ]; then

 LFLAGS="$LFLAGS -L/opt/X11R6/lib"
 CFLAGS="$CFLAGS -I/opt/X11R6/include"
 echo " X11 libraries location: /opt/X11R6/lib" 1>&6

else

 echo " X11 libraries location: Unknown (assuming linker will find them)" 1>&6

fi
echo " " 1>&6


##########################################################################
# Check if we are using GNU C
##########################################################################
echo "Checking whether we are using GNU C... " 1>&6
echo "$config_script: checking whether we are using GNU C" >&5

cat > conftest.c <<EOF
#ifdef __GNUC__
  yes;
#endif
EOF

if { ac_try='$CC -E conftest.c'; { (eval echo $config_script: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; }; } | egrep yes >/dev/null 2>&1; then
  use_gcc=yes
else
  use_gcc=no
fi
rm -f conftest*

echo " Using GNU C: ""$use_gcc" 1>&6
if [ "x$use_gcc" = xyes ]; then
  CC=gcc
fi
echo " " 1>&6


##########################################################################
# Check for XFree86 VidMode availability
##########################################################################
echo "Checking for XFree86 VidMode 1.0 support... " 1>&6
echo "$config_script: Checking for XFree86 VidMode 1.0 support" >&5
has_xf86vm=no

cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#if defined(__APPLE_CC__)
#error Not supported under Mac OS X
#endif

int main() {; return 0;}
EOF

if { (eval echo $config_script: \"$compile\") 1>&5; (eval $compile) 2>&5; }; then
  rm -rf conftest*
  has_xf86vm=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " XFree86 VidMode extension: ""$has_xf86vm" 1>&6
if [ "x$has_xf86vm" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_XF86VIDMODE"
   LIBS="$LIBS -lXxf86vm -lXext"
fi
echo " " 1>&6


##########################################################################
# Check for glXGetProcAddressXXX availability
##########################################################################
echo "Checking for glXGetProcAddress support... " 1>&6
echo "$config_script: Checking for glXGetProcAddress support" >&5
has_glXGetProcAddress=no
has_glXGetProcAddressARB=no
has_glXGetProcAddressEXT=no

# glXGetProcAddress check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddress("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddress=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

# glXGetProcAddressARB check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddressARB("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddressARB=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

# glXGetProcAddressEXT check
cat > conftest.c <<EOF
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
int main() {void *ptr=(void*)glXGetProcAddressEXT("glFun"); return 0;}
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_glXGetProcAddressEXT=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " glXGetProcAddress extension:    ""$has_glXGetProcAddress" 1>&6
echo " glXGetProcAddressARB extension: ""$has_glXGetProcAddressARB" 1>&6
echo " glXGetProcAddressEXT extension: ""$has_glXGetProcAddressEXT" 1>&6
if [ "x$has_glXGetProcAddress" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESS"
fi
if [ "x$has_glXGetProcAddressARB" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESSARB"
fi
if [ "x$has_glXGetProcAddressEXT" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_GLXGETPROCADDRESSEXT"
fi
echo " " 1>&6


##########################################################################
# Check for dlopen support
##########################################################################
echo "Checking for dlopen support... " 1>&6
echo "$config_script: Checking for dlopen support" >&5
has_dlopen=no

cat > conftest.c <<EOF
#include <dlfcn.h>
int main() {void *l=dlopen("libGL.so",RTLD_LAZY|RTLD_GLOBAL); return 0;}
EOF

# First try without -ldl
if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_dlopen=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi

# Now try with -ldl if the previous attempt failed
if [ "x$has_dlopen" = xno ]; then
  LIBS_OLD="$LIBS"
  LIBS="$LIBS -ldl"
  if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
    rm -rf conftest*
    has_dlopen=yes
  else
    echo "$config_script: failed program was:" >&5
    cat conftest.c >&5
  fi
  if [ "x$has_dlopen" = xno ]; then
    LIBS="$LIBS_OLD"
  fi
fi
rm -f conftest*

echo " dlopen support: ""$has_dlopen" 1>&6
if [ "x$has_dlopen" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_DLOPEN"
fi
echo " " 1>&6


##########################################################################
# Check for sysconf support
##########################################################################
echo "Checking for sysconf support... " 1>&6
echo "$config_script: Checking for sysconf support" >&5
has_sysconf=no

cat > conftest.c <<EOF
#include <unistd.h>
#ifndef _SC_NPROCESSORS_ONLN
#ifndef _SC_NPROC_ONLN
#error Neither _SC_NPROCESSORS_ONLN nor _SC_NPROC_ONLN available
#endif
#endif
int main() {long x=sysconf(_SC_ARG_MAX); return 0; }
EOF

if { (eval echo $config_script: \"$link\") 1>&5; (eval $link) 2>&5; }; then
  rm -rf conftest*
  has_sysconf=yes
else
  echo "$config_script: failed program was:" >&5
  cat conftest.c >&5
fi
rm -f conftest*

echo " sysconf support: ""$has_sysconf" 1>&6
if [ "x$has_sysconf" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_SYSCONF"
fi
echo " " 1>&6


##########################################################################
# Check for sysctl support
##########################################################################
echo "Checking for sysctl support... " 1>&6
echo "$config_script: Checking for sysctl support" >&5
has_sysctl=no

cat > conftest.c <<EOF
#include <sys/types.h>
#include <sys/sysctl.h>
#ifdef CTL_HW
#ifdef HW_NCPU
  yes;
#endif
#endif
EOF

if { ac_try='$CC -E conftest.c'; { (eval echo $config_script: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; }; } | egrep yes >/dev/null 2>&1; then
  has_sysctl=yes
fi
rm -f conftest*

echo " sysctl support: ""$has_sysctl" 1>&6
if [ "x$has_sysctl" = xyes ]; then
   CFLAGS="$CFLAGS -D_GLFW_HAS_SYSCTL"
fi
echo " " 1>&6



##########################################################################
# Post fixups
##########################################################################
if [ "x$CC" = xgcc ]; then
  CFLAGS_SPEED="-c -I. -I.. $CFLAGS -O3 -ffast-math -Wall"
  CFLAGS="-c -I. -I.. $CFLAGS -Os -Wall"
  CFLAGS_LINK="-O3 -ffast-math -Wall"
else
  CFLAGS_SPEED="-c -I. -I.. $CFLAGS -O"
  CFLAGS="-c -I. -I.. $CFLAGS -O"
  CFLAGS_LINK="-O"
fi
CFLAGS_LINK="-I../include $CFLAGS_LINK"
if [ "x"`uname -s` = xQNX ]; then
  LFLAGS="$LFLAGS -L../lib/x11 -lglfw -lGLU $LIBS -lsocket -lm"
elif [ "x"`uname -s` = xFreeBSD ]; then
  LFLAGS="$LFLAGS -L../lib/x11 -lglfw -lGLU $LIBS -pthread -lm"
  CFLAGS_LINK="$CFLAGS_LINK -I/usr/X11R6/include"
else
  LFLAGS="$LFLAGS -L../lib/x11 -lglfw -lGLU $LIBS -lpthread -lm"
fi

##########################################################################
# Create Makefiles
##########################################################################

# ./lib/x11/Makefile.x11
MKNAME="./lib/x11/Makefile.x11""$makefile_ext"
echo "Creating ""$MKNAME""..." 1>&6
echo " " 1>&6
echo "$config_script: Creating ""$MKNAME""..." >&5
echo "##########################################################################" >$MKNAME
echo "# Automatically generated Makefile for GLFW" >>$MKNAME
echo "##########################################################################" >>$MKNAME
echo "CC           = $CC" >>$MKNAME
echo "CFLAGS       = $CFLAGS" >>$MKNAME
echo "CFLAGS_SPEED = $CFLAGS_SPEED" >>$MKNAME
echo " " >>$MKNAME
cat ./lib/x11/Makefile.x11.in >>$MKNAME

# ./examples/Makefile.x11
MKNAME="./examples/Makefile.x11""$makefile_ext"
echo "Creating ""$MKNAME""..." 1>&6
echo " " 1>&6
echo "$config_script: Creating ""$MKNAME""..." >&5
echo "##########################################################################" >$MKNAME
echo "# Automatically generated Makefile for GLFW" >>$MKNAME
echo "##########################################################################" >>$MKNAME
echo "CC     = $CC" >>$MKNAME
echo "CFLAGS = $CFLAGS_LINK" >>$MKNAME
echo "LFLAGS =$LFLAGS" >>$MKNAME
echo " " >>$MKNAME
cat ./examples/Makefile.x11.in >>$MKNAME
