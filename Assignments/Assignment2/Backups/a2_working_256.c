/******************************************************************************
 * FILE: assignment1.c
 * DESCRIPTION:  
 *   This code takes in two 64 digit HEX numbers and uses a 256 bit 
 *   hierarchical CLA with 4 bit blocks to compute their sum
 *   Anirban Das (dasa2@rpi.edu)
 *   01/30/2018
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>
#include <math.h>

#define BLKSIZE 4
#define MSGSIZE 256
int          taskid, ntasks;
MPI_Status   status;
MPI_Request send_request,recv_request;
unsigned int sssCarry=0;

/*Main CLA routine having all steps*/
void cla(char *first_num, char *second_num, char *converted_hex,
         int chunksize);
/*Read the two numbers from stdin*/
void read_numbers(char *first_num, char *second_num);

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
void bin_to_hex(unsigned int *binary, char *converted_hex);
void reverse_string(char *str);
void simple_adder(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *sum);

/* MAIN FUNCTION */
int main(int argc , char **argv) {

    char         *first_num; /* the first 64digit hex number      */
    char         *second_num; /* the second 64digit hex number    */
    char         converted_hex[65]; /* the result 64digit hex number */
    //int          taskid, ntasks;
    unsigned long chunks;
	//MPI_Status   status;
	//MPI_Request	send_request,recv_request;
	int          ierr,i,j,itask,recvtaskid;
  	double       inittime,totaltime,recvtime,recvtimes[1024];

	/*========================================================================*/
	/* MPI Initialisation.                                                    */
	MPI_Init(&argc, &argv);

	/*========================================================================*/
	/* Get the number of MPI tasks and the taskid of this task.               */
	MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

    chunks = MSGSIZE*4/ntasks;
    char *binary_first_num; /*variable for binary version of 1st number*/
    char *binary_second_num; /*variable for binary version of 2nd number*/
    char *sub_first_num;
    char *sub_second_num;
    sub_first_num = (char *)malloc((chunks+1)*sizeof(char));
    sub_second_num = (char *)malloc((chunks+1)*sizeof(char));
    
	/*========================================================================*/
	/*code for rank zero											          */
	if ( taskid == 0 ){
    	printf("\n\n\n");
    	printf("##########################################################\n\n");
    	printf(" no. of chunks : %lu\n",chunks);
    	printf(" Number of tasks: %d\n\n",ntasks);
    	printf("##########################################################\n\n");

        /*read the two numbers from input redirected file                         */
        first_num = (char *)malloc((MSGSIZE+2)*sizeof(char));
        second_num = (char *)malloc((MSGSIZE+2)*sizeof(char));
        read_numbers( first_num, second_num);
        printf("1st num %s\n",first_num );
        printf("2nd num %s\n",second_num );
        
        binary_first_num = (char *)malloc((MSGSIZE*4+1)*sizeof(char));
        binary_second_num = (char *)malloc((MSGSIZE*4+1)*sizeof(char));
            
        /*convert the hex numbers to binary*/
        hex_to_bin(first_num, binary_first_num);
        hex_to_bin(second_num, binary_second_num);
    }

    /*scatter the data from the rank 0 node */
    MPI_Scatter(binary_first_num,
                chunks,
                MPI_CHAR,
                sub_first_num,
                chunks,
                MPI_CHAR,
                0,
                MPI_COMM_WORLD);
    sub_first_num[chunks] = '\0';
    //printf("1st number from %d is : %s\n",taskid, sub_first_num );
    MPI_Scatter(binary_second_num,
                chunks,
                MPI_CHAR,
                sub_second_num,
                chunks,
                MPI_CHAR,
                0,
                MPI_COMM_WORLD);
    sub_second_num[chunks] = '\0';
    //printf("2nd number from %d is : %s  %d\n",taskid, sub_second_num, strlen(sub_second_num));

    /*========================================================================*/
    /*code for rest of the ranks                                              */
    if (taskid != 0){
        ierr=MPI_Irecv(&sssCarry,1 ,MPI_UNSIGNED,
               taskid-1,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request);
    }
    //else{
    //    ierr=MPI_Irecv(&sssCarry,1 ,MPI_UNSIGNED,
    //           ntasks-1,MPI_ANY_TAG,MPI_COMM_WORLD,&recv_request);
    //}
    //ierr=MPI_Wait(&recv_request,&status);

    cla(sub_first_num, sub_second_num, converted_hex, chunks*4);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("the received value by %d ---> %d\n",taskid, sssCarry);










    /*all the main cla routine to do all teh cla steps and generate 
    the converted_hex, which is the result*/
    //cla(first_num, second_num, converted_hex);
    printf("3rd num from %d %s\n",taskid, converted_hex );
    free(sub_first_num);
    free(sub_second_num);
    if(taskid ==0){
        free(first_num); free(second_num);
        free(binary_first_num); free(binary_second_num);    
    }
    MPI_Finalize();
    return 0;
}

