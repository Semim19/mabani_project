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

// struct
typedef struct alias{
    char ghadimy[51];
    char jadid[101];
    struct alias *next;
} alias;
//struct end

//functions
void add_to_list(alias **head, char line[]);
void add_command(alias **head, char mode[]);
void add_message(alias **head);
int run_init(int argc, char* const argv[]);
int run_add(int argc, char* const argv[]);
int run_reset(int argc, char* const argv[]);
int run_config(int argc, char* const argv[]);
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
// #define _DEBUG_
#ifdef _DEBUG_
int main(){
    int argc = 3;
    char* argv[] = {"sem", "reset", "-undo"};
#else
int main(int argc, char* argv[]){
#endif
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    alias* message = NULL;
    // add_message(&message);
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
