#include <stdlib.h>
#include <stdio.h>
#include "hashLinkedList.h"
#include "hashTable.h"

HashNode* addHead(HashNode *list, HTEntry *entry){
   HashNode *head;
   head = calloc(1, sizeof(HashNode));
   if (head == NULL){
      fprintf(stderr, "malloc failure\n");
      exit(EXIT_FAILURE);
   }
   head->entry = entry;
   head->next = list;
   return head;
}

HashNode* addTail(HashNode *list, HTEntry *entry){
   HashNode *tail;
   HashNode *start = list;
   tail = calloc(1, sizeof(HashNode));
   if (tail == NULL){
      fprintf(stderr, "malloc failure");
      exit(EXIT_FAILURE);
   }
   tail->entry = entry;
   tail->next = NULL;
   if(list == NULL)
   {
      tail->next = list;
      return tail;
   }
   while(list->next != NULL)
      list = list->next;
   list->next=tail;
   return start;
}

HashNode* deleteNode(HashNode *list, int index, HTFunctions *funcs){
   HashNode *current = list;
   HashNode *previous;
   int count = 0;
   while(current != NULL && count != index){
      previous = current;
      current = current->next;
      ++count;
   }
   if(index == 0 && current != NULL)
      list = current->next;
   else
      previous->next = current->next;
   if(current->entry != 0 && funcs->destroy != NULL)
      funcs->destroy(current->entry->data);
   free(current->entry->data);
   free(current->entry);
   free(current);
   return list;
}
