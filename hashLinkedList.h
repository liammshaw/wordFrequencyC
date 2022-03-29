/*
 * Provided source - DO NOT MODIFY !!!
 */
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "hashTable.h"

typedef struct node{
   HTEntry *entry;
   struct node *next;
}HashNode;

/*
 * Adds the value to the front of the list. Has O(1) performance.
 * 
 * Parameter list: The head of the list, NULL if empty.
 * Parameter value: The value to add to the beginning of the list.
 *
 * Return: The head of the list.
 */ 
HashNode* addHead(HashNode *list, HTEntry *entry);

/*
 * Adds the value to the end of the list. Has O(N) performance.
 * 
 * Parameter list: The head of the list, NULL if empty.
 * Parameter value: The value to add to the end of the list.
 *
 * Return: The head of the list.
 */ 
HashNode* addTail(HashNode *list, HTEntry *entry);

/*
 * Deletes the node at the specified index. Indexes are zero-base. Reports
 * an error in the index is out-out-bounds (see reference solution behavior
 * for specific text you must match). Has O(N) performance.
 * 
 * Parameter list: The head of the list, NULL if empty.
 * Parameter index: The zero-based index of the value to remove from the list.
 *
 * Return: The head of the list.
 */ 
HashNode* deleteNode(HashNode *list, int index, HTFunctions *funcs);

#endif
