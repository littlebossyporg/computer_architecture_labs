// computer_architecture_lab_01.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h> 
#include "mpi.h"
#include <sys/timeb.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>


using namespace std;
//используется квадратная матрица, но для более оптимальных результатов и точнх исследований рекомендуется 
//воспользоваться матрицеё с большим числом столбцов (например 1000) и относительно небольшим числом строк (например 100)
int  matrix_size = 1000;
double * matrix, * part, * vector, * result, * result_part;
double  rows_in_part, counting_start, counting_end;
int  number_of_processes = 0, process_rank = 0;


void matrix_rand_generation(double* &matrix_) {
	for (int counter = 0; counter < matrix_size; counter++) {
		for(int count = 0; count < matrix_size; count++) {
			matrix_[counter*matrix_size + count] = rand() % 40 + 10;
		}
	}
}


void vector_rand_generation(double* &vector_) {
	for (int counter = 0; counter < matrix_size; counter++) {
		vector_[counter] = rand() % 40 + 10;
	}
}


void main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes); 
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);	
	rows_in_part = matrix_size / number_of_processes;
	matrix = new double[matrix_size*matrix_size];
	vector = new double[matrix_size];
	matrix_rand_generation(matrix);
	vector_rand_generation(vector);
	result = new double[rows_in_part];
	for (int counter = 0; counter < rows_in_part; counter++) result[counter] = 0;
	part = new double[rows_in_part*matrix_size];
	for (int counter = 0; counter < rows_in_part; counter++) {
		for (int count = 0; count < matrix_size; count++) {
			part[counter*matrix_size + count] = rand() % 40 + 10;
		}
	}
	result_part = new double[matrix_size];
	for (int counter = 0; counter < matrix_size; counter++) result_part[counter] = 0;
	counting_start = MPI_Wtime();
	MPI_Bcast(vector, matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
	MPI_Scatter(matrix, matrix_size * rows_in_part, MPI_DOUBLE, part, matrix_size * rows_in_part, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
	for (int counter = 0; counter < rows_in_part; counter++) {
		for (int count = 0; count < matrix_size; count++) {
			result[counter] += part[counter*matrix_size + count] * vector[count];
		}
	}
	MPI_Gather(result, rows_in_part, MPI_DOUBLE, result_part, rows_in_part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	counting_end = MPI_Wtime();	
	if (process_rank == 0) {
		cout << counting_end - counting_start;
	}
	MPI_Finalize();
}