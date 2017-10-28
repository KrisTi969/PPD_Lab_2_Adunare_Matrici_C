// PPDLab3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <stdlib.h>
#include <fstream>
using namespace std;
using namespace chrono;
int matrix1[5000][5000], matrix2[5000][5000], matrix3[5000][5000];
int no_threads, n1, m1, n2, m2;


void add(int matrix1[5000][5000], int matrix2[5000][5000], int res[5000][5000], int lineBegin, int lineEnd, int cols) {

	for (int i = lineBegin; i < lineEnd; i++) {
		for (int j = 0; j < cols; j++) {
			res[i][j] = matrix1[i][j] + matrix2[i][j];
		}
	}
}


void parallelAdd(int matrix1[5000][5000], int matrix2[5000][5000], int res[5000][5000], int n, int m) {
	vector<thread> threads;
	int k = n / no_threads;

	int i = 0;
	int nn = n;
	if (n % no_threads != 0) {
		nn = n - 1;
	}
	long start = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
	for (i = 0; i < nn; i = i + k) {
		thread t(add, matrix1, matrix2, res, i, i + k, m);
		threads.push_back(move(t));

	}
	thread t(add, matrix1, matrix2, res, i - k, n, m);
	threads.push_back(move(t));

	for (thread &t : threads) {
		t.join();
	}
	long end = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
	cout << "parallel add: " << (end - start) << " ms" << endl;
}

void generateMatrix(int threads, int n, int m, int n2, int m2)
{
	srand(time(NULL));
	ofstream f;
	f.open("matrix-data.txt");

	f << threads << " " << n << " " << m << " " << n2 << " " << m2 << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			f << (rand() % 10) << " ";
		}
		f << endl;
	}

	f << endl;

	for (int i = 0; i < n2; i++) {
		for (int j = 0; j < m2; j++) {
			f << (rand() % 10) << " ";
		}
		f << endl;
	}
	f.close();
}

void readData() {
	ifstream f("matrix-data.txt");
	f >> no_threads;
	f >> n1;
	f >> m1;
	f >> n2;
	f >> m2;
	for (int i = 0; i < n1; i++)
		for (int j = 0; j < m1; j++) {
			f >> matrix1[i][j];
		}
	for (int i = 0; i < n2; i++)
		for (int j = 0; j < m2; j++) {
			f >> matrix2[i][j];
		}
	f.close();
}

void printMatrix(int matrix[5000][5000], int n, int m) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
}

void addOptimised(int matrix1[5000][5000], int matrix2[5000][5000], int res[5000][5000], int iStart, int iStop, int jStart, int jStop, int n) {
	int iStartLocal = iStart;
	int iStopLocal = iStop;
	if (jStart > 0) {
		iStartLocal++;
		for (int i = jStart; i < n; i++) {
			res[iStart][i] = matrix1[iStart][i] + matrix2[iStart][i];
		}
	}
	if (jStop < n) {
		iStopLocal--;
		for (int i = 0; i < jStop; i++) {
			res[iStop][i] = matrix1[iStart][i] + matrix2[iStart][i];
		}
	}
	for (int i = iStart; i <= iStopLocal; i++) {
		for (int j = 0; j < n; j++) {
			//matrice3.setMatrice(i, j, matrice1.getMatrice()[i][j] + matrice2.getMatrice()[i][j]);
			res[i][j] = matrix1[i][j] + matrix2[i][j];
		}
	}
}


void parallelAddOptimised(int matrix1[5000][5000], int matrix2[5000][5000],int res[5000][5000], int nrThreaduri, int n) {
	int operatiiThread = (n * n) / nrThreaduri;
	int rest = (n * n) % nrThreaduri;

	int iStart = 0, jStart = 0, iStop = 0, jStop = 0;

	vector<thread> threads;

	for (int i = 0; i < nrThreaduri; ++i) {
		int operatiiFinal;
		if (rest > 0) {
			operatiiFinal = operatiiThread + 1;
			rest--;
		}
		else {
			operatiiFinal = operatiiThread;
		}
		while (operatiiFinal != 0) {
			if (jStop == n) {
				jStop = 0;
				iStop++;
			}
			jStop++;
			if (jStop == n) {
				jStop = 0;
				iStop++;
			}
			operatiiFinal--;
		}
		
		//////////////////////////////
		thread t(addOptimised, matrix1, matrix2, res, iStart, iStop, jStart, jStop, n);
		threads.push_back(move(t));

		iStart = iStop;
		jStart = jStop + 1;
		if (jStart > n) {
			jStart = 0;
			++iStart;
		}

	}
	long start = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
	for (thread &t : threads) {
		t.join();
	}
	long end = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
	cout << "parallel add execution time: " << (end - start) << " ms" << endl;
}

void multiplyOptimised(int matrix1[5000][5000], int matrix2[5000][5000], int res[5000][5000], int iStart, int iStop, int jStart, int jStop, int n) {
	for (int i = iStart; i <= iStop; ++i) {
		for (int j = 0; j < n; j++) {
			if (i == iStart) {
				if (j >= jStart) {
					int result = 0;
					for (int index = 0; index < n; index++) {
						result += (matrix1[i][index] * matrix2[index][j]);
					}
					res[i][j] = result;
				}
			}
			else if (i == iStop) {
				if (j <= jStop) {
					int result = 0;
					for (int index = 0; index < n; index++) {
						result += (matrix1[i][index] * matrix2[index][j]);
					}
					res[i][j] = result;
				}
			}
			else {
				int result = 0;
				for (int index = 0; index < n; ++index) {
					result += (matrix1[i][index] * matrix2[index][j]);
				}
				res[i][j] = result;
			}
		}
	}
}



void parallelMultiplyOptimised(int matrix1[5000][5000], int matrix2[5000][5000], int res[5000][5000], int nrThreaduri, int n) {
	int operatiiThread = (n * n) / nrThreaduri;
	int rest = (n * n) % nrThreaduri;

	int iStart = 0, jStart = 0, iStop = 0, jStop = 0;

	vector<thread> threads;

	for (int i = 0; i < nrThreaduri; ++i) {
		int operatiiFinal;
		if (rest > 0) {
			operatiiFinal = operatiiThread + 1;
			rest--;
		}
		else {
			operatiiFinal = operatiiThread;
		}
		while (operatiiFinal != 0) {
			if (jStop == n) {
				jStop = 0;
				iStop++;
			}
			jStop++;
			if (jStop == n) {
				jStop = 0;
				iStop++;
			}
			operatiiFinal--;
		}

		//////////////////////////////
		thread t(multiplyOptimised, matrix1, matrix2, res, iStart, iStop, jStart, jStop, n);
		threads.push_back(move(t));

		iStart = iStop;
		jStart = jStop + 1;
		if (jStart > n) {
			jStart = 0;
			++iStart;
		}

	}
	long start = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
	for (thread &t : threads) {
		t.join();
	}
	long end = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
	cout << "parallel add execution time: " << (end - start) << " ms" << endl;
}

int main()
{
	//thread t1(call_from_thread);
	//int a;
	//cin >> a;
	generateMatrix(8, 2, 2, 2, 2);
	readData();
	//parallelAddOptimised(matrix1, matrix2, matrix3, no_threads, n1);
	parallelMultiplyOptimised(matrix1, matrix2, matrix3, no_threads, n1);
	printMatrix(matrix3, n1, m1);
	cin >> n1;
	return 0;
}