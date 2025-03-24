#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MASTER_RANK 0
#define M_SIZE 100
#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

void ReadMatrixFromFile(const char* filename, int* matrix, int rows) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < M_SIZE; ++j) {
            if (fscanf(file, "%d,", &matrix[i * M_SIZE + j]) != 1) {
                fprintf(stderr, "[ERROR] Invalid format in file: %s at row %d column %d\n", filename, i + 1, j + 1);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
    fclose(file);
}

void WriteMatrixToFile(const char* filename, int* matrix, int rows) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open file for writing: %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < M_SIZE; ++j) {
            fprintf(file, "%d", matrix[i * M_SIZE + j]);
            if (j < M_SIZE - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <matrix_A_file> <matrix_B_file> <matrix_C_file>\n", argv[0]);
        return 1;
    }
    
    MPI_Init(&argc, &argv);
    int communicator_size, process_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (communicator_size < 2) {
        fprintf(stderr, "[ERROR] Not enough processes. At least 2 required.\n");
        MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
    }

    int rows_per_process = M_SIZE / (communicator_size - 1);
    int remainder = M_SIZE % (communicator_size - 1);
    int local_rows = rows_per_process + (process_rank <= remainder && process_rank > 0 ? 1 : 0);

    int* a = NULL;
    int* b = (int*)malloc(M_SIZE * M_SIZE * sizeof(int));
    int* c = NULL;
    if (process_rank == MASTER_RANK) {
        a = (int*)malloc(M_SIZE * M_SIZE * sizeof(int));
        c = (int*)malloc(M_SIZE * M_SIZE * sizeof(int));
        ReadMatrixFromFile(argv[1], a, M_SIZE);
        ReadMatrixFromFile(argv[2], b, M_SIZE);
    }

    int* local_a = (int*)malloc(local_rows * M_SIZE * sizeof(int));
    int* local_c = (int*)malloc(local_rows * M_SIZE * sizeof(int));

    MPI_Scatter(a, local_rows * M_SIZE, MPI_INT, local_a, local_rows * M_SIZE, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(b, M_SIZE * M_SIZE, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < M_SIZE; j++) {
            local_c[i * M_SIZE + j] = 0;
            for (int k = 0; k < M_SIZE; k++) {
                local_c[i * M_SIZE + j] += local_a[i * M_SIZE + k] * b[k * M_SIZE + j];
            }
        }
    }

    MPI_Gather(local_c, local_rows * M_SIZE, MPI_INT, c, local_rows * M_SIZE, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);

    if (process_rank == MASTER_RANK) {
        WriteMatrixToFile(argv[3], c, M_SIZE);
        free(a);
        free(c);
    }
    free(b);
    free(local_a);
    free(local_c);

    MPI_Finalize();
    return 0;
}