#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>


const unsigned int SHARED_MEMORY_SIZE = 1;
const int CHILD_COUNT = 2;
const char* SHARED_FILE_NAME[] = {"1_shared_file", "2_shared_file"};
const char* SHARED_MUTEX_NAME[] = {"1_shared_mutex","2_shared_mutex"};
const char* SHARED_COND_NAME[] = {"1_shared_cond", "2_shared_cond"};


int write_to_process(char* shared_file, pthread_mutex_t* mutex, pthread_cond_t* condition,  const char symbol) {
    if(pthread_mutex_lock(mutex) != 0) {
        perror("mutex lock error");
        return 1;
    }
    while (shared_file[0] != 0) {
        if(pthread_cond_wait(condition, mutex) != 0) {
            perror("condition wait error");
            return 1;
        }
    }
    shared_file[0] = symbol;
    if(pthread_cond_signal(condition) != 0) {
        perror("condition signal error");
        return 1;
    }
    if(pthread_mutex_unlock(mutex) != 0) {
        perror("mutex unlock error");
        return 1;
    }
    return 0;
}
int main() {
    char file[256];
    int output[CHILD_COUNT];
    for (int current_child = 0; current_child < CHILD_COUNT; ++current_child) {
        if (gets(file) == NULL) {
            perror("error reading file name");
            return 1;
        }
        output[current_child] = open(file, O_CREAT | O_RDWR, 0777);
        if(output[current_child] == -1) {
            perror("open file error");
            return 1;
        }
    }

    int file_descriptor[CHILD_COUNT];
    int file_descriptor_mutex[CHILD_COUNT];
    int file_descriptor_condition[CHILD_COUNT];
    pthread_mutex_t* mutex[CHILD_COUNT];
    pthread_cond_t* condition[CHILD_COUNT];
    pthread_mutexattr_t mutex_attribute;
    if(pthread_mutexattr_init(&mutex_attribute) != 0) {
        perror("initializing mutex attribute error");
        return 1;
    }
    if(pthread_mutexattr_setpshared(&mutex_attribute, PTHREAD_PROCESS_SHARED) != 0) {
        perror("sharing mutex attribute error");
        return 1;
    }
    pthread_condattr_t condition_attribute;
    if(pthread_condattr_init(&condition_attribute) != 0) {
        perror("initializing condition attribute error");
        return 1;
    }
    if(pthread_condattr_setpshared(&condition_attribute, PTHREAD_PROCESS_SHARED) != 0) {
        perror("sharing condition attribute error");
        return 1;
    }
    for (int current_child = 0; current_child < CHILD_COUNT; ++current_child) {
        file_descriptor[current_child] = shm_open(SHARED_FILE_NAME[current_child], O_RDWR | O_CREAT, S_IRWXU);
        if(file_descriptor[current_child] == -1) {
            perror("file descriptor error");
            return 1;
        }
        if(ftruncate(file_descriptor[current_child], SHARED_MEMORY_SIZE) != 0) {
            perror("ftruncate file descriptor error");
            return 1;
        }
        file_descriptor_mutex[current_child] = shm_open(SHARED_MUTEX_NAME[current_child], O_RDWR | O_CREAT, S_IRWXU);
        if(file_descriptor_mutex[current_child] == -1) {
            perror("file descriptor mutex error");
            return 1;
        }
        if(ftruncate(file_descriptor_mutex[current_child], sizeof(pthread_mutex_t)) != 0) {
            perror("ftruncate file descriptor mutex error");
            return 1;
        }
        mutex[current_child] = (pthread_mutex_t*) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_mutex[current_child], 0);
        if(mutex[current_child] == MAP_FAILED) {
            perror("mapping shared mutex error");
            return 1;
        }
        if(pthread_mutex_init(mutex[current_child], &mutex_attribute) != 0) {
            perror("initialiszing mutex error");
            return 1;
        }
        file_descriptor_condition[current_child] = shm_open(SHARED_COND_NAME[current_child], O_RDWR | O_CREAT, S_IRWXU);
        if(file_descriptor_condition[current_child] == -1) {
            perror("file descriptor condition error");
            return 1;
        }
        if(ftruncate(file_descriptor_condition[current_child], sizeof(pthread_cond_t)) != 0) {
            perror("ftruncate file descriptor condition error");
            return 1;
        }
        condition[current_child] = (pthread_cond_t*) mmap(NULL, sizeof(pthread_cond_t), PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_condition[current_child], 0);
        if(pthread_cond_init(condition[current_child], &condition_attribute) != 0) {
            perror("condition initializing error");
            return 1;
        }
    }
    if(pthread_mutexattr_destroy(&mutex_attribute) != 0) {
        perror("destroy mutex attribute error");
        return 1;
    }
    if(pthread_condattr_destroy(&condition_attribute) != 0) {
        perror("destroy condition attribute error");
        return 1;
    }
    int id1 = fork();
    if(id1 == -1) {
        perror("1 fork error");
        return 1;
    }
    if (id1 == 0) {
        if(dup2(output[0], STDOUT_FILENO) == -1) {
            perror("dup2 error");
            return 1;
        }
        char *Child1_argv[] = {"child_1", (char*) SHARED_FILE_NAME[0], (char*) SHARED_MUTEX_NAME[0], (char*) SHARED_COND_NAME[0], NULL};
        if(execv("child", Child1_argv) == -1) {
            perror("execv error");
            return 1;
        }
    } else {
        int id2 = fork();
        if(id2 == -1) {
            perror("2 fork error");
            return 1;
        }
        if (id2 == 0) {
            if(dup2(output[1], STDOUT_FILENO) == -1) {
                perror("dup2 error");
                return 1;
            }
            char *Child2_argv[] = {"child_2", (char*) SHARED_FILE_NAME[1], (char*) SHARED_MUTEX_NAME[1], (char*) SHARED_COND_NAME[1], NULL};
            if(execv("child", Child2_argv) == -1) {
                perror("execv error");
                return 1;
            }
        } else {
            char* shared_file[CHILD_COUNT];
            for (int current_child = 0; current_child < CHILD_COUNT; ++current_child) {
                shared_file[current_child] = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor[current_child], 0);
                if(shared_file[current_child] == MAP_FAILED) {
                    perror("creating shared file error");
                    return 1;
                }
                shared_file[current_child][0] = 0;
            }
            char symbol;
            int current_child = 0;
            while (scanf("%c", &symbol) > 0) {
                if (current_child != 0 && current_child != 1) {
                    perror("index error");
                    return 1;
                }
                if(write_to_process(shared_file[current_child], mutex[current_child], condition[current_child], symbol) != 0) {
                    perror("write to process error");
                    return 1;
                }
                if (symbol == '\n') {
                    current_child ^= 1;
                }
            }
            for (int current_child = 0; current_child < CHILD_COUNT; ++current_child) {
                if(write_to_process(shared_file[current_child], mutex[current_child], condition[current_child], -1) != 0) {
                    perror("write to process error");
                    return 1;
                }
                if(munmap(shared_file[current_child], SHARED_MEMORY_SIZE) == -1) {
                    perror("munmap shared file error");
                    return 1;
                }
            }
        }
    }
    for (int current_child = 0; current_child < CHILD_COUNT; ++current_child) {
        if(munmap(mutex[current_child], sizeof(pthread_mutex_t)) == -1) {
            perror("mumap mutex error");
            return 1;
        }
        if(munmap(condition[current_child], sizeof(pthread_cond_t)) == -1) {
            perror("munmap condition error");
            return 1;
        }

        if(shm_unlink(SHARED_FILE_NAME[current_child]) == -1) {
            perror("unlink shared file error");
            return 1;
        }
        if(shm_unlink(SHARED_MUTEX_NAME[current_child]) == -1) {
            perror("unlink shared mutex error");
            return 1;
        }
        if(shm_unlink(SHARED_COND_NAME[current_child]) == -1) {
            perror("unlink shared condition error");
            return 1;
        }

        if(close(output[current_child]) != 0) {
            perror("close file error");
            return 1;
        }
    }
    return 0;
}
