CC=mpicc 
CFLAGS=-Wall -Werror 

mpi.o: mpi.c 
	${CC} -g mpi.c -o mpi  ${CFLAGS}

clean: 
	rm -f mpi 
	rm -f *.o