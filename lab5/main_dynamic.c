#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


const char* DYNAMIC_LIBRARY_1 = "./librealisation1.so";
const char* DYNAMIC_LIBRARY_2 = "./librealisation2.so";


int main(int argc, const char** argv) {
    int counter = 1;
    void* handle = dlopen(DYNAMIC_LIBRARY_1, RTLD_LAZY);
    if(handle == NULL) {
        perror("dlopen error");
        return 1;
    }

    char* (*translation)(long);
    void* (*sort)(int*, int);
    translation = dlsym(handle, "translation");
    sort = dlsym(handle, "sort");

    char* error = dlerror();
    if(error != NULL) {
        perror("dlsym error");
        return 1;
    }
    int choice, number, size;


    while (scanf("%d", &choice) > 0) {
        if(choice == 0) {
            dlclose(handle);
            if (counter) {
                handle = dlopen(DYNAMIC_LIBRARY_2, RTLD_LAZY);
            } else {
                handle = dlopen(DYNAMIC_LIBRARY_1, RTLD_LAZY);
            }
            if(handle == NULL) {
                perror("dlopen error");
                return 1;
            }
            
            translation = dlsym(handle, "translation");
            sort = dlsym(handle, "sort");
            error = dlerror();
            if(error != NULL) {
                perror("dlsym error");
                return 1;
            }
            counter = counter ^ 1;
            printf("Switch.\n");
        }else if(choice == 1) {
            scanf("%d", &number);
            printf("translation(%d) = %s\n", number, translation(number));
        } else if(choice == 2) {
            printf("size of array = ");
            scanf("%d", &size);
            int* array = malloc(sizeof(int) * size);
            printf("print array:\n");

            for(int i = 0; i < size; ++i) {
                scanf("%d", &array[i]);
            }
            for(int i = 0; i < size; ++i) {
                printf("%d ", array[i]);
            }
            printf("\n");

            sort(array, size);
            printf("sorted:\n");
            for(int i = 0; i < size; ++i) {
                printf("%d ", array[i]);
            }
            printf("\n");
            free(array);
        } else{
            printf("End.\n");
            dlclose(handle);
            return 0;
        }
    }
}
