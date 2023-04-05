#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define WRITE 1
#define READ 0


int main(int argc, const char *argv[]) {
    int file_descriptors1[2]; // P -> C1
    int file_descriptors2[2]; // P -> C2
    int pipe1 = pipe(file_descriptors1);
    int pipe2 = pipe(file_descriptors2);
    if (pipe1 == -1 || pipe2 == -1) {
        perror("pipe error\n");
        return 1;
    }
    char filename1[20], filename2[20];
    printf("Enter name of first file: ");
    scanf("%s", filename1);
    printf("Enter name of second file: ");
    scanf("%s", filename2);
    int file1 = open(filename1, O_WRONLY | O_CREAT, 0777);
    int file2 = open(filename2, O_WRONLY | O_CREAT, 0777);
    if (file1 == -1 || file2 == -1) {
        perror("files open error");
        return 2;
    }
    switch (fork()) {
    case -1:
        perror("fork error\n");
        return 1;
        break;
    case 0:
        // child 1
        close(file_descriptors1[WRITE]);
        close(file_descriptors2[WRITE]);
        close(file_descriptors2[READ]);
        if (dup2(file_descriptors1[READ], STDIN_FILENO) == -1) {
            perror("dup error");
            return 1;
        }
        if (dup2(file1, STDOUT_FILENO) == -1) {
            perror("dup error");
            return 1;
        }
        char *Child1_argv[] = {"child", NULL};
        if (execv("child", Child1_argv) == -1) {
            perror("exec child1 error");
            return 1;
        }
        break;
    default:
        switch (fork()) {
        case -1:
            perror("fork error");
            return 1;
            break;
        case 0:
            // child 2
            close(file_descriptors2[WRITE]);
            close(file_descriptors1[WRITE]);
            close(file_descriptors1[READ]);
            if (dup2(file_descriptors2[READ], STDIN_FILENO) == -1) {
                perror("dup error");
                return 1;
            }
            if (dup2(file2, STDOUT_FILENO) == -1) {
                perror("dup error");
                return 1;
            }
            char *Child2_argv[] = {"child", NULL};
            if (execv("child", Child2_argv) == -1) {
                perror("exec child2 error");
                return 1;
            }
            break;
        default:
            close(file_descriptors1[READ]);
            close(file_descriptors2[READ]);
            printf("Enter strings:\n");
            int k = 0;
            while(1) {
                char line[50];
                if(gets(line) == NULL) {
                    perror("gets error");
                    return 1;
                }
                int length = strlen(line);
                k++;
                if (length == 0 && k > 1) {
                    if(write(file_descriptors1[WRITE], &length, sizeof(length)) == -1) {
                        perror("write to child1 error");
                        return 1;
                    }
                    if (write(file_descriptors2[WRITE], &length, sizeof(length)) == -1) {
                        perror("write to child2 error");
                        return 1;
                    }
                    close(file_descriptors1[WRITE]);
                    close(file_descriptors2[WRITE]);
                    return 0;
                }
                if (k > 1) {
                    if (k % 2) {
                        // pipe1
                        if (write(file_descriptors1[WRITE], &length, sizeof(length)) == -1) {
                            perror("write length to child1 error");
                            return 1;
                        }
                        if(write(file_descriptors1[WRITE], &line, sizeof(char) * length) == -1) {
                            perror("write line to child1 error");
                            return 1;
                        }
                    } else {
                        // pipe2
                        if(write(file_descriptors2[WRITE], &length, sizeof(length)) == -1) {
                            perror("write length to child2 error");
                            return 1;
                        }
                        if(write(file_descriptors2[WRITE], &line, sizeof(char) * length) == -1) {
                            perror("write line to child2 error");
                            return 1;
                        }
                    }
                }
            }
            break;
        }
        break;
    }
    return 0;
}
