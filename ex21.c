//Dor Huri 209409218
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#define MAX_SIZE 150
#define MAGIC_NUMBER 32
#define ASCII_A 65
#define ASCII_Z 90
#define ZERO 0
#define ONE 1
#define ERROR -1
#define FIRST 1
#define SECOND 2
#define WHITESPACE ' '
#define BREAK_LINE '\n'
#define IDENTICAL 1
#define DIFFERENT 2
#define SIMILAR 3
/*********************************************
 * this function check if character is an upper
 * case char and return the char in lower case
 * otherwise function return the same char.
 ********************************************/
char toLower(char buffer){
    if(buffer >= ASCII_A && buffer <= ASCII_Z){
        return buffer + MAGIC_NUMBER;
    }
    return buffer;
}
int main(int argc,char *argv[]) {
    //initializing flag and 2 fd
    int fd1,fd2,flag=ONE;
    //try to open 2 files from path we get in CLI argument return error if unsuccessful
    if((fd1=open(argv[FIRST],O_RDONLY)) < ZERO || (fd2=open(argv[SECOND],O_RDONLY)) < ZERO) {
        perror("Error in: open\n");
        exit(ERROR);
    }
    //initializing buffers
    char buf1 [MAX_SIZE] , buf2[MAX_SIZE] , buf1_1[ONE],buf2_1[ONE];
    //variable for reading files
    int count1=ZERO,count2=ZERO;
    do {
        //try to read max size from files return error if you unsuccessful
        if((count1=read(fd1,buf1,MAX_SIZE)) < ZERO || (count2=read(fd2,buf2,MAX_SIZE)) < ZERO){
            perror("Error in: read\n");
            exit(ERROR);
        }
        //check if the files are identical
        if(strcmp(buf1,buf2)){
            flag = SIMILAR;
            break;
        }

    } while(count1 > ZERO && count2 > ZERO);
    //close files and if unsuccessful return error
    if(close(fd1) < ZERO || close(fd2) < ZERO){
        perror("Error in: close\n");
        exit(ERROR);
    }
    if(flag == SIMILAR) {
        //try to open 2 files you get in cli argument return error if you unsuccessful
        if((fd1=open(argv[FIRST],O_RDONLY)) < ZERO || (fd2=open(argv[SECOND],O_RDONLY)) < ZERO) {
            perror("Error in: open\n");
            exit(ERROR);
        }
        do {
            //try to read single char from files return error if you unsuccessful
            if ((count1 = read(fd1, buf1_1, ONE)) < ZERO || (count2 = read(fd2, buf2_1, ONE)) < ZERO) {
                perror("Error in: read\n");
                exit(ERROR);
            }
            //check if char is whitespace or \n if so go to next char
            while(buf1_1[ZERO] == WHITESPACE || buf1_1[ZERO] == BREAK_LINE){
                //if you reached EOF end loop
                if(count1 == ZERO){
                    break;
                }
                //read next character return error if you unsuccessful
                if((count1 = read(fd1, buf1_1, ONE)) < ZERO){
                    perror("Error in: read\n");
                    exit(ERROR);
                }
            }
            //check if char is whitespace or \n if so go to next char
            while(buf2_1[ZERO] == WHITESPACE || buf2_1[ZERO] == BREAK_LINE){
                //if you reached EOF end loop
                if(count2 == ZERO){
                    break;
                }
                //read next character return error if you unsuccessful
                if((count2 = read(fd2, buf2_1, ONE) )< ZERO){
                    perror("Error in: read\n");
                    exit(ERROR);
                }
            }
            //if characters are in upper case we make them lower case otherwise they remain the same
            buf1_1[ZERO] = toLower( buf1_1[ZERO]);
            buf2_1[ZERO] = toLower(buf2_1[ZERO]);
            /*
              checking if files are in EOF because in this case they will remain with the
              last character they read, so we make him an empty string
             */
            if(count1 == ZERO){
                strcpy(buf1_1, "");
            }
            if(count2 == ZERO){
                strcpy(buf2_1,"");
            }
            //checking if character differ
            if(buf1_1[ZERO] != buf2_1[ZERO]){
                //if so update flag to indicate that file are different
                flag = DIFFERENT;
                break;
            }
        } while(count1 > ZERO && count2 > ZERO);
        //close files and if unsuccessful return error
        if(close(fd1) < ZERO || close(fd2) < ZERO){
            perror("Error in: close\n");
            exit(ERROR);
        }
    }
    //flag store the result of the files(identical,different,similar)
    switch (flag) {
        case IDENTICAL:
            return IDENTICAL;
        case DIFFERENT:
            return DIFFERENT;
        case SIMILAR:
            return SIMILAR;
    }
}
