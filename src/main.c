#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

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

// struct
typedef struct alias{
    char ghadimy[51];
    char jadid[101];
    struct alias *next;
} alias;
//struct end

//functions
void show_info(char id[]);
void previdfind(char id[]);
int isHEAD(char id[], char branch[]);
int isNum(char reshte[]);
int makeName(char filename[], char filepath[]);
int inc_last_commit_ID();
int get_username(char name[]);
int get_useremail(char email[]);
void add_to_list(alias **head, char line[]);
void add_command(alias **head, char mode[]);
void add_message(alias **head);
int run_init(int argc, char* const argv[]);
int run_add(int argc, char* const argv[]);
int run_reset(int argc, char* const argv[]);
int run_config(int argc, char* const argv[]);
int run_checkout(int argc, char* const argv[]);
int run_branch(int argc, char* const argv[]);
int run_log(int argc, char* const argv[]);
int run_revert(int argc, char* const argv[]);
int run_grep(int argc, char* const argv[]);
int run_tag(int argc, char* const argv[]);
int add_to_staging(char *filepath);
int isDir(const char* fileName);
int dir_staging(const char* dirname);
int file_copying(FILE *input, FILE *output);
void directory_tree(char *filepath, int max_depth, int curr_depth);
int check_staged(char *filepath);
int reseting(char *filepath);
int removing_file_address(char *fileabs);
int dir_reseting(const char *filepath);
int adding_file_address(char *fileabs);
int run_set_message(int argc, char* const argv[]);
int remove_message(int argc, char* const argv[]);
int replace_message(int argc, char* const argv[]);
int run_commit(int argc, char* const argv[]);
int compare_date(char *file1, char *file2);
int checkoutid(char ID[]);

