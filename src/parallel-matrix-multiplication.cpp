#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

void ReadMatrixFromFile(const std::string& filename, int matrix[M_SIZE][M_SIZE]) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "[ERROR] Failed to open file: " << filename << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    std::string line;
    for (int i = 0; i < M_SIZE; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "[ERROR] Unexpected end of file: " << filename << " at line " << i+1 << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        std::stringstream ss(line);
        std::string value;
        for (int j = 0; j < M_SIZE; ++j) {
            if (!std::getline(ss, value, ',')) {
                std::cerr << "[ERROR] Invalid format in file: " << filename << " at row " << i+1 << " column " << j+1 << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            try {
                matrix[i][j] = std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Non-numeric value in file: " << filename << " at row " << i+1 << " column " << j+1 << " (" << value << ")" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
}

void WriteMatrixToFile(const std::string& filename, int matrix[M_SIZE][M_SIZE]) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "[ERROR] Failed to open file for writing: " << filename << std::endl;
        return;
    }
    
    for (int i = 0; i < M_SIZE; ++i) {
        for (int j = 0; j < M_SIZE; ++j) {
            file << matrix[i][j];
            if (j < M_SIZE - 1) file << ",";
        }
        file << "\n";
    }
    if (!file) {
        std::cerr << "[ERROR] Error occurred while writing to file: " << filename << std::endl;
    }
} 

int main(int argc, char *argv[]) {
    int communicator_size, process_rank, process_id, offset, rows_num, workers_num, remainder, whole_part, message_tag;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (communicator_size < 2) {
        std::cerr << "[ERROR] Not enough processes. At least 2 required." << std::endl;
        MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
    }

    if (process_rank == MASTER_RANK) {
        printf("%sReading matrixes from files%s\n", CYN, RESET);
        ReadMatrixFromFile("/mnt/fsx/slurm/matrix_A.in", a);
        ReadMatrixFromFile("/mnt/fsx/slurm/matrix_B.in", b);

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

        printf("\nWriting result to file: matrix_C.out\n");
        WriteMatrixToFile("/mnt/fsx/slurm/matrix_C.out", c);

        long long int end = clock();
        double diff = (double)((end - start) / (1.0 * MICRO));
        printf("\n%dx%d - %f seconds\n", M_SIZE, M_SIZE, diff);
    }
    
    MPI_Finalize();
    return 0;
}
