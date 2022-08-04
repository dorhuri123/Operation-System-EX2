//Dor Huri 209409218
#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#define MAX_SIZE 150
#define RUN_COMP "./comp.out"
#define STUDENT_OUTPUT "student_output.txt"
#define ERRORS_FD "errors.txt"
#define RESULTS_CSV "results.csv"
#define NO_C_FILE_GRADE_0 ",0,NO_C_FILE"
#define COMPILATION_ERROR_GRADE_10 ",10,COMPILATION_ERROR"
#define WRONG_GRADE_50 ",50,WRONG"
#define SIMILAR_GRADE_75 ",75,SIMILAR"
#define EXCELLENT_GRADE_100 ",100,EXCELLENT"
#define CASE_NO_C_FILE 1
#define CASE_COMPILATION_ERROR 2
#define CASE_WRONG 4
#define CASE_SIMILAR 5
#define CASE_EXCELLENT 6
#define ONE 1
#define ZERO 0
#define TWO 2
#define THREE 3
#define ERROR -1
#define IDENTICAL 1
#define DIFFERENT 2
#define READ_WRITE_EXTENSION 0666
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define EOF_C_FILE_1 '.'
#define EOF_C_FILE_2 'c'
#define OUT_FILE_EXTENSION "/a.out"
#define COMPILE "gcc"
#define COMPILE_FLAG "-o"
#define SLASH "/"
#define CURRENT_DIRECTORY "."
#define FATHER_DIRECTORY ".."
#define END_LINE "\n"
/*************************************************
 * this function get the path to the correct output
 * so we compare output on student_output.txt to the
 * correct output using comp.out and return the result
 *************************************************/
int compareOutPuts(char* correctOutput){
    int state,pid,errors_fd,res;
    //initializing variable for execvp
    char *compare_arg[]={RUN_COMP ,correctOutput, STUDENT_OUTPUT, NULL};
    //in case of error in fork
    if((pid = fork()) < ZERO){
        perror("Error in: fork");
        exit(ERROR);
    }
    //father process
    if (pid){
        //wait for child to finish
        wait(&state);
        //get the exit status
        res = WEXITSTATUS(state);
        //return case according result from comp.out
        if(res == IDENTICAL){
            return CASE_EXCELLENT;
        }
        else if(res == DIFFERENT){
            return CASE_WRONG;
        } else{
            return CASE_SIMILAR;
        }
    }
    //child process
    else{
        //open/create error.txt in case of error exit
        if((errors_fd = open(ERRORS_FD, O_WRONLY | O_CREAT | O_APPEND,READ_WRITE_EXTENSION)) < 0){
            perror("Error in: open");
            exit(ERROR);
        }
        //we redirect stderr to error
        if(dup2(errors_fd, STDIN) < ZERO){
            perror("Error in: dup2");
            exit(ERROR);
        }
        //closing fd
        if(close(errors_fd) < ZERO ){
            perror("Error in: close");
            exit(ERROR);
        }
        //running comp.out on both files in case of error in execvp exit
        if (execvp(RUN_COMP, compare_arg) < ZERO){
            perror("Error in: execvp");
            exit(ERROR);
        }
    }
}
/*************************************************
 * this function get the file name and check if he
 * is a file of type c.
 *************************************************/
int check_c_file(char *fileName){
    return fileName[strlen(fileName)-TWO] == EOF_C_FILE_1 && fileName[strlen(fileName)-ONE] == EOF_C_FILE_2;
}
/***********************************************************
 * this function get the student directory path
 * when we know that a c file exist then the function
 * run the c file and save error and output in created files
 ***********************************************************/
