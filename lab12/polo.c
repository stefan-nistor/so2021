//
// Created by rosawan on 21/05/2021.
//

/// To execute this example use: UNIX> ./marco & ./polo

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    int fdIn, fdOut, iterations = 20;
    char c = '0';

    /// Creating FIFO channels
    /// permissions - 0600 is like file permissions prw-------.
    /// The owner's processes have read and write permission, but no one else.
    /// The p means the type of this "file" is a pipe

    mkfifo("fifo_marco2polo", 0600);
    mkfifo("fifo_polo2marco", 0600);

    fdIn = open("fifo_marco2polo", O_RDONLY);
    fdOut = open("fifo_polo2marco", O_WRONLY);

    while(iterations--){

        read(fdIn, &c, 1);     /// read from FIFO channel

        printf("Polo! ");
        fflush(stdout);

        write(fdOut, &c, 1);     /// write in FIFO channel
    }

    printf("End of Marco Polo\n");
    return 0;
}