//
// Created by rosawan on 28/05/2021.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

int slaveWork(int[], int[]);
int masterWork(int[], int[]);

int main (int argc, char * argv[]){

    /// pipe[0] - read
    /// pipe[1] - write

    int p1[2]; /// Child  -> Parent
    int p2[2]; /// Parent -> Child


    if(pipe(p1) == -1){
        printf("An error occurred with opening the pipe\n");
        return 1;
    }

    if(pipe(p2) == -1){
        printf("An error occurred with opening the pipe\n");
        return 1;
    }

    int pid = fork();
    if(pid == -1){
        printf("An error occurred with fork\n");
        return 2;
    }

    if(pid == 0){
        slaveWork(p1, p2);
    }
    else{
        masterWork(p1, p2);
        wait(NULL);
    }

    return 0;
}

int slaveWork(int pipe1[], int pipe2[]){
    close(pipe1[0]);
    close(pipe2[1]);
    int x;
    if(read(pipe2[0], &x, sizeof(int)) == -1){
        printf("An error occurred with reading from pipe\n");
        return 3;
    }

    printf("Received %d\n", x);
    x *= 4;

    if(write(pipe1[1], &x, sizeof(int)) == -1){
        printf("An error occurred with writing to the pipe\n");
        return 4;
    }
    printf("Wrote %d\n", x);

    close(pipe1[1]);
    close(pipe2[0]);
}

int masterWork(int pipe1[], int pipe2[]){

    close(pipe1[1]);
    close(pipe2[0]);

    srand(time(NULL));
    int x = rand() % 10;

    if(write(pipe2[1], &x, sizeof(int)) == -1){
        printf("An error occurred with writing to pipe\n");
        return 4;
    }

    if(read(pipe1[0], &x, sizeof(int)) == -1){
        printf("An error occurred with reading from pipe\n");
        return 3;
    }
    printf("Result is %d", x);

    close(pipe1[0]);
    close(pipe2[1]);
    wait(NULL);

}