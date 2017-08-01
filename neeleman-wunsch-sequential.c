#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int max(int num1, int num2);
char *substring(char *string, int position, int length);
char *charStrConcat(char c, char *str);

int main(int argc, char *argv[]){

  //input file name and sequence length
  char *fileName;
  int numOfChars;

  //command-line arguments
  if(argc < 2){
    printf("Too few arguments\n");
    exit(1);
  } else {
    fileName = argv[1];
    numOfChars = atol(argv[2]) +  atol(argv[2);
  }

  //allocate memory for sequences
  char* refSeq = (char*) malloc(sizeof(char *) * numOfChars);
  char* alignSeq = (char*) malloc(sizeof(char *) * numOfChars);

  //open file and get the sequences
  FILE* fp;
  fp = fopen(fileName, "r");
  fgets(refSeq, numOfChars, (FILE*) fp);
  fgets(alignSeq, numOfChars, (FILE*) fp);
  fclose(fp);

  int refSeqCnt = strlen(refSeq) + 1;
  int alignSeqCnt = strlen(alignSeq) + 1;
  int scoringMatrix [alignSeqCnt][refSeqCnt];

  int scoreLeft, scoreUp;
  double elapsTime;
  clock_t start,end;

  //get start time
  start = clock();

  //initialization
  for (int i = 0; i < alignSeqCnt; i++){
    for (int j = 0; j < refSeqCnt; j++){
      scoringMatrix[i][j] = 0;
    }
  }

  //fill the matrix
  for (int i = 1; i < alignSeqCnt; i++){
    for (int j = 1; j < refSeqCnt; j++){
      int scoreDiag = 0;
      char *sub1,*sub2;
      char c1 = refSeq[j-1];
      char c2 = alignSeq[i-1];
      if (c1 == c2){
        scoreDiag = scoringMatrix[i - 1][j - 1] + 2;
      } else {
        scoreDiag = scoringMatrix[i - 1][j - 1] + -1;
      }

      scoreLeft = scoringMatrix[i][j - 1] - 2;
      scoreUp = scoringMatrix[i - 1][j] - 2;

      int maxScore = max(max(scoreDiag, scoreLeft), scoreUp);
      scoringMatrix[i][j] = maxScore;
    }
  }

  //traceback
  char *AlignmentA=" ";
  char *AlignmentB=" ";

  int m = alignSeqCnt - 1;
  int n = refSeqCnt - 1;

  while (m > 0 || n > 0){
    int scoreDiag = 0;
    if (m == 0 && n > 0){
      AlignmentA = charStrConcat(refSeq[n-1],AlignmentA);
      AlignmentB = charStrConcat('-',AlignmentB);
      n = n - 1;
    } else if (n == 0 && m > 0){
      AlignmentA = charStrConcat('-',AlignmentA);
      AlignmentB = charStrConcat(alignSeq[m-1],AlignmentB);
      m = m - 1;
    } else {
      if (alignSeq[m - 1] == refSeq[n - 1]){
        scoreDiag = 2;
      } else {
        scoreDiag = -1;
      }

      if (m > 0 && n > 0 && scoringMatrix[m][n] == scoringMatrix[m - 1][n - 1] + scoreDiag){
        AlignmentA = charStrConcat(refSeq[n-1],AlignmentA);
        AlignmentB = charStrConcat(alignSeq[m-1],AlignmentB);
        m = m - 1;
        n = n - 1;
      } else if (n > 0 && scoringMatrix[m][n] == scoringMatrix[m][n - 1] - 2) {
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
  printf("\n\nTotal Time(in Milliseconds) = %lf\n\n",elapsTime * 1000);

  printf("%s\n", AlignmentA);
  printf("%s\n", AlignmentB);
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
