/* This header file is provided with the hash table project and the associated
 * unit testing exercise for the same project.
 *
 *    ! ! !  DO NOT MODIFY THIS FILE ! ! !
 *
 * An unmodified version of this file will be used during evaluation even if
 * you submit a different one. If you need additional header file features you
 * must create your own separate header file and include it with your
 * submission.
 */
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>

/* The hash table entry structure returned by htLookUp and htToArray.
 */
typedef struct
{
   void *data;
   unsigned frequency;
} HTEntry;

/* The hash table metric structure returned by htMetrics.
 */
typedef struct
{
   unsigned numberOfChains;
   unsigned maxChainLength;
   float avgChainLength;
} HTMetrics;

/* Function types for the data-specific functions the user of the hash table
 * is required to provide to the hash table's htCreate function.
 *
 *    FNHash: Required. Calculates and returns the hash value (not an index!)
 *       for the specified data.
 *
 *    FNCompare: Required. Compares two data items to determine data1 is
 *       less-than, equal-to, or greater than data2. The function returns
 *       a negative, zero, or positive value, respectively.
 *
 *    FNDestroy: Optional (may be NULL). This function must be provided
 *       (not NULL) when the data being stored in the hash table itself has
 *       additional dynamically allocated memory (sub-allocations). Because
 *       only the user of the hash table knows the details about the data being
 *       stored in the hash table, only the user would know about any such
 *       sub-allocations. When such sub-allocations exist, the user of the
 *       hash table MUST provide a function to free them. When they do not
 *       exist this function is NOT necessary.
 *
 *       IMPORTANT: This function MUST NEVER free the data added to the hash
 *                  table via htAdd - the hash table's htDestroy function
 *                  is responsible for freeing that data and calling free
 *                  more than once on the same allocation results in an ABORT
 *                  signal and terminates the program.
 */
typedef unsigned (*FNHash)(const void *data);
typedef int (*FNCompare)(const void *data1, const void *data2);
typedef void (*FNDestroy)(const void *data);

/* Structure of function pointers provided to and used by the hash table.
 */
typedef struct
{
   FNHash hash;
   FNCompare compare;
   FNDestroy destroy;
} HTFunctions;

/* Description: Creates a new hash table as specified.
 *
 * Notes:
 *   1. The function asserts (man 3 assert) if numSizes is not 1 or more.
 *   2. The function asserts (man 3 assert) if any of the sizes are not
 *      greater than the immediately preceding size.
 *   3. The function asserts (man 3 assert) If the load factor is not greater
 *      than 0.0 and less than or equal to 1.0.
 *   4. You must make a deep copy of the _functions_ pointers and _sizes_
 *      array since you don't know where the caller allocated them - they
 *      could be on the stack and you don't want to point there - that's a
 *      potential and likely _unlucky bug_ waiting to happen!
 *
 * Parameters:
 *    functions: A structure of function pointers to the data-specific
 *       functions required by the hash table.
 *    sizes: An array of hash table sizes. Must always be at least one size
 *       greater than zero specified, more if rehashing is desired. Prime
 *       numbers are best but not required/checked for. That being said, you
 *       should use prime numbers!
 *    numSizes: The number of values in the sizes array.
 *    rehashLoadFactor: The load factor to rehash at. Should be a value greater
 *       than 0.0 and less than or equal to 1.0. A value of 1.0 means
 *       "do not rehash".
 *
 * Return: A pointer to an anonymous (file-local) structure representing a
 *         hash table.
 */
void* htCreate(
   HTFunctions *functions,
   unsigned sizes[],
   int numSizes,
   float rehashLoadFactor 
); 

/* Description: Frees all of the dynamically allocated memory allocated by the
 *    hash table itself AS WELL AS all of the data object added to the hash
 *    table by the user via htAdd.
 *
 *    IMPORTANT: When an FNDestroy function was provided to htCreate, i.e.,
 *               FNDestroy is not NULL, this function (htDestroy) MUST call the
 *               provided FNDestroy function and pass each data object added to
 *               the hash table by the user via htAdd PRIOR to freeing that
 *               object. 
 *
 *               The htDestroy function, and ONLY the htDestroy function, calls
 *               free on the data added to the hash table via htAdd AND ALWAYS
 *               calls it!
 *
 *               Be sure to test htDestroy with and without an FNDestroy
 *               function and be sure to include data with a sub-allocation
 *               (maybe a structure with a pointer to some dynamically
 *               allocated memory?) to be sure both situations work correctly!
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *
 * Return: None
 */