/*main routine doing all the CLA steps and return the sum*/ 
void cla(char *binary_first_num, char *binary_second_num, 
            char *converted_hex, int chunksize){
    int i=0; /*dummy value for counting stuff*/
    unsigned int *gi; /*variable for individual generate*/
    gi = (unsigned int *)calloc(chunksize, sizeof(unsigned int));
    unsigned int *pi; /*variable for individual propagate*/
    pi = (unsigned int *)calloc(chunksize, sizeof(unsigned int));
    unsigned int *ci; /*variable for individual carry*/
    ci = (unsigned int *)calloc(chunksize, sizeof(unsigned int));

    unsigned int *ggj; /*variable for group generate*/
    ggj = (unsigned int *)calloc(chunksize/4, sizeof(unsigned int));
    unsigned int *gpj; /*variable for group propagate*/
    gpj = (unsigned int *)calloc(chunksize/4, sizeof(unsigned int));
    unsigned int *gcj; /*variable for group carry*/
    gcj = (unsigned int *)calloc(chunksize/4, sizeof(unsigned int));

    unsigned int *sgk; /*variable for section generate*/
    sgk = (unsigned int *)calloc(chunksize/16, sizeof(unsigned int));
    unsigned int *spk; /*variable for section propagate*/
    spk = (unsigned int *)calloc(chunksize/16, sizeof(unsigned int));
    unsigned int *sck; /*variable for section carry*/
    sck = (unsigned int *)calloc(chunksize/16, sizeof(unsigned int));

    unsigned int *ssgl; /*variable for super section generate*/
    ssgl = (unsigned int *)calloc(chunksize/64, sizeof(unsigned int));
    unsigned int *sspl; /*variable for super section propagate*/
    sspl = (unsigned int *)calloc(chunksize/64, sizeof(unsigned int));
    unsigned int *sscl; /*variable for super section carry*/
    sscl = (unsigned int *)calloc(chunksize/64, sizeof(unsigned int));

    unsigned int *sssgm; /*variable for super super section generate*/
    sssgm = (unsigned int *)calloc(chunksize/256, sizeof(unsigned int));
    unsigned int *ssspm; /*variable for super super section propagate*/
    ssspm = (unsigned int *)calloc(chunksize/256, sizeof(unsigned int));
    unsigned int *ssscm; /*variable for super super section carry*/
    ssscm = (unsigned int *)calloc(chunksize/256, sizeof(unsigned int));

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
    /*calculate section generate and propagate*/
    section_gen_prop(ggj, gpj, sgk, spk);
    /*calculate super section generate propagate*/
    super_section_gen_prop(sgk, spk, ssgl, sspl);
    /*calculate super super section generate propagate*/
    super_super_section_gen_prop(ssgl, sspl, sssgm, ssspm);

    /*calculate super super section carry*/
    super_super_section_carry(sssgm, ssspm, ssscm);
    /*calculate super section carry*/
    MPI_Barrier(MPI_COMM_WORLD);
    super_section_carry(ssgl, sspl, sscl, ssscm);
    /*calculate section carry*/
    section_carry(sgk, spk, sck, sscl);
    /*calculate group carries*/
    group_carry(ggj, gpj, gcj, sck);
    /*calculate individual carries*/
    single_carry(gi, pi, ci, gcj);
    /*calculate the actual sum*/
    generate_sum(binary_first_num, binary_second_num, ci, sum, gi, pi);

    /*reverse the number to form the actual number*/
    for(i=256;i>0;i--){ 
        actual_sum[256-i] = sum[i-1];
    }
    
    /* convert the binary result to HEX*/
    bin_to_hex(actual_sum, converted_hex);
    
    /*free the binary num alloctions*/
    free(gi); free(pi); free(ci);
    free(ggj); free(gpj); free(gcj);
    free(sgk); free(spk); free(sck);
    free(ssgl); free(sspl); free(sscl);
    free(sssgm); free(ssspm); free(ssscm);
    free(sum); free(actual_sum);
}

/*read input numbers */
void read_numbers(char *first_num, char *second_num){
    //printf("Please enter 1st hex number:");
    //scanf("%s", first_num); 
    fgets(first_num, MSGSIZE+2, stdin); /* read 1st number */
    first_num[strcspn(first_num, "\n")]=0;
    //printf("Please enter 2nd hex number:");
    //scanf("%s", second_num);
    fgets(second_num, MSGSIZE+2, stdin); /* read 2nd number */
    second_num[strcspn(second_num, "\n")]=0;
    //printf("1st num %s and second num %s", first_num, second_num);
}

