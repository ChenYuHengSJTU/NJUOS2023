#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"
#include "thread-sync.h"

#define MAXN 10000
int T, N, M;
char A[MAXN + 1], B[MAXN + 1];
int dp[MAXN][MAXN];
int result;

#define DP(x, y) (((x) >= 0 && (y) >= 0) ? dp[x][y] : 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MAX3(x, y, z) MAX(MAX(x, y), z)

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void Tworker(int id) {
  if (id != 1) {
    // This is a serial implementation
    // Only one worker needs to be activated
    return;
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      // Always try to make DP code more readable
      int skip_a = DP(i - 1, j);
      int skip_b = DP(i, j - 1);
      int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
      dp[i][j] = MAX3(skip_a, skip_b, take_both);
    }
  }

  result = dp[N - 1][M - 1];
}

void* do_work(void* args){
  int i = *((int*)args);
  int j = *(((int*)args) + 1);

  int skip_a = DP(i - 1, j);
  int skip_b = DP(i, j - 1);
  int take_both = DP(i - 1, j - 1) + (A[i] == B[j]);
  dp[i][j] = MAX3(skip_a, skip_b, take_both);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // No need to change
  assert(scanf("%s%s", A, B) == 2);
  N = strlen(A);
  M = strlen(B);
  T = !argv[1] ? 1 : atoi(argv[1]);

  printf("%d\n", T);
  // Add preprocessing code here

  Tworker(1);
  // printf("single thread: %d\n", dp[N -1][M - 1]);

  // for(int i = 0;i < MAXN;++i)
  //   memset(dp[i], 0, sizeof(int) * MAXN);

  // for (int i = 0; i < T; i++) {
  //   create(Tworker);
  // }
  // join();  // Wait for all workers

  int cnt = 0;
  int loc[M + N - 1][2];
  loc[0][0] = 0;
  loc[0][1] = 1;
  loc[1][0] = 1;
  loc[1][1] = 0;
  // cnt = 8;

  pthread_t threads[T];

  for(int round = 0 ; round < M + N - 1 ; round++){

    if(cnt == 0)
      break;

    int cur_thread_num = 0;
    while(cur_thread_num < cnt){
      int thread_to_create = MIN(T, cnt - cur_thread_num);
      for(int i = 0;i < thread_to_create; ++i){
        pthread_create(&threads[i] , NULL, do_work, (void*)(loc[cur_thread_num + i]));
        if(loc[i + cur_thread_num][1] + 1 < M){
          loc[cnt][0] = loc[i + cur_thread_num][0];
          loc[cnt][1] = loc[i + cur_thread_num][1] + 1;
          cnt++;
        }
      }

    // int tmp = cnt;
    // cnt = 0;
    // for(int i = 0;i < tmp;++i){
    //   if(loc[i][1] + 1 < M){
    //     loc[cnt][0] = loc[i][0];
    //     loc[cnt][1] = loc[i][1] + 1;
    //     cnt++;
    //   }
    // }
    // if(loc[tmp - 1][0] + 1 < N){
    //   loc[cnt][0] = loc[tmp - 1][0] + 1;
    //   loc[cnt][1] = 0; 
    //   cnt++;
    // }
  
      for(int i = 0;i < thread_to_create; ++i)
        pthread_join(threads[i], NULL);
      cur_thread_num += thread_to_create;
      // printf("round %d with %d jobs finished\n", round, cnt);
    }

    // printf("round %d with %d jobs finished\n", round, cnt);
    // for(int i = 0;i < cnt;++i){
    //   printf("%d,%d\n", loc[i][0], loc[i][1]);
    // }

    int tmp = cnt;
    cnt = 0;
    // for(int i = 0;i < tmp;++i){
    //   if(loc[i][1] + 1 < M){
    //     loc[cnt][0] = loc[i][0];
    //     loc[cnt][1] = loc[i][1] + 1;
    //     cnt++;
    //   }
    // }
    if(loc[tmp - 1][0] + 1 < N){
      loc[cnt][0] = loc[tmp - 1][0] + 1;
      loc[cnt][1] = 0; 
      cnt++;
    }
  }

  result = dp[N - 1][M - 1];
  printf("%d\n", result);
}