void htDestroy(void *hashTable);

/* Description: Adds a shallow copy of the data to the hash table. The data
 *    being added MUST BE dynamically allocated because it will be freed
 *    when the hash table is destroyed!
 * 
 * Notes:
 *    1. The function is expected to have O(1) performance.
 *    2. The function asserts (man 3 assert) if data is NULL. 
 *    3. The function rehashes to the next size when:
 *          1. The rehash load factor is NOT 1.0, a value of 1.0 indicates
 *             rehashing is not desired.
 *          2. And there is a next size. Otherwise continue with the current
 *             size.
 *          3. And the ratio of unique entries TO the current hash table size
 *             (BEFORE adding the new data) exceeds the rehash load factor.
 *    4. When the data being added is a duplicate, the original entry is kept
 *       in the hash table AND the caller is responsible for freeing the
 *       duplicate.
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *    data: The data to add.
 *
 * Return: The frequency of the data in the hash table. A value of 1 means it
 *    is a new and unique entry, values greater than 1 mean it is a duplicate
 *    with the indicated frequency.
 */
unsigned htAdd(void *hashTable, void *data);

/* Description: Determines if the data is in the hash table or not.
 * 
 * Notes:
 *    1. The function is expected to have O(1) performance.
 *    2. The function asserts (man 3 assert) if data is NULL. 
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *    data: The data to look for.
 *
 * Return: Returns an HTEntry with a shallow copy of the data and its
 *    frequency if found, otherwise NULL data and frequency 0.
 */
HTEntry htLookUp(void *hashTable, void *data);

/* Description: Returns a dynamically allocated array with shallow copies of
 * all of the hash table entries.
 *
 * Notes:
 *    1. The caller is responsible for freeing the memory for the returned
 *       array BUT MUST NOT free the data in each HTEntry in the array - that
 *       memory is still in use by the hash table! To free the data call
 *       htDestroy when you are completely done with the hash table AND all
 *       arrays returned by this function.
 * 
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *    size: Output parameter updated with the array's size.
 *
 * Return: A dynamically allocated array with all of the hash table entries or
 *    NULL if the hash table is empty (note that free can be called on NULL).
 */
HTEntry* htToArray(void *hashTable, unsigned *size);

/* Description: Reports the current capacity of the hash table.
 * 
 * Notes:
 *    1. The function is expected to have O(1) performance.
 *    2. This function should always return one of the sizes passed to htCreate.
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *
 * Return: The current capacity of the hash table.
 */
unsigned htCapacity(void *hashTable);

/* Description: Returns the number of unique entries in the hash table.
 * 
 * Notes: 
 *    1. The function is expected to have O(1) performance which implies it must
 *       be stored and maintained by the hash table as data is added - NOT
 *       calculated when this function is called.
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *
 * Return: The number of unique entries in the hash table.
 */
unsigned htUniqueEntries(void *hashTable);

/* Description: Returns the total number of data objects added to the hash
 *    table which is equivalent to the sum of the frequencies of all entries
 *    in the hash table (see Notes, below, for important information about
 *    the expected performance of this function).
 * 
 * Notes: 
 *    1. The function is expected to have O(1) performance which implies it must
 *       be stored and maintained by the hash table as data is added - NOT
 *       calculated when this function is called.
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *
 * Return: The sum of the frequencies of all entries in the hash table.
 */
unsigned htTotalEntries(void *hashTable);

/* Description: Returns various metrics on the hash table to aid in performance
 *    tuning of CPU and memory usage for a particular problem domain.
 * 
 * Notes:
 *    1. Because this function will only be used for performance tuning and not
 *       during actual use of the hash table it DOES NOT need to have O(1)
 *       performance. FYI: O(N) performance is expected.
 *    2. The number of chains is a count of the non-empty chains in the
 *       hash table.
 *    3. The maximum chain length is the length of the longest chain in the
 *       hash table.
 *    4. The average chain length is the sum of the length of all chains
 *       in the hash table divided by the number of non-zero length chains.
 *
 * Parameters:
 *    hashTable: A pointer returned by htCreate.
 *
 * Return: An HTMetric struct with the current metrics for the specified hash
 *    table.
 */
HTMetrics htMetrics(void *hashTable);

#endif