void previdfind(char id[]){
    char tmp[20];
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    chdir(".sem/commits");
    chdir(id);
    FILE *file = fopen("previd", "r");
    fscanf(file, "%s", tmp);
    fclose(file);
    tmp[strcspn(tmp, "\n")] = 0;
    strcpy(id, tmp);
    chdir(cwd);
    return;
}
int isNum(char reshte[]){
    int x = strlen(reshte);
    for(int i = 0; i < x; i++){
        if(!isdigit(reshte[i]))
        return 0;
    }
    return 1;
}
int makeName(char filename[], char filepath[]){
    char *filename2 = malloc(1000);
    strcpy(filename2, filepath);
    strcpy(filename, filepath);
    while((filename2 = strchr(filename2, '/')) != NULL){
        filename2++;
        strcpy(filename, filename2);
    }
    free(filename2);
    return 0;
}
int get_useremail(char email[]){
    FILE *file = fopen(".sem/config/useremail", "r");
    char configpath[1000];
    sprintf(configpath, "%s", getenv("HOME"));
    strcat(configpath, "/.semconfig/useremail");
    FILE *file2 = fopen(configpath, "r");
    if(file == NULL && file2 == NULL){
        perror("Please config your useremail!");
        return 1;
    }
    char line[1000];
    if(file == NULL){
        fgets(line, 1000, file2);
        line[strcspn(line, "\n")] = 0;
        strcpy(email, line);
        fclose(file2);
        return 0;
    }
    if(file2 == NULL){
        fgets(line, 1000, file);
        line[strcspn(line, "\n")] = 0;
        strcpy(email, line);
        fclose(file);
        return 0;
    }
    if(compare_date(".sem/config/useremail", configpath)){
        fgets(line, 1000, file);
        line[strcspn(line, "\n")] = 0;
        strcpy(email, line);
        fclose(file);
        fclose(file2);
        return 0;
    } else {
        fgets(line, 1000, file2);
        line[strcspn(line, "\n")] = 0;
        strcpy(email, line);
        fclose(file);
        fclose(file2);
        return 0;
    }
}int get_username(char name[]){
    FILE *file = fopen(".sem/config/username", "r");
    char configpath[1000];
    sprintf(configpath, "%s", getenv("HOME"));
    strcat(configpath, "/.semconfig/username");
    FILE *file2 = fopen(configpath, "r");
    if(file == NULL && file2 == NULL){
        perror("Please config your username!");
        return 1;
    }
    char line[1000];
    if(file == NULL){
        fgets(line, 1000, file2);
        line[strcspn(line, "\n")] = 0;
        strcpy(name, line);
        fclose(file2);
        return 0;
    }
    if(file2 == NULL){
        fgets(line, 1000, file);
        line[strcspn(line, "\n")] = 0;
        strcpy(name, line);
        fclose(file);
        return 0;
    }
    if(compare_date(".sem/config/username", configpath)){
        fgets(line, 1000, file);
        line[strcspn(line, "\n")] = 0;
        strcpy(name, line);
        fclose(file);
        fclose(file2);
        return 0;
    } else {
        fgets(line, 1000, file2);
        line[strcspn(line, "\n")] = 0;
        strcpy(name, line);
        fclose(file);
        fclose(file2);
        return 0;
    }
}
int compare_date(char *file1, char *file2){
    struct stat attr1;
    struct stat attr2;
    stat(file1, &attr1);
    stat(file2, &attr2);
    if(attr1.st_mtime > attr2.st_mtime){
        return 1;
    }
    return 0;
}
void add_to_list(alias **head, char line[]){
    alias *curr = (alias*) malloc(sizeof(alias));
    char *token;
    token = strtok(line, ":");
    int khat = 0;
    while(token != NULL){
        if(khat == 0){
            strcpy(curr->ghadimy, token);
            khat++;
        } else {
            strcpy(curr->jadid, token);
        }
        token = strtok(NULL, ":");
    }
    if(*head == NULL){
        *head = curr;
    } else {
        alias *temp = *head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = curr;
    }
}
void add_message(alias **head){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    FILE *file = fopen(".sem/config/message", "r");
    if(file == NULL){
        return;
    }
    char line[1000];
    while(fgets(line, 1000, file) != NULL){
        line[strcspn(line, "\n")] = 0;
        add_to_list(head, line);
    }
}
void add_command(alias **head, char mode[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    if(strcmp(mode, "--global") != 0){
        chdir(".sem/config");
        FILE *file = fopen("alias", "r");
        char line[1000];
        while(fgets(line, 1000, file) != NULL){
            line[strcspn(line, "\n")] = 0;
            add_to_list(head, line);
        }
        chdir(cwd);
        return;
    } else {
        char path[1000];
        sprintf(path, "%s", getenv("HOME"));
        chdir(path);
        if(access(".semconfig", F_OK) != 0)
            return;
        chdir(".semconfig");
        if(access("alias", F_OK) != 0)
            return;
        FILE *file = fopen("alias", "r");
        char line[1000];
        while(fgets(line, 1000, file) != NULL){
            line[strcspn(line, "\n")] = 0;
            add_to_list(head, line);
        }
        chdir(cwd);
        return;

    }
}
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
        if(mkdir("reset", 0755) != 0) return 1;
        if(mkdir("config", 0755) != 0) return 1;
        if(mkdir("branches", 0755) != 0) return 1;
        if(mkdir("commits", 0755) != 0) return 1;
        if(mkdir("tags", 0755) != 0) return 1;

        chdir(cwd);
        FILE *file = fopen(".sem/staging/fileAddress", "w");
        fclose(file);
        file = fopen(".sem/reset/fileAddress", "w");
        fclose(file);
        file = fopen(".sem/config/alias", "w");
        fclose(file);
        file = fopen(".sem/config/message", "w");
        fclose(file);
        file = fopen(".sem/tracks", "w");
        fclose(file);
        file = fopen(".sem/lastid", "w");
        fprintf(file, "%d\n", 0);
        fclose(file);
        file = fopen(".sem/currbranch", "w");
        fprintf(file, "%s", "master");
        fclose(file);
        file = fopen(".sem/state", "w");
        fprintf(file, "%s", "HEAD");
        fclose(file);
        file = fopen(".sem/branches/master", "w");
        fprintf(file, "%d\n", 0);
        fclose(file);
        file = fopen(".sem/hookstatus", "w");
        fprintf(file, "%d\n", 1);
        fclose(file);
        file = fopen(".sem/config/hooks", "w");
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
    if(curr_depth > max_depth)
        return;
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
            if(dp->d_type == DT_DIR && strcmp(dp->d_name, ".sem") != 0){
                printf("|-- %s\n" , dp->d_name);
            directory_tree(path, max_depth, curr_depth + 1);
            }
            
        }
    }

    closedir(dir);
}
int adding_file_address(char *fileabs){
    FILE *file = fopen(".sem/reset/fileAddress", "r");
    FILE *file2 = fopen(".sem/staging/fileAddress", "a");
    FILE *file3 = fopen(".sem/reset/newfile", "w");
    char line[1000];
    while((fgets(line, 1000, file)) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;
        if(strcmp(fileabs, line) == 0){
            fputs(line, file2);
            fputs("\n", file2);
        }
        else{
            fputs(line, file3);
            fputs("\n", file3);
        }
    }
    fclose(file);
    fclose(file2);
    fclose(file3);
    system("cd .sem/reset && rm fileAddress");
    system("cd .sem/reset && mv newfile fileAddress");
    return 0;
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
    else if(strcmp(argv[2], "-redo") == 0){
        DIR *dir = opendir(".sem/reset");
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "fileAddress") != 0){
                char command[1000];
                sprintf(command, "cd .sem && mv reset/%s staging/%s", entry->d_name, entry->d_name);
                system(command);
            }
        }
        FILE *file = fopen(".sem/reset/fileAddress", "r");
        char path[1000];
        while(fgets(path, 1000, file) != NULL){
            path[strcspn(path, "\n")] = 0;
            if(strcmp(path, "=") != 0)
                adding_file_address(path);
        }
        fclose(file);
        file = fopen(".sem/reset/fileAddress", "w");
        return 0;
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
    int flag = 1;
    int flag2 = 1;
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

        if(strcmp(path, line) == 0){
            flag = 0;
            break;
        }
    }
    fclose(file);
    file = fopen(".sem/tracks", "r");
    while(fgets(line, sizeof(line), file) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;

        if(strcmp(path, line) == 0){
            flag2 = 0;
            break;
        }
    }
    fclose(file);
    file = fopen(".sem/staging/fileAddress", "a");
    FILE *file2 = fopen(".sem/tracks", "a");
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
    if(flag)
        fprintf(file, "%s\n", path);
    if(flag2)
        fprintf(file2, "%s\n", path);
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
int dir_reseting(const char* dirname){
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
            dir_reseting(full_address);
        }
        strcpy(full_address, dirname);
        strcat(full_address, "/");
        strcat(full_address, entry->d_name);
        if(entry->d_type != DT_DIR){
            reseting(full_address);
        }
    }
    return 0;
}
int run_reset(int argc, char* const argv[]){
    if(argc < 3){
        perror("Please choose a file!");
        return 1;
    }
    if(strcmp(argv[2], "-undo") == 0){
        FILE *reset = fopen(".sem/reset/fileAddress", "r");
        FILE *replace = fopen(".sem/reset/rand", "w");
        int chand = 0;
        char line[1000];
        while(fgets(line, 1000, reset) != NULL){
            if(strcmp(line, "=\n") == 0){
                fclose(replace);
                replace = fopen(".sem/reset/rand", "w");
                chand++;
            }
            fprintf(replace, "%s", line);
        }
        fclose(reset);
        fclose(replace);
        replace = fopen(".sem/reset/rand", "r");
        while(fgets(line, 1000, replace) != NULL){
            if(strcmp(line, "=\n") == 0){
                continue;
            }
            line[strcspn(line, "\n")] = 0;
            int flag = 1;
            int flag2 = 1;
            char *filename = malloc(1000);
            char *filename2 = malloc(1000);
            strcpy(filename2, line);
            strcpy(filename, line);
            while((filename2 = strchr(filename2, '/')) != NULL){
                filename2++;
                strcpy(filename, filename2);
            }
            char command[1000];
            sprintf(command, "mv .sem/reset/%s .sem/staging/%s", filename, filename);
            system(command);
            free(filename2);
            free(filename);
        }
        fclose(replace);
        FILE *file = fopen(".sem/reset/rand", "r");
        FILE *file2 = fopen(".sem/staging/fileAddress", "a");
        while((fgets(line, 1000, file)) != NULL){
            // remove \n
            line[strcspn(line, "\n")] = 0;
            fprintf(file2, "%s\n", line);
        }
        fclose(file);
        fclose(file2);
        FILE *rand = fopen(".sem/reset/rand2", "w");
        reset = fopen(".sem/reset/fileAddress", "r");
        int counter = 0;
        while(1){
            fgets(line, 1000, reset);
            if(strcmp(line, "=\n") == 0){
                counter++;
                if(counter >= chand){
                    break;
                }
            }
            fprintf(rand, "%s", line);
        }
        fclose(rand);
        fclose(reset);
        remove(".sem/reset/rand");
        remove(".sem/reset/fileAddress");
        system("mv .sem/reset/rand2 .sem/reset/fileAddress");
        return 0;
    }
    FILE *file = fopen(".sem/reset/fileAddress", "a");
    fprintf(file, "=\n");
    fclose(file);
    if(strcmp(argv[2], "-f") == 0){
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
                dir_reseting(argv[i]);
            else
                reseting(argv[i]);
            
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
                dir_reseting(argv[i]);
            else
                reseting(argv[i]);
        }
    }
    return 0;
    
}

