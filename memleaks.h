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

#ifdef __cplusplus
# define ML_CPLUSPLUS
#endif
/* ------------------------------------------------------------------------------------ */

/* Nessecary headers. */
#include <stdio.h>
#include <stdlib.h>

#ifndef ML_C_VER90
# include <stdint.h>
#endif
/* ------------------------------------------------------------------------------------ */

/* In case some compilers are not supporting these macros. */

#ifdef __LINE__
# define ML_LINE __LINE__
#else
# warning "__LINE__ is not supported by this compiler. line is initialized to 0."
# define ML_LINE 0
#endif

#ifdef __FILE__
# define ML_FILE __FILE__
#else
# warning "__FILE__ is not supported by this compiler. file is initilized to Unknown."
# define ML_FILE "Unknown"
#endif

#ifdef ML_C_VER90
/* Unfortunately these macros are not part of the ISO C. */
# ifdef __FUNCTION__
#  define ML_FUNC __FUNCTION__
# else
#  if defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__)
#   define ML_FUNC __PRETTY_FUNCTION__
#  elif defined(_MSC_VER)
#   define ML_FUNC __FUNCSIG__
#  else
#   warning "__func__ is not supported in C90, And your compiler is not supporting __FUNCSIG__ or __PRETTY_FUNCTION__ or __FUNCTION__. function is initialized to Unknown."
#   define ML_FUNC "Unknown"
#  endif
# endif
#else
# define ML_FUNC __func__
#endif
/* ------------------------------------------------------------------------------------ */

/* Type definitions for Ansi C */
#if defined(ML_C_VER90) && !defined(ML_CPLUSPLUS)
 typedef unsigned long ml_size_t;
#else
 typedef size_t ml_size_t;
#endif
/* ------------------------------------------------------------------------------------ */

#ifdef ML_CPLUSPLUS
 extern "C" {
#endif
/* ------------------------------------------------------------------------------------ */

 /* Initializing global vector. */
extern void  _ml_vector_init(); 

/* Push data to back of the vector. similar to std::vector::push_back. */
extern void* _ml_vector_push_data(void* ptr, const char* p_file, const char* p_func, ml_size_t line);

/* Finds where p_src is located, and update its values to a new ones. */
extern void* _ml_vector_update_data(const void* p_src, 
                                    void* p_dest, const char* p_file, const char* p_func, ml_size_t line);

/* Finds the first ptr and removes it. */
extern void  _ml_vector_first_remove_data(const void* ptr);

/* ------------------------------------------------------------------------------------ */

/* Printing all of the leaks at the end of program. */
extern void _ml_exit_message();
/* ------------------------------------------------------------------------------------ */

#ifdef ML_CPLUSPLUS
 }
#endif
/* ------------------------------------------------------------------------------------ */

#define INIT_MEMLEAKS              \
    do {                           \
        _ml_vector_init();         \
        atexit(&_ml_exit_message); \
    } while(0)

#define malloc(bytes) \
    _ml_vector_push_data(malloc(bytes), ML_FILE, ML_FUNC, ML_LINE)

#define calloc(items, size) \
    _ml_vector_push_data(calloc(items, size), ML_FILE, ML_FUNC, ML_LINE)

#define realloc(ptr, size) \
    _ml_vector_update_data(ptr, realloc(ptr, size), ML_FILE, ML_FUNC, ML_LINE)

#define free(ptr)                           \
    do {                                    \
        _ml_vector_first_remove_data(ptr);  \
        free(ptr);                          \
    } while(0)      

#endif /* MEMORY_LEAKS_H */

