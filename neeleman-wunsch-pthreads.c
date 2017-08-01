#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <string.h>

int max(int num1, int num2);
char *substring(char *string, int position, int length);
char *charStrConcat(char c, char *str);
int maxScore(int *scoringMatrix,char *refSeq,char *alignSeq, int i,int j,int refSeqCnt);

int numOfThrs = 2,refSeqCnt,alignSeqCnt;
pthread_mutex_t mutex;
pthread_barrier_t barrier;
char *fileName;
int numOfChars;
char *refSeq,*alignSeq;
int *scoringMatrix;

void *thrFun(void *arg){

  int thrNo,i,j;

  //get thread number
  thrNo = *((int *) arg);
  int p0i,p0j,p1i,p1j;
  p0i = 1;
  p1j = 1;

  //fill the matrix
  for(int k = 0; k < numOfChars; k++){
    if(thrNo == 0){
      for (j = p0i; j < refSeqCnt; j++){
        *(scoringMatrix + (p0i)* refSeqCnt + (j)) = maxScore(scoringMatrix,refSeq,alignSeq,p0i,j,refSeqCnt);
      }
      p0i++;
    }
    if(thrNo == 1){
      for (i = p1j; i < alignSeqCnt; i++){
        *(scoringMatrix + (i)* refSeqCnt + (p1j)) = maxScore(scoringMatrix,refSeq,alignSeq,i,p1j,refSeqCnt);
      }
      p1j++;
    }
    pthread_barrier_wait(&barrier);
  }
  pthread_exit(NULL);
}


int main(int argc, char *argv[]){

  pthread_t *threads;
  int *args,i,j;
  clock_t start,end;
  double elapsTime;

  //command-line arguments
  if(argc < 2){
    printf("Too few arguments\n");
    exit(1);
  } else {
    fileName = argv[1];
    numOfChars = atol(argv[2]) + atol(argv[2]);
  }

  //allocate memory
  refSeq = (char*) malloc(sizeof(char *) * numOfChars);
  alignSeq = (char*) malloc(sizeof(char *) * numOfChars);

  //open file and get the sequences
  FILE* fp;
  fp = fopen(fileName, "r");
  fgets(refSeq, numOfChars, (FILE*) fp);
  fgets(alignSeq, numOfChars, (FILE*) fp);

  refSeqCnt = strlen(refSeq) + 1;
  alignSeqCnt = strlen(alignSeq) + 1;

  //allocate memory
  scoringMatrix = (int *)malloc(sizeof(int *) * refSeqCnt * alignSeqCnt);
  threads = malloc(numOfThrs * sizeof(pthread_t));
  args = malloc(numOfThrs * sizeof(int));

  //initialization
  for (i = 0; i < alignSeqCnt; i++)
  {
    for (j = 0; j < refSeqCnt; j++)
    {
      *(scoringMatrix + i * refSeqCnt + j) = 0;
    }
  }

  //get start time
  start = clock();

  //intitalize lock and barrier
  pthread_mutex_init(&mutex, NULL);
  pthread_barrier_init(&barrier, NULL, numOfThrs);

  //create threads
  for(i = 0;i < numOfThrs;i++){
    args[i] = i;
    int res = pthread_create(&threads[i], NULL, thrFun, &args[i]);
    if(res != 0){
      printf("Error while creating threads.\n");
      exit(1);
    }
  }

  //join threads
  for(i = 0;i < numOfThrs;i++){
    int res = pthread_join(threads[i], NULL);
    if(res != 0){
      printf("Error while joining threads.\n");
      exit(1);
    }
  }

  //traceback
  char *AlignmentA=" ";
  char *AlignmentB=" ";

  int m = alignSeqCnt - 1;
  int n = refSeqCnt - 1;

  while (m > 0 || n > 0)
  {
    int scoreDiag = 0;
    if (m == 0 && n > 0){
      AlignmentA = charStrConcat(refSeq[n-1],AlignmentA);
      AlignmentB = charStrConcat('-',AlignmentB);
      n = n - 1;
    }
    else if (n == 0 && m > 0){
      AlignmentA = charStrConcat('-',AlignmentA);
      AlignmentB = charStrConcat(alignSeq[m-1],AlignmentB);
      m = m - 1;
    } else {

      if (alignSeq[m - 1] == refSeq[n - 1]){
        scoreDiag = 2;
      } else {
        scoreDiag = -1;
      }

      if (m > 0 && n > 0
        && *(scoringMatrix + m * refSeqCnt + n) == *(scoringMatrix + (m-1) * refSeqCnt + (n-1)) + scoreDiag){
          AlignmentA = charStrConcat(refSeq[n-1],AlignmentA);
          AlignmentB = charStrConcat(alignSeq[m-1],AlignmentB);
          m = m - 1;
          n = n - 1;
        } else if (n > 0 && *(scoringMatrix + m * refSeqCnt + n) == *(scoringMatrix + (m) * refSeqCnt + (n-1)) - 2)
        {
          AlignmentA = charStrConcat(refSeq[n-1],AlignmentA);
          AlignmentB = charStrConcat('-',AlignmentB);
          n = n - 1;
        } else {
          AlignmentA = charStrConcat('-',AlignmentA);
          AlignmentB = charStrConcat(alignSeq[m-1],AlignmentB);
          m = m - 1;
        }
      }
    }

    //get end time
    end = clock();

    //calculate total time
    elapsTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n\nTotal Time(in Milliseconds) = %lf\n\n",
    elapsTime * 1000);
    printf("%s\n", AlignmentA);
    printf("%s\n", AlignmentB);

    //free memory
    free(scoringMatrix);
    free(threads);
    free(args);

    return 0;
  }

  int max(int num1, int num2) {
    return num1 > num2 ? num1 : num2;
  }

  char *substring(char *string, int position, int length){
    char *pointer;
    int c;
    pointer = malloc(length+1);

    if (pointer == NULL){
      printf("Unable to allocate memory.\n");
      exit(1);
    }

    for (c = 0 ; c < length ; c++){
      *(pointer+c) = *(string+position-1);
      string++;
    }

    *(pointer+c) = '\0';
    return pointer;
  }

  char *charStrConcat(char c, char *str){
    char *tempStr;
    char charStr[2];
    tempStr = (char*) malloc(sizeof(char *)* (strlen(str)));
    strcpy(tempStr,str);
    str = (char *) malloc(sizeof(char *) * (strlen(str)+1));
    charStr[0] = c;
    charStr[1] = '\0';
    strcpy(str, charStr);
    strcat(str,tempStr);
    return str;
  }

  int maxScore(int *scoringMatrix,char *refSeq,char *alignSeq, int i,int j,int refSeqCnt){
    int scoreDiag = 0;
    int scoreLeft, scoreUp;
    char *sub1,*sub2;
    char c1 = refSeq[j-1];
    char c2 = alignSeq[i-1];
    if (c1 == c2){
      scoreDiag = *(scoringMatrix + (i-1)* refSeqCnt + (j-1)) + 2;
    } else {
      scoreDiag = *(scoringMatrix + (i-1)* refSeqCnt + (j-1)) + -1;
    }
    scoreLeft = *(scoringMatrix + (i)* refSeqCnt + (j-1)) - 2;
    scoreUp = *(scoringMatrix + (i-1)* refSeqCnt + (j)) - 2;
    return max(max(scoreDiag, scoreLeft), scoreUp);
  }
