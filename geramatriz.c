#include <stdlib.h>
#include <stdio.h>


#define MAX 1000
int writeMatrix(char* filename, int** matriz, int linesCount, int columnsCount){
	FILE* outputFile = NULL;
	int i, j;

	outputFile = fopen(filename, "w");
	
	if(outputFile == NULL){
		printf("File Error!");
		exit(1);
	}

	fprintf(outputFile, "LINHAS = %d\n", linesCount);
	fprintf(outputFile, "COLUNAS = %d\n", columnsCount);

	for(i = 0; i < linesCount; i++)	{
		for(j = 0; j < columnsCount-1; j++){
			fprintf(outputFile, "%d ", matriz[i][j]);
		}
		fprintf(outputFile, "%d\n", matriz[i][j-1]);
	}

	fclose(outputFile);
	return 0;
}

int geraMatriz(int n, int m)
{
	srand(time(NULL));

	int i, j;

	int** matriz = (int**)calloc(n,sizeof(int*));

	for(i = 0; i < n; i++)
	{
		matriz[i] = (int*)calloc(m,sizeof(int));

		for(j = 0; j < m; j++)
		{
			matriz[i][j] = rand()%MAX - MAX/2;
		}
	}

	writeMatrix("ini1.txt",matriz,n,m);
	writeMatrix("ini2.txt",matriz,n,m);

	return 0;
}

int main(int argc, char const *argv[])
{
	geraMatriz(atoi(argv[1]),atoi(argv[2]));

	return 0;
}
