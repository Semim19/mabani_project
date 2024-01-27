#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
int run_init(int argc, char* const argv[]);
int run_init(int argc, char* const argv[]){
    // current working directory
    char cwd[1000];
    if(getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    // just like cwd but used when we want to check
    // the parent so we need to change the directory
    char tmp_cwd[1000];
    bool exists = false; // determine .neogit existence
    struct dirent *entry; // pointer to check each file
    do{
        DIR* dir = opendir(".");
        if(dir == NULL){
            perror("Cannot open current directory.\n");
            return 1;
        }
        while((entry = readdir(dir)) != NULL){
            if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0){
                exists = true;
            }
        }
        closedir(dir);
        // update tmp_cwd
        if(getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // moving to parent if it is not root
        if(strcmp(tmp_cwd, "/") != 0){
            if(chdir("..") != 0) return 1;
        }
    }while(strcmp(tmp_cwd, "/") != 0);

    // returning to cwd
    if(chdir(cwd) != 0) return 1;

    // TODO if there was no .neogit
    if(!exists){
        // make .neogit directory
        if(mkdir(".neogit", 0755) != 0) return 1;

    } else {
        perror("neogit repo has already been initialized!\n");
    }
    return 0;
    
}

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stdout, "Please enter a valid command\n");
        return 1;
    }
    if(strcmp(argv[1], "init") == 0){
        return run_init(argc, argv);
    }
    return 0;
}
