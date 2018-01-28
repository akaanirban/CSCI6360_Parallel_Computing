#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


void simple_adder(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *sum);
void pointer_test(char *str);

void hex_to_bin(char *hex, char *binary_num);
void reverse_string(char *str);
void read_numbers(char *first_num, char *second_num);
void single_gen_prop(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi);
void single_carry(unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *gcj);

void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj);
void group_carry(unsigned int *ggj, unsigned int *gpj, unsigned int *gcj, unsigned int *sck);

void section_gen_prop(unsigned int *ggj, unsigned int *gpj, unsigned int *sgk, unsigned int *spk);
void section_carry(unsigned int *sgk, unsigned int *spk, unsigned int *sck);

void super_section_gen_prop();
void calculate_super_super_section();

void generate_sum(char *binary_first_num, char *binary_second_num, unsigned int *ci, unsigned int *sum, unsigned int *gi, unsigned int *pi);


int main() {
    int i=0;
    char first_num[65];
    char second_num[65];
    
    unsigned int gi[256];
    unsigned int pi[256];
    unsigned int ci[256];

    unsigned int ggj[64];
    unsigned int gpj[64];
    unsigned int gcj[64];

    unsigned int sgk[16];
    unsigned int spk[16];
    unsigned int sck[16];

    unsigned int ssgl[4];
    unsigned int sspl[4];
    unsigned int sscl[4];

    unsigned int sum[256];

    char *binary_first_num;
    char *binary_second_num;
    binary_first_num = (char *)malloc(257*sizeof(char));
    binary_second_num = (char *)malloc(257*sizeof(char));
    read_numbers(first_num, second_num);
    
    //convert to binary
    hex_to_bin(first_num, binary_first_num);
    hex_to_bin(second_num, binary_second_num);
    //printf("\nBiNARY num 1- %s  , length = %u \n", binary_first_num, (unsigned)strlen(binary_first_num));
    //printf("\nBiNARY num 2- %s  , length = %u \n", binary_second_num, (unsigned)strlen(binary_second_num));

    //calculate individual generate and propagate
    single_gen_prop(binary_first_num, binary_second_num, gi, pi);
    //simple_adder(binary_first_num, binary_second_num, gi, pi, ci, sum);
    //calculate group generates and propagates
    group_gen_prop(gi, pi, ggj, gpj);
    //calculate section generate and propagate
    section_gen_prop(ggj, gpj, sgk, spk);
    //calculate section carry
    section_carry(sgk, spk, sck);
    //calculate group carries
    group_carry(ggj, gpj, gcj, sck);
    //calculate individual carries
    single_carry(gi, pi, ci, gcj);
    //calculate the actual sum
    generate_sum(binary_first_num, binary_second_num, ci, sum, gi, pi);


/*
    for(i=0;i<strlen(binary_first_num); i++){
        printf("gi = %u pi = %u \n", gi[i], pi[i]);
    }
    printf("------------------------------------\n");
    for(i=0;i<strlen(binary_first_num)/4; i++){
        printf("group%d ggj = %u gpj = %u \n", i, ggj[i], gpj[i]);
    }
    printf("------------------------------------\n");
    for(i=0;i<strlen(binary_first_num)/16; i++){
        printf("section%d sgk = %u spk = %u \n", i, sgk[i], spk[i]);
    }

*/
    printf("-----------------------------\n");
    for(i=16;i>0;i--){
        printf("%u", sum[i-1]);
    }
    printf("\n");
    //-----------------free the binary num alloctions
    free(binary_first_num);
    free(binary_second_num);
    //pointer_test(first_num);
    return 0;
}

//read input numbers
void read_numbers(char *first_num, char *second_num){
    printf("Please enter 1st hex number:");
    //scanf("%s", first_num); //does not read more than 4 characters
    fgets(first_num, sizeof(first_num), stdin);
    first_num[strcspn(first_num, "\n")]=0;
    printf("Please enter 2nd hex number:");
    //scanf("%s", second_num);
    fgets(second_num, sizeof(second_num), stdin);
    second_num[strcspn(second_num, "\n")]=0;

    printf("1st num %s and second num %s", first_num, second_num);
}

