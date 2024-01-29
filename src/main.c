#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
int run_init(int argc, char* const argv[]);
int run_add(int argc, char* const argv[]);
int add_to_staging(char *filepath);
int isDir(const char* fileName);
int dir_staging(const char* dirname);

int run_init(int argc, char* const argv[]){
    // current working directory
    char cwd[1000];
    if(getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    // just like cwd but used when we want to check
    // the parent so we need to change the directory
    char tmp_cwd[1000];
    bool exists = false; // determine .sem existence
    struct dirent *entry; // pointer to check each file
    do{
        DIR* dir = opendir(".");
        if(dir == NULL){
            perror("Cannot open current directory.\n");
            return 1;
        }
        while((entry = readdir(dir)) != NULL){
            if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".sem") == 0){
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

    // TODO if there was no .sem
    if(!exists){
        // make .sem directory
        if(mkdir(".sem", 0755) != 0) return 1;
        chdir(".sem");
        if(mkdir("staging", 0755) != 0) return 1;
        if(mkdir("tracks", 0755) != 0) return 1;
        chdir(cwd);
        FILE *file = fopen(".sem/staging/fileAddress", "w");
        fclose(file);
    } else {
        perror("sem repo has already been initialized!\n");
    }
    return 0;
    
}
int isDir(const char* fileName)
{
    struct stat path;
    stat(fileName, &path);
    return S_ISREG(path.st_mode);
}
int dir_staging(const char* dirname){
    // char cwd[1000];
    // if(getcwd(cwd, sizeof(cwd) == NULL)) return 1;
    // printf("%s\n", cwd);
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    char full_address[1000];
    strcpy(full_address, dirname);
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        // strcat(full_address, "/");
        // strcat(full_address, entry->d_name);
        if(entry->d_type == DT_DIR){
            chdir(dirname);
            dir_staging(entry->d_name);
        }
        else{
            chdir(dirname);
            add_to_staging(entry->d_name);
        }
        // strcpy(full_address, dirname);
        chdir("..");
    }
    return 0;
}
int run_add(int argc, char* const argv[]){
    if(argc < 3){
        perror("Please choose a file!\n");
        return 1;
    }
    if(strcmp(argv[2], "-f") == 0){
        if(argc < 4){
            perror("Please choose a file!\n");
            return 1;
        }
        for(int i = 3; i < argc; i++){
            if(isDir(argv[i]) != 0)
                return add_to_staging(argv[i]);
            else
                return dir_staging(argv[i]);
            
        }
    }
    for(int i = 2; i < argc; i++){
        if(isDir(argv[i]) != 0)
            return add_to_staging(argv[i]);
        else
            return dir_staging(argv[i]);
    }
}
int add_to_staging(char *filepath){
    char cwd[1000];
    if(getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    struct dirent *entry; // pointer to check each file
    // returning to the directory with .sem
    do{
        int flag = 0;
        DIR* dir = opendir(".");
        if(dir == NULL){
            perror("Cannot open current directory.\n");
            return 1;
        }
        while((entry = readdir(dir)) != NULL){
            if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".sem") == 0){
                flag = 1;
                break;
            }
        }
        if(flag)
            break;
        closedir(dir);
        if(chdir("..") != 0) return 1;
    }while(1);
    FILE *file = fopen(".sem/staging/fileAddress", "r");
    if(file == NULL){
        perror("Cannot open .sem/staging/fileAddress!\n");
        return 1;
    }
    char line[1000];
    while(fgets(line, sizeof(line), file) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;

        if(strcmp(filepath, line) == 0) return 0;
    }
    fclose(file);

    file = fopen(".sem/staging/fileAddress", "a");
    if(file == NULL) return 1;
    chdir(cwd);
    char *path = realpath(filepath, NULL);
    if(path == NULL){
        fprintf(stderr, "Cannot find file with name [%s]", filepath);
    }
    fprintf(file, "%s\n", path);
    fclose(file);
    
    return 0;
}

// #define _DEBUG_
#ifdef _DEBUG_
int main(){
    int argc = 3;
    char* argv[] = {"sem", "add", "test"};
#else
int main(int argc, char* argv[]){
#endif
    if(argc < 2){
        fprintf(stdout, "Please enter a valid command\n");
        return 1;
    }
    if(strcmp(argv[1], "init") == 0){
        return run_init(argc, argv);
    }
    else if(strcmp(argv[1], "add") == 0){
        return run_add(argc, argv);
    }
    return 0;
}
