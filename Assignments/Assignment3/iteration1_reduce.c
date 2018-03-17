/******************************************************************************
 * FILE: assignment2.c
 * DESCRIPTION:  
 *  
 *   Anirban Das (dasa2@rpi.edu)
 *   03/23/2018
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <math.h>

#define ARRSIZE 1024//1073741824
#define BGQ 1  // when running BG/Q,

//#ifdef BGQ
//#include<hwi/include/bqc/A2_inlines.h>
//#else
#define GetTimeBase MPI_Wtime
//#endif

int          taskid, ntasks, ierr;
MPI_Status   status;
MPI_Request  send_request,recv_request;

void prepareArray(unsigned long long limit, unsigned long long *localArray);
unsigned long long arraySum(unsigned long long * array, unsigned long long size);

/* MAIN FUNCTION */
int main(int argc , char **argv) {

    /*========================================================================*/
    /* MPI Initialisation.                                                    */
    MPI_Init(&argc, &argv);

    //========================================================================
    /* Get the number of MPI tasks and the taskid of this task.               */
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&ntasks);



    unsigned long long chunksize;
    chunksize = ARRSIZE/ntasks;
    unsigned long long globalSum, localSum; 
    unsigned long long *localArray;
    localArray = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));

    if( argc != 1 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }


    prepareArray(chunksize, localArray);  
    localSum = arraySum(localArray, chunksize);


    double time_in_secs = 0;
    double processor_frequency = 1600000000.0;
    unsigned long long start_cycles=0;
    unsigned long long end_cycles=0;
    start_cycles= GetTimeBase();

    

    MPI_Reduce(&localSum, &globalSum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if(taskid==0){
        printf("\nSum --> %llu\n", globalSum);
        end_cycles= GetTimeBase();
        //printf("End cycles %llu\n",end_cycles );
        //printf("Start cycles %llu\n",start_cycles );
        time_in_secs = ((double)(end_cycles - start_cycles)) ;//processor_frequency;
        printf("Time taken for Ntasks= %d is %1.10f \n", taskid, time_in_secs/processor_frequency);
    }


    MPI_Finalize();
    free(localArray);
    return 0;
}


void prepareArray(unsigned long long limit, unsigned long long *localArray){
    unsigned long long i, j;
    j = taskid*limit;
    for (i = 0; i<limit; i++){
        localArray[i] = i+j;
    }
}

unsigned long long arraySum(unsigned long long * array, unsigned long long size){
    unsigned long long i, sum=0;
    for (i = 0; i< size; i++){
        sum += array[i];
    }
    return sum;
}

