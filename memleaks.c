#include "memleaks.h"
#undef malloc
#undef calloc
#undef free
/* ------------------------------------------------------------------------------------ */

/* Globally vector initialization. */
static MemoryInfoVector global_vector;

#define VEC_DATA     global_vector.p_data
#define VEC_INDEX    global_vector.index
#define VEC_SIZE     VEC_INDEX
#define VEC_MAX_SIZE global_vector.max_size

#define VEC_EMPTY    (VEC_SIZE == 0)
#define VEC_CLEAR    free(VEC_DATA)
/* ------------------------------------------------------------------------------------ */

/* Printing an error message and exiting.. */
static
void invoke_runtime_error(const char* str)
{
    puts(str);
    VEC_CLEAR;
    exit(EXIT_FAILURE);
}
/* ------------------------------------------------------------------------------------ */

/* Prepare vector members. */
void _ml_vector_init() 
{
    VEC_INDEX    = 0;
    VEC_MAX_SIZE = 1;
    VEC_DATA     = (pMemoryInfo) malloc(sizeof(MemoryInfo));

    if(global_vector.p_data == NULL)
        invoke_runtime_error("Failed to initialize vector.");
}

/* Increase vector size by 2. */
static 
void vector_increase_size()
{
    VEC_MAX_SIZE *= 2;
    VEC_DATA = (pMemoryInfo) realloc(VEC_DATA, VEC_MAX_SIZE * sizeof(MemoryInfo));
    
    if(VEC_DATA == NULL)
        invoke_runtime_error("Failed to increase vector.");
}

/* Push data into vector. */
void* _ml_vector_push_data(void* ptr, const char* p_file, ml_size_t line)
{
    if(VEC_INDEX == VEC_MAX_SIZE)
        vector_increase_size();

    /* Creating possible memory leak info. */
    if(ptr != NULL)
    {
        MemoryInfo info;
        info.p_ptr  = ptr;
        info.p_file = p_file;
        info.line   = line;

        VEC_DATA[VEC_INDEX] = info;
        VEC_INDEX++;
    }

    return ptr;
}

/* Remove data from vector by index. */
static
void vector_remove_by_index(ml_size_t index)
{
    ml_size_t i;

    if(VEC_INDEX <= index)
        invoke_runtime_error("Index is out of bounds.");

    for(i = index; i < VEC_SIZE - 1; ++i)
        VEC_DATA[i] = VEC_DATA[i + 1];

    VEC_INDEX--;
}

/* Remove data from vector. */
void _ml_vector_first_remove_data(const void* ptr)
{
    ml_size_t i;
    for(i = 0; i < VEC_INDEX; ++i)
    {
        if(VEC_DATA[i].p_ptr == ptr)
        {
            vector_remove_by_index(i);
            break;
        }
    }
}
/* ------------------------------------------------------------------------------------ */

void _ml_exit_message()
{
    ml_size_t i;

    if(VEC_EMPTY)
        printf("Great job! no leaks!\n");
    else
    {
        printf("Found %zu leaks:\n", VEC_SIZE);

        for(i = 0; i < VEC_SIZE; ++i)
        {
            pMemoryInfo p_info = &VEC_DATA[i];
            printf(" #%zu Leaking address: %p, In file: %s, In line: %zu\n", 
                    i + 1, p_info->p_ptr, p_info->p_file, p_info->line);
        }
    }

    VEC_CLEAR;
}
