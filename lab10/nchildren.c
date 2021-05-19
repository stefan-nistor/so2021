#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char ** argv){

    int n = 10;
    pid_t pid;

    /*  Getting number of children
     *  if(argc < 2){
     *      printf("Enter number of children: ");
     *      while(1 != scanf("%d", &n) && N < 1){
     *          fprintf(stderr, "\n Error: Invalid number of children. Please try again...");
     *      }
     */

    for(int i = 0; i < n; i++){
        pid = fork();
        ///sleep(1);
        if(pid == 0){
            printf("I am child process %d, with PID: %d, my Parent has PID: %d.\n", i, getpid(), getppid());
            return i;
        }
     ///   wait(NULL);       you can wait here or in other for-loop but could affect program performance
    }

    for(int i = 0; i < n; i++)
        wait(NULL);

    /// If we use two for-loops, the n processes will be executed in parallel. Otherwise sequentially.

    return 0;
}
