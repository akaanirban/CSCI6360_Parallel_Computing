#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


void hex_to_bin(char *hex, char *binary_num, int len);
void reverse_string(char *str);
void read_numbers(char *first_num, char *second_num);
void calculate_group(char *str);
void pointer_test(char *str);
void calculate_section();
void calculate_super_section();
void calculate_super_super_section();

int main() {
    char first_num[65];
    char second_num[65];

    char *binary_num;
    binary_num = (char *)malloc(257*sizeof(char));
    read_numbers(first_num, second_num);
    
    //convert to binary
    hex_to_bin(first_num, binary_num, (unsigned)strlen(first_num));
    printf("\nBiNARY - %s  , %u \n", binary_num, (unsigned)strlen(first_num));

    //-----------------
    free(binary_num);



    pointer_test(first_num);
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
void hex_to_bin(char *hex, char *binary_num, int len){
    int i = 0, j=0;
    for(i=0; i<len; i++){
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
    printf("------%s\n", str);
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
