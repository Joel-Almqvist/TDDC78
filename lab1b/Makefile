CC = icc
CFLAGS =
LFLAGS = -lpthread -lrt -lm -O3 -std=gnu99

all: blurpt threshpt

clean:
	-$(RM) *.o blurc thresc blurmpi threshmpi threshpt


arc:
	tar cf - *.c *.h Makefile data/* | gzip - > filters.tar.gz

blurpt: ppmio.o gaussw.o blurfilter.o
	$(CC) -o blurpt ppmio.o gaussw.o blurfilter.o blurmain_pthreads.c $(LFLAGS)

threshpt: ppmio.o thresfilter.o
		$(CC) -o threshpt ppmio.o thresfilter.o threshmain_pthreads.c $(LFLAGS)
