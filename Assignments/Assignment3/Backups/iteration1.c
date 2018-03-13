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

#define ARRSIZE 1024
#define BGQ 1  // when running BG/Q,

//#ifdef BGQ
//#include<hwi/include/bqc/A2_inlines.h>
//#else
//#define GetTimeBase MPI_Wtime
//#endif

int          taskid, ntasks, ierr;
MPI_Status   status;
MPI_Request  send_request,recv_request;
unsigned long long *globalArray;

/* GENERATE AND PROPAGATE CALCULATION FUNCTIONS
* single_gen_prop        : Generate individual generate and propagate 
*/
int MPI_P2P_reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
               MPI_Op op, int root, MPI_Comm comm);
void prepareArray(unsigned long long limit);
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
    unsigned long long sum;
    unsigned long long *mpidata; 
    unsigned long long *globalsumArray;
    mpidata = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));
    globalsumArray = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));
    
    if( argc != 1 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }

    if(taskid==0){
        globalArray = (unsigned long long*)calloc(ARRSIZE, sizeof(unsigned long long));
        prepareArray(ARRSIZE);
    }


    if(ntasks !=1) {
        MPI_Scatter(globalArray,
                chunksize,
                MPI_LONG_LONG,
                mpidata,
                chunksize,
                MPI_LONG_LONG,
                0,
                MPI_COMM_WORLD);    
    
        printf("\nRank = %d , Starting index = %llu, chunk size = %llu\n", taskid, mpidata[0], chunksize);
        MPI_Reduce(mpidata, globalsumArray, chunksize,MPI_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);
    }

    
    if(taskid==0){
        if(ntasks==1){
            sum = arraySum(globalArray, chunksize);
        }
        else{
            sum = arraySum(globalsumArray, chunksize);
        }
        printf("\nSum --> %llu\n", sum);
    }



    MPI_Finalize();
    free(globalArray);
    return 0;
}



/*calculate super super section carry*/
//void super_super_section_carry(unsigned int *sssgm, unsigned int *ssspm, unsigned *ssscm){
//        ierr=MPI_Isend(&toSend,1,MPI_UNSIGNED,
//               taskid+1,0,MPI_COMM_WORLD,&send_request);
//        ierr=MPI_Wait(&send_request,&status);
//
//}

void prepareArray(unsigned long long limit){
    unsigned long long i;
    for (i =0; i<limit; i++){
        globalArray[i] = i;
    }
}

unsigned long long arraySum(unsigned long long * array, unsigned long long size){
    unsigned long long i, sum=0;
    for (i = 0; i< size; i++){
        sum += array[i];
    }
    return sum;
}

/*
stride = 2; noSteps = log(ARRSIZE); 
while(noSteps){
    for(i = 0;i<ntasks;i++){
        if (taskid%stride ==0){
            ierr=MPI_Irecv(&sssCarry,1 ,MPI_UNSIGNED,
                   taskid+stride/2 ,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request); //receive from higher stride thing
            toSend = ownSum + sssCarry;
        }
        else if ((taskid-stride/2)%stride ==0){
            ierr=MPI_Isend(&toSend,1,MPI_UNSIGNED,
                   taskid-stride/2,0,MPI_COMM_WORLD,&send_request);
            ierr=MPI_Wait(&send_request,&status);
        }
        stride=stride*2;
    }
    noSteps-=1;
}
*/
