#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>


void traverse(char* dir, int depth){
    DIR* dp = NULL;
    struct dirent* entry;
    struct stat statbuf;

    dp = opendir(dir);
    
    if(dp == NULL){
        fprintf(stderr, "cannot open folder %s\n", dir);
    }

    chdir(dir);

    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0){
                continue;
            }
            // printf("%*s%s/\n", depth, "", entry->d_name);
            traverse(entry->d_name, depth + 4);
        }
        else{
            // printf("%s\n", entry->d_name);
            if(strcmp(entry->d_name, "stat") == 0){
                // printf("find a stat");
                FILE* fp;
                fp = fopen(entry->d_name, "r");

                if(fp == NULL){
                    fprintf(stderr, "cannot open file\n");
                }

                char info[1024] = "";
                int pid, ppid;
                char ch;
                fscanf(fp, "%d (%[^)]) %c %d", &pid, info, &ch, &ppid);
                // if(ppid != 0 && ppid != 93)
                // printf("%d ", (int)ch);
                // printf("%s[%d] --> %d\n", info, pid, ppid);
                fclose(fp);
            }
            else if(strcmp(entry->d_name, "children") == 0){
                // printf("get child\n");
                FILE* fp;
                fp = fopen(entry->d_name, "r");

                if(fp == NULL){
                    fprintf(stderr, "cannot open file\n");
                }
                int cnt = 0;
                int child[1024];
                while(fscanf(fp, "%d", &(child[cnt++])) != EOF){
                    // printf("%d ", child[cnt - 1]);
                }
                printf("%d\n", cnt - 1);
                fflush(stdout);
                fclose(fp);
            }
        }
    }

    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[]){
    traverse("/proc", 0);
    return 0;
}