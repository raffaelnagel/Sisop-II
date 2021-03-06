#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#define tamanhoAmostra 10

typedef struct {
	int id;
} object;

int readMatrix(const char *filename, int *linesCount, int *columnsCount, int ***resultMatrix)
{
  FILE *inputFile = NULL;
  char stringBuffer[1048576], tmpBuffer[1048576]; 
  int i,j;
  int **mMatrix;
  
  inputFile = fopen(filename, "r");
  if (inputFile == NULL){
    printf("\nFile Error!");
    return 0;
  }
    
  //get LINHAS
  fgets(stringBuffer, sizeof(stringBuffer), inputFile);
  for (i = 0; stringBuffer[i] != '='; i++); 
  i+=2;
  for (j = 0; ((stringBuffer[i] != '\n') && (stringBuffer[i] != ' ') && (stringBuffer[i] != '\t')); tmpBuffer[j++] = stringBuffer[i++]);
  tmpBuffer[j] = '\0';
  
  *linesCount = atoi(tmpBuffer);
 
  //get COLUNAS
  fgets(stringBuffer, sizeof(stringBuffer), inputFile);
  for (i = 0; stringBuffer[i] != '='; i++);
  i+=2;
  for (j = 0; ((stringBuffer[i] != '\n') && (stringBuffer[i] != ' ') && (stringBuffer[i] != '\t')); tmpBuffer[j++] = stringBuffer[i++]);
  tmpBuffer[j] = '\0';
  
  *columnsCount = atoi(tmpBuffer);
  
  //Define result matrix
  mMatrix = NULL;
  mMatrix = (int **) malloc(sizeof(int *) * (*linesCount));
  
  for (i = 0; i < *linesCount; i++){
    mMatrix[i] = (int *) malloc (sizeof(int) * (*columnsCount));   
  }
  
  //get Values
  i = 0;
  j = 0; 
  int itBuffer = 0, itTmp = 0;
  
  while ((fgets(stringBuffer,sizeof(stringBuffer),inputFile) != NULL) && (i < *linesCount)){
		j = 0;
		itBuffer = 0;    
	 
		while ((itBuffer < strlen(stringBuffer)) && (j < (*columnsCount))){
			  for (itTmp = 0; ((stringBuffer[itBuffer] != '\n') && (stringBuffer[itBuffer] != ' ') && (stringBuffer[itBuffer] != '\t')); tmpBuffer[itTmp++] = stringBuffer[itBuffer++]);
			  tmpBuffer[itTmp] = '\0';
			  itBuffer++;
			  			  
			  mMatrix[i][j] = atoi(tmpBuffer);
			  j++;
		}		
		i++;
  }
  
  *resultMatrix = mMatrix;
    
  return 1; //ok
}

int writeMatrix(char* filename, int** matriz, int linesCount, int columnsCount)
{
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

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1){
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

int ** matrixA;         
int ** matrixB;  
int ** matrixResult;

int mAlines, mAcolumns;  
int mBlines, mBcolumns;  
int mResultlines, mResultcolumns; 

int numThreads;

void printMatrix(int** mMatrix, int lines, int columns);
int  multiplyLineColumn(int* lineValue, int* columnValue, int size);
void getLineValues(int** mat, int numColunas, int indiceLinha, int* out);
void getColumnValues(int** mat, int numLinhas, int indiceColuna, int* out);
void init(int argc, char** argv);
void* worker(void* args);


int main(int argc, char** argv)
{
	int i,j;
	pthread_t* threads;
	pthread_attr_t pthread_custom_attr;
	object* obj;

	init(argc,argv);

	//get time here
	struct timeval tvBegin, tvEnd, tvDiff;
	gettimeofday(&tvBegin, NULL);
   
	
	printf("starting...\n");	
	
	threads = (pthread_t*)malloc(numThreads*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

	obj = (object*)malloc(numThreads*sizeof(object));

	//for(i = 0; i < tamanhoAmostra; i++){		
		for(j = 0; j < numThreads; j++){
			obj[j].id = j;	
			pthread_create(&threads[j],&pthread_custom_attr,worker,(void*)&obj[j]);
		}	
		//JOIN
		for(j = 0; j < numThreads; j++){
			pthread_join(threads[j],NULL);
		}
	//}
    free(obj);
    
	//get time here
	gettimeofday(&tvEnd, NULL);    
	
	printf("finish\n");
	
	//print time diff
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n\tTime elapsed = %ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
	
    writeMatrix("out.txt",matrixResult,mResultlines,mResultcolumns);

	return 1;
}

void printMatrix(int** mMatrix, int lines, int columns){
  int i, j;

  for(i = 0; i < lines; i++){
    for(j = 0; j < columns; j++){
      printf("%d ", mMatrix[i][j]);
    }
    printf("\n");
  }
}

int multiplyLineColumn(int* lineValue, int* columnValue, int size){
  int i, result = 0;  

  for(i = 0; i < size; i++){
    result += lineValue[i]*columnValue[i];
  }

  return result;
}

void getLineValues(int** mMatrix, int columns, int index, int* values){
  int i;

  for(i = 0; i < columns; i++) {
    values[i] = mMatrix[index][i];
  }
}

void getColumnValues(int** mMatrix, int lines, int index, int* values){
  int i;

  for(i = 0; i < lines; i++){
    values[i] = mMatrix[i][index];
  }
}

void init(int argc, char** argv){
  int i; 
  
  if (argc != 2 || atoi(argv[1]) <= 0)
	exit(1);  
  
  numThreads = atoi(argv[1]);  
   
  //Read Files
  readMatrix("ini1.txt", &mAlines, &mAcolumns, &matrixA);
  readMatrix("ini2.txt", &mBlines, &mBcolumns, &matrixB);
  
  //check
  if (mAcolumns != mBlines){
    printf("Can't multiply.\n\n");
    exit(1);
  }

  mResultlines = mAlines;
  mResultcolumns = mBcolumns;
  matrixResult = (int**)malloc(mResultlines*sizeof(int*));

  for(i = 0; i < mResultlines; i++){
  	matrixResult[i] = (int*)malloc(mResultcolumns*sizeof(int));
  }
  
  if (numThreads > mResultlines){
    numThreads = mResultlines;
  }
}

void* worker(void *args)
{
  int i,j;
  object *obj=(object *)args; 
  int threadId = obj->id;

  for(i = threadId; i < mAlines; i += numThreads){    
	   
     int* threadLine = (int*) malloc(mAcolumns * sizeof(int));
     getLineValues(matrixA, mAcolumns, i, threadLine);
     
     for(j = 0; j < mBcolumns; j++){
       int* threadColumn = (int*) malloc(mBlines * sizeof(int));
       
       getColumnValues(matrixB,mBlines,j,threadColumn);
       
       matrixResult[i][j] = multiplyLineColumn(threadLine, threadColumn, mAcolumns);
     }
  }

  return (NULL);
}


