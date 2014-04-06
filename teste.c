#include <stdio.h>
#include <stdlib.h>

typedef struct{
		int lines;
		int columns;
		int *matrix;
} matrix_t;

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
int main(int argc, char *argv[]){
	
	
	matrix_t *MA = readMatrix("ini1.txt");	
	//matrix_t *MB = readMatrix("ini2.txt");
	
	writeMatrix("out.txt", *MA);
	
	printMatrix(*MA);
	//printMatrix(*MB);
	
	
	
	return 0;
}
