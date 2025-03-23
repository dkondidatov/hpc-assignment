#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <time.h>

#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"
#define CYN   "\x1B[36m"

#define MASTER_RANK 0

#define MASTER_TAG 1
#define WORKER_TAG 2

#define M_SIZE 100
#define MICRO 1000000

#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

MPI_Status status;

int a[M_SIZE][M_SIZE];
int b[M_SIZE][M_SIZE];
int c[M_SIZE][M_SIZE];

void ReadMatrixFromFile(const char* filename, int matrix[M_SIZE][M_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    char line[1024];
    for (int i = 0; i < M_SIZE; ++i) {
        if (!fgets(line, sizeof(line), file)) {
            fprintf(stderr, "[ERROR] Unexpected end of file: %s at line %d\n", filename, i+1);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        char* token = strtok(line, ",");
        for (int j = 0; j < M_SIZE; ++j) {
            if (!token) {
                fprintf(stderr, "[ERROR] Invalid format in file: %s at row %d column %d\n", filename, i+1, j+1);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            matrix[i][j] = atoi(token);
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void WriteMatrixToFile(const char* filename, int matrix[M_SIZE][M_SIZE]) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "[ERROR] Failed to open file for writing: %s\n", filename);
        return;
    }
    
    for (int i = 0; i < M_SIZE; ++i) {
        for (int j = 0; j < M_SIZE; ++j) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < M_SIZE - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    if (ferror(file)) {
        fprintf(stderr, "[ERROR] Error occurred while writing to file: %s\n", filename);
    }
    fclose(file);
} 

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <matrix_A_file> <matrix_B_file> <matrix_C_file>\n", argv[0]);
        return 1;
    }
    
    const char* matrix_A_file = argv[1];
    const char* matrix_B_file = argv[2];
    const char* matrix_C_file = argv[3];

    int communicator_size, process_rank, process_id, offset, rows_num, workers_num, remainder, whole_part, message_tag;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (communicator_size < 2) {
        fprintf(stderr, "[ERROR] Not enough processes. At least 2 required.\n");
        MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
    }

    if (process_rank == MASTER_RANK) {
        printf("%sReading matrices from files%s\n", CYN, RESET);
        ReadMatrixFromFile(matrix_A_file, a);
        ReadMatrixFromFile(matrix_B_file, b);

        printf("\nStarting multiplication ... \n");
        long long int start = clock();

        workers_num = communicator_size - 1;
        whole_part = M_SIZE / workers_num;
        remainder = M_SIZE % workers_num;
        offset = 0;

        message_tag = MASTER_TAG;
        for (process_id = 1; process_id <= workers_num; process_id++) {
            rows_num = process_id <= remainder ? whole_part + 1 : whole_part;
            MPI_Send(&offset, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
            MPI_Send(&rows_num, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows_num * M_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
            MPI_Send(&b, M_SIZE * M_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD);
            offset += rows_num;
        }

        message_tag = WORKER_TAG;
        for (process_id = 1; process_id <= workers_num; process_id++) {
            MPI_Recv(&offset, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows_num, 1, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows_num * M_SIZE, MPI_INT, process_id, message_tag, MPI_COMM_WORLD, &status);
        }

        printf("\nWriting result to file: %s\n", matrix_C_file);
        WriteMatrixToFile(matrix_C_file, c);

        long long int end = clock();
        double diff = (double)((end - start) / (1.0 * MICRO));
        printf("\n%dx%d - %f seconds\n", M_SIZE, M_SIZE, diff);
    }
    
    MPI_Finalize();
    return 0;
}
