#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct{
		int lines;
		int columns;
		int *matrix;
} matrix_t;

typedef struct{
		matrix_t *matrixA;
		matrix_t *matrixB;
		matrix_t *matrixResult;
		int idThread;
		int threadCount;
} function_t;

void setMatrixValue(matrix_t *mMatrix, int line, int column, int value){
		mMatrix->matrix[line * 10 + column] = value;
}

int getMatrixValue(matrix_t *mMatrix, int line, int column){
		return mMatrix->matrix[line * 10 + column];
}

matrix_t *readMatrix(char *filename){
		matrix_t *mMatrix = calloc(1, sizeof(matrix_t));
		int lines, columns;
		int i,j, tmp;
		
		FILE *input = fopen(filename, "r");
		
		if(input == NULL){
			printf("File Error!");
			exit(1);
		}
		
		fscanf(input, "LINHAS = %d\n", &lines);
		fscanf(input, "COLUNAS = %d\n", &columns);
		
		mMatrix->lines = lines;
		mMatrix->columns = columns;
		mMatrix->matrix = calloc(mMatrix->lines*mMatrix->columns, sizeof(int));
		
		for(i = 0; i < lines; ++i){
			for(j = 0; j < columns; ++j){
				fscanf(input, "%d", &tmp);
				setMatrixValue(mMatrix, i, j, tmp);
			}
		}
		
		fclose(input);
		return mMatrix;
}

void writeMatrix(char *filename, matrix_t mMatrix){
	FILE *output = fopen(filename, "w+");
	int i,j;
	
	if(output == NULL){
		printf("File Error!");
		exit(1);
	}
	
	fprintf(output, "LINHAS = %d\n", mMatrix.lines);
	fprintf(output, "COLUNAS = %d\n", mMatrix.columns);
	
	for(i = 0; i < mMatrix.lines; ++i){
		for(j = 0; j < mMatrix.columns; ++j){
			fprintf(output, "%d ", getMatrixValue(&mMatrix, i, j));
		}
		fprintf(output, "\n");
	}
	fclose(output);
}

void printMatrix(matrix_t mMatrix){
	int i,j;
	for(i = 0; i < mMatrix.lines; ++i){
		for(j = 0; j < mMatrix.columns; ++j){
			printf("%d ", getMatrixValue(&mMatrix, i, j));
		}
		printf("\n");
	}
}

int multiplyLineColumn(int line, int column, matrix_t matrixA, matrix_t matrixB){
	int i,sum=0;
	
	for(i = 0; i < matrixA.lines; ++i){
		//printf("\n %d * %d ", getMatrixValue(&matrixA, line, i) , getMatrixValue(&matrixB, i, column));
		sum += getMatrixValue(&matrixA, line, i) * getMatrixValue(&matrixB, i, column);
	}
	return sum;
}

matrix_t createResultMatrix(matrix_t mA, matrix_t mB){
	matrix_t mResult;
	mResult.lines = mA.lines;
	mResult.columns = mB.columns;
	mResult.matrix = calloc(mResult.lines * mResult.columns, sizeof(int));
	
	return mResult;
}

void *threadFunction(void *parameter){
	function_t *object = (function_t *) parameter;
	int i,j;
	
	for(i = object->idThread; i < object->matrixResult->lines; i+=object->threadCount){
		for(j = 0; j < object->matrixResult->columns; j++){
			setMatrixValue(object->matrixResult, i, j, multiplyLineColumn(i, j, *object->matrixA, *object->matrixB));
		}
	}
	pthread_exit(NULL);
}

matrix_t multiplyMatrix(matrix_t mA, matrix_t mB, int threadsNumber){
	int i;
	matrix_t MC = createResultMatrix(mA, mA);	
	pthread_t *newThreads = calloc(threadsNumber, sizeof(pthread_t));
	
	for(i = 0; i < threadsNumber; i++){
		function_t *object = calloc(1, sizeof(function_t));
		object->matrixResult = &MC;
		object->matrixA = &mA;
		object->matrixB = &mA;
		object->threadCount = threadsNumber;
		object->idThread = i;
		
		pthread_create(&newThreads[i], NULL, threadFunction, object); 
	}
	
	//join
	for(i = 0; i < threadsNumber; i++){
		pthread_t joinThread = newThreads[i];
		pthread_join(joinThread, NULL);
	}
	return MC;
}

int main(int argc, char *argv[]){
	
	matrix_t *MA = readMatrix("ini1.txt");	
	//MB = readMatrix("ini2.txt");
	
	////////////////////////////////////////////////////////////////////
	int threadsNumber = 2;
	
	matrix_t mResult = multiplyMatrix(*MA, *MA, threadsNumber);
	
	writeMatrix("out.txt", mResult);
	
	
	return 0;
}
