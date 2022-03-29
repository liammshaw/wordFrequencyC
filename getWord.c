#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "getWord.h"

#define AVERAGE_WORD_LENGTH 5

void checkAllocation(void *mem){
   if(NULL == mem)
   {
      fprintf(stderr, "malloc error\n");
      exit(EXIT_FAILURE);
   }
}

int getWord(FILE *file, Byte **word, unsigned *wordLength, int *hasPrintable){
   int c;
   unsigned currentAllocation = AVERAGE_WORD_LENGTH;
   *wordLength = *hasPrintable = 0;
   checkAllocation(*word = malloc(AVERAGE_WORD_LENGTH));
   fscanf(file, " ");
   while((c = tolower(fgetc(file))) != EOF && !isspace(c)){
      if(*wordLength >= currentAllocation){
         currentAllocation *= 2;
         checkAllocation(*word = realloc(*word, currentAllocation));
      }
      if(isprint(c))
         *hasPrintable = 1;
      (*word)[*wordLength] = c;
      *wordLength += 1;
   }
   if(*wordLength != 0){
      checkAllocation(*word = realloc(*word, *wordLength + 1));
      (*word)[*wordLength] = '\0';
   }
   else
      *word = realloc(*word, 0);
   if(feof(file))
      return EOF;
   return 0;
}