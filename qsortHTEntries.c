#include <stdlib.h>
#include <stdio.h>
#include "hashTable.h"
#include "getWord.h"
#include "qsortHTEntries.h"

#define GREATER_THAN 1;
#define LESS_THAN -1;

static Word* smallerWord(Word *w1, Word *w2){
   if(w1->length > w2->length)
      return w2;
   return w1;
}

int compareWords(Word *word1, Word *word2){
   unsigned i;
   Word *sW;
   sW = smallerWord(word1, word2);
   for(i = 0; i < sW->length; i++){
      if(word1->bytes[i] < word2->bytes[i])
         return LESS_THAN;
      if(word1->bytes[i] > word2->bytes[i])
         return GREATER_THAN;
   }
   if(word1->length == word2->length)
      return 0;
   if(sW->length == word1->length)
      return LESS_THAN;
   return GREATER_THAN;
}

int compareHTEntry(const void *entry1, const void *entry2){
   if(((HTEntry*)entry1)->frequency > ((HTEntry*)entry2)->frequency)
      return LESS_THAN;
   if(((HTEntry*)entry1)->frequency < ((HTEntry*)entry2)->frequency)
      return GREATER_THAN;
   return compareWords(((HTEntry*)entry1)->data, ((HTEntry*)entry2)->data);
}

void qsortHTEntries(HTEntry *entries, int numberOfEntries){
   qsort(entries, numberOfEntries, sizeof(HTEntry), compareHTEntry);
}
