#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <stdbool.h>

#define MAXPROC 32768
#define SIZE(A) (sizeof(A) / sizeof(pid_t))
struct Proc{
  // int pid;
  char name[32];
  int childnum;
  int threadnum;
  pid_t* cpid;
  pid_t* tpid;
};

struct Proc* procs[MAXPROC << 2];

void print_aux(int t){

}

void Print(pid_t pid, int depth){
  printf("%*s(%d)", depth, procs[pid]->name, pid);
  
  if(procs[pid]->cpid != NULL){
    int sz = procs[pid]->childnum;
    for(int i = 0;i < sz;++i)
      Print(procs[pid]->cpid[i], depth + 2);
  }
}


bool isProc(const char* pid){
  int len = strlen(pid);

  for(int i = 0;i < len;++i){
    if(pid[i] < '0' || pid[i] > '9')
      return false;
  }

  return true;
}

void getthread(const char* dir, pid_t pid){
    DIR* dp = NULL;
    struct dirent* entry;
    struct stat statbuf;

    char task[64] = "";
    strncpy(task, dir, 32);

    strncat(task, "/task", 5);

    dp = opendir(task);
    
    if(dp == NULL){
        fprintf(stderr, "cannot open folder %s\n", task);
    }

    pid_t thread[MAXPROC];
    int cnt = 0;

    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0){
                continue;
            }
            // printf("%*s%s/\n", depth, "", entry->d_name);
            // traverse(entry->d_name, depth + 4);
            pid_t tpid;
            if(isProc(entry->d_name)){
              tpid = atoi(entry->d_name);
              if(tpid != pid)
                thread[cnt++] = tpid;
            }
        }
    }

    procs[pid]->tpid = (pid_t*)malloc(sizeof(pid_t) * cnt);
    procs[pid]->threadnum = cnt;
    if(procs[pid]->tpid == NULL)
      perror("get thread malloc error");

    memcpy(procs[pid]->tpid, thread, sizeof(int) * cnt);

    // printf("pid[%d] has following threads:", cnt);
    // for(int i = 0;i < cnt;++i)
    //   printf("%d ", procs[pid]->tpid[i]);

    // printf("\n");
    // fflush(stdout);
}

void getchild(const char* dir, pid_t pid){
  char task[64] = "";

  sprintf(task, "%s/task/%d", dir, pid);
  // strncpy(task, dir, 32);
  // strncat(task, "task\0", 5);
  // strncat(task, itoa(pid), 5);
  
  DIR* dp = NULL;
  struct dirent* entry;
  struct stat statbuf;

  dp = opendir(task);
    
  if(dp == NULL){
      fprintf(stderr, "cannot open folder %s\n", task);
  }

  while((entry = readdir(dp)) != NULL){
    lstat(entry->d_name, &statbuf);
    if(S_ISREG(statbuf.st_mode)){
      if(strcmp(entry->d_name, "children") == 0){
        FILE * fp = fopen(entry->d_name, "r");
        pid_t child[MAXPROC];
        int cnt = 0;

        if(fp == NULL)
          perror("open file error");

        while(fscanf(fp, "%d", &child[cnt]) != EOF){
          cnt++;
        }

        procs[pid]->cpid = (pid_t*)malloc(sizeof(pid_t) * cnt);
        procs[pid]->childnum = cnt;
        memcpy(procs[pid]->cpid, child, cnt * sizeof(pid_t));

        return;
      }
    }
  }
}

void getprocinfo(const char* dir, pid_t pid){
    DIR* dp = NULL;
    struct dirent* entry;
    struct stat statbuf;

    dp = opendir(dir);
    
    if(dp == NULL){
        fprintf(stderr, "cannot open folder %s\n", dir);
    }

    // chdir(dir);

    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0){
                continue;
            }
            
          if(strcmp(entry->d_name, "stat") == 0){
            FILE* fp;
            fp = fopen(entry->d_name, "r");

            if(fp == NULL){
                fprintf(stderr, "cannot open file %s/%s\n", dir, entry->d_name);
            }

            char info[32] = "";
            int pid, ppid;
            char ch;
            fscanf(fp, "%d (%[^)]) %c %d", &pid, info, &ch, &ppid);

            strncpy(procs[pid]->name, info, 32);

            fclose(fp);
          }
        }
    }

    // chdir("..");
    closedir(dp);
}

void getproc(const char* dir){
    DIR* dp = NULL;
    struct dirent* entry;
    struct stat statbuf;

    dp = opendir(dir);
    
    if(dp == NULL){
        fprintf(stderr, "cannot open folder %s\n", dir);
    }

    // chdir(dir);

    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0){
                continue;
            }
            // printf("%*s%s/\n", depth, "", entry->d_name);
            // traverse(entry->d_name, depth + 4);
            // printf("%s ", entry->d_name);
            pid_t pid;
            if(isProc(entry->d_name)){
              pid = atoi(entry->d_name);
              // printf("%s ", entry->d_name);

              // printf("traverse to pid[%d]\n", pid);
              // printf("%d ", pid);
              fflush(stdout);

              procs[pid] = (struct Proc*)malloc(sizeof(struct Proc));
              assert(procs[pid] != NULL);

              // strncpy(procs[pid]->name, entry->d_name, 32);
              
              // read thread 
              char procfile[512] = "";
              sprintf(procfile, "/proc/%s", entry->d_name);
              getprocinfo(procfile, pid);
              getthread(procfile, pid);
              getchild(procfile, pid);
            }
        }
    }

    // chdir("..");
    closedir(dp);
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);

  getproc("/proc");

  Print(1, 0);
  return 0;
}
