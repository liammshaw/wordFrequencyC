#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qsortHTEntries.h"
#include "hashTable.h"
#include "getWord.h"
#include <ctype.h>

void exitWrongArguments(){
   fprintf(stderr, "Usage: wf [-nX] [file...]\n");
   exit(EXIT_FAILURE);
}

void checkValidFile(char *argv[], int i, FILE *fp)
{
   if(fp == NULL){
      fprintf(stderr, "wf: ");
      perror(argv[i]);
      exit(EXIT_FAILURE);
   }
}

static void checkAllocation(void *mem){
   if(mem == NULL){
      fprintf(stderr, "malloc failure in %s at %d\n", __FILE__, __LINE__);
      exit(EXIT_FAILURE);
   }
}

static int compareHTWords(const void *word1, const void *word2){
   return compareWords((Word*)word1, (Word*)word2);
}

static unsigned hashString(const void *data)
{
   unsigned hash;
   const char *str = (char*)((Word*)data)->bytes;

   for (hash = 0; *str;  str++)
      hash = *str + 31 * hash;

   /* Return the raw hash value - the modulo must be done by caller */
   return hash;
}

static void destroyWord(const void *word){
   free(((Word*)word)->bytes);
}

unsigned findNValue(char *argv[], unsigned indexOfFlag){
   unsigned nValue, i;
   char c;
   nValue = 0;
   for(i = 2; i < strlen(argv[indexOfFlag]); i++){
      c = argv[indexOfFlag][i];
      if(c >= '0' && c <= '9')
         nValue = nValue*10 + (c - '0');
      else
         exitWrongArguments();
   }
   return nValue;
}

int checkArgs(unsigned i, char *argv[], unsigned *flags){
   if(argv[i][0] == '-'){
      if(argv[i][1] == 'n' && argv[i][2] > '0' && argv[i][2] <= '9'){
         *flags += 1;
         return findNValue(argv, i);
      }
      exitWrongArguments();
   }
   return 0;
}

static void printWord(const Byte *word, unsigned wordLength, int hasPrintable)
{
   int i;
   /* For readability, this program limits each line to 30 characters max */
   for (i = 0; i < wordLength && i < 30; i++)
   {
      if (isprint(word[i]))
         printf("%c", word[i]);
      else
         printf("%c", '.');
   }
   
   /* Indicate the word is longer that was actually displayed */
   if (wordLength > 30)
      printf("...");

   printf("\n");
}

static void printWords(void* ht, unsigned nValue){
   unsigned size, i;
   HTEntry* htArray = htToArray(ht, &size);
   qsortHTEntries(htArray, htUniqueEntries(ht));
   if(nValue)
      size = nValue;
   else if(size > 10)
      size = 10;
   printf("%d unique words found in %d total words\n", 
      htUniqueEntries(ht), htTotalEntries(ht));
   for(i = 0; i < size && i < htUniqueEntries(ht); i++){
      printf("%10u - ", htArray[i].frequency);
      printWord((Byte*)((Word*)htArray[i].data)->bytes, 
         ((Word*)htArray[i].data)->length, ((Word*)htArray[i].data)->isP);
   }
   free(htArray);
}

static void countWordsHelper(
   Byte **word, 
   unsigned *wordLength, 
   int *isPrintable, 
   void *ht)
{
   unsigned freq;
   Word *htWord;
   checkAllocation(htWord = malloc(sizeof(Word)));
   htWord->bytes = *word;
   htWord->length = *wordLength;
   htWord->isP = *isPrintable;
   if(!*wordLength || !*isPrintable){
      free(*word);
      free(htWord);
      return;
   }
   else
      freq = htAdd(ht, htWord);
   if(freq > 1){
      free(*word);
      free(htWord);
   }
}

void countWords(FILE *fp, void *ht){
   Byte *word;
   unsigned wordLength;
   int isPrintable;

   while(getWord(fp, &word, &wordLength, &isPrintable) != EOF){
      countWordsHelper(&word, &wordLength, &isPrintable, ht);
   }
   if(wordLength){
      countWordsHelper(&word, &wordLength, &isPrintable, ht);
   }
}

void runWithFile(int argc, char *argv[]){
   unsigned i, nValue, flags;
   FILE *fp;
   unsigned sizes[] = {11, 23, 47, 89, 191, 401, 853, 1709, 3467, 7919};
   HTFunctions funcs = {hashString, compareHTWords, destroyWord};
   void *ht = htCreate(&funcs, sizes, 10, 0.75);
   nValue = 0;
   for(i = 1; i < argc; i++){
      if((nValue = checkArgs(i, argv, &flags))){
         continue;
      }
      fp = fopen(argv[i], "r");
      checkValidFile(argv, i, fp);
      countWords(fp, ht);
      fclose(fp);
   }
   printWords(ht, nValue);
   htDestroy(ht);
}

void runSTOUT(unsigned nValue){
   FILE *fp = stdin;
   unsigned sizes[] = {11, 23, 47, 89, 191, 401, 853, 1709, 3467, 7919};
   HTFunctions funcs = {hashString, compareHTWords, destroyWord};
   void *ht = htCreate(&funcs, sizes, 10, 0.75);
   countWords(fp, ht);
   printWords(ht, nValue);
   htDestroy(ht);
}

int main(int argc, char *argv[]){
   unsigned nValue, flags = 0, i;
   for(i = 0; i < argc; i++){
      nValue = checkArgs(i, argv, &flags);
   }
   if(argc - flags == 1)
      runSTOUT(nValue);
   else
      runWithFile(argc, argv);
   return 0;
}
