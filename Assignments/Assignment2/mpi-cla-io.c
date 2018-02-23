/******************************************************************************
 * FILE: assignment2.c
 * DESCRIPTION:  
 *   This code takes in two 262144 digit HEX numbers and uses a 1048576 bit 
 *   hierarchical CLA with 16 bit blocks to compute their sum using MPI
 *   to parallelize the calculations
 *   Anirban Das (dasa2@rpi.edu)
 *   02/20/2018
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <math.h>

#define BLKSIZE 16
#define MSGSIZE 262144
int          taskid, ntasks, ierr;
MPI_Status   status;
MPI_Request  send_request,recv_request;
unsigned int sssCarry;

/*Main CLA routine having all steps*/
void cla(char *first_num, char *second_num, char *converted_hex,
         unsigned long chunksize);
/*Read the two numbers from stdin*/
void read_numbers(char *first_num, char *second_num, char *ipfilename);

/* GENERATE AND PROPAGATE CALCULATION FUNCTIONS
* single_gen_prop        : Generate individual generate and propagate 
* group_gen_prop         : Generate group generate and propagate
* section_gen_prop       : Generate section generate and propagate
* super_section_gen_prop : Generate super section generate and propagate
*/
void single_gen_prop(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi);
void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj);
void section_gen_prop(unsigned int *ggj, unsigned int *gpj, unsigned int *sgk, unsigned int *spk);
void super_section_gen_prop(unsigned int *sgk, unsigned int *spk, unsigned int *ssgl, unsigned int *sspl);
void super_super_section_gen_prop(unsigned int *ssgl, unsigned int *sspl, unsigned int *sssgm, unsigned int *ssspm);



/* CARRY CALCULATION FUNCTIONS
* single_carry        : Generate individual carries 
* group_carry         : Generate group carries
* section_carry       : Generate section carries
* super_section_carry : Generate super section carries
*/
void single_carry(unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *gcj);
void group_carry(unsigned int *ggj, unsigned int *gpj, unsigned int *gcj, unsigned int *sck);
void section_carry(unsigned int *sgk, unsigned int *spk, unsigned int *sck, unsigned int *sscl);
void super_section_carry(unsigned int *ssgl, unsigned int *sspl, unsigned *sscl, unsigned int *ssscm);
void super_super_section_carry(unsigned int *sssgm, unsigned int *ssspm, unsigned int *ssscm);

/* Generate the actual sum from the generate, proppagate and carries*/
void generate_sum(char *binary_first_num, char *binary_second_num, unsigned int *ci, unsigned int *sum, unsigned int *gi, unsigned int *pi);

//helper functions
void hex_to_bin(char *hex, char *binary_num);
void bin_to_hex(unsigned int *binary, char *converted_hex, int size);
void reverse_string(char *str);
void simple_adder(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *sum);
unsigned int propagateChain(unsigned int *propagate, unsigned long start, unsigned long stop);

