CC = icc
CFLAGS =
LFLAGS = -lpthread -lrt -lm -O3

all: blurpt

clean:
	-$(RM) *.o blurc thresc blurmpi threshmpi


arc:
	tar cf - *.c *.h Makefile data/* | gzip - > filters.tar.gz

blurpt: ppmio.o gaussw.o blurfilter.o
	$(CC) -o blurpt ppmio.o gaussw.o blurfilter.o blurmain_pthreads.c $(LFLAGS)
