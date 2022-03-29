#include <limits.h>
#include <assert.h>
#include <string.h>
#include "hashTable.h"
#include "getWord.h"
#include "hashLinkedList.h"

typedef struct{
   HTFunctions functions;
   unsigned *sizes;
   int numSizes;
   float rehashLoadFactor;
   unsigned uniqueEntries;
   unsigned totalEntries;
   unsigned capacity;
   unsigned capacityIndex;
   HashNode **theArray;
}HashTable;

static void checkAllocation(void *mem){
   if(mem == NULL){
      fprintf(stderr, "malloc failure in %s at %d\n", __FILE__, __LINE__);
      exit(EXIT_FAILURE);
   }
}

void* htCreate(
   HTFunctions *functions,
   unsigned sizes[],
   int numSizes,
   float rehashLoadFactor)
{
   int i;
   HashTable *ht;
   assert(numSizes > 0);
   assert(rehashLoadFactor > 0);
   assert(rehashLoadFactor <= 1);
   assert(sizes[0] > 0); 
   checkAllocation(ht = malloc(sizeof(HashTable)));
   checkAllocation(ht->sizes = calloc(sizes[0], sizeof(unsigned)));
   for(i = 0; i < numSizes; i++){
      if(i != numSizes - 1)
         assert(sizes[i] < sizes[i+1]);
      ht->sizes[i] = sizes[i];
   }
   ht->functions = *functions;
   ht->capacityIndex = 0;
   ht->capacity = sizes[ht->capacityIndex];
   ht->numSizes = numSizes;
   ht->rehashLoadFactor = rehashLoadFactor;
   ht->uniqueEntries = 0;
   ht->totalEntries = 0;
   checkAllocation(ht->theArray = 
      calloc(sizes[ht->capacityIndex], sizeof(HashNode)));
   return ht;
}

static void deleteChains(HashTable *ht, int i){
   while((ht->theArray[i] = 
      deleteNode(ht->theArray[i], 0, &ht->functions)) != 0)
      continue;
}

static void htDestroyHelper(HashTable *ht)
{
   int i;
   for(i = 0; i < ht->capacity; i++)
      if(ht->theArray[i] != 0)
         deleteChains(ht, i);
   free(ht->theArray);
}

void htDestroy(void *hashTable)
{
   HashTable *ht = hashTable;
   htDestroyHelper(ht);
   free(ht->sizes);
   free(hashTable);
}

static void rehashHelper(HashTable *ht, HashNode *n, HashNode **rehashedArray){
   HashNode *temp = n;
   unsigned currentHash;
   while(n != 0)
   {
      currentHash = ht->functions.hash(n->entry->data) % 
         ht->sizes[ht->capacityIndex + 1];
      if(rehashedArray[currentHash] != 0){
         rehashedArray[currentHash] = 
            addTail(rehashedArray[currentHash], n->entry);
         n = n->next;
         free(temp);
      }
      else{
         rehashedArray[currentHash] = n;
         n = n->next;
         temp->next = NULL;
      }
      temp = n;
   }
}

static void rehash(HashTable *ht){
   unsigned i;
   HashNode **rehashedArray = 
      calloc(ht->sizes[ht->capacityIndex+1], sizeof(HashNode));
   HashNode *n;
   checkAllocation(rehashedArray);
   for(i = 0; i < ht->capacity; i ++){
      n = ht->theArray[i];
      rehashHelper(ht, n, rehashedArray);
   }
   free(ht->theArray);
   ht->theArray = rehashedArray;
   ht->capacityIndex += 1;
   ht->capacity = ht->sizes[ht->capacityIndex];
}

static unsigned duplicateEntries(HashTable *ht, HTEntry *entry, unsigned hash){
   HashNode *n;
   n = ht->theArray[hash];
   while(n != 0){
      if(!ht->functions.compare(entry->data, n->entry->data)){
         n->entry->frequency += 1;
         ht->totalEntries += 1;
         free(entry);
         return n->entry->frequency;
      }
      n = n->next;
   }
   return 0;
}