/* MAIN FUNCTION */
int main(int argc , char **argv) {

    char         *first_num; /* the first hex number      */
    char         *second_num; /* the second hex number    */
    char         *converted_hex; /* the result sum hex number */
    unsigned long chunksize;       /*the number of bits in binary*/

	/*========================================================================*/
	/* MPI Initialisation.                                                    */
	MPI_Init(&argc, &argv);

	/*========================================================================*/
	/* Get the number of MPI tasks and the taskid of this task.               */
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

    if( argc != 3 )
    {
      printf("Not sufficient arguments, only %d found \n", argc);
      exit(-1);
    }
	
    //double t1, t2;
	//t1 = MPI_Wtime();


    chunksize = MSGSIZE*4/ntasks; //number of binary bits per rank
    char *binary_first_num; /*variable for binary version of 1st number*/
    char *binary_second_num; /*variable for binary version of 2nd number*/
    char *sub_first_num;
    char *sub_second_num;
    sub_first_num = (char *)calloc((chunksize+1),sizeof(char));
    sub_second_num = (char *)calloc((chunksize+1),sizeof(char));
    converted_hex = (char *)calloc((MSGSIZE/ntasks),sizeof(char));

	/*========================================================================*/
	/*code for rank zero											          */
	if ( taskid == 0 ){
        /*read the two numbers from input redirected file                         */
        first_num = (char *)calloc((MSGSIZE+2),sizeof(char));
        second_num = (char *)calloc((MSGSIZE+2),sizeof(char));
        read_numbers( first_num, second_num, argv[1]);
      
        binary_first_num = (char *)calloc((MSGSIZE*4+1),sizeof(char));
        binary_second_num = (char *)calloc((MSGSIZE*4+1),sizeof(char));

        /*convert the hex numbers to binary*/
        hex_to_bin(first_num, binary_first_num);
        //        printf("\n conversion don1e \n" );
        hex_to_bin(second_num, binary_second_num);
        //printf("\n conversion done \n" );
    }

    /*scatter the 1st number from the rank 0 node */
    MPI_Scatter(binary_first_num,
                chunksize,
                MPI_CHAR,
                sub_first_num,
                chunksize,
                MPI_CHAR,
                0,
                MPI_COMM_WORLD);
    sub_first_num[chunksize] = '\0';
    /*scatter the 2nd number from the rank 0 node */
    MPI_Scatter(binary_second_num,
                chunksize,
                MPI_CHAR,
                sub_second_num,
                chunksize,
                MPI_CHAR,
                0,
                MPI_COMM_WORLD);
    sub_second_num[chunksize] = '\0';

    /*========================================================================*/
    /*code for rest of the ranks                                              */
    if (taskid != 0){
        ierr=MPI_Irecv(&sssCarry,1 ,MPI_UNSIGNED,
               taskid-1,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request);
    }

    /*all the main cla routine to do all teh cla steps and generate 
    the converted_hex, which is the result*/
    cla(sub_first_num, sub_second_num, converted_hex, chunksize);
    //MPI_Barrier(MPI_COMM_WORLD);

    char *actualNumber;
    actualNumber = (char *)calloc((MSGSIZE),sizeof(char));

    /* MPI GATHER to gather parts of the hex sum from the rest of the Ranks   */
    ierr=MPI_Gather(converted_hex, MSGSIZE/ntasks ,MPI_CHAR,
                   actualNumber,MSGSIZE/ntasks, MPI_CHAR,
                   0,MPI_COMM_WORLD);

    
    if(taskid ==0){
        FILE *output_file=NULL; 
        reverse_string(actualNumber);
    /*write output to the file------------------------------------------------*/
        output_file = fopen( argv[2], "w");
        fprintf( output_file, "%s", actualNumber);
        fclose(output_file);
    }


    free(sub_first_num);
    free(sub_second_num);
    free(converted_hex);
    free(actualNumber);
    if(taskid ==0){
        free(first_num); free(second_num);
        free(binary_first_num); free(binary_second_num);    
    }
    //t2 = MPI_Wtime();
   // printf("MPI_Wtime measured at %d task at %d ranks, 1 second sleep to be: %1.4f\n", taskid, ntasks, t2-t1);fflush(stdout);
    MPI_Finalize();
    return 0;
}

