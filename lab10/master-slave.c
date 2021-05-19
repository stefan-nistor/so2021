///
///     "Distributed" calculation of sum, using two slave processes and file communication between master and slave
///

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>

void masterInit();
void masterFinish();
void slaveWork(char *fi, char *fo);

int main (int argc, char** argv){

    int pid1, pid2;

    /// Clean the results of previous executions - Mandatory
    remove("f1o"); remove("f2o");

    /// Read the input from keyboard and divide it between f1i and f2i
    masterInit();

    /// Creating first slave
    pid1 = fork();
    if(pid1 == -1)  return -1;      /// check for errors

    /// If we are on first slave branch...
    if(pid1 == 0) {
        slaveWork("f1i", "f1o");
        return 0;       /// End of first slave execution
    }

    /// Creating second slave
    pid2 = fork();
    if(pid2 == -1)  return -2;      /// check for errors

    /// If we are on second slave branch...
    if(pid2 == 0){
        slaveWork("f2i", "f2o");
        return 0;       /// End of second slave execution
    }

    /// If we are not in pid == 0 branch, then we are in "master branch"
    masterFinish();
    return 0;
}

void masterInit(){
    int nr, flag = 1;
    FILE *f1 = fopen("f1i", "wt");
    FILE *f2 = fopen("f2i", "wt");

    printf("Enter the numbers. (0 for exit):\n");

    do{
        scanf("%d", &nr);
        fprintf( (flag == 1 ? f1 : f2), "%d", nr);
        flag = 3 - flag;
    }while(nr != 0);

    fclose(f1);
    fclose(f2);
}

void masterFinish(){
    int sp1, sp2, code = 0;
    FILE * f1 = fopen("f1o", "rt");
    FILE * f2 = fopen("f2o", "rt");

    do{
        code = (fscanf(f1, "%d", &sp1) == 1);
        if(!code)
            sleep(2);
    }while(!code);
    fclose(f1);

    do{
        code = (fscanf(f2, "%d", &sp2) == 1);
        if(!code)
            sleep(2);
    }while(!code);
    fclose(f2);

    printf("[Master PID = %d] Sum of input number is: %d\n", getpid(), sp1 + sp2);
}

void slaveWork(){
    int nr, code, sp = 0;
    FILE *f1 = fopen("fi", "rt");

    do{
        code = fscanf(f1, "%d", &nr);
        if (code == 1)
            sp += nr;
    }while(code != EOF);

    fclose(f1);

    FILE *f2 = fopen("fo", "wt");
    fprintf(f2, "%d", sp);
    fclose(f2);
    printf("[Slave PID = %d] Partial Sum = %d\n", getpid(), sp);
}