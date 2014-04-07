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

matrix_t *MA;
matrix_t *MB;
matrix_t *MC;


void setMatrixValue(matrix_t *mMatrix, int line, int column, int value){
		mMatrix->matrix[line * 10 + column] = value;
}

int getMatrixValue(matrix_t *mMatrix, int line, int column){
		return mMatrix->matrix[line * 10 + column];
}

matrix_t *readMatrix(char *filename){
		FILE *file = NULL;
		matrix_t *mMatrix = calloc(1, sizeof(matrix_t));
		char strLinha[1048576];
		char strNumero[1048576]; 
		int lines, columns;
		int i,j, tmp;
		
		file = fopen(filename, "r");
		
		if(file == NULL){
			printf("File Error!");
			exit(1);
		}
		
		//get Linhas
		fgets(strLinha, sizeof(strLinha), file);
		for (i = 0; strLinha[i] != '='; i++); 
		i+=2;//pula o espaço depois de =
		for (j = 0; ((strLinha[i] != '\n') && (strLinha[i] != ' ') && (strLinha[i] != '\t')); strNumero[j++] = strLinha[i++]);
		strNumero[j] = '\0';
		lines = atoi(strNumero);
		
		//get Colunas		
		fgets(strLinha, sizeof(strLinha), file);
		for (i = 0; strLinha[i] != '='; i++);
		i+=2;//pula o espaço depois de =
		for (j = 0; ((strLinha[i] != '\n') && (strLinha[i] != ' ') && (strLinha[i] != '\t')); strNumero[j++] = strLinha[i++]);
		strNumero[j] = '\0';  
		columns = atoi(strNumero);
		
		
		mMatrix->lines = lines;
		mMatrix->columns = columns;
		mMatrix->matrix = calloc(mMatrix->lines*mMatrix->columns, sizeof(int));
		
		//get values
		i = 0; //linha
	    j = 0; //coluna
	    int k = 0; //iterador da string linha
	    int l = 0; //iterador da string numero
	  
	    while ((fgets(strLinha,sizeof(strLinha),file) != NULL) && (i < lines))
	    {
			j = 0;
			k = 0;		
			//le numero a numero da linha
			while ((k < strlen(strLinha)) && (j < columns))
			{
				for (l = 0; ((strLinha[k] != '\n') && (strLinha[k] != ' ') && (strLinha[k] != '\t')); strNumero[l++] = strLinha[k++]);
				strNumero[l] = '\0';
				k++; //pula o espaço
		  
				setMatrixValue(mMatrix, i, j, atoi(strNumero));
				j++;
			}
			i++;
		}
		
		fclose(file);
		return mMatrix;
}

void writeMatrix(char *filename, matrix_t mMatrix){
	FILE *file = fopen(filename, "w");
	int i,j;
	
	if(file == NULL){
		printf("File Error!");
		exit(1);
	}
	
	fprintf(file, "LINHAS = %d\n", mMatrix.lines);
	fprintf(file, "COLUNAS = %d\n", mMatrix.columns);
	
	for(i = 0; i < mMatrix.lines; ++i){
		for(j = 0; j < mMatrix.columns; ++j){
			fprintf(file, "%d ", getMatrixValue(&mMatrix, i, j));
		}
		fprintf(file, "\n");
	}
	fclose(file);
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
	
	MA = readMatrix("ini1.txt");	
	MB = readMatrix("ini2.txt");
	
	////////////////////////////////////////////////////////////////////
	int threadsNumber = 2;
	//matrix_t mResult = createResultMatrix(*MA,*MA);
	//mResult = multiplyMatrix(*MA, *MA, threadsNumber);
	
	writeMatrix("out.txt", *MB);
	printf("\n\nMatriz A \n");
	printMatrix(*MA);
	printf("\n\nMatriz B \n");
	printMatrix(*MB);
	
	
	printf("\n\nMA[1][4] = %d \n\n", getMatrixValue(MA, 1, 4));
	
	return 0;
}
