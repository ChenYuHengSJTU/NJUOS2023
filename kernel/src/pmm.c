#include <common.h>

void Panic(const char* str){
  printf("%s", str);
  while(1);
}

#define BASIC_NUM 12 * (1 << 10)
#define FACTOR 127
#define MAX_BLOCK_SIZE 1024

uintptr_t Heap;

struct Block{
  uintptr_t start, end;
  struct Block* nxt;
};

struct Block* blocks[7];
uintptr_t block_beg[7];
struct Block* head[7];
struct Block* tail[7];

int block_sz[7] = {16, 32, 64, 128, 256, 512, 1024};

struct Spinlock{
  int locked;
};

struct Spinlock memlock[7];

void Acquire(struct Spinlock*);
void Release(struct Spinlock*);

uintptr_t fetch(int sz, int n){
  int tmp = n;
  Acquire(&memlock[sz]);
  struct Block* Head = head[sz];
  uintptr_t Res = Head->start;
  struct Block* Tmp = Head;

  while(Head->nxt && tmp--){
    // Tmp = Head;
    Head = Head->nxt;
  }
  head[sz] = Head;
  // if(sz != 7 && tmp){
  //   if(!fetch(sz + 1, tmp >> 1)){
  //     return NULL;
  //   }
  // }
  Release(&memlock[sz]);
  if(tmp != 0)
    return (uintptr_t)NULL;
  else
    Tmp->nxt = NULL;
  
  printf("[%p, %p) alloced\n", Head->start, Tmp->end);
  return Res;
}

void merge(uintptr_t tofree, int sz){
  Acquire(&memlock[sz]);
  struct Block* Head = (struct Block*)tofree;
  // int sz = ((Head->end - Head->start) >> 5);
  struct Block* Tmp = Head;

  tail[sz]->nxt = Head;
  while(Tmp->nxt){
    Tmp = Tmp->nxt;
  }
  tail[sz] = Tmp;
  Release(&memlock[sz]);
}

void Acquire(struct Spinlock* lk){
  if(lk->locked){
    // printf("spinlock acquire error\n");
    Panic("spinlock acquire error\n");
    // exit(1);
  }
  while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;
  __sync_synchronize();
}

void Release(struct Spinlock* lk){
  if(!lk->locked){
    // printf("spinlock release error\n");
    Panic("spinlock release error\n");
    // exit(1);
  }
  __sync_synchronize();

  __sync_lock_release(&lk->locked);
}

static void *kalloc(size_t size) {
  printf("%d asked\n", size);
  return NULL;
  // if(size >= (1 << 24))
  //   return NULL;
  // int block_size = 0;
  // int index = 0;
  // for(int i = 0;i < 7;++i){
  //   if(size < block_sz[i]){
  //     block_size = block_sz[i];
  //     index = i;
  //     break;
  //   }
  // }

  // if(size > MAX_BLOCK_SIZE){
  //   block_size = MAX_BLOCK_SIZE;
  //   index = 7;
  // }

  // int a = size / block_size + 1;

  // return (void*)fetch(index, a);
}

static void kfree(void *ptr) {
  uintptr_t Ptr = (uintptr_t)ptr;
  for(int i = 0;i < 7;++i){
    int block_size = block_sz[i];
    if((Ptr - block_beg[i]) / block_size < (1 << (6 - i))){
      merge(Ptr, i + 4);
      return;
    }
  }
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  
  // Heap = (uintptr_t)heap.end + 4;
  // *((int*)Heap) = 8;
  // printf("%d\n", *(int*)Heap);
  // Heap = (uintptr_t)heap.end + BASIC_NUM * sizeof(struct Block) * FACTOR;
  // // blocks = (struct Block*)heap.start;
  // uintptr_t curheap = (uintptr_t)heap.start;
  // struct Block* prev;

  // for(int i = 4;i <= 10; ++i){
  //   int block_num = BASIC_NUM * (1 << (10 - i));
  //   blocks[i - 4] = (struct Block*)curheap;
  //   block_beg[i - 4] = curheap;
  //   int block_size = (1 << i);
  //   prev = head[i - 4];
  //   for(int j = 0;j < block_num;++j){
  //     blocks[i - 4][j].start = curheap;
  //     curheap += block_size;
  //     blocks[i - 4][j].end = curheap;
  //     prev->nxt = &blocks[i - 4][j];
  //     prev = &blocks[i - 4][j];
  //   }
  //   tail[i - 4] = &blocks[i - 4][block_num - 1];
  //   tail[i - 4]->nxt = NULL;
  // }

  // for(int i = 0;i < 7;++i){
  //   memlock[i].locked = 0;
  // }

  printf("pmm end init\n");
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
