#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
    int rank,nbranks;
    double t0,t1,t2,t3,elaps;
    
    size_t size = atoi(argv[1]);
    if (size > 65392) { //only for bufferised mode
        printf("Buff mode!\n");
        return 1;
    }
    char* buffer = (char *)malloc(size * sizeof(char));
    if (buffer == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    for(int i=0; i<size; i++){
        buffer[i] = 'c';
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nbranks);
    if (nbranks != 2)
    {
      printf("ranks need to be 2!\n");
      MPI_Finalize();
      return 1;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        t0 = MPI_Wtime();
        MPI_Send(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        t1 = MPI_Wtime();
        MPI_Send(&t1, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
        printf("Given timing : %lu\t%g\n", size, t1 - t0);
    } else if (rank == 1) {
        t2 = MPI_Wtime();
        MPI_Recv(buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        t3 = MPI_Wtime();
        MPI_Recv(&t1, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(t1<=t2){ //message copied in buffer then recv started then send then end
            elaps = t3 - t2;
        }else{//recv started then message copied in buffer then send then end
            elaps = t3 - t1;
        }
        printf("My timing : %lu\t%f\n", size, elaps);
    }
    
    MPI_Finalize();
    
    free(buffer);
    
    return 0;
}
