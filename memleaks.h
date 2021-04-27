/**
 * Copyright (c) 2021 Eviatar
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#ifndef MEMORY_LEAKS_H
#define MEMORY_LEAKS_H

/* Checks if the compiler is Ansi C. */
#ifndef __STDC_VERSION__
# define ML_C_VER90
#endif
/* ------------------------------------------------------------------------------------ */

/* Nessecary headers. */
#include <stdio.h>
#include <stdlib.h>

#ifndef ML_C_VER90
# include <stdint.h>
# include <stdbool.h>
#endif
/* ------------------------------------------------------------------------------------ */

/* Type definitions for Ansi C */
#if defined(ML_C_VER90)
# ifndef bool
#  bool  char
#  true  1
#  false 0
# endif
 typedef unsigned long ml_size_t;
#else
 typedef size_t ml_size_t;
#endif
/* ------------------------------------------------------------------------------------ */

/* Holds all of the necessary information about the memory allocation. */
typedef struct {
    const void*     p_ptr;
    const char*     p_file;
    ml_size_t       line;
} MemoryInfo, *pMemoryInfo;
/* ------------------------------------------------------------------------------------ */

/* Imitate the behaviour of std::vector. */
typedef struct {
    ml_size_t       max_size;
    ml_size_t       index;
    pMemoryInfo     p_data;
} MemoryInfoVector, *pMemoryInfoVector;

extern void  _ml_vector_init(); 
extern void* _ml_vector_push_data(void* ptr, const char* p_file, ml_size_t line);
extern void  _ml_vector_first_remove_data(const void* ptr);

/* ------------------------------------------------------------------------------------ */

/* Printing all of the leaks at the end of program. */
extern void _ml_exit_message();
/* ------------------------------------------------------------------------------------ */

#define INIT_MEMLEAKS              \
    do {                           \
        _ml_vector_init();         \
        atexit(&_ml_exit_message); \
    } while(false)

#define malloc(bytes) \
    _ml_vector_push_data(malloc(bytes), __FILE__, __LINE__)

#define calloc(items, size) \
    _ml_vector_push_data(calloc(items, size), __FILE__, __LINE__)

#define free(ptr)                           \
    do {                                    \
        _ml_vector_first_remove_data(ptr);  \
        free(ptr);                          \
    } while(false)      

#endif /* MEMORY_LEAKS_H */
