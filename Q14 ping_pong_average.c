#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_REP 10000
#define MAX_MULTI 200

int main(int argc, char **argv){
    
    int rank,nbranks;
    double elaps[MAX_REP];
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
    
    for(int i = 1; i< MAX_MULTI ; i+=10){
        
        size = 32*i;
        
        char* buffer = (char *)malloc(size * sizeof(char));
        if (buffer == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }
        for(int i=0; i<size; i++){
            buffer[i] = 'c';
        }
        
        for(int rep = 0; rep<MAX_REP ; rep++){
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank == 0) {
                double t0 = MPI_Wtime();
                MPI_Send(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                double t1 = MPI_Wtime();
                elaps[rep] = t1 - t0;
            } else if (rank == 1) {
                MPI_Recv(buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(buffer, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }
        
        if (rank == 0) {
            double total = 0.0;
            for(int j=0; j< MAX_REP; j++){
                total += elaps[j];
            }
            double elaps_mean = total/MAX_REP;
            printf("%lu\t%g\n", size, elaps_mean);
        }
        
        free(buffer);
    }
    
    MPI_Finalize();
    
    return 0;
}
