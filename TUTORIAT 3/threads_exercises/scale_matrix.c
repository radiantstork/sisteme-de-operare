#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

// Put these limits out of lazyness, you can dynamically allocate it if you want
#define MAX_NR_LINES 10
#define MAX_NR_COL 10
#define DEFAULT_SCALE 5

// Struct for easier matrix handling
struct Matrix{
    int mat[MAX_NR_LINES][MAX_NR_COL];
    int nr_lines, nr_col;
} matrix;

// Will be needed for scaleRow
struct ScaleRowArgs{
   int* arr;
   int size;
   int scale;
   int row_index; //just for printing
};

// The function that will be executed by each thread
// args: ScaleRowArgs*
// ret: NULL
void* scaleRow(void* args);

//Print matrix to file
void printMatrix(FILE* fout, const struct Matrix* mat);

//creates matrix and initializes it from file
void readMatrix(FILE* fin, struct Matrix* mat);

//Initializes matrix values
void initMatrixValues(struct Matrix* mat);

// Creates matrix.nr_lines - 1 threads and scales each row of the matrix with DEFAULT_SCALE
// Extra: Read different scales from a file and make each thread apply a different scale
int main(){
    FILE* matrix_in = fopen("matrix.in", "r");
    if (matrix_in == NULL){
        perror("fopen");
        return -1;
    }

    readMatrix(matrix_in, &matrix);
    
    //Keeping track of all created threads
    //pthread_t threads[matrix.nr_lines]
    pthread_t* threads = malloc(sizeof(pthread_t) * (matrix.nr_lines - 1));
    if (threads == NULL){
        fclose(matrix_in);
        perror("malloc");
        return -1;
    }

    // Set-up threads for the rows 1 to matrix.nr_lines - 1
    for (int i = 1; i < matrix.nr_lines; ++i){
        //Initializing arguments for scaleRow
        //IMPORTANT: Using dynamically allocated args (WHY)? 
        struct ScaleRowArgs* args = malloc(sizeof(struct ScaleRowArgs));
        if (args == NULL){
            fclose(matrix_in);
            free(threads);
            perror("malloc");
            return -1;
        }
        
        // Set-up arguments for scaleRow
        *args = (struct ScaleRowArgs){.arr = &matrix.mat[i][0], .size = matrix.nr_col, .scale = DEFAULT_SCALE, .row_index = i};

        //Attemping to create thread to scale row i
        if (pthread_create(&threads[i - 1], NULL, scaleRow, args)){
            fclose(matrix_in);
            free(threads);
            perror("pthread_create");
            return -1;
        }
    }
    
    // Scale row 0 (why did I not allocate args dynamically?)
    struct ScaleRowArgs args = {.arr = &matrix.mat[0][0], .size = matrix.nr_col, .scale = DEFAULT_SCALE, .row_index = 0};
    scaleRow(&args);

    //Joining threads with parent
    for (int i = 0; i < matrix.nr_lines - 1; ++i)
        if (pthread_join(threads[i], NULL)){
            fclose(matrix_in);
            free(threads);
            perror("pthread_join");
            return -1;
        }
    
    //Open file for printing the result matrix
    FILE* matrix_out = fopen("matrix.out", "w");
    if (matrix_out == NULL){
        fclose(matrix_in);
        free(threads);
        perror("fopen");
        return -1;
    }

    // Print result matrix
    printMatrix(matrix_out, &matrix);

    //Freeing resources
    free(threads);
    fclose(matrix_in), fclose(matrix_out);
}

void* scaleRow(void* args){
    struct ScaleRowArgs* conv_args = args;

    printf("Thread %ld is handling row %d\n", pthread_self(), conv_args->row_index);
    for (int i = 0; i < conv_args->size; ++i)
        conv_args->arr[i] *= conv_args->scale;
    
    printf("Thread %ld finished row %d\n", pthread_self(), conv_args->row_index);
    return NULL;
}


void printMatrix(FILE* fout, const struct Matrix* mat){
    fprintf(fout, "%d %d\n", mat->nr_lines, mat->nr_col);
    for (int i = 0; i < mat->nr_lines; ++i){
        for (int j = 0; j < mat->nr_col; ++j)
            fprintf(fout, "%d ", mat->mat[i][j]);
        fprintf(fout, "\n");
    }           
}

void initMatrixValues(struct Matrix* mat){
    for (int i = 0; i < mat->nr_lines; ++i)
       for (int j = 0; j < mat->nr_col; ++j)
            mat->mat[i][j] = 0;
}


void readMatrix(FILE* fin, struct Matrix* mat){
    fscanf(fin, "%d %d", &mat->nr_lines, &mat->nr_col);

    for (int i = 0; i < mat->nr_lines; ++i)
        for (int j = 0; j < mat->nr_col; ++j)
            fscanf(fin, "%d", &(mat->mat[i][j]));
}