/*main routine doing all the CLA steps and return the sum*/ 
void cla(char *binary_first_num, char *binary_second_num, 
            char *converted_hex, unsigned long chunksize){
    int i=0; /*dummy value for counting stuff*/
    unsigned int *gi; /*variable for individual generate*/
    gi = (unsigned int *)calloc(chunksize, sizeof(unsigned int));
    unsigned int *pi; /*variable for individual propagate*/
    pi = (unsigned int *)calloc(chunksize, sizeof(unsigned int));
    unsigned int *ci; /*variable for individual carry*/
    ci = (unsigned int *)calloc(chunksize, sizeof(unsigned int));

    unsigned int *ggj; /*variable for group generate*/
    ggj = (unsigned int *)calloc(chunksize/BLKSIZE, sizeof(unsigned int));
    unsigned int *gpj; /*variable for group propagate*/
    gpj = (unsigned int *)calloc(chunksize/BLKSIZE, sizeof(unsigned int));
    unsigned int *gcj; /*variable for group carry*/
    gcj = (unsigned int *)calloc(chunksize/BLKSIZE, sizeof(unsigned int));

    unsigned int *sgk; /*variable for section generate*/
    sgk = (unsigned int *)calloc(chunksize/pow(BLKSIZE,2), sizeof(unsigned int));
    unsigned int *spk; /*variable for section propagate*/
    spk = (unsigned int *)calloc(chunksize/pow(BLKSIZE,2), sizeof(unsigned int));
    unsigned int *sck; /*variable for section carry*/
    sck = (unsigned int *)calloc(chunksize/pow(BLKSIZE,2), sizeof(unsigned int));

    unsigned int *ssgl; /*variable for super section generate*/
    ssgl = (unsigned int *)calloc(chunksize/pow(BLKSIZE,3), sizeof(unsigned int));
    unsigned int *sspl; /*variable for super section propagate*/
    sspl = (unsigned int *)calloc(chunksize/pow(BLKSIZE,3), sizeof(unsigned int));
    unsigned int *sscl; /*variable for super section carry*/
    sscl = (unsigned int *)calloc(chunksize/pow(BLKSIZE,3), sizeof(unsigned int));

    unsigned int *sssgm; /*variable for super super section generate*/
    sssgm = (unsigned int *)calloc(chunksize/pow(BLKSIZE,4), sizeof(unsigned int));
    unsigned int *ssspm; /*variable for super super section propagate*/
    ssspm = (unsigned int *)calloc(chunksize/pow(BLKSIZE,4), sizeof(unsigned int));
    unsigned int *ssscm; /*variable for super super section carry*/
    ssscm = (unsigned int *)calloc(chunksize/pow(BLKSIZE,4), sizeof(unsigned int));

    unsigned int *sum; /*variable for the generated sum in REVERSE order in BINARY*/
    sum = (unsigned int *)calloc(chunksize, sizeof(unsigned int));
    unsigned int *actual_sum; /*variable for the generated sum in CORRECT order in BINARY*/
    actual_sum = (unsigned int *)calloc(chunksize, sizeof(unsigned int));

    /*calculate individual generate and propagate*/
    single_gen_prop(binary_first_num, binary_second_num, gi, pi);

    /* simple ripple carry adder code to validate results */
    /* 
    simple_adder(binary_first_num, binary_second_num, gi, pi, ci, sum); 
    */
    
    /*calculate group generates and propagates*/
    group_gen_prop(gi, pi, ggj, gpj);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate section generate and propagate*/
    section_gen_prop(ggj, gpj, sgk, spk);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate super section generate propagate*/
    super_section_gen_prop(sgk, spk, ssgl, sspl);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate super super section generate propagate*/
    super_super_section_gen_prop(ssgl, sspl, sssgm, ssspm);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate super super section carry*/
    super_super_section_carry(sssgm, ssspm, ssscm);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate super section carry*/
    super_section_carry(ssgl, sspl, sscl, ssscm);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate section carry*/
    section_carry(sgk, spk, sck, sscl);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate group carries*/
    group_carry(ggj, gpj, gcj, sck);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate individual carries*/
    single_carry(gi, pi, ci, gcj);
    //MPI_Barrier(MPI_COMM_WORLD);

    /*calculate the actual sum*/
    generate_sum(binary_first_num, binary_second_num, ci, sum, gi, pi);
    //MPI_Barrier(MPI_COMM_WORLD);


    /*reverse the number to form the actual number*/
    for(i=chunksize;i>0;i--){ 
        actual_sum[chunksize-i] = sum[i-1];
    }
    
    /* convert the binary result to HEX*/
    bin_to_hex(actual_sum, converted_hex, chunksize);

    /*free the binary num alloctions*/
    free(gi); free(pi); free(ci);
    free(ggj); free(gpj); free(gcj);
    free(sgk); free(spk); free(sck);
    free(ssgl); free(sspl); free(sscl);
    free(sssgm); free(ssspm); free(ssscm);
    free(sum); free(actual_sum);
}


