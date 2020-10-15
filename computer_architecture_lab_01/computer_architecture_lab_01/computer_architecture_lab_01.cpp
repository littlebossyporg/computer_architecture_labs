// computer_architecture_lab_01.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h> 
#include "C:\Program Files (x86)\Microsoft SDKs\MPI\Include\mpi.h"
#include <sys/timeb.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>


using namespace std;


int  matrix_size = 1000;
double * matrix;
double * a = new double[matrix_size*matrix_size];
double * vector = new double[matrix_size];
double * result = new double[matrix_size];
double * c = new double[matrix_size];
double  start, finish, duration;
int  number_of_processes = 0, process_rank = 0;


void matrix_rand_generation(double* &matrix_) {
	matrix_ = new double[matrix_size*matrix_size];
	for (int counter = 0; counter < matrix_size; counter++) {
		for(int count = 0; count < matrix_size; count++) {
			matrix_[counter*matrix_size + count] = rand() % 4000 + 100;
		}
	}
}


void vector_rand_generation(double* &vector_) {
	for (int counter = 0; counter < matrix_size; counter++) {
		vector_[counter] = rand() % 4000 + 100;
	}
}


void matrix_print(double* &matrix_) {
	cout << "Matrix : " << endl;
	for (int counter = 0; counter < matrix_size; counter++) {
		for (int count = 0; count < matrix_size; count++) {
			cout.width(6);
			cout << matrix_[counter*matrix_size + count];
		}
		cout << endl;
	}
}


void vector_print(double* &vector_) {
	cout << "Vector : " << endl;
	for (int counter = 0; counter < matrix_size; counter++) {
		cout.width(6);
		cout << vector_[counter];
	}
	cout << endl;
}

void main(int argc, char* argv[]) {
	matrix_rand_generation(matrix);
	//matrix_print(matrix);	// распечатываем матрицу
	vector_rand_generation(vector);
	//vector_print(vector);	// распечатываем вектор
	MPI_Init(&argc, &argv);
	double counting_start = MPI_Wtime(); //начало вычислений
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes); //вычислили количество процессов
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);	//ранг текущего процессора
	int local_n = matrix_size / number_of_processes;
	MPI_Bcast(vector, matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD); //передаём вектор
	MPI_Scatter(a, matrix_size * local_n, MPI_DOUBLE, matrix, matrix_size * local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD); //передаём кусочки матрицы
	for (int i = 0; i < local_n; i++) {
		result[i] = 0;
		for (int j = 0; j < matrix_size; j++)
			result[i] += matrix[i*matrix_size + j] * vector[j];
	}

	MPI_Gather(result, local_n, MPI_DOUBLE, c, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD); //собирает данные со всех процессов, ответ лежит в векторе C
	double counting_end = MPI_Wtime();	//конец вычислений
	if (process_rank == 0) {
		cout << counting_end - counting_start;
	}
	MPI_Finalize();
}

