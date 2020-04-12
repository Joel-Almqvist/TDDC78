/* Hello World in MPI.
 * (c) C. Kessler, Linkoping University, TDDC78
 * Compile with MPI C compiler (e.g., mpicc)
 * Run with 8 MPI processes (e.g., mpirun -np8 a.out).
 */
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main( int argc, char *argv[] )
{

  int p;   // number of started MPI processes
  int me;  // my rank
  char *str = (char *) malloc(6*sizeof(char));
  MPI_Status status;

  MPI_Init( &argc, &argv );

  MPI_Comm_size( MPI_COMM_WORLD, &p );
  MPI_Comm_rank( MPI_COMM_WORLD, &me );

  printf("P: %i, rank: %i\n", p, me);


  if(me == 0){
    MPI_Send("Hello\0", 6, MPI_CHAR, 1, 1234, MPI_COMM_WORLD);
  }
  else if (me == 1){
    MPI_Recv(str, 6, MPI_CHAR, 0, 1234, MPI_COMM_WORLD, &status);
    printf("P1 received %s from Process %d with tag %d\n",
           str, status.MPI_SOURCE, status.MPI_TAG );
  }


  MPI_Finalize();
  return 0;

  printf("Hello from %d of %d\n", me, p );

  if (me == 2)
    MPI_Send( "Hello\0", 6, MPI_CHAR, 7, 1234, MPI_COMM_WORLD );

  if (me == 7) {
    MPI_Recv( str, 6, MPI_CHAR, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
    printf("P7 received %s from Process %d with tag %d\n",
           str, status.MPI_SOURCE, status.MPI_TAG );
  }

  printf("Goodbye from %d\n", me );

  MPI_Finalize();
  return 0;
}
