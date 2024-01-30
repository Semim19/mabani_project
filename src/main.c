#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// color define
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
// color define end
int run_init(int argc, char* const argv[]);
int run_add(int argc, char* const argv[]);
int add_to_staging(char *filepath);
int isDir(const char* fileName);
int dir_staging(const char* dirname);
int file_copying(FILE *input, FILE *output);
void directory_tree(char *filepath, int max_depth, int curr_depth);
int check_staged(char *filepath){
    FILE *file = fopen(".sem/staging/fileAddress", "r");
    char line[1000];
    while(fgets(line, 1000, file) != NULL){
        line[strcspn(line, "\n")] = 0;
        if(strcmp(filepath, line) == 0)
            return 1;
    }
    return 0;
}

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
            perror("Cannot open current directory.");
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
        perror("sem repo has already been initialized!");
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
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        strcpy(full_address, dirname);
        strcat(full_address, "/");
        strcat(full_address, entry->d_name);
        if(entry->d_type == DT_DIR){
            dir_staging(full_address);
        }
        strcpy(full_address, dirname);
        strcat(full_address, "/");
        strcat(full_address, entry->d_name);
        if(entry->d_type != DT_DIR){
            add_to_staging(full_address);
        }
    }
    return 0;
}
void directory_tree(char *filepath, int max_depth, int curr_depth){
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(filepath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            for (int i = 0; i < curr_depth; i++)
                printf("  ");
            strcpy(path, filepath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            if(dp->d_type != DT_DIR){
                char *realdir = realpath(path, F_OK);
                if(check_staged(realdir)){
                    printf("|-- " GRN "%s (staged)\n" RESET, dp->d_name);
                }
                else{
                    printf("|-- " RED "%s (unstaged)\n" RESET, dp->d_name);
                }
            }
            if(dp->d_type == DT_DIR){
                printf("|-- %s\n" , dp->d_name);
            directory_tree(path, max_depth, curr_depth + 1);
            }
            
        }
    }

    closedir(dir);
}
int run_add(int argc, char* const argv[]){
    if(argc < 3){
        perror("Please choose a file!");
        return 1;
    }
    if(strcmp(argv[2], "-n") == 0){
        if(argc != 4){
            perror("Please enter a valid number");
            return 1;
        }
        int depth = atoi(argv[3]);
        directory_tree(".", depth, 0);
    }
    else if(strcmp(argv[2], "-f") == 0){
        if(argc < 4){
            perror("Please choose a file!");
            return 1;
        }
        for(int i = 3; i < argc; i++){
            if(access(argv[i], F_OK) != 0){
                perror("File doesn't exist!");
                return 1;
            }
            if(isDir(argv[i]) == 0)
                dir_staging(argv[i]);
            else
                add_to_staging(argv[i]);
            
        }
        return 0;
    }
    else{
        for(int i = 2; i < argc; i++){
            if(access(argv[i], F_OK) != 0){
                perror("File doesn't exist!");
                return 1;
            }
            if(isDir(argv[i]) == 0)
                dir_staging(argv[i]);
            else
                add_to_staging(argv[i]);
        }
    }
    return 0;
}
int add_to_staging(char *filepath){
    char *filename = malloc(1000);
    char *filename2 = malloc(1000);
    strcpy(filename2, filepath);
    strcpy(filename, filepath);
    while((filename2 = strchr(filename2, '/')) != NULL){
        filename2++;
        strcpy(filename, filename2);
    }
    char cwd[1000];
    if(getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    struct dirent *entry; // pointer to check each file
    FILE *file = fopen(".sem/staging/fileAddress", "r");
    if(file == NULL){
        perror("Cannot open .sem/staging/fileAddress!");
        return 1;
    }
    char line[1000];
    char *path = realpath(filepath, NULL);
    if(path == NULL){
        fprintf(stderr, "Cannot find file with name [%s]", path);
        return 1;
    }
    while(fgets(line, sizeof(line), file) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;

        if(strcmp(path, line) == 0) return 0;
    }
    fclose(file);

    file = fopen(".sem/staging/fileAddress", "a");
    if(file == NULL) return 1;
    if(chdir(".sem/staging") != 0) return 1;
    FILE *file_output = fopen(filename, "w");
    if(file_output == NULL){
        perror("Cannot open copy file!");
        return 1;
    }
    if(chdir(cwd) != 0) return 1;
    FILE *file_input = fopen(filepath, "r");
    if(file_input == NULL){
        perror("Cannot open original file!");
        return 1;
    }
    fprintf(file, "%s\n", path);
    fclose(file);
    free(filename);
    free(filename2);
    return file_copying(file_input, file_output);
}
int file_copying(FILE *input, FILE *output){
    char c = fgetc(input); 
    while (c != EOF) 
    { 
        fputc(c, output); 
        c = fgetc(input); 
    }
    fclose(input);
    fclose(output);
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
