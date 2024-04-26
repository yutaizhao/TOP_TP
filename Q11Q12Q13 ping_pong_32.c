#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
    int rank,nbranks;
    size_t size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nbranks);
    if (nbranks != 2)
    {
        printf("ranks need to be 2!\n");
        MPI_Finalize();
        return 1;
    }
    
    for(int i = 1; i<=20 ; i++){
        
        size = 32*i;
        
        char* buffer = (char *)malloc(size * sizeof(char));
        if (buffer == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }
        for(int i=0; i<size; i++){
            buffer[i] = 'c';
        }
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            double t0 = MPI_Wtime();
            MPI_Send(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            double t1 = MPI_Wtime();
            printf("%lu\t%g\n", size, t1 - t0);
        } else if (rank == 1) {
            MPI_Recv(buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
        
        free(buffer);
    }
    
    MPI_Finalize();
    
    return 0;
}
