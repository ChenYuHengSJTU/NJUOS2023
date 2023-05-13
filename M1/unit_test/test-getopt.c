#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    char ch;
    while((ch = getopt(argc, argv, "pnV")) != -1){
        printf("%c\n", ch);
        switch(ch){
            case 'p':
                printf("p opt\n");
                break;
            case 'n':
                printf("n opt\n");
                break;
            case 'V':
                printf("V opt\n");
                break;
            default:
                printf("undefined opt %c \n", ch);
        }
    }

    return 0;
}