//read input numbers from the file
void read_numbers(char *first_num, char *second_num, char *ipfilename){
    //printf("Please enter 1st hex number:");
    //scanf("%s", first_num); 
    FILE* fp=NULL; 
    if( (fp = fopen( ipfilename, "r")) == NULL )
    {
      printf("Failed to open input data file: %s \n", ipfilename);
    }

    //fscanf( fp, "%s %s", first_num, second_num);
    fgets(first_num, MSGSIZE+2, fp);
    fgets(second_num, MSGSIZE+1, fp);
    fclose(fp);
}

/*hexadecimal to binary converter*/
void hex_to_bin(char *hex, char *binary_num){
    int i = 0, j=0;
    for(i=0; i<MSGSIZE; i++){
        switch(tolower(hex[i])){  /* Simple switch case to convert each hex digit */
            case '0':
                binary_num[j] = '0';
                binary_num[j+1] = '0';
                binary_num[j+2] = '0';
                binary_num[j+3] = '0';
                break;
            case '1':
                binary_num[j] = '0';
                binary_num[j+1] = '0';
                binary_num[j+2] = '0';
                binary_num[j+3] = '1';
                break;
            case '2':
                binary_num[j] = '0';
                binary_num[j+1] = '0';
                binary_num[j+2] = '1';
                binary_num[j+3] = '0';
                break;
            case '3':
                binary_num[j] = '0';
                binary_num[j+1] = '0';
                binary_num[j+2] = '1';
                binary_num[j+3] = '1';
                break;
            case '4':
                binary_num[j] = '0';
                binary_num[j+1] = '1';
                binary_num[j+2] = '0';
                binary_num[j+3] = '0';
                break;
            case '5':
                binary_num[j] = '0';
                binary_num[j+1] = '1';
                binary_num[j+2] = '0';
                binary_num[j+3] = '1';
                break;
            case '6':
                binary_num[j] = '0';
                binary_num[j+1] = '1';
                binary_num[j+2] = '1';
                binary_num[j+3] = '0';
                break;
            case '7':
                binary_num[j] = '0';
                binary_num[j+1] = '1';
                binary_num[j+2] = '1';
                binary_num[j+3] = '1';
                break;
            case '8':
                binary_num[j] = '1';
                binary_num[j+1] = '0';
                binary_num[j+2] = '0';
                binary_num[j+3] = '0';
                break;
            case '9':
                binary_num[j] = '1';
                binary_num[j+1] = '0';
                binary_num[j+2] = '0';
                binary_num[j+3] = '1';
                break;
            case 'a':
                binary_num[j] = '1';
                binary_num[j+1] = '0';
                binary_num[j+2] = '1';
                binary_num[j+3] = '0';
                break;
            case 'b':
                binary_num[j] = '1';
                binary_num[j+1] = '0';
                binary_num[j+2] = '1';
                binary_num[j+3] = '1';
                break;
            case 'c':
                binary_num[j] = '1';
                binary_num[j+1] = '1';
                binary_num[j+2] = '0';
                binary_num[j+3] = '0';
                break;
            case 'd':
                binary_num[j] = '1';
                binary_num[j+1] = '1';
                binary_num[j+2] = '0';
                binary_num[j+3] = '1';
                break;
            case 'e':
                binary_num[j] = '1';
                binary_num[j+1] = '1';
                binary_num[j+2] = '1';
                binary_num[j+3] = '0';
                break;
            case 'f':
                binary_num[j] = '1';
                binary_num[j+1] = '1';
                binary_num[j+2] = '1';
                binary_num[j+3] = '1';
                break;
        }
        j+=4;
    }
    //return string in reverse order
    reverse_string(binary_num);
}

