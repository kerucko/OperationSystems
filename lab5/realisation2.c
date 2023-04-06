#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char* translation(long x) {
    char* result = malloc(sizeof(char) * 20);
    int size = 0;
    while (x > 0) {
        char symbol = x % 3 + '0';
        for(int counter = size; counter > 0; --counter) {
            result[counter] = result[counter - 1];
        }
        result[0] = symbol;
        ++size;
        x /= 3;
    }
    return result;
}


int partition(int array[], int l, int r) {
    int v = array[(l + r) / 2];
    int i = l;
    int j = r;
    while(i <= j) {
        while(array[i] < v) {
            ++i;
        }
        while (array[j] > v) {
            --j;
        }
        if(i >= j) {
            break;
        }
        int swap = array[i];
        array[i] = array[j];
        array[j] = swap;
        ++i;
        --j;
    }
    return j;
}


void quicksort(int array[], int l, int r) {
    if(l < r) {
        int q = partition(array, l, r);
        quicksort(array, l, q);
        quicksort(array, q + 1, r);
    }
}


void sort(int array[], int size) {
    quicksort(array, 0, size - 1);
}
