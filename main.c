#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define BUFFER_SIZE 1000
#define READ_END 0
#define WRITE_END 1

char textwritebuf[BUFFER_SIZE];
char textreadbuf[BUFFER_SIZE];
int key = 7;
char cryptedMSG1[BUFFER_SIZE];
char cryptedMSG2[BUFFER_SIZE];
char cryptedMSG3[BUFFER_SIZE];
char cryptedMSG4[BUFFER_SIZE];
char decryptedMSG[BUFFER_SIZE];
char sifreli[BUFFER_SIZE];
int textkalani;
int textbolumu;
pthread_t tid[4];

void *cesarSifrele(void *arg) {
    if (pthread_equal(pthread_self(), tid[0])) {
        printf("Thread <0> executing.\n");
        char temp1;
        int b = 0;
        for (int i = 0; i < textbolumu; i++) {

            if (textreadbuf[i] == 32) {
                cryptedMSG1[b] = ' ';
            } else if (textreadbuf[i] >= 97 && textreadbuf[i] <= 122) {
                temp1 = (char) (((int) textreadbuf[i] - 97 + key) % 26 + 97);
                cryptedMSG1[b] = temp1;
            } else {
                temp1 = (char) (((int) textreadbuf[i] - 65 + key) % 26 + 65);
                cryptedMSG1[b] = temp1;
            }
            b++;
        }
    } else if (pthread_equal(pthread_self(), tid[1])) {
        printf("Thread <1> executing.\n");
        char temp2;
        int g = 0;
        for (int i = textbolumu; i < textbolumu * 2; i++) {

            if (textreadbuf[i] == 32) {
                cryptedMSG2[g] = ' ';
            } else if (textreadbuf[i] >= 97 && textreadbuf[i] <= 122) {
                temp2 = (char) (((int) textreadbuf[i] - 97 + key) % 26 + 97);
                cryptedMSG2[g] = temp2;
            } else {
                temp2 = (char) (((int) textreadbuf[i] - 65 + key) % 26 + 65);
                cryptedMSG2[g] = temp2;
            }
            g++;
        }
    } else if (pthread_equal(pthread_self(), tid[2])) {
        printf("Thread <2> executing.\n");
        char temp3;
        int k = 0;
        for (int i = textbolumu * 2; i < textbolumu * 3; i++) {

            if (textreadbuf[i] == 32) {
                cryptedMSG3[k] = ' ';
            } else if (textreadbuf[i] >= 97 && textreadbuf[i] <= 122) {
                temp3 = (char) (((int) textreadbuf[i] - 97 + key) % 26 + 97);
                cryptedMSG3[k] = temp3;
            } else {
                temp3 = (char) (((int) textreadbuf[i] - 65 + key) % 26 + 65);
                cryptedMSG3[k] = temp3;
            }
            k++;
        }
    } else if (pthread_equal(pthread_self(), tid[3])) {
        printf("Thread <3> executing.\n");
        char temp4;
        int j = 0;
        for (int i = textbolumu * 3; i < (textbolumu * 4) + textkalani; i++) {
            if (textreadbuf[i] == 32) {
                cryptedMSG4[j] = ' ';
            } else if (textreadbuf[i] >= 97 && textreadbuf[i] <= 122) {
                temp4 = (char) (((int) textreadbuf[i] - 97 + key) % 26 + 97);
                cryptedMSG4[j] = temp4;
            } else {
                temp4 = (char) (((int) textreadbuf[i] - 65 + key) % 26 + 65);
                cryptedMSG4[j] = temp4;
            }
            j++;
        }
    } else {
        printf("\nOOPS! THERE IS A PROBLEM HERE!!!\n");
    }
    return NULL;
}

void cesarSifreCozme() {
    char temp;
    for (int i = 0; i < strlen(textwritebuf); i++) {
        if (sifreli[i] == 32) {
            decryptedMSG[i] = ' ';
        } else if (sifreli[i] >= 97 && sifreli[i] <= 122) {
            temp = (char) ((((int) sifreli[i] - 97 + 26 - key) % 26) + 97);
            decryptedMSG[i] = temp;
        } else {
            temp = (char) ((((int) sifreli[i] - 65 + 26 - key) % 26) + 65);
            decryptedMSG[i] = temp;
        }
    }
    printf("\nDecrypted Message: %s\n", decryptedMSG);
}

int main() {
    while (1){
        printf("Type Something between 150-1000 characters: ");
        gets(textwritebuf);
        if(strlen(textwritebuf)<150 || strlen(textwritebuf)>1000){
            printf("* * Text must be min 150 & max 1000 characters! * *\n");
        }
        else{
            break;
        }
    }

    textkalani = strlen(textwritebuf) % 4;
    textbolumu = (strlen(textwritebuf) - textkalani) / 4;
    pid_t pid;
    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe Failed\n");
    }
    const int SIZE = 4096;
    const char *NAME = "CesarSHM";
    int shm_fd;
    void *ptr;
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid > 0) {
        close(fd[READ_END]);
        write(fd[WRITE_END], textwritebuf, strlen(textwritebuf) + 1);
        printf("\nParent process wrote the message to pipe.\n");
        close(fd[WRITE_END]);
        wait(NULL);
        shm_fd = shm_open(NAME, O_RDONLY, 0666);
        ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        printf("Parent process read the message from Shared Memory.\n");
        sifreli[200];
        strcat(sifreli, ptr);
        cesarSifreCozme();
        shm_unlink(NAME);
        printf("\nParent process executed.\n");
    } else {
        close(fd[WRITE_END]);
        read(fd[READ_END], textreadbuf, BUFFER_SIZE);
        printf("Child process read the message from pipe.\n\n");
        close(fd[READ_END]);
        pthread_create(&(tid[0]), NULL, &cesarSifrele, NULL);
        pthread_create(&(tid[1]), NULL, &cesarSifrele, NULL);
        pthread_create(&(tid[2]), NULL, &cesarSifrele, NULL);
        pthread_create(&(tid[3]), NULL, &cesarSifrele, NULL);
        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
        pthread_join(tid[2], NULL);
        pthread_join(tid[3], NULL);
        strcat(cryptedMSG1, cryptedMSG2);
        strcat(cryptedMSG1, cryptedMSG3);
        strcat(cryptedMSG1, cryptedMSG4);
        printf("\nHere is the crypted message: %s\n", cryptedMSG1);
        shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
        ftruncate(shm_fd, SIZE);
        ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
        sprintf(ptr, "%s", cryptedMSG1);
        ptr += strlen(cryptedMSG1);
        printf("\nChild process wrote the message to Shared Memory.\n");
        printf("Child process executed.\n");
    }
    return 0;
}