/*binary to hexadecimal converter*/
void bin_to_hex(unsigned int *binary, char *converted_hex, int size){
    int i = 0, k=0;
    for(i=0; i<size; i=i+4){
        char temp[5];
        /* Create 4bit binary block */
        temp[0] = binary[i] + '0';
        temp[1] = binary[i+1] + '0';
        temp[2] = binary[i+2] + '0';
        temp[3] = binary[i+3] + '0';
        temp[4] = '\0';
        /* Simple if else to match each 4bit binary block to hex */
        if(strcmp(temp, "0000")==0){
            converted_hex[k] = '0';
        } else if(strcmp(temp, "0001")==0){
            converted_hex[k] = '1';
        } else if(strcmp(temp, "0010")==0){
            converted_hex[k] = '2';
        } else if(strcmp(temp, "0011")==0){
            converted_hex[k] = '3';
        } else if(strcmp(temp, "0100")==0){
            converted_hex[k] = '4';
        } else if(strcmp(temp, "0101")==0){
            converted_hex[k] = '5';
        } else if(strcmp(temp, "0110")==0){
            converted_hex[k] = '6';
        } else if(strcmp(temp, "0111")==0){
            converted_hex[k] = '7';
        } else if(strcmp(temp, "1000")==0){
            converted_hex[k] = '8';
        } else if(strcmp(temp, "1001")==0){
            converted_hex[k] = '9';
        } else if(strcmp(temp, "1010")==0){
            converted_hex[k] = 'A';
        } else if(strcmp(temp, "1011")==0){
            converted_hex[k] = 'B';
        } else if(strcmp(temp, "1100")==0){
            converted_hex[k] = 'C';
        } else if(strcmp(temp, "1101")==0){
            converted_hex[k] = 'D';
        } else if(strcmp(temp, "1110")==0){
            converted_hex[k] = 'E';
        } else if(strcmp(temp, "1111")==0){
            converted_hex[k] = 'F';
        }
        else
            printf("^^^^^^^^^^%s^^^^^^^", temp);
        k = k+1;
    }
    converted_hex[k] = '\0';
    reverse_string(converted_hex);
}

/*inplace reverse string*/
void reverse_string(char *str){ 
    /* if null or empty skip*/
    if (str == 0 || *str == 0){
        return;
    }
    /* get range of string*/
    //printf("\n%s\n", str);
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;
    /* reverse */
    while (end > start){
        temp = *start;
        *start = *end;
        *end = temp;
        ++start;
        --end;
    }
    //printf("\n%s\n", str);
}

/*calculate individual generate and propagates*/
void single_gen_prop(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi){
    unsigned long i;
    for(i=0;i<MSGSIZE*4/ntasks; i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        gi[i] = a & b; /* Create the generate */
        pi[i] = a ^ b; /* Create the propagate */
    }
}

unsigned int propagateChain(unsigned int *propagate, unsigned long start, unsigned long stop){
    unsigned int count, bitValue;
    //bitValue = propagate[stop+1];
    bitValue = propagate[start];
    for (count = start-1; count> stop; count--){
       bitValue = bitValue & propagate[count];
    }
    //for(count = stop+2; count<= start; count++){
    //    bitValue = bitValue & propagate[count];
    //}


    return bitValue;    
}

/*calculate group generates and propagates*/
void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj){
    unsigned long i=0, j=0;//,c;
    for(i=0; i<MSGSIZE*4/ntasks; i=i+BLKSIZE){  /* Each 16 individual ith bits makes jth group */
       ggj[j] = (gi[i+15]) 
                | (gi[i+14]&pi[i+15]) 
                | (gi[i+13]&pi[i+15]&pi[i+14]) 
                | (gi[i+12]&pi[i+15]&pi[i+14]&pi[i+13]) 
                | (gi[i+11]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12])
                | (gi[i+10]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11])
                | (gi[i+9]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10])
                | (gi[i+8]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9])
                | (gi[i+7]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8])
                | (gi[i+6]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7])
                | (gi[i+5]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6])
                | (gi[i+4]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5])
                | (gi[i+3]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5]&pi[i+4])
                | (gi[i+2]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5]&pi[i+4]&pi[i+3])
                | (gi[i+1]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5]&pi[i+4]&pi[i+3]&pi[i+2])
                | (gi[i]&pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5]&pi[i+4]&pi[i+3]&pi[i+2]&pi[i+1])
                ;
    
        //ggj[j] = gi[i+BLKSIZE-1];
       //for(c = BLKSIZE-2; c>=0; c--){
      //      ggj[j] = ggj[j] | (gi[i+c]&propagateChain(pi, i+BLKSIZE-1, i+c));
       //     printf("stop is %d\n",  c);
      //      if (c==0){ break;}
      // }
        gpj[j] = pi[i+15]&pi[i+14]&pi[i+13]&pi[i+12]&pi[i+11]&pi[i+10]&pi[i+9]&pi[i+8]&pi[i+7]&pi[i+6]&pi[i+5]&pi[i+4]&pi[i+3]&pi[i+2]&pi[i+1]&pi[i];
      // gpj[j] = propagateChain(pi, i+BLKSIZE-1, i-1);
        j = j+1;
    }

}

