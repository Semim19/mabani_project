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
int folder_staging(const char* dirname);

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
        FILE *file = fopen(".sem/staging", "w");
        fclose(file);
        file = fopen(".sem/tracks", "w");
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
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    char full_address[1000];
    strcpy(full_address, dirname);
    while((entry = readdir(dir)) != NULL){
        strcat(full_address, entry->d_name);
        if(entry->d_type == DT_DIR){
            dir_staging(full_address);
        }
        else{
            add_to_staging(full_address);
        }
        strcpy(full_address, dirname);
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
    FILE *file = fopen(".sem/staging", "r");
    if(file == NULL){
        perror("Cannot open .sem/staging!\n");
        return 1;
    }
    char line[1000];
    while(fgets(line, sizeof(line), file) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;

        if(strcmp(filepath, line) == 0) return 0;
    }
    fclose(file);

    file = fopen(".sem/staging", "a");
    if(file == NULL) return 1;

    fprintf(file, "%s\n", filepath);
    fclose(file);
    
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
    else if(strcmp(argv[1], "add") == 0){
        return run_add(argc, argv);
    }
    return 0;
}
