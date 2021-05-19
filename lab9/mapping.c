//
// Created by rosawan on 18/05/2021.
//
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define handleError(msg) \
    {perror(msg), exit(EXIT_FAILURE);}

int main (int argc, char** argv){
    char *map_addr;
    int fd;
    struct stat sb;
    off_t offset;

    size_t length;
    ssize_t code;

    long pageSize = sysconf(_SC_PAGE_SIZE);
    if(pageSize == -1)
        handleError("Error at sysconf")

    if (argc < 3 || argc > 4)
        handleError("Not enough arguments")


}