#include <stdio.h>
#include <mpi.h>
#include <fstream>
#include <vector>
#include <sstream>

#define MASTER_RANK 0
#define MASTER_TAG 1
#define WORKER_TAG 2
#define M_SIZE 100
#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

MPI_Status status;

int a[M_SIZE][M_SIZE];
int b[M_SIZE][M_SIZE];
int c[M_SIZE][M_SIZE];

void ReadMatrixFromFile(const char* filename, int matrix[M_SIZE][M_SIZE]) {
    std::ifstream file(filename);
    std::string line;
    for (int i = 0; i < M_SIZE; i++) {
        std::getline(file, line);
        std::stringstream ss(line);
        for (int j = 0; j < M_SIZE; j++) {
            ss >> matrix[i][j];
            if (ss.peek() == ',') ss.ignore();
        }
    }
}

void WriteMatrixToFile(const char* filename, int matrix[M_SIZE][M_SIZE]) {
    std::ofstream file(filename);
    for (int i = 0; i < M_SIZE; i++) {
        for (int j = 0; j < M_SIZE; j++) {
            file << matrix[i][j];
            if (j < M_SIZE - 1) file << ",";
        }
        file << "\n";
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        if (MASTER_RANK == 0) {
            printf("Usage: mpirun -np <num_processes> %s <matrix_file_1> <matrix_file_2> <output_file>\n", argv[0]);
        }
        return 1;
    }

    const char* matrix_file_1 = argv[1];
    const char* matrix_file_2 = argv[2];
    const char* output_file = argv[3];

    int communicator_size, process_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (communicator_size < 2) {
        MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
    }

    if (process_rank == MASTER_RANK) {
        ReadMatrixFromFile(matrix_file_1, a);
        ReadMatrixFromFile(matrix_file_2, b);

        int workers_num = communicator_size - 1;
        int whole_part = M_SIZE / workers_num;
        int remainder = M_SIZE % workers_num;
        int offset = 0;

        for (int process_id = 1; process_id <= workers_num; process_id++) {
            int rows_num = process_id <= remainder ? whole_part + 1 : whole_part;
            MPI_Send(&offset, 1, MPI_INT, process_id, MASTER_TAG, MPI_COMM_WORLD);
            MPI_Send(&rows_num, 1, MPI_INT, process_id, MASTER_TAG, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows_num * M_SIZE, MPI_INT, process_id, MASTER_TAG, MPI_COMM_WORLD);
            MPI_Send(&b, M_SIZE * M_SIZE, MPI_INT, process_id, MASTER_TAG, MPI_COMM_WORLD);
            offset += rows_num;
        }

        for (int process_id = 1; process_id <= workers_num; process_id++) {
            MPI_Recv(&offset, 1, MPI_INT, process_id, WORKER_TAG, MPI_COMM_WORLD, &status);
            int rows_num;
            MPI_Recv(&rows_num, 1, MPI_INT, process_id, WORKER_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows_num * M_SIZE, MPI_INT, process_id, WORKER_TAG, MPI_COMM_WORLD, &status);
        }

        WriteMatrixToFile(output_file, c);
    } else {
        int offset, rows_num;
        MPI_Recv(&offset, 1, MPI_INT, MASTER_RANK, MASTER_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows_num, 1, MPI_INT, MASTER_RANK, MASTER_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&a, rows_num * M_SIZE, MPI_INT, MASTER_RANK, MASTER_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, M_SIZE * M_SIZE, MPI_INT, MASTER_RANK, MASTER_TAG, MPI_COMM_WORLD, &status);

        for (int k = 0; k < M_SIZE; k++) {
            for (int i = 0; i < rows_num; i++) {
                c[i][k] = 0;
                for (int j = 0; j < M_SIZE; j++) {
                    c[i][k] += a[i][j] * b[j][k];
                }
            }
        }

        MPI_Send(&offset, 1, MPI_INT, MASTER_RANK, WORKER_TAG, MPI_COMM_WORLD);
        MPI_Send(&rows_num, 1, MPI_INT, MASTER_RANK, WORKER_TAG, MPI_COMM_WORLD);
        MPI_Send(&c, rows_num * M_SIZE, MPI_INT, MASTER_RANK, WORKER_TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
