#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *globaldata=NULL;
    int *localdata;
    localdata = malloc(2 * sizeof(int) );

    if (rank == 0) {
        globaldata = malloc(size*2 * sizeof(int) );
        for (int i=0; i<size*2; i++)
            globaldata[i] = 2*i+1;

        printf("Processor %d has data: ", rank);
        for (int i=0; i<size*2; i++)
            printf("%d ", globaldata[i]);
        printf("\n");
    }

    MPI_Scatter(globaldata, 2, MPI_INT, localdata, 2, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("%d\n", localdata[0]);
    printf("Processor %d has data %d %d\n", rank, localdata[0], localdata[1]);
    //localdata[0] *= 2;
    //printf("Processor %d doubling the data, now has %d  %d\n", rank, localdata[0], localdata[1]);

    /*MPI_Gather(&localdata, 1, MPI_INT, globaldata, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Processor %d has data: ", rank);
        for (int i=0; i<size; i++)
            printf("%d ", globaldata[i]);
        printf("\n");
    }
*/
    if (rank == 0)
        free(globaldata);

    MPI_Finalize();
    return 0;
}