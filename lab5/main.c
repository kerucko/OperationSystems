#include <stdio.h>
#include "realisation.h"


int main(int argc, const char** argv) {
    int choice;
    while (scanf("%d", &choice) > 0) {
        if (choice == 1) {
            int number;
            scanf("%d", &number);
            printf("translation(%d) = %s\n", number, translation(number));
        } else if (choice == 2) {
            int size;
            printf("size of array = ");
            scanf("%d", &size);
            int array[size];
            printf("print array:\n");
            for(int counter = 0; counter < size; ++counter) {
                scanf("%d", &array[counter]);
            }
            for(int counter = 0; counter < size; ++counter) {
                printf("%d ", array[counter]);
            }

            printf("\n");
            sort(array, size);
            printf("sorted:\n");
            for(int counter = 0; counter < size; ++counter) {
                printf("%d ", array[counter]);
            }
            printf("\n");
        } else {
            printf("End.\n");
            return 0;
        }
    }
}
