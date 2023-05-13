#include <stdio.h>
#include <stdint.h>

#define MAXSIZE 1000

uint64_t random_uint64(void) {
    uint64_t result;
    unsigned char ok;
    __asm__ __volatile__("rdrand %0; setc %1"
                         : "=r" (result), "=qm" (ok));
    if (ok) {
        return result;
    } else {
        // RDRAND failed, fall back to something else
        fprintf(stderr, "generate random number failed\n");
        return 0;
    }
}

int main(){
    
    char a[MAXSIZE];
    char b[MAXSIZE];

    for(int i = 0;i < MAXSIZE;++i){
        int k = random_uint64() % 26;
        a[i] = k + 97;
        k = random_uint64() % 26;
        b[i] = k + 97;
    }

    printf("%s\n%s\n", a, b);
    fflush(stdout);
    return 0;
}