void run_code(char *filePath){
    int state, output_fd, errors_fd,pid;
    char path[MAX_SIZE];
    char* path_arg[]={path, NULL};
    //setting variable path with full path to out file
    strcat(strcpy(path, filePath),OUT_FILE_EXTENSION);
    //in case of error in fork
    if ((pid = fork()) < ZERO){
        perror("Error in: fork");
        exit(ERROR);
    }
    //father process
    if (pid){
        //wait for child process then finish

        wait(&state);
        return;
    }
    //child process
    else{
        //open/create error.txt and student_output.txt and exit in case of error
        if ((output_fd = open(STUDENT_OUTPUT, O_RDWR | O_CREAT | O_TRUNC, READ_WRITE_EXTENSION)) < ZERO || (errors_fd = open(ERRORS_FD, O_WRONLY | O_CREAT | O_APPEND, READ_WRITE_EXTENSION)) < ZERO){
            perror("Error in: open");
            exit(ERROR);
        }
        //setting input to beginning
        if(lseek(ZERO, ZERO, SEEK_SET) < ZERO){
            perror("Error in: lseek");
            exit(ERROR);
        }
        //we redirect stderr to error and stdout to student_output
        if(dup2(output_fd, STDOUT) < ZERO || dup2(errors_fd, STDERR) < ZERO){
            perror("Error in: dup2");
            exit(ERROR);
        }
        //closing fd
        if(close(output_fd) < ZERO || close(errors_fd) < ZERO){
            perror("Error in: dup2");
            exit(ERROR);
        }
        //running the c file on the child process and exit in case of error
        if(execvp(path, path_arg) < ZERO ){
            perror("Error in: execvp");
            exit(ERROR);
        }
    }

}
/***********************************************************
 * this function get the student directory path and correct
 * output then the function compile c file(if exist) run it
 * and return the grade of the student using the result of
 * comp.out
 ***********************************************************/
int gradeStudent(char *filePath, char* correctOutput){
    int state,errors_fd,pid;
    char path[MAX_SIZE], out_file[MAX_SIZE];
    //initializing variable for execvp
    char *compile_arg[] = {COMPILE,path, COMPILE_FLAG, out_file, NULL };
    DIR *dir;
    struct dirent *openDir;
    struct stat dir_stat;
    //getting directory to dir variable in case of error exit
    if ((dir = opendir(filePath)) == NULL){
        perror("Error in: opendir");
        exit(ERROR);
    }
    //loop over files in directory
    while((openDir = readdir(dir)) != NULL){
        //setting variable path with the full path to file
        strcat(strcat(strcpy(path, filePath),SLASH),openDir->d_name);
        //using stat for details on file in case of error exit
        if(stat(path, &dir_stat) < ZERO){
            perror("Error in: stat");
            exit(ERROR);
        }
        //checking if file is regular and end wit .c
        if(S_ISREG(dir_stat.st_mode) && check_c_file(openDir->d_name)){
            //full path to out file
            strcat(strcpy(out_file, filePath),OUT_FILE_EXTENSION);
            //in case fork failed
            if ((pid = fork()) < ZERO){
                perror("Error in: fork");
                exit(ERROR);
            }
            //father process
            if (pid){
                //wait till child process finish
                wait(&state);
                //if child finish with not normal exit code that mean c file didn't compile
                if(WEXITSTATUS(state)){
                    return CASE_COMPILATION_ERROR;
                }
                //call func for running c file
                run_code(filePath);
                if(closedir(dir) < ZERO){
                    perror("Error in: closedir");
                    exit(ERROR);
                }
                //call func for comparing output and returning result
                return compareOutPuts(correctOutput);
            }
            //child process
            else {
                //open|create error.txt in case of error exit
                if((errors_fd = open(ERRORS_FD, O_RDWR | O_CREAT | O_APPEND, READ_WRITE_EXTENSION)) < ZERO){
                    perror("Error in: open");
                    exit(ERROR);
                }
                //we redirect stderr to error
                if(dup2(errors_fd, STDERR) < ZERO){
                    perror("Error in: dup2");
                    exit(ERROR);
                }
                //closing fd
                if(close(errors_fd) < ZERO){
                    perror("Error in: close");
                    exit(ERROR);
                }
                //compiling c file in child process in case of error exit
                if((execvp("gcc",compile_arg)) < ZERO){
                    perror("Error in: execvp");
                    exit(ERROR);
                }
            }
        }
    }
    if(closedir(dir) < ZERO){
        perror("Error in: closedir");
        exit(ERROR);
    }
    //in case there is no c file in directory
    return CASE_NO_C_FILE;

}
/***********************************************************
 * this function check if the 3 line we get from conf.txt
 * are valid path and that we can access it otherwise the
 * program will exit with an error
 ***********************************************************/
void check_access(char conf[THREE][MAX_SIZE]){
    //check first line if valid directory
    if (access(conf[ZERO], F_OK)){
        perror("Not a valid directory");
        exit(ERROR);
    }
    //check second line if exists Input file
    if (access(conf[ONE], F_OK)){
        perror("Input file not exists");
        exit(ERROR);
    }
    //check third line if exists Output file
    if (access(conf[TWO], F_OK)){
        perror("Output file not exists");
        exit(ERROR);
    }
}
/*************************************************
 * this function get the student name and grade then
 * and write the appropriate details in result.csv
 *************************************************/