//hexadecimal to binary converter
void hex_to_bin(char *hex, char *binary_num){
    int i = 0, j=0;
    for(i=0; i<strlen(hex); i++){
        switch(tolower(hex[i])){
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

//inplace reverse string
void reverse_string(char *str){ 
    /* if null or empty skip*/
    if (str == 0 || *str == 0){
        return;
    }

    /* get range of string*/
    printf("\n%s\n", str);
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    /* reverse */
    while (end > start){
        /* swap */
        temp = *start;
        *start = *end;
        *end = temp;

        /* move */
        ++start;
        --end;
    }
}

void pointer_test(char *str){
    printf("\n Test 1 ----- str %s\n", str);
    char *c = str;
    printf("\n Test 2 ------ *c %u\n", c); 
    printf("\n Test 3 ------ &*c %u\n", &*c); 
    printf("\n Test 4 ------ *(c+2) prints 3rd element of string %c\n", *(c+2)); 
}

//calculate individual generate and propagates
void single_gen_prop(char *first_num, char *second_num, unsigned int *gi, unsigned int *pi){
    int i;
    //printf("length of string = %d\n", strlen(first_num));
    for(i=0;i<strlen(first_num); i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        gi[i] = a & b;
        //pi[i] = a | b;
        pi[i] = a ^ b;
    }
}

//calculate group generates and propagates
void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj){
    int i=0, j=0;
    for(i=0; i<16; i=i+4){
        //printf("group--%d\n", i);
        ggj[j] = (gi[i+3]) | (gi[i+2]&pi[i+3]) | (gi[i+1]&pi[i+3]&pi[i+2]) | (gi[i]&pi[i+3]&pi[i+2]&pi[i+1]);
        gpj[j] = pi[i+3]&pi[i+2]&pi[i+1]&pi[i];
        j = j+1;
    }
}

//calculate section generate and propagate
void section_gen_prop(unsigned int *ggj, unsigned int *gpj, unsigned int *sgk, unsigned int *spk){
    int j=0, k=0;
    for(j=0; j<4; j=j+4){
        //printf("section--%d\n", j);
        sgk[k] = (ggj[j+3]) | (ggj[j+2]&gpj[j+3]) | (ggj[j+1]&gpj[j+3]&gpj[j+2]) | (ggj[j]&gpj[j+3]&gpj[j+2]&gpj[j+1]);
        spk[k] = gpj[j+3]&gpj[j+2]&gpj[j+1]&gpj[j];
        k = k+1;
    }
}

//calculate section carries
void section_carry(unsigned int *sgk, unsigned int *spk, unsigned int *sck){
    int k=0;
    sck[0] = 0;//sgk[0] | (spk[0] & 0);
    for(k=0; k<1; k++){
        //sck[k] = sgk[k] | (spk[k] & 0)
        //sck[k] = sgk[k] | (spk[k] & sck[k-1]);
    }
}


//calculate group carries
void group_carry(unsigned int *ggj, unsigned int *gpj, unsigned int *gcj, unsigned int *sck){
    int j = 0;
    for(j=0; j<4;j=j+4){
        gcj[j] = ggj[j] | (gpj[j] & sck[(j+1)/4]);
        gcj[j+1] = ggj[j+1] | (gpj[j+1] & gcj[j]);
        gcj[j+2] = ggj[j+2] | (gpj[j+2] & gcj[j+1]);
        gcj[j+3] = ggj[j+3] | (gpj[j+3] & gcj[j+2]);
        printf("j%d  (j+1)%d   (j+2)%d   (j+3)%d\n", gcj[j], gcj[j+1], gcj[j+2], gcj[j+3]);
    }
}

//calculate individual carries
void single_carry(unsigned int *gi, unsigned int *pi, unsigned int *ci, unsigned int *gcj){
    int i=0;
    for(i=0; i<16;i=i+4){
        printf("gc --- %d             %d\n", gcj[(i+1)/4], (i+1)/4);
        ci[i] = gi[i] | (pi[i] & gcj[(i+1)/4]);
        ci[i+1] = gi[i+1] | (pi[i+1] & ci[i]);
        ci[i+2] = gi[i+2] | (pi[i+2] & ci[i+1]);
        ci[i+3] = gi[i+3] | (pi[i+3] & ci[i+2]);
    }
}

//calculate the actual sum
void generate_sum(char *first_num, char *second_num, unsigned int *ci, unsigned int *sum, unsigned int *gi, unsigned int *pi){
    int i=0;
    for(i=0;i<16;i++){
        unsigned int a = first_num[i] - '0';
        unsigned int b = second_num[i] - '0';
        printf("a%d   b%d  c%d  g%d  p%d\n", a,b, ci[i], gi[i], pi[i]);
        sum[i] = a ^ b ^ ci[i]; 
    }
}

//working simple adder to validate results
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