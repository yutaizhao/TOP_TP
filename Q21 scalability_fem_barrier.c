#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void sum(double* x, int n){
    
    if(x[0] == -1 || x[n+1]== -1){
        printf("Prob values communications! \n");
        exit(0);
    }
    for (int i=1; i < n+1; i++){
        x[i] = (x[i-1] + 2*x[i] + x[i+1])/4 ;
    }
}

void communications(double *x, int n)
{
    int rank, size;
    int tag, left_neighbor, right_neighbor;
    MPI_Status status;
    MPI_Request req[4];
    
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    
    tag = 0;
    left_neighbor  = ( rank - 1 + size ) % size;
    right_neighbor = ( rank + 1 + size ) % size;
    
    MPI_Isend( &x[1], 1, MPI_DOUBLE, left_neighbor, tag, MPI_COMM_WORLD, req+0);
    MPI_Isend( &x[n], 1, MPI_DOUBLE, right_neighbor, tag, MPI_COMM_WORLD , req+1);
    MPI_Irecv( &x[0], 1, MPI_DOUBLE, left_neighbor, tag, MPI_COMM_WORLD,  req+2 );
    MPI_Irecv( &x[n + 1], 1, MPI_DOUBLE, right_neighbor, tag, MPI_COMM_WORLD,  req+3 );
    
    MPI_Waitall(4,req, MPI_STATUSES_IGNORE);

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
    double* X = (double *)calloc(n+2, sizeof(double));
    
    //initialisation
    if (complet_X != NULL && X != NULL) {
        
        //init of complet_X
        for (int i=0; i < N; i++){
            complet_X[i] = i;
        }
        
        //distribute subparts, init of X
        X[0] = -1;
        X[n+1] = -1;
        for (int i=0; i < n; i++){
            int index = rank*n + i;
            X[i+1] = complet_X[index];
        }
        
    } else {
        printf("Memory allocations failed!\n");
        return 1;
    }
    
    //sum
    for(int rep = 0; rep<max_rep ; rep++){
        
        if (rank == 0) {
            double t0 = MPI_Wtime();
            MPI_Barrier(MPI_COMM_WORLD);
            communications(X,n);
            sum(X, n);
            double t1 = MPI_Wtime();
            elaps[rep] = t1 - t0;
        }else{
            MPI_Barrier(MPI_COMM_WORLD);
            communications(X,n);
            sum(X, n);
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
