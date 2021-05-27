//
// Created by rosawan on 28/05/2021.
//
#include <sys/wait.h>
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"

int slaveWork  (int pipe[]);
int masterWork (int pipe[]);


int main(int argc, char* argv[]){

    int fd[2];
    /*
     *  file descriptors are inherited so each process has two pipe ends (fd[0], fd[1])
     *  fd[0] - read
     *  fd[1] - write
     */

    if(pipe(fd) == -1){
        printf("An error occurred with opening the pipe\n");
        return 1;
    }

    int pid = fork();   /// creating the child process
    if(pid == -1){
        printf("An error occurred with fork\n");
        return 4;
    }

    if(pid == 0){
        slaveWork(fd);
    }
    else{
        masterWork(fd);
        wait(NULL);
    }
    return 0;
}


int slaveWork(int pipe[]){
    close(pipe[0]);   /// close the read end because we do not read form the other end of the pipe (parent proc)

    int x;
    printf("Input a number: ");
    scanf("%d", &x);
    if(write(pipe[1], &x, sizeof(int)) == -1){   /// write the input into write end of the pipe (pipe[1])
        printf("An error occurred with writing to the pipe\n");
        return 2;
    }
    close(pipe[1]);                       /// close the write end after writing into pipe
}

int masterWork(int pipe[]){
    close(pipe[1]);   /// close write end because we do not write in the other end of the pipe (child proc)
    int x;
    if( read(pipe[0], &x, sizeof(int)) == -1){
        printf("An error occurred with reading from the pipe\n");
        return 3;
    }
    x = x * 3;
    printf("Got from child process via pipe: %d\n", x);
    close(pipe[0]);   /// close read end after reading from pipe
}