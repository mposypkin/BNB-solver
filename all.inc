#General make file settings
CC = cc
CPP = c++
LIBTOOL = ar -r

#platform-specific options
#Compaq compiler C++ options:
#PCPPOPTS=-D__USE_STD_IOSTREAM -tweak

#MPICH-2 specific option
#PCPPOPTS=-DMPICH_IGNORE_CXX_SEEK

#Altivec specific C options:
#PCOPTS=-maltivec -D _OPT_ALTIVEC_

#Pthread linking support options
PTHREADLD=-lpthread

#OpenMP specific compiler options
#OPENMPOPT=-fopenmp

#OpenMP specific linker options
#OPENMPLD=-fopenmp

#optimization options
OPTIMOPT=-g
#OPTIMOPT=-O

#Poll option
#POLLOPT=-D__USE_POLL__

#Static linking option (if needed)
#STATIC=-static

#C++ 11 support on
STD_OPT = -std=c++11


EXTRA_INC = -I$(ROOT)


#common options
COMOPTS = $(STD_OPT)\
	   -I$(ROOT)\
           $(EXTRA_INC)\
       $(OPENMPOPT)\
       $(OPTIMOPT)\
       $(POLLOPT)
#C options
COPTS=$(COMOPTS) $(ADDIT_INCS) $(PCOPTS)

#C++ options
CPPOPTS= $(COMOPTS) $(ADDIT_INCS) $(PCPPOPTS)

#Linker options for static linking
LDFLAGS = -lm -ldl $(OPENMPLD) $(PTHREADLD) $(STATIC)


MFLAGS = MPICC=$(MPICC) COPTS="$(COPTS)" CPPOPTS="$(CPPOPTS) LIBTOOL="$(LIBTOOL)"


all: $(BINS) $(LIBS)

tests:
	@for i in $(TESTS); do if ./$$i > /dev/null; then echo TEST PASSED; continue; else echo TEST FAILED; fi done

# Uncomment this if you want testing finishes after first finished test
#	@for i in $(TESTS); do if ./$$i > /dev/null; then echo TEST PASSED; continue; else echo TEST FAILED; return 1; fi done
	

$(BINS): $(LIBS) $(EXTLIBS)

$(LIBS): $(OBJS)
	$(LIBTOOL) $@ $(OBJS)

indent: 
	indent *.cpp ; indent *.hpp
clean: 
	rm -f *.exe *.o *.a *~ *.log deps.inc

dep:
	$(CC) $(COPTS) -MM -c *.c > deps.inc;\
	$(CPP) $(CPPOPTS) -MM -c *.cpp >> deps.inc;\
        true

.o.exe:
	$(CPP) $(OPTS) -o $@ $< $(LIBS) $(EXTLIBS) $(LDFLAGS)

.cpp.o:
	$(CPP) $(CPPOPTS) -c $<

.c.o:
	$(CC) $(COPTS) -c $<

.SUFFIXES:
.SUFFIXES: .o .a .cpp .c .exe
