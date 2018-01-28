#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



void bin_to_hex(unsigned int *binary, char *converted_hex);
void reverse_string(char *str);






int main() {
    int i=0;
    char first_num[65] = "000000000000000000000000000000000000000000000000000000000000FFFF";
    char second_num[65] = "0000000000000000000000000000000000000000000000000000000000000001";
    char converted_hex[65];
    
    char binary[257] = "1011000001010011100010000110001110101111101100100100010111111101010001000011010010100100100000000000100101011100111100000110000100110011000010110011111101111100011101100001000100001000110111111000001111010001010000110110111001101100011001111111000000000000";

int temp;
        char num[10];
        sprintf(num, binary[i], binary[i+1]);//, binary[i+2], binary[i+3]);
        printf("%c\n", binary[i]);
//    bin_to_hex(binary, converted_hex);

//printf("%s\n", converted_hex);
    return 0;
}



//binary to hexadecimal converter
void bin_to_hex(unsigned int *binary, char *converted_hex){
    int i = 0, k=0;
    for(i=0; i<256; i=i+4){
        int n;
        char num[10];
        temp = sprintf(num, binary[i], binary[i+1], binary[i+2], binary[i+3]);
        printf("%s\n", num);
        if(strcmp(temp, "0000")==0){
            converted_hex[k] = '0';
        }
        else if(strcmp(temp, "0001")==0){
            converted_hex[k] = '1';
        }
        else if(strcmp(temp, "0010")==0){
            converted_hex[k] = '2';
        }
        else if(strcmp(temp, "0011")==0){
            converted_hex[k] = '3';
        }
        else if(strcmp(temp, "0100")==0){
            converted_hex[k] = '4';
        }
        else if(strcmp(temp, "0101")==0){
            converted_hex[k] = '5';
        }
        else if(strcmp(temp, "0110")==0){
            converted_hex[k] = '6';
        }
        else if(strcmp(temp, "0111")==0){
            converted_hex[k] = '7';
        }
        else if(strcmp(temp, "1000")==0){
            converted_hex[k] = '8';
        }
        else if(strcmp(temp, "1001")==0){
            converted_hex[k] = '9';
        }
        else if(strcmp(temp, "1010")==0){
            converted_hex[k] = 'a';
        }
        else if(strcmp(temp, "1011")==0){
            converted_hex[k] = 'b';
        }
        else if(strcmp(temp, "1100")==0){
            converted_hex[k] = 'c';
        }
        else if(strcmp(temp, "1101")==0){
            converted_hex[k] = 'd';
        }
        else if(strcmp(temp, "1110")==0){
            converted_hex[k] = 'e';
        }
        else if(strcmp(temp, "1111")==0){
            converted_hex[k] = 'f';
        }
        k = k+1;
    }
    converted_hex[k] = '\0';
    //printf("%s\n", converted_hex);
    //reverse_string(converted_hex); //reverse the string to get actual number
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

