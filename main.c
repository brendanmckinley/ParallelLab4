#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>

static const long Num_To_Sort = 100000;

// Obtained from https://www.geeksforgeeks.org/bubble-sort/
// Used to verify that sort was functioning properly
void printArray(int* arr)
{
    int i;
    for (i = 0; i < Num_To_Sort; i++)
        printf("%i\n", arr[i]);
}

// Swap function
// Obtained from https://www.geeksforgeeks.org/bubble-sort/
void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
// Obtained from https://www.geeksforgeeks.org/bubble-sort/
void sort_s(int *arr) {
    int i, j;
    for (i = 0; i < Num_To_Sort-1; i++) {
        // Last i elements are already in place
        for (j = 0; j < Num_To_Sort - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(&arr[j], &arr[j + 1]);
        }
    }
}

// Parallel version of your sort
// Using algorithm from http://www.personal.kent.edu/~rmuhamma/Algorithms/MyAlgorithms/Sorting/bubbleSort.htm
void sort_p(int *arr) {
    int i, j, k;
    #pragma omp parallel num_threads(2)
    {
        for (i = 0; i < Num_To_Sort - 1; i++) {
            // split the sort into two different threads
            if (omp_get_thread_num() == 0) {
                for (j = 0; j < Num_To_Sort / 2; j++) {
                    if (arr[2 * j] > arr[2 * j + 1])
                        swap(&arr[2 * j], &arr[2 * j + 1]);
                }
            } else if (omp_get_thread_num() == 1) {
                for (k = 0; k < Num_To_Sort / 2 - 1; k++) {
                    if (arr[2 * k + 1] > arr[2 * k + 2])
                        swap(&arr[2 * k + 1], &arr[2 * k + 2]);
                }
            }
        }
    }
}

int main() {
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = rand_r(&seed);
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    sort_s(arr_s);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    sort_p(arr_p);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}

