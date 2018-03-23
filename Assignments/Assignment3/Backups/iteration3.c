/*
 *   DESCRIPTION:  
 *  
 *   Anirban Das (dasa2@rpi.edu)
 *   03/23/2018
 *   Ans: 576460751766552576
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <math.h>

#define ARRSIZE  1073741824
#define BGQ 0  // when running BG/Q,

//#ifdef BGQ
   // #include<hwi/include/bqc/A2_inlines.h>
//      #include <A2_inlines.h>
//#else
    #define GetTimeBase MPI_Wtime
//#endif

int          taskid, ntasks, ierr;
MPI_Status   status;
MPI_Request  send_request,recv_request;
unsigned long long *localArray, ownSum;

int MPI_P2P_reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, int root, MPI_Comm comm);
void prepareArray(unsigned long long limit, unsigned long long* localArray);
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
unsigned long long  time_in_secs = 0;
    double processor_frequency = 1600000000.0;
    unsigned long long start_cycles;
    unsigned long long end_cycles;

    //if(taskid==0)
//      start_cycles= MPI_Wtime();;

    unsigned long long chunksize;
    chunksize = ARRSIZE/ntasks;
    unsigned long long globalSum;
//    unsigned long long *localArray; 
    //unsigned long long *globalsumArray;
    localArray = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));
    //globalsumArray = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));

    if( argc != 1 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }

    prepareArray(chunksize, localArray);
   // ownSum = arraySum(localArray, chunksize);


    if(taskid==0)
        start_cycles= GetTimeBase();

    MPI_P2P_reduce(&ownSum, &globalSum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

        if(taskid==0){
                printf("Final sum = %llu\n", globalSum);
                //end_cycles= MPI_Wtime();
                end_cycles = GetTimeBase();
                //printf("End cycles %llu\n",end_cycles );
                //printf("Start cycles %llu\n",start_cycles );
                time_in_secs = (end_cycles - start_cycles);///processor_frequency;
        printf("Time taken for Ntask= %d is %llu \n", taskid, time_in_secs);//processor_frequency);

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


int MPI_P2P_reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, int root, MPI_Comm comm){

        int taskid, ntasks, m=0;
        unsigned long long stride,noSteps;

        MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
        MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
        MPI_Status   status;
        MPI_Request  send_request,recv_request;


        unsigned long long otherSum, toSendSum, chunksize;
    chunksize = ARRSIZE/ntasks;
    //ownSum = arraySum(localArray, chunksize);
    *((unsigned long long *)sendbuf) = arraySum(localArray, chunksize);

    printf("Local sum for taskid = %d is %llu\n", taskid, ownSum);
    toSendSum = *((unsigned long long *)sendbuf);
    stride = 2; noSteps = log(ntasks)/log(2);
        while(noSteps){
                if (taskid ==0)
                m++;
        if (taskid%stride ==0){
            ierr=MPI_Irecv(&otherSum, count ,MPI_UNSIGNED_LONG_LONG,
                   taskid+stride/2 ,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request); //receive from higher stride thing
            ierr=MPI_Wait(&recv_request,&status);
            toSendSum = toSendSum + otherSum;
        }
        else if ((taskid-stride/2)%stride ==0){
            ierr=MPI_Isend(&toSendSum, count ,MPI_UNSIGNED_LONG_LONG,
                   taskid-stride/2,0,MPI_COMM_WORLD,&send_request);
            ierr=MPI_Wait(&send_request,&status);
        }
            stride=stride*2;
        noSteps-=1;
                 }
                *((unsigned long long *)recvbuf) = toSendSum;
                return 0;
}