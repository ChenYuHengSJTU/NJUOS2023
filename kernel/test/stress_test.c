#include <common.h>
#include <klib.h>

enum ops { OP_ALLOC = 1, OP_FREE };
struct malloc_op {
  enum ops type;
  union { size_t sz; void *addr; };
};

uint64_t random_uint64(void) {
    uint64_t result;
    unsigned char ok;
    __asm__ __volatile__("rdrand %0; setc %1"
                         : "=r" (result), "=qm" (ok));
    if (ok) {
        return result;
    } else {
        // RDRAND failed, fall back to something else
        printf("generate random number failed\n");
        return 0;
    }
}

#define MAXOP 10000
#define MAXSZ 100000000

struct malloc_op op_stack[MAXOP];
int top = -1;

struct malloc_op random_op(){
    int type = random_uint64() % 2;
    struct malloc_op op;
    if(type && top >= 0){
        op = op_stack[top--];
    }
    else{
        op.type = OP_ALLOC;
        op.sz = random_uint64() % MAXSZ;
    }
    return op;
}

void stress_test() {
  while (1) {
    struct malloc_op op = random_op();
    switch (op.type) {
      case OP_ALLOC: pmm->alloc(op.sz); break;
      case OP_FREE:  pmm->free(op.addr); break;
    }
  }
}

// int main() {
//   os->init();
//   mpe_init(stress_test);
// }