/*calculate section generate and propagate*/
void section_gen_prop(unsigned int *ggj, unsigned int *gpj, unsigned int *sgk, unsigned int *spk){
    unsigned long j=0, k=0, c;
    for(j=0; j<MSGSIZE*4/(ntasks*BLKSIZE); j=j+BLKSIZE){  /* Each 16 jth groups makes kth section */
        sgk[k] = ggj[j+BLKSIZE-1];
        for(c = BLKSIZE-2; c>=0; c--){
            sgk[k] = sgk[k] | (ggj[j+c]&propagateChain(gpj, j+BLKSIZE-1, j+c));
            if (c==0){ break;}
        }
        spk[k] = propagateChain(gpj, j+BLKSIZE-1, j-1);
        k = k+1;
    }
}

/*calculate super section generate and propagate*/
void super_section_gen_prop(unsigned int *sgk, unsigned int *spk, unsigned int *ssgl, unsigned int *sspl){
    unsigned long k=0, l=0, c;
    for(k=0; k<MSGSIZE*4/(ntasks*pow(BLKSIZE,2)); k=k+BLKSIZE){  /* Each 16 kth section makes lth super section */
        ssgl[l] = sgk[k+BLKSIZE-1];
        for(c = BLKSIZE-2; c>=0; c--){
            ssgl[l] = ssgl[l] | (sgk[k+c]&propagateChain(spk, k+BLKSIZE-1, k+c));
            if (c==0){ break;}
        }
        sspl[l] = propagateChain(spk, k+BLKSIZE-1, k-1);
        l = l+1;
    }    
}

/*calculate super super section generate and propagate*/
void super_super_section_gen_prop(unsigned int *ssgl, unsigned int *sspl, unsigned int *sssgm, unsigned int *ssspm){
    unsigned long l=0, m=0, c;
    for(l=0; l<MSGSIZE*4/(ntasks*pow(BLKSIZE,3)); l=l+BLKSIZE){  /* Each 16 lth super section makes mth super super section */
        sssgm[m] = ssgl[l+BLKSIZE-1];
        for(c = BLKSIZE-2; c>=0; c--){
            sssgm[m] = sssgm[m] | (ssgl[l+c]&propagateChain(sspl, l+BLKSIZE-1, l+c));
            if (c==0){ break;}
        }
        ssspm[m] = propagateChain(sspl, l+BLKSIZE-1, l-1);
        m = m+1;
    }    
}

/*calculate super super section carry*/
void super_super_section_carry(unsigned int *sssgm, unsigned int *ssspm, unsigned *ssscm){
    int m;

    for(m = 0; m < BLKSIZE/ntasks; m++ ){
        if(m == 0) { /*1st super super section*/
            if(taskid ==0){
                ssscm[m] = sssgm[m] | (ssspm[m] & 0);
            }
            else {
                ierr=MPI_Wait(&recv_request,&status);
                ssscm[m] = sssgm[m] | (ssspm[m] & sssCarry);
            }
        }
        else{
            ssscm[m] = sssgm[m] | (ssspm[m] & ssscm[m-1]);
        }
    }
    unsigned int toSend = ssscm[BLKSIZE/ntasks -1];
    if (taskid != ntasks -1) {
    /* this is not the last supersection block             
    *  send the super super section carry to the next Ranks
    */ 
        ierr=MPI_Isend(&toSend,1,MPI_UNSIGNED,
               taskid+1,0,MPI_COMM_WORLD,&send_request);
        ierr=MPI_Wait(&send_request,&status);
    }

}

