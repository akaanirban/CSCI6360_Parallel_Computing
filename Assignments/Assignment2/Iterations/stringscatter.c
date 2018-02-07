#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_numbers(char *first_num, char *second_num);

int main(int argc, char **argv) {
    int size, rank;
    char         first_num[67]; /* the first 64digit hex number      */
    char         second_num[67]; /* the second 64digit hex number    */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char *globaldata=NULL;
    char *localdata;
    localdata = malloc((size + 1) * sizeof(char) );

    if (rank == 0) {
        //globaldata = malloc(size*2 * sizeof(char) );
        //for (int i=0; i<size*2; i++)
        //   globaldata[i] = i+'0';

        //printf("Processor %d has data: ", rank);
        //for (int i=0; i<size*2; i++)
        //    printf("%c ", globaldata[i]);
        //printf("\n");

    read_numbers( first_num, second_num);
    printf("1st num %s\n",first_num );

    }

    //MPI_Scatter(globaldata, 2, MPI_CHAR, localdata, 2, MPI_CHAR, 0, MPI_COMM_WORLD);
    //printf("%d\n", localdata[0]);
    //printf("Processor %d has data %c %c\n", rank, localdata[0], localdata[1]);

    MPI_Scatter(first_num, size, MPI_CHAR, localdata, size, MPI_CHAR, 0, MPI_COMM_WORLD);
    //printf("%d\n", localdata[0]);
    localdata[size] = '\0';
    printf("Processor %d has data %s\n", rank, localdata);


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

/*read input numbers */
void read_numbers(char *first_num, char *second_num){
    //printf("Please enter 1st hex number:");
    //scanf("%s", first_num); 
    fgets(first_num, 66, stdin); /* read 1st number */
    first_num[strcspn(first_num, "\n")]=0;
    //printf("Please enter 2nd hex number:");
    //scanf("%s", second_num);
    fgets(second_num, 66, stdin); /* read 2nd number */
    second_num[strcspn(second_num, "\n")]=0;
    //printf("1st num %s and second num %s", first_num, second_num);
}