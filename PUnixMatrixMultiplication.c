#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/unistd.h>

int ** matrixA;         
int ** matrixB;  
int * matrixResult;

int mAlines, mAcolumns;  
int mBlines, mBcolumns;  
int mResultlines, mResultcolumns; 

int numUnixProcess;

int readMatrix(const char *filename, int *linesCount, int *columnsCount, int ***resultMatrix);
int writeMatrix(char* filename, int* matriz, int linesCount, int columnsCount);
void printMatrix(int** mMatrix, int lines, int columns);
int  multiplyLineColumn(int* lineValue, int* columnValue, int size);
void getLineValues(int** mat, int numColunas, int indiceLinha, int* out);
void getColumnValues(int** mat, int numLinhas, int indiceColuna, int* out);
void init(int argc, char** argv);
void UnixProcessFunction(int processId);
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);


int main(int argc, char** argv){
	
	int i, status;
	pid_t forkId;
	pid_t * processId;
	

	init(argc,argv);


    //int j;
    //for(j=0; j < 10; j++){

	//get time here
	struct timeval tvBegin, tvEnd, tvDiff;
	
	
	
	processId = (pid_t *)malloc(sizeof(pid_t) * (numUnixProcess - 1));
	if(NULL == processId){
		printf("error");
		exit(1);
	}
	
	gettimeofday(&tvBegin, NULL);	
	printf("starting...\n");
	////////////////////////////////////////////////////////////////////
	for(i = 1; i < numUnixProcess; i++){
			forkId = fork();
			
			if(forkId != 0){
				//processo pai
				UnixProcessFunction(0);
			}else{
				//processo forked
				UnixProcessFunction(i);
				exit(0);
			}
			//printf("\nProcesso %d\n", forkId);
			processId[i-1] = forkId;			
	}
	
	waitpid(forkId, &status, 0);
	
	////////////////////////////////////////////////////////////////////
	//get time here
	gettimeofday(&tvEnd, NULL);  
	
	printf("finish\n");		
	
	//print time diff
	timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    printf("\n\tTime elapsed = %ld.%06ld\n", tvDiff.tv_sec, tvDiff.tv_usec);
	//}
    
    writeMatrix("out.txt",matrixResult,mResultlines,mResultcolumns);

	return 1;
}

int readMatrix(const char *filename, int *linesCount, int *columnsCount, int ***resultMatrix){
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

int writeMatrix(char* filename, int* matriz, int linesCount, int columnsCount){
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
			fprintf(outputFile, "%d ", matriz[i * columnsCount + j]);
		}
		fprintf(outputFile, "%d\n", matriz[i * columnsCount + columnsCount - 1]);
	}

	fclose(outputFile);
	return 0;
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
  
  if (argc != 2 || atoi(argv[1]) <= 0)
	exit(1);  
  
  numUnixProcess = atoi(argv[1]);  
   
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

  matrixResult = (int*) mmap(0, mResultlines * mResultcolumns * sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);
  
  if (numUnixProcess > mResultlines){
    numUnixProcess = mResultlines;
  }
}

void UnixProcessFunction(int processId){
  int i,j;

  for(i = processId; i < mAlines; i += numUnixProcess){    
	   
     int* processLine = (int*) malloc(mAcolumns * sizeof(int));
     getLineValues(matrixA, mAcolumns, i, processLine);
     
     for(j = 0; j < mBcolumns; j++){
       int* processColumn = (int*) malloc(mBlines * sizeof(int));
       
       getColumnValues(matrixB,mBlines,j,processColumn);
       
       matrixResult[i * mResultcolumns + j] = multiplyLineColumn(processLine, processColumn, mAcolumns);
     }
  }
}

int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1){
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}