/*hexadecimal to binary converter*/
void hex_to_bin(char *hex, char *binary_num){
    int i = 0, j=0;
    for(i=0; i<strlen(hex); i++){
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
void bin_to_hex(unsigned int *binary, char *converted_hex){
    int i = 0, k=0;
    for(i=0; i<256; i=i+4){
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
        k = k+1;
    }
    converted_hex[k] = '\0';
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
    int i;
    for(i=0;i<strlen(first_num); i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        gi[i] = a & b; /* Create the generate */
        pi[i] = a ^ b; /* Create the propagate */
    }
}

/*calculate group generates and propagates*/
void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj){
    int i=0, j=0;
    for(i=0; i<256; i=i+4){  /* Each 4 individual ith bits makes jth group */
        //printf("group--%d\n", i);
        ggj[j] = (gi[i+3]) | (gi[i+2]&pi[i+3]) | (gi[i+1]&pi[i+3]&pi[i+2]) | (gi[i]&pi[i+3]&pi[i+2]&pi[i+1]);
        gpj[j] = pi[i+3]&pi[i+2]&pi[i+1]&pi[i];
        j = j+1;
    }
}

/*calculate section generate and propagate*/
void section_gen_prop(unsigned int *ggj, unsigned int *gpj, unsigned int *sgk, unsigned int *spk){
    int j=0, k=0;
    for(j=0; j<64; j=j+4){  /* Each 4 jth groups makes kth section */
        //printf("section--%d\n", j);
        sgk[k] = (ggj[j+3]) | (ggj[j+2]&gpj[j+3]) | (ggj[j+1]&gpj[j+3]&gpj[j+2]) | (ggj[j]&gpj[j+3]&gpj[j+2]&gpj[j+1]);
        spk[k] = gpj[j+3]&gpj[j+2]&gpj[j+1]&gpj[j];
        k = k+1;
    }
}

/*calculate super section generate and propagate*/
void super_section_gen_prop(unsigned int *sgk, unsigned int *spk, unsigned int *ssgl, unsigned int *sspl){
    int k=0, l=0;
    for(k=0; k<16; k=k+4){  /* Each 4 kth section makes lth super section */
        //printf("section--%d\n", j);
        ssgl[l] = (sgk[k+3]) | (sgk[k+2]&spk[k+3]) | (sgk[k+1]&spk[k+3]&spk[k+2]) | (sgk[k]&spk[k+3]&spk[k+2]&spk[k+1]);
        sspl[l] = spk[k+3]&spk[k+2]&spk[k+1]&spk[k];
        l = l+1;
    }    
}

/*calculate super super section generate and propagate*/
void super_super_section_gen_prop(unsigned int *ssgl, unsigned int *sspl, unsigned int *sssgm, unsigned int *ssspm){
    int l=0, m=0;
    for(l=0; l<4; l=l+4){  /* Each 4 kth section makes lth super section */
        //printf("section--%d\n", j);
        sssgm[m] = (ssgl[l+3]) | (ssgl[l+2]&sspl[l+3]) | (ssgl[l+1]&sspl[l+3]&sspl[l+2]) | (ssgl[l]&sspl[l+3]&sspl[l+2]&sspl[l+1]);
        ssspm[m] = sspl[l+3]&sspl[l+2]&sspl[l+1]&sspl[l];
        m = m+1;
    }    
}

/*calculate super super section carry*/
void super_super_section_carry(unsigned int *sssgm, unsigned int *ssspm, unsigned *ssscm){
    int m =0, c, ierr;
    for(m = 0; m < BLKSIZE/ntasks; m++ ){
        if(m = 0 && taskid == 0) { /*1st s3section*/
            ssscm[0] = sssgm[0] | (ssspm[0] & 0);
        }
        else if ((m+1)%BLKSIZE ==0){ /* every fourth block*/
            ssscm[m] = sssgm[m] | (ssspm[m] & ssscm[(m+1)/BLKSIZE-1]);
        }
        else{
            ssscm[m] = sssgm[m] | (ssspm[m] & ssscm[m-1]);
        }
    }
    /*for(m=0; m<pow(BLKSIZE,1); m=m+BLKSIZE){
        if(m==0){
            ssscm[m] = sssgm[m] | (ssspm[m] & 0);
        }
        else{
            ssscm[m] = sssgm[m] | (ssspm[m] & ssscm[(m+1)/BLKSIZE-1]);
        }
        for(ct = 1; ct <BLKSIZE; ct++ ){
            ssscm[m+ct] = sssgm[m+ct] | (ssspm[m+ct] & ssscm[m+ct-1]); 
        } 
    }*/
    unsigned int toSend = ssscm[BLKSIZE/ntasks -1];
    printf("%d fdgdsgsdgsdgsdgsdgsdgsdgs ---- ssscm %d \n", taskid, toSend);
    
    if (taskid != ntasks -1) {/* this is the last supersection block*/ 
        ierr=MPI_Isend(&toSend,1,MPI_UNSIGNED,
               taskid+1,0,MPI_COMM_WORLD,&send_request);
    }
}

/*calculate super section carry*/
void super_section_carry(unsigned int *ssgl, unsigned int *sspl, unsigned *sscl, unsigned int *ssscm){
    int l =0, ct, ierr;
    for(l=0; l<pow(BLKSIZE,2); l=l+BLKSIZE){
        if(l==0){
            if(taskid ==0){ /* i.e. this is the first block, */
                sscl[l] = ssgl[l] | (sspl[l] & 0); /* since Super Section Carry[-1] = 0*/
            } 
            else
                printf("%d says sssCarry received is %d\n",taskid ,sssCarry );
                sscl[l] = ssgl[l] | (sspl[l] & sssCarry);
        }
        else{
            sscl[l] = ssgl[l] | (sspl[l] & ssscm[(l+1)/BLKSIZE-1]);
        }
        for(ct = 1; ct <BLKSIZE; ct++ ){
            sscl[l+ct] = ssgl[l+ct] | (sspl[l+ct] & sscl[l+ct-1]); 
        }
    }
}

/*calculate section carries*/
void section_carry(unsigned int *sgk, unsigned int *spk, unsigned int *sck, unsigned int *sscl){
    int k=0, ct;
    for(k=0; k<pow(BLKSIZE,2); k=k+BLKSIZE){
        if (k==0){
            if(taskid == 0 )
                sck[k] = sgk[k] | (spk[k] & 0); /* since Section Carry[-1] = 0*/
            else
                sck[k] = sgk[k] | (spk[k] & sssCarry);
        }
        else{
            sck[k] = sgk[k] | (spk[k] & sscl[(k+1)/BLKSIZE-1]);
        }
        for(ct = 1; ct <BLKSIZE; ct++ ){
            sck[k+ct] = sgk[k+ct] | (spk[k+ct] & sck[k+ct-1]);
        }
    }
}

/*calculate group carries*/
void group_carry(unsigned int *ggj, unsigned int *gpj, unsigned int *gcj, unsigned int *sck){
    int j = 0, ct;
    for(j=0; j<pow(BLKSIZE,3); j=j+BLKSIZE){
        if(j==0){
            if(taskid ==0)
                gcj[j] = ggj[j] | (gpj[j] & 0); /* since Group Carry[-1] = 0*/
            else
                gcj[j] = ggj[j] | (gpj[j] & sssCarry);
        }
        else{
            gcj[j] = ggj[j] | (gpj[j] & sck[(j+1)/BLKSIZE-1]);
        }
        for(ct = 1; ct <BLKSIZE; ct++ ){
            gcj[j+ct] = ggj[j+ct] | (gpj[j+ct] & gcj[j+ct-1]);
        }
    }
}

/*calculate individual carries*/
void single_carry(unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *gcj){
    int i=0, ct;
    for(i=0; i<pow(BLKSIZE,4);i=i+BLKSIZE){
        if(i==0){
            if(taskid ==0)
                ci[i] = gi[i] | (pi[i] & 0); /* since individual Carry[-1] = 0*/
            else
                ci[i] = gi[i] | (pi[i] & sssCarry);
        }
        else{
            ci[i] = gi[i] | (pi[i] & gcj[(i+1)/BLKSIZE-1]);
        }
        for(ct = 1; ct <BLKSIZE; ct++ ){
            ci[i+ct] = gi[i+ct] | (pi[i+ct] & ci[i+ct-1]);
        }
    }
}

/*calculate the actual sum from carry, generate and propagate*/
void generate_sum(char *first_num, char *second_num, unsigned int *ci, unsigned int *sum, unsigned int *gi, unsigned int *pi){
    int i=0;
    unsigned int a = first_num[i] - '0';
    unsigned int b = second_num[i] - '0';
    sum[0] = a ^ b ^ sssCarry;                  /* Sum for the 1st bit*/
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
    for(i=1;i<16;i++){
        ci[i] = gi[i-1] | (pi[i-1] & ci[i-1]);
    }
    for(i=0;i<16;i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        sum[i] = a ^ b ^ ci[i];
    }
}