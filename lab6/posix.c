#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define ASCII_BLOCK_SIZE 256

/// 4096 is the exact size of memory page
#define BLOCK_SIZE 4096

unsigned char buffer[BLOCK_SIZE];
int count[ASCII_BLOCK_SIZE];

int main(int argc, char **argv){

    if(argc == 1) {
        perror("Please specify the file to be processed as a parameter\n");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if(-1 == fd){
        perror("Error at opening file\n");
        return -1;
    }

    /// loop of processing input file -- reads BLOCK_SIZE bytes from file each iteration
    while (1){
        int bytes_read = read (fd, buffer, BLOCK_SIZE);

        if(bytes_read == 0) break; ///EOF

        if(bytes_read == -1) {
            perror("Error at reading file\n");
            return -1;
        }

        for(int i = 0; i < bytes_read; i++)
            count[buffer[i]]++;
    }

    close(fd);

    for(int i = 0; i < ASCII_BLOCK_SIZE; i++)
        if(count[i])
            printf("The character %c with ASCII code %d appears %d times in the file.\n", i,i,count[i]);

    return 0;
}

