#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>


void add_start(char string[], char symbol, int size) {
    for(int i = size; i > 0; --i) {
        string[i] = string[i - 1];
    }
    string[0] = symbol;
}


int main(int argument_counter, char** arguments) {
    if (argument_counter < 4) {
        perror("not enough arguments");
        return 1;
    }
    int file_descriptor = shm_open(arguments[1], O_RDWR, S_IRWXU);
    if (file_descriptor == -1) {
        perror("file descriptor in child error");
        return 1;
    }
    struct stat buffer;
    if(fstat(file_descriptor, &buffer) == -1) {
        perror("fstat error");
        return 1;
    }
    char* shared_file = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
    if(shared_file == MAP_FAILED) {
        perror("shared file mmap error");
        return 1;
    }
    int file_descriptor_mutex = shm_open(arguments[2], O_RDWR, S_IRWXU);
    if(file_descriptor_mutex == -1) {
        perror("file descriptor mutex in child error");
        return 1;
    }
    pthread_mutex_t* mutex = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_mutex, 0);
    if(mutex == MAP_FAILED) {
        perror("mutex mmap error");
        return 1;
    }
    int file_descriptor_condition = shm_open(arguments[3], O_RDWR, S_IRWXU);
    if(file_descriptor_condition == -1) {
        perror("file descriptor condition error");
        return 1;
    }
    pthread_cond_t* condition = mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_condition, 0);
    if(condition == MAP_FAILED) {
        perror("condition mmap error");
        return 1;
    }
    char string[100] = "";
    int size = 0;
    while (1) {
        if(pthread_mutex_lock(mutex) != 0) {
            perror("mutex lock in child error");
            return 1;
        }
        while (shared_file[0] == 0) {
            if(pthread_cond_wait(condition, mutex) != 0) {
                perror("condition wait in child error");
                return 1;
            }
        }
        if (shared_file[0] == -1) {
            if(pthread_mutex_unlock(mutex) != 0) {
                perror("mutex unllock in child error");
                return 1;
            }
            break;
        }
        if(shared_file[0] == '\n') {
            printf("%s\n", string);
            size = 0;
            memset(&string, 0, 100);
        } else {
            add_start(string, shared_file[0], size);
            ++size;
        }
        shared_file[0] = 0;
        if(pthread_cond_signal(condition) != 0) {
            perror("condition signal in child error");
            return 1;
        }
        if(pthread_mutex_unlock(mutex) != 0) {
            perror("mutex unlock in child error");
            return 1;
        }
    }
    if(pthread_mutex_destroy(mutex) != 0) {
        perror("mutex destroy in child error");
        return 1;
    }
    if(pthread_cond_destroy(condition) != 0) {
        perror("condition destroy in child error");
        return 1;
    }
    if(munmap(mutex, sizeof(pthread_mutex_t)) == -1) {
        perror("mutex munmap error");
        return 1;
    }
    if(munmap(condition, sizeof(pthread_cond_t)) == -1) {
        perror("condition munmap error");
        return 1;
    }
    if(munmap(shared_file, buffer.st_size) == -1) {
        perror("shared file munmap error");
        return 1;
    }
    return 0;
}
