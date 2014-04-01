#include <stdio.h>
#include <stdlib.h>


void main(int argc, char argv[]){
	
	int M1_linhas,M1_colunas, M2_linhas, M2_colunas,i, j, temp;
	
	FILE *input = fopen("ini1.txt", "r");
	
	fscanf(input, "LINHAS = %d\n", &M1_linhas);
	fscanf(input, "COLUNAS = %d\n", &M1_colunas);
	
	int m1[M1_linhas][M1_colunas];
	
	for(i = 0; i < M1_linhas; ++i){
			for(j = 0; j < M1_colunas; ++j){
				fscanf(input, "%d", &temp);
				m1[i][j] = temp;
			}
	}	
	fclose(input);	
	FILE *input2 = fopen("ini2.txt", "r");
	
	fscanf(input2, "LINHAS = %d\n", &M2_linhas);
	fscanf(input2, "COLUNAS = %d\n", &M2_colunas);
	
	int m2[M2_linhas][M2_colunas];
	
	for(i = 0; i < M2_linhas; ++i){
			for(j = 0; j < M2_colunas; ++j){
				fscanf(input2, "%d", &temp);
				m2[i][j] = temp;
			}
	}	
	fclose(input);	


	
	//PRINT
	
	printf("\n\n--M1--\n");
	for(i=0; i < M1_linhas; ++i){
		for(j=0; j < M1_colunas; ++j){
			printf("%d ", m1[i][j]);
		}
		printf("\n");
	}
	printf("\n\n--M2--\n");
	for(i=0; i < M2_linhas; ++i){
		for(j=0; j < M2_colunas; ++j){
			printf("%d ", m2[i][j]);
		}
		printf("\n");
	}
	
	
	
}
