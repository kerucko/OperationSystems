#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


#define SIZE 20;
int matrix[20][20], new_matrix[20][20];
int window, frame;


typedef struct thread_arguments{
    int number_threads;
    int current_thread;
}thread_arguments;


void print_matrix() {
    for (int i = frame; i < SIZE - frame; ++i) {
        for (int j = frame; j < SIZE - frame; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


void print_without_frame() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


void fill_matrix() {
    srand(time(NULL));
    for (int i = frame; i < SIZE - frame; ++i) {
        for (int j = frame; j < SIZE - frame; ++j) {
            matrix[i][j] = rand() % 10;
        }
    }
    for (int i = 1; i < SIZE - 1; ++i) {
        for (int j = 0; j < frame; ++j) {
            matrix[i][j] = matrix[i][frame];
            matrix[i][SIZE - j - 1] = matrix[i][SIZE - frame - 1];
        }
    }
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < frame; ++j) {
            matrix[j][i] = matrix[frame][i];
            matrix[SIZE - j -1][i] = matrix[SIZE - frame - 1][i];  
        }
    }
}


void result_to_matrix() {
    for (int i = frame; i < SIZE - frame; ++i) {
        for (int j = frame; j < SIZE - frame; ++j) {
            matrix[i][j] = new_matrix[i][j];
        }
    }
    for (int i = 1; i < SIZE - 1; ++i) {
        for (int j = 0; j < frame; ++j) {
            matrix[i][j] = matrix[i][frame];
            matrix[i][SIZE - j - 1] = matrix[i][SIZE - frame - 1];
        }
    }
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < frame; ++j) {
            matrix[j][i] = matrix[frame][i];
            matrix[SIZE - j -1][i] = matrix[SIZE - frame - 1][i];  
        }
    }
}


void sort(int array[], int length) {
    int swap;
    for (int i = 0; i < length - 1; ++i) {
        for (int j = 0; j < length - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                swap = array[j];
                array[j] = array[j + 1];
                array[j + 1] = swap;
            }
        }
    }
}


int median(int i, int j) {
    int size_sort = window * window - 1;
    int numbers[size_sort];
    int counter = 0;

    for (int l = i - frame; l <= i + frame; ++l) {
        for (int k = j - frame; k <= j + frame; ++k) {
            numbers[counter] = matrix[l][k];
            ++counter;
        }
    }
    sort(numbers, size_sort);
    return numbers[size_sort / 2];
}


void filter_for_string(int number_string) {
    for (int j = frame; j < SIZE - frame; ++j) {
        new_matrix[number_string][j] = median(number_string, j);
    }
}


void* thread_filter(void* arg) {
    thread_arguments data = *((thread_arguments*) arg);
    for (int i = data.current_thread; i < SIZE; i += data.number_threads) {
        filter_for_string(i);
    }
}


int main(int argc, const char *argv[]) {
    int overlays, number_threads;
    number_threads = atoi(argv[1]);
    printf("number of threads = %d\n", number_threads);
    printf("number of overlays = ");
    scanf("%d", &overlays);
    printf("SIZE of window(odd number) = ");
    scanf("%d", &window);
    frame = window / 2;
    fill_matrix();
    printf("matrix:\n");
    print_matrix();
    for (int k = 0; k < overlays; ++k) {
        pthread_t threads[number_threads];
        thread_arguments* data = malloc(sizeof(thread_arguments) * number_threads);
        for (int i = 0; i < number_threads; ++i) {
            data[i].current_thread = i;
            data[i].number_threads = number_threads;
        }
        for (int i = 0; i < number_threads; ++i) {
            if (pthread_create(&threads[i], NULL, &thread_filter, &data[i]) != 0) {
                perror("Failed to create thread");
                return 1;
            }
        }
        for (int i = 0; i < number_threads; ++i) {
            if (pthread_join(threads[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }
        result_to_matrix();
    }
    printf("new matrix:\n");
    print_matrix();
    return 0;
}
