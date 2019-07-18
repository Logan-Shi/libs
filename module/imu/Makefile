TOPDIR=../../

LIB=libImu.a
INCPATH=$(TOPDIR)/include
LIBPATH=$(TOPDIR)/lib
CFLAGS= -I$(INCPATH) 
LDFLAG= -lm -L/usr/lib -L/usr/include/lib
include $(TOPDIR)/Makefile.env

#all: serial.o
#	ar rv ${LIBPATH}/libserial.a  *.o    

#serial.o: serial.c
#	${CC} -c $^  $(CFLAGS)

clean: 
	rm -f  *.o *.d *.a
	rm -f *~
	rm -f *swp
	rm -f ${LIBPATH}/${LIB}
