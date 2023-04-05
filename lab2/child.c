#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1

int main(int argc, const char *argv[]) {
    while (1) { 
        int length;
        char line[20] = "";
        if (read(STDIN_FILENO, &length, sizeof(length)) == -1) {
            perror("read child error");
            return 1;
        }

        if (length == 0) {
            return 0;
        }

        if (read(STDIN_FILENO, &line, sizeof(char) * length) == -1) {
            perror("read child error");
            return 1;
        }

        char swap;
        for (int i = 0; i < length / 2; ++i) {
            swap = line[i];
            line[i] = line[length - i - 1];
            line[length - i - 1] = swap;
        }

        printf("%s\n", line);
    }

    return 0;
}