int reseting(char *filepath){
    char *path = realpath(filepath, F_OK);
    if(!check_staged){
        perror("The file is not staged!");
        return 1;
    }
    int flag = 1;
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
    chdir(".sem/reset");
    FILE *file = fopen("fileAddress", "a");
    chdir("../..");
    fputs(path, file);
    fputs("\n", file);
    fclose(file);
    char command[1000];
    sprintf(command, "cd .sem && mv staging/%s reset/%s", filename, filename);
    system(command);
    removing_file_address(path);
    return 0;
}

int removing_file_address(char *fileabs){
    FILE *file = fopen(".sem/staging/fileAddress", "r");
    FILE *file2 = fopen(".sem/staging/newfile", "w");
    char line[1000];
    while((fgets(line, 1000, file)) != NULL){
        // remove \n
        line[strcspn(line, "\n")] = 0;
        if(strcmp(fileabs, line) != 0){
            fputs(line, file2);
            fputs("\n", file2);
        }
    }
    fclose(file);
    fclose(file2);
    char cwd[1000];
    if(getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    system("cd .sem/staging && rm fileAddress");
    system("cd .sem/staging && mv newfile fileAddress");
    return 0;
}
int run_config(int argc, char* const argv[]){
    int khat = 0;
    char line[1000];
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    if(strcmp(argv[2], "--global") == 0){
        char home_path[1000];
        sprintf(home_path, "%s", getenv("HOME"));
        chdir(home_path);
        if(access(".semconfig", F_OK) != 0){
            mkdir(".semconfig", 0755);
        }
        if(strstr(argv[3], "alias") == NULL){
            if(argc < 5){
                perror("Enter a valid command!");
                return 1;
            }
            chdir(".semconfig");
            FILE *file;
            if(strcmp(argv[3], "user.name") == 0){
                file = fopen("username", "w");
                fprintf(file, "%s", argv[4]);
                fclose(file);
            }
            else if(strcmp(argv[3], "user.email") == 0){
                file = fopen("useremail", "w");
                fprintf(file, "%s", argv[4]);
                fclose(file);
            }
            else{
                perror("Enter a valid info");
                chdir(cwd);
                return 1;
            }
            chdir(cwd);
            return 0;
            
        }
        else {
            if(argc < 5){
                perror("Enter a valid command!");
                return 1;
            }
            char primary_command[100];
            sscanf(argv[3], "alias.%s", primary_command);
            chdir(".semconfig");
            FILE *file;
            if(access("alias", F_OK) != 0){
                file = fopen("alias", "w");
                fclose(file);
            }
            file = fopen("alias", "a");
            fprintf(file, "%s:%s\n", primary_command, argv[4]);
            fclose(file);
        }
    }
    else if(argc == 4){
        if(strstr(argv[2], "alias") == NULL){
            chdir(".sem/config");
            FILE *file;
            if(strcmp(argv[2], "user.name") == 0){
                file = fopen("username", "w");
                fprintf(file, "%s", argv[3]);
                fclose(file);
            }
            else if(strcmp(argv[2], "user.email") == 0){
                file = fopen("useremail", "w");
                fprintf(file, "%s", argv[3]);
                fclose(file);
            }
            else{
                perror("Enter a valid info");
                chdir(cwd);
                return 1;
            }
            chdir(cwd);
            return 0;
        }
        else {
            char primary_command[100];
            sscanf(argv[2], "alias.%s", primary_command);
            chdir(".sem/config");
            FILE *file;
            if(access("alias", F_OK) != 0){
                file = fopen("alias", "w");
                fclose(file);
            }
            file = fopen("alias", "a");
            fprintf(file, "%s:%s\n", primary_command, argv[3]);
            fclose(file);
            chdir(cwd);
        }
    }
    else{
        perror("Enter a valid command!");
        return 1;
    }
}

int run_set_message(int argc, char* const argv[]){
    if(argc < 6 || strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0){
        perror("Invalid input!");
        return 1;
    }
    FILE *file = fopen(".sem/config/message", "a");
    fprintf(file, "%s:%s\n", argv[5], argv[3]);
}
int replace_message(int argc, char* const argv[]){
    if(argc < 6 || strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0){
        perror("Invalid input!");
        return 1;
    }
    char shortcut[100];
    strcpy(shortcut, argv[5]);
    strcat(shortcut, ":");
    FILE *file = fopen(".sem/config/message", "r");
    FILE *file2 = fopen(".sem/config/random", "w");
    char line[1000];
    while(fgets(line, 1000, file) != NULL){
        if(strstr(line, shortcut) == NULL){
            fprintf(file2, "%s", line);
        } else {
            fprintf(file2, "%s:%s", argv[5], argv[3]);
        }
    }
    fclose(file);
    fclose(file2);
    system("cd .sem/config && rm message");
    system("cd .sem/config && mv random message");
    return 0;
}
int remove_message(int argc, char* const argv[]){
    if(argc < 4 || strcmp(argv[2], "-s") != 0){
        perror("Invalid input!");
        return 1;
    }
    char shortcut[100];
    strcpy(shortcut, argv[3]);
    strcat(shortcut, ":");
    FILE *file = fopen(".sem/config/message", "r");
    FILE *file2 = fopen(".sem/config/random", "w");
    int exist = 0;
    char line[1000];
    while(fgets(line, 1000, file) != NULL){
        if(strstr(line, shortcut) == NULL){
            fprintf(file2, "%s", line);
        } else {
            exist = 1;
        }
    }
    fclose(file);
    fclose(file2);
    system("cd .sem/config && rm message");
    system("cd .sem/config && mv random message");
    if(exist == 0){
        perror("Shortcut does not exist!");
        return 1;
    }
    return 0;
}
int run_commit(int argc, char* const argv[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char branch[1000];
    int curr_id;
    int prev_id;
    char message[1000] = "-1";
    char username[1000];
    char useremail[1000];
    char ID[20];
    char prevID[20];
    if(argc != 4 || (strcmp(argv[2], "-m") != 0 && strcmp(argv[2], "-s") != 0)){
        perror("Enter a valid command!");
        return 1;
    }
    char line[1000];
    FILE *file = fopen(".sem/state", "r");
    if(fgets(line, 1000, file) != NULL){
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, "HEAD") != 0){
            perror("You are not at HEAD, you can not commit!");
            return 1;
        }
    }
    fclose(file);
    if(get_username(username) != 0)     return 1;
    if(get_useremail(useremail) != 0)   return 1;
    file = fopen(".sem/staging/fileAddress", "r");
    if(fgets(line, 1000, file) == NULL){
        perror("Staging area is empty!");
        fclose(file);
        return 1;
    }
    fclose(file);
    if(strcmp(argv[2], "-m") == 0){
        strcpy(message, argv[3]);
    }
    else if(strcmp(argv[2], "-s") == 0){
        alias* payam = NULL;
        add_message(&payam);
        while(payam != NULL){
            if(strcmp(payam->ghadimy, argv[3]) == 0){
                strcpy(message, payam->jadid);
                break;
            }
            payam = payam->next;
        }
        if(strcmp(message, "-1") == 0){
            perror("Invalid shortcut message!");
            return 1;
        }
    }
    if(strlen(message) > 73){
        perror("Message is too long!");
        return 1;
    }
    file = fopen(".sem/currbranch", "r");
    fgets(branch, 1000, file);
    branch[strcspn(branch, "\n")] = 0;
    fclose(file);
    char branchpath[1000];
    strcpy(branchpath, ".sem/branches/");
    DIR *dir = opendir(".sem/branches");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, branch) == 0){
            strcat(branchpath, entry->d_name);
            break;
        }
    }
    file = fopen(branchpath, "r");
    fscanf(file, "%d\n", &prev_id);
    fclose(file);
    curr_id = inc_last_commit_ID();
    sprintf(ID, "%d", curr_id);
    sprintf(prevID, "%d", prev_id);
    chdir(".sem/commits");
    if(prev_id == 0){
        if(mkdir(ID, 0755) != 0){
            perror("Cannot make the first directory!");
            return 1;
        }
        chdir(cwd);
    } else {
        char command[1000];
        sprintf(command, "cp -r %s %s", prevID, ID);
        system(command);
        chdir(ID);
        FILE *prev = fopen("previd", "w");
        fprintf(prev, "%d", prev_id);
        fclose(prev);
        chdir(cwd);
    }
    char tmp_path[1000];
    strcpy(tmp_path, ".sem/branches/");
    strcat(tmp_path, branch);
    file = fopen(tmp_path, "w");
    fprintf(file, "%d", curr_id);
    fclose(file);
    chdir(".sem/commits");
    chdir(ID);
    file = fopen("Branch", "w");
    fprintf(file, "%s", branch);
    fclose(file);
    file = fopen("userinfo", "w");
    fprintf(file, "%s, %s", username, useremail);
    fclose(file);
    file = fopen("message", "w");
    fprintf(file, "%s", message);
    fclose(file);
    FILE *timedate = fopen("time", "w");
    FILE *timebin = fopen("time.bin", "wb");
    time_t currtime;
    time(&currtime);
    fwrite(&currtime, sizeof(time_t), 1, timebin);
    fclose(timebin);

    struct tm *localTime = localtime(&currtime);
    fprintf(timedate, "%s\n", asctime(localTime));
    fclose(timedate);
    chdir(cwd);
    chdir(".sem/staging");
    DIR *secdir = opendir(".");
    struct dirent *secentry;
    while((secentry = readdir(secdir)) != NULL){
        if(strcmp(secentry->d_name, ".") != 0 && strcmp(secentry->d_name, "..") != 0 && strcmp(secentry->d_name, "fileAddress") != 0){
            char command[1000];
            sprintf(command, "mv -f %s ../commits/%s/%s", secentry->d_name, ID, secentry->d_name);
            system(command);
        }
    }
    file = fopen("fileAddress", "w");
    fclose(file);
    chdir("..");
    FILE *trackings = fopen("tracks", "r");
    char path[1000];
    while(fgets(path, 1000, trackings)){
        path[strcspn(path, "\n")] = 0;
        char name[1000];
        makeName(name, path);
        char newpath[2000];
        sprintf(newpath, "commits/%s/%s", ID, name);
        if(access(path, F_OK) != 0){
            remove(newpath);
            FILE *trackings2 = fopen("tracks", "r");
            FILE *finaltrack = fopen("tmp_track", "w");
            while(fgets(line, 1000, trackings2) != NULL){
                line[strcspn(line, "\n")] = 0;
                if(strcmp(line, path) != 0){
                    fprintf(finaltrack, "%s\n", line);
                }
            }
            fclose(trackings2);
            fclose(finaltrack);
            remove("tracks");
            rename("tmp_track", "tracks");
        }
    }
    fclose(trackings);
    char command[1000];
    sprintf(command, "cp -f %s commits/%s/%s", "tracks", ID, "tracks");
    system(command);

    chdir("commits");
    chdir(ID);
    int count = 0;
    FILE *numfiles = fopen("numfiles", "w");
    DIR *newdir = opendir(".");
    struct dirent *newentry;
    while((newentry = readdir(newdir)) != NULL){
        if(strcmp(newentry->d_name, ".") != 0 && strcmp(newentry->d_name, "..") != 0){
            count++;
        }
    }
    count -= 7;
    fprintf(numfiles, "%d", count);
    fclose(numfiles);
    chdir(cwd);
    printf("commit with ID: %d\nDate and Time: %sMessage: %s\n", curr_id, asctime(localTime), message);
    return 0;
}
int inc_last_commit_ID() {
    FILE *file = fopen(".sem/lastid", "r");
    if (file == NULL) return -1;
    
    FILE *tmp_file = fopen(".sem/tmp_config", "w");
    if (tmp_file == NULL) return -1;

    int last_commit_ID;
    char line[1000];
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%d\n", &last_commit_ID);
        last_commit_ID++;
        fprintf(tmp_file, "%d\n", last_commit_ID);
    }
    fclose(file);
    fclose(tmp_file);

    remove(".sem/lastid");
    rename(".sem/tmp_config", ".sem/lastid");
    return last_commit_ID;
}
int run_branch(int argc, char* const argv[]){
    if(argc > 3){
        perror("Invalid input!");
        return 1;
    }
    if(argc == 2){
        char name[1000];
        FILE *file = fopen(".sem/currbranch", "r");
        fscanf(file, "%s", name);
        name[strcspn(name, "\n")] = 0;
        DIR *dir = opendir("./.sem/branches");
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                if(strcmp(name, entry->d_name) == 0){
                    printf(GRN "* %s\n" RESET, name);
                } else {
                    printf("%s\n", entry->d_name);
                }
            }
        }
        return 0;
    } else {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        DIR *dir = opendir("./.sem/branches");
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                if(strcmp(entry->d_name, argv[2]) == 0){
                    perror("Branch already exists!");
                    return 1;
                }
        }
        int curr_id;
        char ID[20];
        int prev_id;
        char prevID[20];
        char prev_branch[100];
        curr_id = inc_last_commit_ID();
        if(curr_id == 1){
            perror("Commit at least once before creating a branch!");
            return 1;
        }
        FILE *last_branch = fopen(".sem/currbranch", "r");
        fscanf(last_branch, "%s", prev_branch);
        fclose(last_branch);
        prev_branch[strcspn(prev_branch, "\n")] = 0;
        char last_branch_path[1000];
        strcpy(last_branch_path, ".sem/branches/");
        strcat(last_branch_path, prev_branch);
        FILE *file = fopen(last_branch_path, "r");
        fscanf(file, "%d", &prev_id);
        fclose(file);
        sprintf(prevID, "%d", prev_id);
        sprintf(ID, "%d", curr_id);
        char command[1000];
        sprintf(command, "cp -r .sem/commits/%s .sem/commits/%s", prevID, ID);
        system(command);
        chdir(".sem/commits");
        chdir(ID);
        FILE *branch = fopen("Branch", "w");
        fprintf(branch, "%s", argv[2]);
        fclose(branch);
        chdir("../../branches");
        branch = fopen(argv[2], "w");
        fprintf(branch, "%d", curr_id);
        fclose(branch);
        chdir("..");
        branch = fopen("lastid", "w");
        fprintf(branch, "%d", curr_id);
        fclose(branch);
        chdir(cwd);
        return 0;
    }
}
int isHEAD(char id[], char branch[]){
    char path[1000];
    char tmp[20];
    strcpy(path, ".sem/branches/");
    strcat(path, branch);
    FILE *file = fopen(path, "r");
    fscanf(file, "%s", tmp);
    tmp[strcspn(tmp, "\n")] = 0;
    if(strcmp(tmp, id) == 0)
        return 1;
    return 0;
}
int checkoutid(char ID[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char currbranch[1000];
    char line[1000];
    FILE *file = fopen(".sem/staging/fileAddress", "r");
    int flag = 0;
    while(fgets(line, 1000, file) != NULL){
        flag = 1;
    }
    if(flag){
        perror("Your staging area must be empty to checkout!");
        fclose(file);
        return 1;
    }
    fclose(file);
    file = fopen(".sem/currbranch", "r");
    fscanf(file, "%s", currbranch);
    fclose(file);
    currbranch[strcspn(currbranch, "\n")] = 0;
    chdir(".sem/commits");
    chdir(ID);
    file = fopen("Branch", "r");
    char tmp_branch[1000];
    fscanf(file, "%s", tmp_branch);
    tmp_branch[strcspn(tmp_branch, "\n")] = 0;
    fclose(file);
    if(strcmp(currbranch, tmp_branch) != 0){
        perror("Checkout to the branch first, then checkout to the commit");
        return 1;
    }
    file = fopen("tracks", "r");
    FILE *tracks = fopen("../../tracks", "r");
    char satr[2000];
    while(fgets(satr, 2000, file) != NULL){
        satr[strcspn(satr, "\n")] = 0;
        char filename[1000];
        makeName(filename, satr);
        // printf(GRN "%s\n%s\n" RESET, filename, satr);
        char command[5000];
        sprintf(command, "cp -f %s %s", filename, satr);
        system(command);
    }
    rewind(file);
    while(fgets(satr, 2000, tracks) != NULL){
        satr[strcspn(satr, "\n")] = 0;
        char line2[2000];
        int removed = 1;
        while(fgets(line2, 2000, file) != NULL){
            line2[strcspn(line2, "\n")] = 0;
            if(strcmp(satr, line2) == 0){
                removed = 0;
                break;
            }
        }
        if(removed == 1)
            remove(satr);
    }
    fclose(file);
    fclose(tracks);
    system("cp -f tracks ../../tracks");
    chdir(cwd);
    if(isHEAD(ID, currbranch)){
        file = fopen(".sem/state", "w");
        fprintf(file, "HEAD");
        fclose(file);
        return 0;
    }
    file = fopen(".sem/state", "w");
    fprintf(file, "%s", ID);
    fclose(file);
    return 0;
}
int run_checkout(int argc, char* const argv[]){
    if(argc > 3){
        perror("Invalid input!");
        return 1;
    }
    if(isNum(argv[2])){
        return checkoutid(argv[2]);
    }
    else if(strcmp(argv[2], "HEAD") == 0){
        char ID[20];
        char namebranch[1000];
        FILE *branch = fopen(".sem/currbranch", "r");
        fscanf(branch, "%s", namebranch);
        fclose(branch);
        namebranch[strcspn(namebranch, "\n")] = 0;
        char path[1000];
        strcpy(path, ".sem/branches/");
        strcat(path, namebranch);
        FILE *mammad = fopen(path, "r");
        fscanf(mammad, "%s", ID);
        ID[strcspn(ID, "\n")] = 0;
        fclose(mammad);
        return checkoutid(ID);
    }
    else if(strstr(argv[2], "HEAD-") != NULL){
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        int count = 0;
        sscanf(argv[2], "HEAD-%d", &count);
        char ID[20];
        char namebranch[1000];
        FILE *branch = fopen(".sem/currbranch", "r");
        fscanf(branch, "%s", namebranch);
        fclose(branch);
        namebranch[strcspn(namebranch, "\n")] = 0;
        char path[1000];
        strcpy(path, ".sem/branches/");
        strcat(path, namebranch);
        FILE *mammad = fopen(path, "r");
        fscanf(mammad, "%s", ID);
        ID[strcspn(ID, "\n")] = 0;
        fclose(mammad);
        for(int i = 0; i < count; i++){
            previdfind(ID);
        }
        return checkoutid(ID);
    } else {
        char ID[20];
        char branchname[1000];
        strcpy(branchname, argv[2]);
        char path[1000];
        strcpy(path, ".sem/branches/");
        strcat(path, branchname);
        FILE *branch = fopen(path, "r");
        if(branch == NULL){
            perror("No such branch exists!");
            return 1;
        }
        fscanf(branch, "%s", ID);
        ID[strcspn(ID, "\n")] = 0;
        fclose(branch);
        branch = fopen(".sem/currbranch", "w");
        fprintf(branch, "%s", branchname);
        fclose(branch);
        return checkoutid(ID);
    }
}
void show_info(char id[]){
    chdir(id);
    char message[100];
    char date[500];
    char name[1000];
    char branch[1000];
    char nums[10];
    FILE *file = fopen("message", "r");
    fgets(message, 100, file);
    message[strcspn(message, "\n")] = 0;
    fclose(file);
    file = fopen("time", "r");
    fgets(date, 500, file);
    date[strcspn(date, "\n")] = 0;
    fclose(file);
    file = fopen("userinfo", "r");
    fgets(name, 1000, file);
    name[strcspn(name, "\n")] = 0;
    fclose(file);
    file = fopen("Branch", "r");
    fgets(branch, 1000, file);
    branch[strcspn(branch, "\n")] = 0;
    fclose(file);
    file = fopen("numfiles", "r");
    fgets(nums, 10, file);
    nums[strcspn(nums, "\n")] = 0;
    fclose(file);
    printf("commit id: %s\n", id);
    printf("done by: %s\n", name);
    printf("message: %s\n", message);
    printf("date: %s\n", date);
    printf("branch: %s\n", branch);
    printf("number of commited files: %s\n", nums);
    printf("---------\n");
}
int run_log(int argc, char* const argv[]){
    char cwd[2000];
    getcwd(cwd, sizeof(cwd));
    if(argc == 2){
        char ID[20];
        FILE *file = fopen(".sem/lastid", "r");
        fgets(ID, 20, file);
        fclose(file);
        ID[strcspn(ID, "\n")] = 0;
        int id = atoi(ID);
        chdir(".sem/commits");
        for(int i = id; i > 0; i--){
            sprintf(ID, "%d", i);
            ID[strcspn(ID, "\n")] = 0;
            show_info(ID);
        }
        chdir(cwd);
        return 0;
    }
    if(argc == 4){
        if(strcmp(argv[2], "-n") == 0){
            char ID[20];
            FILE *file = fopen(".sem/lastid", "r");
            fgets(ID, 20, file);
            fclose(file);
            ID[strcspn(ID, "\n")] = 0;
            int id = atoi(ID);
            chdir(".sem/commits");
            int count = atoi(argv[3]);
            for(int i = id; i > id - count; i--){
                sprintf(ID, "%d", i);
                ID[strcspn(ID, "\n")] = 0;
                show_info(ID);
            }
            chdir(cwd);
            return 0;
        }
        else if(strcmp(argv[2], "-branch") == 0){
            chdir(".sem/branches");
            DIR *dir = opendir(".");
            struct dirent *entry;
            int exist = 0;
            while((entry = readdir(dir)) != NULL){
                if(strcmp(entry->d_name, argv[3]) == 0){
                    exist = 1;
                    break;
                }
            }
            if(exist == 0){
                perror("Enter a valid branch!");
                return 1;
            }
            chdir("../..");
            char ID[20];
            FILE *file = fopen(".sem/lastid", "r");
            fgets(ID, 20, file);
            fclose(file);
            ID[strcspn(ID, "\n")] = 0;
            int id = atoi(ID);
            chdir(".sem/commits");
            while(1){
                show_info(ID);
                if(strcmp(ID, "1") == 0){
                    break;
                }
                previdfind(ID);
                chdir(ID);
                FILE *file = fopen("Branch", "r");
                char branch[1000];
                fscanf(file, "%s", branch);
                branch[strcspn(branch, "\n")] = 0;
                fclose(file);
                if(strcmp(argv[3], branch) != 0)
                    break;
                chdir("..");
            }
            chdir(cwd);
            return 0;
        }
        else if(strcmp(argv[2], "-author") == 0){
            char ID[20];
            FILE *file = fopen(".sem/lastid", "r");
            fgets(ID, 20, file);
            fclose(file);
            ID[strcspn(ID, "\n")] = 0;
            int id = atoi(ID);
            chdir(".sem/commits");
            for(int i = id; i > 0; i--){
                sprintf(ID, "%d", i);
                ID[strcspn(ID, "\n")] = 0;
                chdir(ID);
                FILE *file = fopen("userinfo", "r");
                char name[1000];
                fgets(name, 1000, file);
                name[strcspn(name, ",")] = 0;
                if(strcmp(name, argv[3]) == 0)
                    show_info(ID);
                fclose(file);
                chdir("..");
            }
            chdir(cwd);
            return 0;
        }
    }
    else if(strcmp(argv[2], "-word") == 0){
        char ID[20];
        FILE *file = fopen(".sem/lastid", "r");
        fgets(ID, 20, file);
        fclose(file);
        ID[strcspn(ID, "\n")] = 0;
        int id = atoi(ID);
        chdir(".sem/commits");
        for(int i = id; i > 0; i--){
            sprintf(ID, "%d", i);
            ID[strcspn(ID, "\n")] = 0;
            chdir(ID);
            FILE *file = fopen("message", "r");
            char line[1000];
            fgets(line, 1000, file);
            for(int i = 3; i < argc; i++)
                if(strstr(line, argv[i]) != NULL)
                    show_info(ID);
            fclose(file);
            chdir("..");
        }
        chdir(cwd);
        return 0;
    }
}
int run_revert(int argc, char* const argv[]){
    if(argc == 3){
        char heb_id[20];
        if(strstr(argv[2], "HEAD-") != NULL){
            int count = 0;
            sscanf(argv[2], "HEAD-%d", &count);
            char branch3[1000];
            FILE *file = fopen(".sem/currbranch", "r");
            fscanf(file, "%s", branch3);
            branch3[strcspn(branch3, "\n")] = 0;
            fclose(file);
            char heb_path[1000];
            strcpy(heb_path, ".sem/branches/");
            strcat(heb_path, branch3);
            file = fopen(heb_path, "r");
            fscanf(file, "%s", heb_id);
            heb_id[strcspn(heb_id, "\n")] = 0;
            fclose(file);
            for(int i = 0; i < count; i++){
                previdfind(heb_id);
            }
        } else {
            strcpy(heb_id, argv[2]);
        }
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        if(!isNum(heb_id)){
            perror("Enter a valid id");
            return 1;
        }
        char line[1000];
        FILE *file = fopen(".sem/staging/fileAddress", "r");
        if(fgets(line, 1000, file) != NULL){
            perror("Your staging area must be empty!");
            fclose(file);
            return 1;
        }
        fclose(file);
        char branch[1000];
        char branch2[1000];
        file = fopen(".sem/currbranch", "r");
        fscanf(file, "%s", branch);
        branch[strcspn(branch, "\n")] = 0;
        fclose(file);
        chdir(".sem/commits");
        chdir(heb_id);
        file = fopen("Branch", "r");
        fscanf(file, "%s", branch2);
        branch2[strcspn(branch2, "\n")] = 0;
        fclose(file);
        if(strcmp(branch2, branch) != 0){
            perror("You are not on the same branch!");
            return 1;
        }
        chdir(cwd);
        char path[500];
        strcpy(path, ".sem/branches/");
        strcat(path, branch);
        int lastid;
        file = fopen(path, "r");
        fscanf(file, "%d", &lastid);
        fclose(file);
        int id = inc_last_commit_ID();
        char ID[20];
        sprintf(ID, "%d", id);
        chdir(".sem/commits");
        char command[2000];
        sprintf(command, "cp -r %s %s", heb_id, ID);
        system(command);
        chdir(ID);
        file = fopen("previd", "w");
        fprintf(file, "%d", lastid);
        fclose(file);
        chdir(cwd);
        file = fopen(path, "w");
        fprintf(file, "%d", id);
        fclose(file);
        char* const tmp[] = {"sem", "checkout", ID};
        run_checkout(3, tmp);
        return 0;
    }
    if(argc == 4 && strcmp(argv[2], "-n") == 0){
        char* const tmp[] = {"sem", "checkout", argv[3]};
        run_checkout(3, tmp);
        return 0;
    }

}
int run_grep(int argc, char* const argv[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    char word[100];
    strcpy(word, argv[5]);
    int n_exist = 0;
    int satr = 0;
    int id_exist = 0;
    char ID[20];
    char file[1000];
    FILE *myfile;
    strcpy(file, argv[3]);
    for(int i = 6; i < argc; i++){
        if(strcmp(argv[i], "-n") == 0){
            n_exist = 1;
        }
        if(strcmp(argv[i], "-c") == 0){
            strcpy(ID, argv[i + 1]);
            id_exist = 1;
        }
    }
    if(id_exist){
        chdir(".sem/commits");
        chdir(ID);
        myfile = fopen(file, "r");
        if(myfile == NULL){
            perror("Cannot open given file!");
            return 1;
        }
        chdir(cwd);
    }
    myfile = fopen(file, "r");
    char line[1000];
    char avali[500] = "";
    char dovomi[500] = "";
    char sevomi[500] = "";
    int ekhtelaf = 0;
    int len = strlen(word);
    while(fgets(line, 1000, myfile) != NULL){
        satr++;
        line[strcspn(line, "\n")] = 0;
        if(strstr(line, word) != NULL){
            if(n_exist){
                printf("%d: ",satr);
            }
            ekhtelaf = strstr(line, word) - line;
            snprintf(avali, ekhtelaf + 1, "%s", line);
            snprintf(dovomi, len + 1, "%s", line + ekhtelaf);
            sprintf(sevomi, "%s", line + ekhtelaf + len);
            printf("%s" YEL "%s" RESET "%s\n", avali, dovomi, sevomi);
        }
    }
}
int run_tag(int argc, char* const argv[]){
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    if(argc == 2){
        chdir(".sem/tags");
        system("ls");
    }
    else if(strcmp(argv[2], "show") == 0){
        chdir(".sem/tags");
        char line[1000];
        FILE *ajab = fopen(argv[3], "r");
        while(fgets(line, 1000, ajab) != NULL){
            printf("%s", line);
        }
        return 0;
    }
    else{
        char tag_name[1000];
        char ID[20];
        char message[1000];
        char date[1000];
        char username[1000];
        char useremail[1000];
        get_username(username);
        get_useremail(useremail);
        strcpy(tag_name, argv[3]);
        chdir(".sem/tags");
        DIR *dir = opendir(".");
        int f_flag = 0;
        int c_flag = 0;
        int m_flag = 0;
        for(int i = 2; i < argc; i++){
            if(strcmp(argv[i], "-m") == 0){
                m_flag = 1;
                strcpy(message, argv[i + 1]);
            }
            if(strcmp(argv[i], "-c") == 0){
                c_flag = 1;
                strcpy(ID, argv[i + 1]);
            }
            if(strcmp(argv[i], "-f") == 0)
                f_flag = 1;
        }
        FILE *file;
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                if(strcmp(entry->d_name, tag_name) == 0){
                    if(f_flag == 0){
                        perror("This tag already exists!");
                        return 1;
                    } else {
                        break;
                    }
                }
            }
        }
        chdir(cwd);
        if(c_flag == 0){
            file = fopen(".sem/state", "r");
            fscanf(file, "%s", ID);
            fclose(file);
            ID[strcspn(ID, "\n")] = 0;
            if(strcmp(ID, "HEAD") == 0){
                file = fopen(".sem/currbranch", "r");
                char branch[100];
                fscanf(file, "%s", branch);
                branch[strcspn(branch, "\n")] = 0;
                fclose(file);
                chdir(".sem/branches");
                file = fopen(branch, "r");
                fscanf(file, "%s", ID);
                fclose(file);
                ID[strcspn(ID, "\n")] = 0;
            }
            
        }
        chdir(cwd);
        time_t currtime;
        time(&currtime);
        struct tm *localTime = localtime(&currtime);
        sprintf(date, "%s", asctime(localTime));
        chdir(".sem/tags");
        file = fopen(tag_name, "w");
        fprintf(file, "tag %s\ncommit %s\nAuthor: %s <%s>\nDate: %s", tag_name, ID, username, useremail, date);
        fclose(file);
        if(m_flag){
            file = fopen(tag_name, "a");
            fprintf(file, "Message: %s\n", message);
            fclose(file);
        }
        return 0;
    }
}
// #define _DEBUG_
#ifdef _DEBUG_
int main(){
    int argc = 6;
    char* argv[] = {"sem", "tag", "-a", "v1.0a", "-m", "ajab"};
#else
int main(int argc, char* argv[]){
#endif
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
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
    else if(strcmp(argv[1], "reset") == 0){
        return run_reset(argc, argv);
    }
    else if(strcmp(argv[1], "config") == 0){
        return run_config(argc, argv);
    }
    else if(strcmp(argv[1], "set") == 0){
        return run_set_message(argc, argv);
    }
    else if(strcmp(argv[1], "remove") == 0){
        return remove_message(argc, argv);
    }
    else if(strcmp(argv[1], "replace") == 0){
        return replace_message(argc, argv);
    }
    else if(strcmp(argv[1], "commit") == 0){
        return run_commit(argc, argv);
    }
    else if(strcmp(argv[1], "branch") == 0){
        return run_branch(argc, argv);
    }
    else if(strcmp(argv[1], "checkout") == 0){
        return run_checkout(argc, argv);
    }
    else if(strcmp(argv[1], "log") == 0){
        return run_log(argc, argv);
    }
    else if(strcmp(argv[1], "revert") == 0){
        return run_revert(argc, argv);
    }
    else if(strcmp(argv[1], "grep") == 0){
        return run_grep(argc, argv);
    }
    else if(strcmp(argv[1], "tag") == 0){
        return run_tag(argc, argv);
    }
    else{
        alias* local = NULL;
        alias* global = NULL;
        add_command(&local, "local");
        add_command(&global, "--global");
        alias* temp;
        temp = local;
        while(temp != NULL){
            if(strcmp(temp->ghadimy, argv[1]) == 0){
                chdir(cwd);
                system(temp->jadid);
                return 0;
            }
            temp = temp->next;
        }
        temp = global;
        while(temp != NULL){
            if(strcmp(temp->ghadimy, argv[1]) == 0){
                chdir(cwd);
                system(temp->jadid);
                return 0;
            }
            temp = temp->next;
        }
        perror("Enter a valid command!");
        return 1;
    }
    return 0;
}