static unsigned addHelper(HashTable *ht, HTEntry *entry){
   unsigned hash, freq;
   hash = ht->functions.hash(entry->data) % ht->sizes[ht->capacityIndex];
   if(ht->theArray[hash] != 0){
      if((freq = duplicateEntries(ht, entry, hash)))
         return freq;
      ht->theArray[hash] = addTail(ht->theArray[hash], entry);
   }
   else
      ht->theArray[hash] = addHead(NULL, entry);
   ht->uniqueEntries += 1;
   ht->totalEntries += 1;
   return entry->frequency;
}

unsigned htAdd(void *hashTable, void *data)
{
   /* Excellent, an always wrong result! */
   HashTable *ht = hashTable;
   HTEntry *entry;
   checkAllocation(entry = calloc(1, sizeof(HTEntry)));
   assert(data != NULL);
   if(ht->rehashLoadFactor < 1 && ht->capacityIndex < ht->numSizes-1 
      && (((float)ht->uniqueEntries/ht->capacity) > ht->rehashLoadFactor))
      rehash(ht);
   entry->data = data;
   entry->frequency = 1;
   return addHelper(ht, entry);
}

HTEntry htLookupHelper(HashTable* ht, void *data, HashNode *n, HTEntry entry){
   if(n == 0)
      return entry;
   if(0 == ht->functions.compare(n->entry->data, data))
      return *n->entry;
   return htLookupHelper(ht, data, n->next, entry);
}

HTEntry htLookUp(void *hashTable, void *data){
   HashTable *ht = hashTable;
   HTEntry entry;
   HashNode *n;
   unsigned hash;
   assert(data != NULL);
   hash = ht->functions.hash(data) % ht->sizes[ht->capacityIndex];
   entry.data = NULL;
   entry.frequency = 0;
   n = ht->theArray[hash];
   return htLookupHelper(ht, data, n, entry);
}

HTEntry* htToArray(void *hashTable, unsigned *size)
{
   /* All values possible but UINT_MAX is unlikely and recognizable! */
   HTEntry *htentryarray;
   HashNode *n;
   HashTable *ht = hashTable;
   int i;
   *size = 0;
   if(ht->uniqueEntries == 0){
      return NULL;
   }

   checkAllocation(htentryarray = calloc(ht->uniqueEntries, sizeof(HTEntry)));

   for(i = 0; i < ht->capacity; i++){
      n = ht->theArray[i];
      while(n != 0){
         htentryarray[*size] = *n->entry;
         (*size)++;
         n = n->next;
      }
   }

   /* NULL is a possible correct result, but only when size is zero */
   return htentryarray;
}

unsigned htCapacity(void *hashTable)
{
   /* Always incorrect - htCreate does does not allow a size of zero. */
   HashTable *ht = hashTable;
   return ht->capacity;
}

unsigned htUniqueEntries(void *hashTable)
{
   /* All values possible but UNIT_MAX is unlikely and recognizable! */
   HashTable *ht = hashTable;
   return ht->uniqueEntries;
}

unsigned htTotalEntries(void *hashTable)
{
   /* All values possible but UNIT_MAX is unlikely and recognizable! */
   HashTable *ht = hashTable; 
   return ht->totalEntries;
}

static unsigned countLargestChain(HashNode *n){
   if(n == 0)
      return 0;
   return 1 + countLargestChain(n->next);
}

HTMetrics htMetrics(void *hashTable)
{
   HTMetrics metrics;
   HashTable *ht = hashTable;
   HashNode *n;
   int i;
   unsigned currentLength;
   metrics.numberOfChains = metrics.avgChainLength 
      = metrics.maxChainLength = currentLength =0;

   for(i = 0; i < ht->capacity; i++){
      n = ht->theArray[i];
      if(n != 0)
         metrics.numberOfChains += 1;
      currentLength = countLargestChain(n);
      if(metrics.maxChainLength < currentLength)
         metrics.maxChainLength = currentLength;
   }
   if(metrics.numberOfChains)
      metrics.avgChainLength = 
         (float)ht->uniqueEntries / metrics.numberOfChains;

   return metrics;
}
