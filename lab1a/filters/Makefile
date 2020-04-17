CC = icc
CFLAGS =
LFLAGS = -lpthread -lrt -lm

all: blurc thresc

clean:
	-$(RM) *.o blurc thresc blurmpi threshmpi

blurc: ppmio.o gaussw.o blurfilter.o blurmain.o
	$(CC) -o $@ ppmio.o gaussw.o blurfilter.o blurmain.o $(LFLAGS)

thresc: thresmain.o ppmio.o thresfilter.o
	$(CC) -o $@ thresmain.o ppmio.o thresfilter.o $(LFLAGS)

arc:
	tar cf - *.c *.h Makefile data/* | gzip - > filters.tar.gz

blurmpi: ppmio.o gaussw.o blurfilter.o
	mpicc -std=c99 -o blurmpi ppmio.o gaussw.o blurfilter.o blurmain_mpi.c $(LFLAGS)

threshmpi: ppmio.o thresfilter.o
		mpicc -std=c99 -o threshmpi ppmio.o thresfilter.o threshmain_mpi.c $(LFLAGS)

blurpt: ppmio.o gaussw.o blurfilter.o
	$(CC) -o blurpt ppmio.o gaussw.o blurfilter.o blurmain_pthreads.c $(LFLAGS)
