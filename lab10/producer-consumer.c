/*
  Functionality: illustrates the cooperation between two processes, parent and child, using common memory.

  The parent implements a producer, who will "produce" 20 texts, each of length 10, at random intervals, which he will "communicate"
  to his partner, the consumer, by writing in an anonymous memory mapping.

  And the child implements a consumer, who will "consume" 20 texts, each of length 10, at random intervals, which he will "receive"
  from his partner, the producer, by reading them from an anonymous memory mapping in which the producer wrote them.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

#define NR_TEXTE 20
#define DIM_TEXTE 10

void producerWork();
void consumerWork();

char *map_addr;

int main (int argc, char ** argv){

    pid_t pid_fiu;
    size_t length = NR_TEXTE * DIM_TEXTE;

    long pageSize = sysconf(_SC_PAGE_SIZE);

    long mmap_size_in_pages = (length % pageSize ? 1 : 0) + (length / pageSize);

    map_addr = mmap (
            NULL,
            length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
            );

    /// Step 2: Create a child process
    pid_fiu = fork();

    if (pid_fiu == 0){
        fprintf(stderr, "[Consumer] Notification: This is the child process, with PID = %d\n", getpid());
        consumerWork();
    }
    else{
        fprintf(stderr, "[Producer] Notification: This is the parent process, with PID = %d\n", getpid());
        producerWork();
    }

    /// Delete the anonymous map
    munmap(map_addr, length);

    return EXIT_SUCCESS;
}

void producerWork(){
    char text[NR_TEXTE][DIM_TEXTE];

    srandom( getpid() ); /// Initializing random number generator

    /// Initializing texts
    for(int i = 0; i < NR_TEXTE; i++){
        snprintf(text[i], DIM_TEXTE, "Text %2d", i);
    }

    /// Write texts in anonymous map form memory
    for(int i = 0; i < NR_TEXTE; i++){
        strcpy(map_addr + i * DIM_TEXTE, text[i]);
        fprintf(stderr, "[Producer] Notification: I wrote the %dth text, \"%s\", in the mapping.\n", i, text[i]);

        sleep(random() % 3); /// random pause for max 2 sec before next iteration
    }

    fprintf(stderr, "[Producer] Notification: I finished my job!\n");
}

void consumerWork(){
    char text[NR_TEXTE][DIM_TEXTE];

    srandom( getpid() ); /// Initializing random number generator

    for (int i = 0; i < NR_TEXTE; i++){
        /// Idea of synchronization: wait until producer writes ith text
        /// Wait until only penultimate address is non-zero

        /// First way of sync: busy waiting
        do {

        } while (*(map_addr + (i+1)*DIM_TEXTE - 2) == 0); /// I am "spinning" on the CPU, testing repeatedly for zero

        /// Second way of sync: blocking waiting
        do{
            sleep(1);
        } while(*(map_addr + (i+1)*DIM_TEXTE - 2) == 0); /// I will sleep for 1 second in the waiting state,
                                                        /// then I will become ready, then running and test again for zero

        /// Third way of sync: the minimal non-spinning waiting
        do{
            sleep(0);
        }while(*(map_addr + (i+1)*DIM_TEXTE - 2) == 0); /// I am going to the ready state (i.e., I preempt myself, but without an effective sleep),
                                                      /// then running and test again for zero

      strcpy(text[i], map_addr + i * DIM_TEXTE);
      fprintf(stderr, "[Consumer] Notification I read the %dth text, \"%s\", from the mapping.\n", i, text[i]);

      sleep(random() % 3); /// random pause for max 2 sec before next iteration
    }
    fprintf(stderr, "[Consumer] Notification: I finished my job!\n");
}