void updateGrade(char *student_line_csv, int studentGrade){
    char buffer[MAX_SIZE];
    strcpy(buffer,student_line_csv);
    //append the student line in result.csv according case
    switch (studentGrade)
    {
        case CASE_NO_C_FILE:
            strcat(buffer, NO_C_FILE_GRADE_0);
            break;

        case CASE_COMPILATION_ERROR:
            strcat(buffer, COMPILATION_ERROR_GRADE_10);
            break;

        case CASE_WRONG:
            strcat(buffer, WRONG_GRADE_50);
            break;

        case CASE_SIMILAR:
            strcat(buffer, SIMILAR_GRADE_75);
            break;

        case CASE_EXCELLENT:
            strcat(buffer, EXCELLENT_GRADE_100);
            break;

        default:
            break;
    }
    //appending \n to line
    strcat(buffer,END_LINE);
    //writing to result.csv
    if(write(ONE, buffer, strlen(buffer)) < 0){
        exit(ERROR);
    }
    //deleting student_output.txt
    remove(STUDENT_OUTPUT);
}
int main(int argc, char *argv[]) {
    //in case there is no path argument in cli exit with error
    if(argc!=TWO){
        perror("Not enough parameter");
        exit(ERROR);
    }
    int conf_fd, input_fd, results_csv_fd, i = ZERO, grade;
    char inputLine[THREE][MAX_SIZE], filePath[MAX_SIZE];
    DIR *dir;
    struct dirent *openDir;
    struct stat dir_stat;
    //opening configuration file and creating results.csv in case of error exit
    if((conf_fd = open(argv[ONE], O_RDONLY)) < ZERO || (results_csv_fd=open(RESULTS_CSV, O_RDWR | O_CREAT | O_TRUNC, READ_WRITE_EXTENSION)) < ZERO){
        perror("Error in: open");
        exit(ERROR);
    }
    //we redirect stdout to result.csv and stdin to conf file
    if(dup2(conf_fd, STDIN) < ZERO || dup2(results_csv_fd, STDOUT) < ZERO){
        perror("Error in: dup2");
        exit(ERROR);
    }
    //closing fd in case of error exit
    if(close(conf_fd) < ZERO || close(results_csv_fd) < ZERO){
        perror("Error in: close");
        exit(ERROR);
    }
    //read 3 line of input from conf file
    while (i<THREE){
        //reading line from conf.txt
        scanf("%s", inputLine[i]);
        i++;
    }
    //checking access of input
    check_access(inputLine);
    //dir variable get the dir using open dir in case of error we exit
    if ((dir = opendir(inputLine[ZERO])) == NULL){
        perror("Error in: opendir");
        exit(ERROR);
    }
    //opening input file in case of error we exit
    if((input_fd = open(inputLine[ONE], O_RDONLY)) < ZERO){
        perror("Error in: open");
        exit(ERROR);
    }
    //we redirect stdin to input file
    if(dup2(input_fd, STDIN) < ZERO){
        perror("Error in: dup2");
        exit(ERROR);
    }
    //closing fd
    if(close(input_fd) < ZERO ){
        perror("Error in: close");
        exit(ERROR);
    }
    //iterating over directory files/directories
    while((openDir = readdir(dir)) != NULL){
        //file path get full path to file
        strcat(strcat( strcpy(filePath, inputLine[ZERO]),SLASH),openDir->d_name);
        //using lstat for information on file/dir in case of error we exit
        if((lstat(filePath, &dir_stat)) < ZERO){
            perror("Error in: lstat");
            exit(ERROR);
        }
        /*
         * checking that input from configure file is directory
         * and is not the current directory or father directory
         */
        if(S_ISDIR(dir_stat.st_mode) && strcmp(openDir->d_name, CURRENT_DIRECTORY) && strcmp(openDir->d_name, FATHER_DIRECTORY)){
            //getting student grade
            grade = gradeStudent(filePath, inputLine[TWO]);
            //updating grade in result.csv
            updateGrade(openDir->d_name, grade);
        }
    }
    if(closedir(dir) < ZERO){
        perror("Error in: closedir");
        exit(ERROR);
    }
}