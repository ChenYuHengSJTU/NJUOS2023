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
        fprintf(stderr, "cannot open folder\n");
    }

    chdir(dir);

    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)){
            if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0){
                continue;
            }
            printf("%*s%s/\n", depth, "", entry->d_name);
            traverse(entry->d_name, depth + 4);
        }
        else
            printf("%*s%s\n", depth, "", entry->d_name);
    }

    chdir("..");
    closedir(dp);
}

int main(int argc, char* argv[]){
    traverse("/proc", 0);
    return 0;
}