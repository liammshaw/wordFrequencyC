/*
 * Provided - DO NOT MODIFY!!! 
 */
#ifndef QSORTHTENTRIES_H
#define QSORTHTENTRIES_H
#include "hashTable.h"
#include "getWord.h"

/* Prototype of the function you must write for this exercise. 
 *
 * This function uses the C Standard Library qsort function to sort the
 * provided array HTEntry structures.
 *
 * Note that you will also need to write a compare function to provide to
 * qsort - "man qsort" for details - that you will be calling in this
 * function.
 */
void qsortHTEntries(HTEntry *entries, int numberOfEntries);

int compareWords(Word *word1, Word *word2);

#endif
