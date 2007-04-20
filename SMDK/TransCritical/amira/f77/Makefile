#
# Makefile for Cygwin. This builds a DLL that depends on Cygwin DLL;
# for DLLs independent of Cygwin DLL, use Makefile.nocygwin instead.
#
FC = g77

DEBUG = -g #-O2
# The -mrtd flag tells the compiler to use the PASCAL or "stdcall" calling
# convention required by VBA/Excel/etc.
FFLAGS = -mrtd $(DEBUG)

AS = as
DLLTOOL = dlltool
DLLWRAP = dllwrap

#
# Various targets to build.
#
DLL_NAME = f77dll.dll
DLL_EXP_DEF = f77dll.def

all: $(DLL_NAME)

#
# sources, objects, etc. 
#
SRCS  = $(wildcard *.cc *.c *.f)
OBJS := $(OBJS:.f=.o)

#
# DLL related variables. These are used when building the DLL. See later.
#

# any special flags that you need to pass to the F77 compiler when building 
# the DLL. (lots of software need _DLL defined).
DLL_FFLAGS = 
# This is where you pass linker flags, such as library search path, alternate
# entry point, etc. The -s flag strips the final DLL. The alternate entry
# point is needed since this DLL will be loaded by a non-cygwin application.
DLL_LDFLAGS = -s --entry __cygwin_noncygwin_dll_entry@12
# any extra libraries that your DLL may depend on.
DLL_LDLIBS = 

#
# your sources etc that go into the DLL.
#
DLL_SRCS = f77dll.f 
DLL_OBJS = f77dll.o 

###
#
# Making DLL
#
###

#
# Note that we let dllwrap create both the DEF and IMPORT library in
# one shot. No need to run dlltool anymore.
#
DLLWRAP_FLAGS = --export-all --output-def $(DLL_EXP_DEF) \
	--driver-name $(FC)

$(DLL_NAME) $(DLL_EXP_DEF): $(DLL_OBJS)
	$(DLLWRAP) $(DLLWRAP_FLAGS) -o $(DLL_NAME) \
	    $(DLL_OBJS) $(DLL_LDFLAGS) $(DLL_LDLIBS)

#
# dependencies.
#

fdll.o: fdll.f

#
# default rules for building DLL objects. Note that client programs (ie.,
# the ones that *use* the DLL) have to be compiled without the DLL_CFLAGS
# flags.
#
.f.o:
	$(FC) -c $(DLL_FFLAGS) $(FFLAGS) -o $@ $<

clean:
	-rm -f $(OBJS) $(DLL_OBJS) $(DLL_NAME) $(DLL_EXP_DEF)

