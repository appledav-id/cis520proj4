CC=mpicc 
CFLAGS=-Wall -Werror -std=c99 

mpi.o: mpi.c 
	${CC} -c mpi.c -o mpi.o  ${CFLAGS}

clean: 
	rm -f mpi 
	rm -f *.o