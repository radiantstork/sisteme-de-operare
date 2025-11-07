#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Computes the sum of the first 10 numbers using two threads
// main thread sums the first half, created thread sums the second half
// Results will be stored at the respective last elements (arr[half - 1] and arr[arr_size - 1])

// Struct needed for function sum
struct Vector{
    int* arr;
    int size;
};

// args: Pass Vector*
// ret: NULL, the result is stored in the last element 
void* sum(void* args){
    struct Vector* numbers = args;
    
    int res = 0;
    
    // printf("Thread %ld summing numbers: ", pthread_self());
    for (int i = 0; i < numbers->size - 1; ++i){
        numbers->arr[i + 1] += numbers->arr[i];
        // printf("%d ", numbers->arr[i]);
    }
    
    printf("\nThread %ld sum: %d\n", pthread_self(), numbers->arr[numbers->size - 1]);
    return NULL;
}

int main() {
    const int total_elems = 10;

    if (total_elems <= 1){
        printf("Number of elements %d gives result: %d", total_elems, 1);
        return 0;
    }

    // Allocate memory on the heap for the array
    // We used the heap because we want the memory to be shared
    int* numbers = malloc(sizeof(int) * total_elems);
    if (numbers == NULL){
        perror("malloc");
        return -1;
    }

    // Fill array with numbers (starting after index 1)
    printf("Thread %ld: Writing %d numbers to array:\n", pthread_self(), total_elems);
    for (int i = 0; i < total_elems; i++) {
        numbers[i] = i + 1;
        printf("%d ", numbers[i]);
    }
    printf("\n");

    int half = total_elems / 2;
    
    // New thread
    pthread_t tid;
    struct Vector sum_args_t1 = {.arr = numbers, .size = half};
    struct Vector sum_args_t2 = {.arr = numbers + half, .size = total_elems - half - total_elems % 2};

    // Create new thread for the last half
    int err = pthread_create(&tid, NULL, sum, &sum_args_t2);
    if (err){
        printf("ERROR: pthread_create: %d\n", err);
        free(numbers);
        return -1;
    }
    // Question: How would adding sleep change the ouput?
    // sleep(1);
    
    // Compute first half of the sum
    sum(&sum_args_t1);

    // Join with created thread
    err = pthread_join(tid, NULL);
    if (err){
        printf("ERROR: pthread_join: %d\n", err);
        free(numbers);
        return -1;
    }

    printf("Full sum: %d\n", numbers[half - 1] + numbers[total_elems - 1]);

    free(numbers);
    return 0;
}