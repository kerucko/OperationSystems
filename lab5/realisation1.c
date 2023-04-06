#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char* translation(long number) {
    char* res = malloc(sizeof(char) * 20);
    int size = 0;
    
    while (number > 0) {
        char symbol = number % 2 + '0';
        for(int counter = size; counter > 0; --counter) {
            res[counter] = res[counter - 1];
        }
        res[0] = symbol;
        ++size;
        number /= 2;
    }
    return res;
}


void sort(int* array, int size) {
    for(int i = 0; i < size; ++i) {
        for(int j = i + 1; j < size; ++j) {
            if(array[i] > array[j]) {
                int swap = array[j];
                array[j] = array[i];
                array[i] = swap;
            }
        }
    }
}
