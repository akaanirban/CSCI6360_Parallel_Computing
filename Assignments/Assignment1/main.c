#include <stdio.h>
#include <string.h>

char * hex_to_bin(char *hex);


int main() {
    char first_num[6];
    char second_num[6];

    printf("Please enter 1st hex number:");
    //scanf("%s", first_num); //does not read more than 4 characters
    fgets(first_num, sizeof(first_num), stdin);
    first_num[strcspn(first_num, "\n")]=0;
    printf("Please enter 2nd hex number:");
    //scanf("%s", second_num);
    fgets(second_num, sizeof(second_num), stdin);
    first_num[strcspn(second_num, "\n")]=0;

    printf("1st num %s and second num %s", first_num, second_num);
    return 0;
}


//hexadecimal to binary converter
char * hex_to_bin(char *hex){
    return 0;
}