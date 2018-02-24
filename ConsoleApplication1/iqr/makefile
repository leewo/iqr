CC=clang++
CFLAGS=-O3 -D_GNU_SOURCE -D_EPOLL -D_REENTRANT -D_SENDALL
BINDIR=$(HOME)/bin
INC1=-I../inc
INCLUDE=$(INC1)
COMLIB=$(BINDIR)/lib/libcom.a
LDFLAGS=-lm -lpthread

IQR=iqr.o SendBufIQR.o SockCtrl.o RealCtrl.o

.SUFFIXES : .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all:iqr

iqr:$(IQR)
	$(CC) -o $@ $(IQR) $(COMLIB) $(LDFLAGS) 

clean:
	rm -f *.o
	rm -f iqr

install:
	\rm -f $(BINDIR)/iqr
	\cp -p iqr $(BINDIR)
	
#-- EOF -------------------------------------------------------------
