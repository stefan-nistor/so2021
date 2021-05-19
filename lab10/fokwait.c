#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv){

    int n = 8;
    pid_t pid;

    for(int i = 0; i < n; i++){
        if (-1 == (pid = fork())){
            perror("Fork error!");
            exit(-1);
        }
        if(pid == 0){
            printf("I am child process %d, with PID: %d, my Parent has PID: %d.\n", i, getpid(), getppid());
            return i;
        }
    }

    for(int i = 0; i < n; i++){
        wait(NULL);
    }
    return 0;
}
