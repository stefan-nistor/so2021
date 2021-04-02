/*
   Iată o soluție incompletă, în sensul că s-a omis codul efectiv de punere și de scoatere a lacătului pentru acces exclusiv la fișierul de date partajat,
   precum și tratarea anumitor excepții (i.e., atunci când se încearcă extragerea unei cantități mai mari decât cea disponibilă în rezervorul stației PECO).
*/
// TODO: respectivele omisiuni vă rămân ca temă să le completați dvs. !!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct flock lock;

void initializare_fisier_date();
/*
*  Pentru început, avem nevoie de o rutină ajutătoare, care să creeze conținutul inițial al fișierului de date partajat.
*  Se va crea fișierul și va conține un număr real, stocat în format binar, reprezentând cantitatea inițială de combustibil din rezervor.
*  Vom apela programul cu opțiunea -i pentru a invoca această rutină de inițializare.
*/

void afisare_fisier_date();
/*
*  Similar, la finalul tuturor actualizărilor, vom avea nevoie de o altă rutină ajutătoare, care să ne afișeze conținutul final din rezervor.
*  Vom apela programul cu opțiunea -o pentru a invoca această rutină de afișare finală.
*/

void secventa_de_actualizari(int n, char* nr[]);
/*
*  Rutina principală a programului, care va executa secvența de actualizări primită în linia de comandă, în manieră concurentă,
*  conform celor specificate în enunțul problemei.
*  Observație: rutina principală nu este implementată în întregime mai jos, ci vă rămâne dvs. sarcina să completați "bucățile" de cod
*  care lipsesc (indicate prin comentarii TODO).
*/

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        fprintf(stderr, "Eroare: programul trebuie apelat cu optiunile: -i, -o, sau cu o secventa de numere reale...\n");  return 1;
    }

    /* Cazul de inițializare a fișierului de date. */
    if( strcmp(argv[1],"-i") == 0 )
    {
        initializare_fisier_date();  return 0;
    }

    /* Cazul de afișare a fișierului de date. */
    if( strcmp(argv[1],"-o") == 0 )
    {
        afisare_fisier_date();  return 0;
    }

    /* Cazul general: actualizarea fișierului de date. */
    secventa_de_actualizari(argc, argv);
    return 0;
}


void initializare_fisier_date()
{/* Funcționalitate: inițializarea fișierului de date. */

    int fd;
    float val;

    printf("Dati cantitatea initiala de combustibil (i.e., un numar real pozitiv): ");
    while( (1 != scanf("%f", &val)) && (val < 0) ) { fprintf(stderr,"Eroare: nu ati introdus un numar real pozitiv! Incercati din nou..."); }

    if(-1 == (fd = creat("peco.bin", 0600) ) )
    {
        perror("Eroare la crearea fisierului de date...");  exit(2);
    }

    if(-1 == write(fd, &val, sizeof(float) ) )
    {
        perror("Eroare la scrierea valorii initiale in fisierul de date...");  exit(3);
    }

    close(fd);
}


void afisare_fisier_date()
{/* Funcționalitate: afișarea fișierului de date. */

    int fd;
    float val;

    if ( -1 == (fd = open("peco.bin", O_RDONLY) ) )
    {
        perror("Eroare la deschiderea pentru afisare a fisierului de date...");  exit(4);
    }

    if (-1 == read(fd, &val, sizeof(float) ) )
    {
        perror("Eroare la citirea valorii finale din fisierul de date...");  exit(5);
    }

    close(fd);
    printf("Stocul final de combustibil este: %f litri de combustibil.\n", val);
}


void secventa_de_actualizari(int n, char* nr[])
{ /* Funcționalitate: realizarea secvenței de operații de actualizare a fișierului de date. */

    float valoare, stoc;
    int fd,i;

    if ( -1 == (fd = open("peco.bin", O_RDWR) ) )
    {
        perror("Eroare la deschiderea pentru actualizare a fisierului de date...");  exit(6);
    }

    srandom( getpid() ); // Inițializarea generatorului de numere aleatoare.

    for(i = 1; i < n; i++) // Iterăm prin secvența de numere reale specificată în linia de comandă.
    {
        if(1 != sscanf(nr[i], "%f", &valoare))  // Facem conversie de la string la float.
        {
            fprintf(stderr, "[PID: %d] Eroare: ati specificat o valoare invalida drept argument de tip numar real : %s !\n", getpid(), nr[i] );
            exit(7);
        }

        /* Începutul secțiunii critice! */
        /*
            ... // TODO: implementați singuri punerea lacătului în scriere pe fișier.

        */

        lock.l_type     = F_WRLCK;
        lock.l_whence   = SEEK_SET;
        lock.l_start    = -1;
        lock.l_len      = 1;

        if(-1 == lseek(fd, 0, SEEK_SET) ) // (Re)poziționăm cursorul la începutul fișierului.
        {
            perror("Eroare la repozitionarea in fisierul de date, pentru citire...");  exit(8);
        }
        if (-1 == read(fd, &stoc, sizeof(float) ) )
        {
            perror("Eroare la citirea valorii din fisierul de date...");  exit(9);
        }

        printf("[PID: %d] Se adauga/extrage in/din rezervor cantitatea de %f litri de combustibil.\n", getpid(), valoare );

        stoc = stoc + valoare;
        /*
            ... // TODO: implementați singuri tratarea excepției: stoc negativ
            ... // Varianta 1: refuzarea operației de extragere, cu un mesaj informativ în acest sens.
            ... // Varianta 2: realizarea unei extrageri parțiale, i.e. se extrage doar cât e în stoc, nu cât s-a cerut, cu un mesaj informativ în acest sens.
            ... // Varianta 3: interogarea utilizatorului, ce variantă de rezolvare dorește? , i.e. varianta 1 sau varianta 2 de mai sus?
        */

        if( stoc <= 0 ) {
            printf( "Stocul disponibil (%f) nu este suficient pentru a indeplini cerinta!\n" , stoc);
            printf("Doriti anularea comenzii (press 1) sau extragerea partiala? (press 2)?\n");
            int option;
            scanf("%d", option);

            if(option == 2){
                printf("[PID: %d] Se extrage din rezervor cantitatea de %f litrii de combustibil\n", getpid(), stoc);
                stoc = 0;
            }

            if(option == 1){
                printf("[PID: %d] Comanda a fost anulata!\n", getpid());
            }
        }


        if(-1 == lseek(fd, 0, SEEK_SET) ) // Repoziționăm cursorul la începutul fișierului.
        {
            perror("Eroare la repozitionarea in fisierul de date, pentru scriere...");  exit(10);
        }
        if (-1 == write(fd, &stoc, sizeof(float) ) )
        {
            perror("Eroare la scrierea valorii actualizate in fisierul de date..."); exit(11);
        }

        /* Sfârșitul secțiunii critice! */
        /*
            ... // TODO: implementați singuri scoaterea lacătului de pe fișier.
        */

        lock.l_type = F_UNLCK;
        if ( -1 == fcntl(fd, F_SETLK, &lock)){
            perror("Eroare la deblocaj!");
            exit(13);
        }
        lock.l_type = F_WRLCK;

        sleep( random() % 5 ); // Facem o pauză aleatoare, de maxim 4 secunde, înainte de a continua cu următoarea iterație.
    }

    close(fd);
    printf("[PID: %d] Am terminat de procesat secventa de actualizari.", getpid());
}