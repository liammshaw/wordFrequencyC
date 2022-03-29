#ifndef GETWORD_H
#define GETWORD_H
/* 
 *                   PROVIDED CODE FOR THE GET WORD EXERCISE
 *                          !!! DO NOT MODIFY !!!
 *
 * This function was inspired by the non-standard GNU getline function and
 * intended for use in the Word Frequency project. You may use it, as designed
 * or in modified form, in the Word Frequency project.
 *
 * The function reads a "word" from the specified file and puts it in a
 * dynamically allocated chuck of memory exactly the right size to store it.
 * The caller is responsible to free it when they are done with it.
 *
 * The definition of a word for this function is slightly different from the
 * definition that will be used in the Word Frequency project. The function
 * defines a "word" as one or more contiguous non-whitespace byte-values
 * delineated by one or more whitespace byte-values. The difference between
 * this function and the project is that the project will also require that
 * "words" contain at least one printable character. This function simply
 * indicates whether or not its "word" contain contain one or more printable
 * characters via one of its output parameters. This simplifies the function
 * a bit and it is easy for the caller to deal with the issue with simple
 * conditional logic.
 *
 * RETURN:
 *
 *    The "word", its length, and whether or not it has any printable characters
 *    are returned via one of the function's output parameters.
 *
 *    EOF is explicitly returned when end-of-file is reached, otherwise 0.
 *
 * A FEW IMPORTANT DETAILS...
 *
 *    1. Notice that the resulting "word" IS NOT a C-string, i.e., it is not
 *       nul-terminated! This is because the function is intended to read
 *       "words" from text AND binary files.
 *
 *    2. Because the "word" is not a C-string you MAY NOT USE any of the "str"
 *       functions found in string.h. Instead, you must use the "mem" functions
 *       (also found in string.h) or write your own logic.
 *
 *    3. "Words" are converted to all lowercase (use tolower in ctype.h) - this
 *       will be useful in the Word Frequency project. You can pass any byte
 *       value to tolower and it will only change uppercase letters to
 *       lowercase, all other byte values will be returned unchanged.
 *
 *    4. To avoid wasting memory the allocated memory for the "word" is exactly
 *       the amount needed for the "word". Start with a small allocation and
 *       grow it, as necessary, exponentially to achieve the logarithmic
 *       performance necessary for the long "words" that will be encountered in
 *       the Word Frequency project. Be sure to consult with your teammates,
 *       TA, or instructor if you don't understand what this means or how to do
 *       it. And, don't forget to trim the memory allocation, as necessary, 
 *       before returning it (see discussion of realloc below)!
 *
 *    5. You will find realloc very helpful for growing and shrinking memory
 *       allocations. BE AWARE THAT realloc SOMETIMES RETURNS THE SAME ADDRESS
 *       AND SOMETIMES RETURNS A NEW ADDRESS - you need to handle this
 *       correctly! See the man page for more important details.
 *
 *    6. "Do it right the first time!" For your own sanity you should always
 *       check and handle (perror, exit) any file or memory errors. The
 *       Evaluation System tests may test for such errors - to match the
 *       expected output just print the system specific error message provided
 *       by perror without any additional text.
 */

/* Need to include for use of FILE type in function prototype below */
#include <stdio.h>

/* Handy "new" type definition for this function */
typedef unsigned char Byte;

typedef struct
{
   Byte *bytes;
   unsigned length;
   int isP;
} Word;

/* Prototype of the function you must write */
int getWord(FILE *file, Byte **word, unsigned *wordLength, int *hasPrintable);

#endif
