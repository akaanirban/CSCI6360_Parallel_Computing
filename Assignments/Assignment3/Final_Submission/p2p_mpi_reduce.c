/*
 *   DESCRIPTION: Code for performing scaling test between the inbuilt 
 *                MPI_Reduce function for collective operations and our
 *                point to point communication MPI_P2P_reduce function
 *                to sum a 1 billion long array of numbers
 *  
 *   Anirban Das (dasa2@rpi.edu)
 *   03/23/2018
 *   Ans: 576460751766552576
 *   timings_p2p = [0.420027,0.210152,0.120292,0.052671,0.030676,0.020943,0.010322,0.007179]
*    timings_mpi = [0.419725,0.209963,0.118705,0.052526,0.029684,0.020122,0.003101,0.000735]
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <math.h>

#define ARRSIZE  1073741824
#define BGQ 1  // when running BG/Q,

//#ifdef BGQ
 //   #include<hwi/include/bqc/A2_inlines.h>
//#else
    #define GetTimeBase MPI_Wtime
//#endif

int          taskid, ntasks, ierr;
MPI_Status   status;
MPI_Request  send_request,recv_request;
unsigned long long *localArray, ownSum;

/*
*                                 FUNCTIONS
*
*    MPI_P2P_reduce : takes in the same arguments as MPI_Reduce and does the 
*                     same collective operations 
*    prepareArray   : takes in the limit = size of array and a pointer to the
*					  globally defined 'localArray' for each rank
*    srraySum       : returns the sum of an array
*/
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
    double time_in_secs = 0;
    double processor_frequency = 1600000000.0;
    unsigned long long start_cycles;
    unsigned long long end_cycles;

    unsigned long long chunksize;
    chunksize = ARRSIZE/ntasks;
    unsigned long long globalSum_p2p, globalSum_MPI;

    localArray = (unsigned long long*)calloc(chunksize, sizeof(unsigned long long));

    if( argc != 1 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }

    // prepare the local array for the current MPI rank
    prepareArray(chunksize, localArray);


    //---------------------  Point to point MPI_P2P_reduce  ------------------------------------
    if(taskid==0)
        start_cycles= GetTimeBase();

    MPI_P2P_reduce(&ownSum, &globalSum_p2p, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if(taskid==0){
            end_cycles = GetTimeBase();
            time_in_secs = ((double)(end_cycles - start_cycles))/processor_frequency;
            //printf("P2P_results-%llu::no_of_ranks-%d::time_in_secs-%f\n", globalSum_p2p, ntasks, time_in_secs);
            printf("%llu %1.10f\n", globalSum_p2p, time_in_secs);
   		
    }



   	// ---------------------- Standard MPI_Reduce----------------------------------------------
    if(taskid==0)
        start_cycles= GetTimeBase();

    ownSum = arraySum(localArray, chunksize);
    MPI_Reduce(&ownSum, &globalSum_MPI, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if(taskid==0){
            end_cycles = GetTimeBase();
            time_in_secs = ((double)(end_cycles - start_cycles))/processor_frequency;
            //printf("MPI_results-%llu::no_of_ranks-%d::time_in_secs-%f\n", globalSum_MPI, ntasks, time_in_secs);
            printf("%llu %1.10f\n", globalSum_MPI, time_in_secs);

   	}




    MPI_Finalize();
    free(localArray);
    return 0;
}


//function to prepare the local array for each rank
void prepareArray(unsigned long long limit, unsigned long long *localArray){
    unsigned long long i, j;
    j = taskid*limit;
    for (i = 0; i<limit; i++){
        localArray[i] = i+j;
    }
}


//function to calculate the array sum
unsigned long long arraySum(unsigned long long * array, unsigned long long size){
    unsigned long long i, sum=0;
    for (i = 0; i< size; i++){
        sum += array[i];
    }
    return sum;
}


//function takes in the same inputs as MPI_Reduce
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
    	*((unsigned long long *)sendbuf) = arraySum(localArray, chunksize);

    	toSendSum = *((unsigned long long *)sendbuf);
    	stride = 2; noSteps = log(ntasks)/log(2);
    	

    	// this hardcode is due to the weird log(x)/log(2) bug
    	// in Blue Gene giving result 1 less then it should
    	if(ntasks == 4096){
    		noSteps = 12;
    	}
    	else if(ntasks == 8192){
    		noSteps = 13;
    	}

    	//part where the point to point communication is done using binary tree structure
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