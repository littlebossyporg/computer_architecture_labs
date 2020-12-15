// computer_architecture_lab_01.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <sys/timeb.h>
#include <ctime>
#include <math.h> 
#include <random>
using namespace std;

bool can_be_solved;
int matrix_size, real_matrix_size = 4, main_element_index;
double *matrix, *main_element_line, *part, *matrix_copy;
int number_of_processes = 0, process_rank = 0;
double  rows_in_part, element, coefficient, counting_start, counting_end;

void matrix_rand_generation(double *matrix_, int matrix_size_) {
	for (int counter = 0; counter < matrix_size_ / 2; counter++) {
		for (int count = 0; count < matrix_size_ / 2; count++) matrix_[counter*matrix_size_ + count] = rand() % 40 + 10;
		for (int count = matrix_size_ / 2; count < matrix_size_; count++) {
			if (counter == (count - matrix_size_ / 2)) matrix_[counter*matrix_size_ + count] = 1;
			else matrix_[counter*matrix_size_ + count] = 0;
		}
	}
}

void take_away_from_all_lines(int rank) {
	MPI_Bcast(main_element_line, matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&main_element_index, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(matrix, rows_in_part * matrix_size, MPI_DOUBLE, part, rows_in_part * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD); //отправка матрицы по процессам, где каждому процессу передаётся только часть матрицы
	for (int i = 0; i < rows_in_part; i++) {
		coefficient = part[i*matrix_size + main_element_index] / main_element_line[main_element_index];
		for (int j = 0; j < matrix_size; j++) {
			part[i*matrix_size + j] -= main_element_line[j] * coefficient;
		}
	}
	MPI_Gather(part, rows_in_part * matrix_size, MPI_DOUBLE, matrix_copy, rows_in_part * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv); //инициализация MPI
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
	matrix_size = 2 * real_matrix_size;
	rows_in_part = real_matrix_size / number_of_processes; //выделение памяти под массивы
	matrix = new double[rows_in_part * number_of_processes * matrix_size];
	matrix_copy = new double[rows_in_part * number_of_processes * matrix_size];
	for (int counter = 0; counter < rows_in_part * number_of_processes * matrix_size; counter++) matrix[counter] = matrix_copy[counter] = 0;
	main_element_line = new double[matrix_size]; 
	for (int counter = 0; counter < matrix_size; counter++) main_element_line[counter] = 0;
	part = new double[rows_in_part *matrix_size]; 
	for (int counter = 0; counter < rows_in_part *matrix_size; counter++) part[counter] = 0;
	if (process_rank == 0) {
		matrix_rand_generation(matrix, matrix_size); //заполняем матрицу случайными элементами
		counting_start = MPI_Wtime(); //начали отсчёт времени
		can_be_solved = true; //возможно ли вообще найти обратную матрицу ?
		for (int counter = 0; counter < real_matrix_size; counter++) {
			main_element_index = -1;
			for (int count = 0; count < real_matrix_size; count++) {
				if (matrix[counter*matrix_size + count] != 0) { //если в столбце все элементы нулевые матрица вырожденая
					main_element_index = count;
					break;
				}
			}
			if (main_element_index == -1) {
				cout << "Error: matrix is degenerate !" << endl;
				can_be_solved = false;
				return 0;
			}
			element = matrix[counter*matrix_size + main_element_index];
			for (int count = 0; count < matrix_size; count++) {
				matrix[counter*matrix_size + count] /= element;
				main_element_line[count] = matrix[counter*matrix_size + count];
			}
			take_away_from_all_lines(process_rank);
			matrix = matrix_copy;
			for (int j = 0; j < matrix_size; j++) matrix[counter*matrix_size + j] = main_element_line[j];
		}
	}
	else for (int counter = 0; counter < real_matrix_size; counter++) take_away_from_all_lines(process_rank);
	counting_end = MPI_Wtime();
	if (process_rank == 0) {
		for (int i = 0; i < rows_in_part * number_of_processes * matrix_size; i++) {
			cout.width(13);
			cout << matrix[i];

		}
		cout << endl;
		cout << counting_end - counting_start << endl;
	}
	MPI_Finalize();
	return 0;
}