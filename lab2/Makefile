laplsolv: laplsolv.c
	gcc -std=c11 -fopenmp $^ -lm -o $@

omp: laplsolv-openmp.c
	gcc -std=c11 -fopenmp $^ -lm -o $@


clean:
	-$(RM) laplsolv laplsolv-f
