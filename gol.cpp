#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sys/time.h>
#include <omp.h>

using namespace std;

const int ALIVE = 1;
const int DEAD = 0;
int gameOver = 0;

/* returns the current time in seconds*/
double gettime(void) {
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return( (double)tval.tv_sec + (double)tval.tv_usec/1000000.0 );
}

/* prints the table passed into the the function and its generation */
void printTable(int** table, int gen, int N2){
	cout << "Generation " << gen << ":\n";
	for (int i = 0; i < N2; i++) {
		for (int j = 0; j < N2; j++) {
			cout << table[i][j] << " ";
		}
		cout << "\n";
	}
}

/* modifies the nextGen table to represent the next generation of The Game of Life */
void nextGeneration(int** table, int** nextGen, int gen, int N2) {
	int changed = 0;
	#pragma omp parallel for reduction(+:changed)
	for (int i = 1; i < N2-1; i++) {
		for (int j = 1; j < N2-1; j++) {
			int localChange = 0;
			int neighbors = 0;
			neighbors += table[i - 1][j];
			neighbors += table[i - 1][j-1];
			neighbors += table[i - 1][j+1];
			neighbors += table[i + 1][j];
			neighbors += table[i + 1][j-1];
			neighbors += table[i + 1][j+1];
			neighbors += table[i][j+1];
			neighbors += table[i][j-1];

			if (table[i][j] == DEAD && neighbors == 3) {
				nextGen[i][j] = ALIVE;
				changed = 1;
				localChange = 1;
			}

			if (neighbors <= 1 || neighbors >= 4) {
				if (table[i][j] == ALIVE) {
					nextGen[i][j] = DEAD;
					changed = 1;
					localChange = 1;
				}
			}
			/* this is used to make sure the two tables stay up to date with each other over the generations since they are being swapped after each iteration */
			if(localChange == 0) {
				nextGen[i][j] = table[i][j];
			}

			neighbors = 0;
		}
	}

	//printTable(nextGen, gen+1, N2);

	if (changed == 0) {
		gameOver = true;
		return;
	}
}

/* initializes a table according to the size provided by the user with each element being randomized to be alive or dead */
void initTable(int** table, int N2){
	for (int i = 0; i < N2; i++) {
		if(i > 0 && i < N2-1){
			srand(time(NULL));
		}
		for (int j = 0; j < N2; j++) {
			if (i == N2 - 1 || j == N2 - 1 || i == 0 || j == 0) {
				table[i][j] = DEAD;
			}
			else {
				if (rand() % 2 < 1) {
					table[i][j] = ALIVE;
				}
				else {
					table[i][j] = DEAD;
				}
			}

		}
	}
}

int main(int argc, char *argv[]){
	double starttime, endtime;
	int N = atoi(argv[1]);
	int maxGen = atoi(argv[2]);
	int numThread = atoi(argv[3]);

	omp_set_num_threads(numThread);

	const int N2 = N + 2;

	int** table = new int*[N2];
	for (int i = 0; i < N2; i++) {
		table[i] = new int[N2];
	}

	int** nextGen = new int*[N2];
	for (int i = 0; i < N2; i++) {
		nextGen[i] = new int[N2];
	}

	initTable(table, N2);

	/* copying the initial values of the table into the nextGen table */
	for (int i = 0; i < N2; i++) {
		for (int j = 0; j < N2; j++) {
			nextGen[i][j] = table[i][j];
		}
	}

	//printTable(table, 0, N2);
	starttime = gettime();
	/* the main game loop that continues until the max generation or the game over condition has been met */
	for(int i = 0; i < maxGen; i++){
		if(gameOver == 0){
			nextGeneration(table, nextGen, i, N2);
			swap(table, nextGen);
		}
	}
	endtime = gettime();

	printf("Time taken = %lf seconds\n", endtime-starttime);

	return 0;
}
