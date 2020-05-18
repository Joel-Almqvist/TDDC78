

sequential:main_sequential.c  physics.c
	gcc -std=c99 -g -o $@ $^ -lrt -lm


par:main_parallel.c  physics.c
	mpicc -std=c99 -g -o $@ $^ -lrt -lm


allclean:
	-rm *.o sequential
