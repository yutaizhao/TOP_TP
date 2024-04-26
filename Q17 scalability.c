#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define C 7

void sum(double* x, double c, int n){
    for (int i=0; i < n; i++){
        x[i] += c;
    }
}

int main(int argc, char **argv){
    
    int rank,nbranks;
    
    size_t N = atoi(argv[1]);
    int max_rep = atoi(argv[2]);
    
    double elaps[max_rep];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nbranks);
    
    if (N%nbranks != 0) {
        printf("vector size need to be divisible by node numbers\n");
        return 1;
    }
    
    size_t n = N/nbranks ; //size of the sum for each node
    
    //allocation
    double* complet_X = (double *)calloc(N, sizeof(double));
    double* X = (double *)calloc(n, sizeof(double));

    //initialisation
    if (complet_X != NULL && X != NULL) {
        
        //init of complet_X
        for (int i=0; i < N; i++){
            complet_X[i] = i;
        }
        
        //distribute subparts, init of X
        for (int i=0; i < n; i++){
            int index = rank*n + i;
            X[i] = complet_X[index];
        }
        
    } else {
        printf("Memory allocations failed!\n");
        return 1;
    }
    
    //sum
    for(int rep = 0; rep<max_rep ; rep++){
                
        if (rank == 0) {
            double t0 = MPI_Wtime();
            sum( X , C , n );
            double t1 = MPI_Wtime();
            elaps[rep] = t1 - t0;
        }else{
            sum( X , C , n );
        }
        
    }
    
        
    if (rank == 0) {
        double total = 0.0;
        for(int j=0; j< max_rep; j++){
            total += elaps[j];
        }
        double elaps_mean = total/max_rep;
        printf("%lu\t%g\n", N, elaps_mean);
    }
    
    free(complet_X);
    free(X);
    
    MPI_Finalize();
    
    return 0;
}