/*calculate super section carry*/
void super_section_carry(unsigned int *ssgl, unsigned int *sspl, unsigned *sscl, unsigned int *ssscm){
    int l;
    for(l=0; l<pow(BLKSIZE,2)/ntasks; l++){
        if(l==0){
            if(taskid ==0 ){ /* i.e. this is the first block, */
                sscl[l] = ssgl[l] | (sspl[l] & 0); /* since Super Section Carry[-1] = 0*/
            } 
            else
                sscl[l] = ssgl[l] | (sspl[l] & sssCarry);
        }
        else {
            if (l%BLKSIZE ==0){
                sscl[l] = ssgl[l] | (sspl[l] & ssscm[(l)/BLKSIZE-1]);
            }
            else {
                sscl[l] = ssgl[l] | (sspl[l] & sscl[l-1]); 
            }
        }
    }
}

/*calculate section carries*/
void section_carry(unsigned int *sgk, unsigned int *spk, unsigned int *sck, unsigned int *sscl){
    int k;
    for(k=0; k<pow(BLKSIZE,3)/ntasks; k++){
        if (k==0){
            if(taskid == 0 )
                sck[k] = sgk[k] | (spk[k] & 0); /* since Section Carry[-1] = 0*/
            else
                sck[k] = sgk[k] | (spk[k] & sssCarry);
        }
        else {
            if (k%BLKSIZE ==0 ){
                sck[k] = sgk[k] | (spk[k] & sscl[(k)/BLKSIZE-1]);
            }
            else{
                sck[k] = sgk[k] | (spk[k] & sck[k-1]);
            }
        }
    }
}

/*calculate group carries*/
void group_carry(unsigned int *ggj, unsigned int *gpj, unsigned int *gcj, unsigned int *sck){
    int j;
    for(j=0; j<pow(BLKSIZE,4)/ntasks; j++){
        if(j==0){
            if(taskid ==0)
                gcj[j] = ggj[j] | (gpj[j] & 0); /* since Group Carry[-1] = 0*/
            else{
                gcj[j] = ggj[j] | (gpj[j] & sssCarry);
            }
        }
        else {
            if ((j)%BLKSIZE ==0 ){
                gcj[j] = ggj[j] | (gpj[j] & sck[(j)/BLKSIZE-1]);
            }
            else{
                gcj[j] = ggj[j] | (gpj[j] & gcj[j-1]);
            }
        }
    }
}

/*calculate individual carries*/
void single_carry(unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *gcj){
    int i;
    for(i=0; i<pow(BLKSIZE,5)/ntasks;i++){
        if(i==0){
            if(taskid ==0)
                ci[i] = gi[i] | (pi[i] & 0); /* since individual Carry[-1] = 0*/
            else
                ci[i] = gi[i] | (pi[i] & sssCarry);
        }
        else {
            if ((i)%BLKSIZE ==0){
                ci[i] = gi[i] | (pi[i] & gcj[(i)/BLKSIZE-1]);
            }
            else{
                ci[i] = gi[i] | (pi[i] & ci[i-1]);
            }
        }
    }
}

/*calculate the actual sum from carry, generate and propagate*/
void generate_sum(char *first_num, char *second_num, unsigned int *ci, unsigned int *sum, unsigned int *gi, unsigned int *pi){
    int i=0;
    unsigned int a = first_num[i] - '0';
    unsigned int b = second_num[i] - '0';
     sum[0] = a ^ b ^ sssCarry;
                     
    //for(i=1;i<=strlen(second_num);i++){  /* Sum for the rest of the bits*/
    for(i=1;i<=strlen(second_num);i++){  /* Sum for the rest of the bits*/
        a = first_num[i] - '0';
        b = second_num[i] - '0';
        sum[i] = a ^ b ^ ci[i-1]; 
    }  
}

/*working simple ripple carry adder to validate results*/
void simple_adder(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *sum){
    int i;
    ci[0] = 0;
    for(i=1;i<MSGSIZE*4;i++){
        ci[i] = gi[i-1] | (pi[i-1] & ci[i-1]);
    }
    for(i=0;i<MSGSIZE*4;i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        sum[i] = a ^ b ^ ci[i];
    }
}
