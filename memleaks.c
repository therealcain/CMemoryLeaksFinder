#include "memleaks.h"
#undef malloc
#undef calloc
#undef realloc
#undef free
/* ------------------------------------------------------------------------------------ */

#define MAX_ML_SIZE_T ((ml_size_t)-1)
/* ------------------------------------------------------------------------------------ */

/* Holds all of the necessary information about the memory allocation. */
typedef struct {
    const void*     p_ptr;
    const char*     p_file;
    const char*     p_func;
    ml_size_t       line;
} MemoryInfo, *pMemoryInfo;
/* ------------------------------------------------------------------------------------ */

/* Imitate the behaviour of std::vector. */
typedef struct {
    ml_size_t       max_size;
    ml_size_t       index;
    pMemoryInfo     p_data;
} MemoryInfoVector, *pMemoryInfoVector;

/* Globally vector initialization. */
static MemoryInfoVector global_vector;

#define VEC_DATA     global_vector.p_data
#define VEC_INDEX    global_vector.index
#define VEC_SIZE     VEC_INDEX
#define VEC_MAX_SIZE global_vector.max_size

#define VEC_EMPTY()    (VEC_SIZE == 0)
#define VEC_CLEAR()    free(VEC_DATA)
/* ------------------------------------------------------------------------------------ */

/* Printing an error message and exiting.. */
static
void invoke_runtime_error(const char* str)
{
    puts(str);
    VEC_CLEAR();
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

/* Looking for ptr in vector, and returns its pointer. */
static
pMemoryInfo vector_find_data_by_pointer(const void* ptr) 
{
    ml_size_t i;
    for(i = 0; i < VEC_SIZE; ++i)
    {
        if(VEC_DATA[i].p_ptr == ptr)
            return &VEC_DATA[i];
    }
    
    return NULL;
}

static
ml_size_t vector_find_data_by_pointer_and_get_index(const void* ptr)
{
    ml_size_t i;
    for(i = 0; i < VEC_SIZE; ++i)
    {
        if(VEC_DATA[i].p_ptr == ptr)
            return i;
    }

    return MAX_ML_SIZE_T;
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
void* _ml_vector_push_data(void* ptr, const char* p_file, const char* p_func, ml_size_t line)
{
    if(VEC_INDEX == VEC_MAX_SIZE)
        vector_increase_size();

    /* Creating possible memory leak info. */
    if(ptr != NULL)
    {
        /* MemoryInfo info = {.p_ptr = ptr, .p_file = p_file, .p_func = p_func, .line = line }; */
        MemoryInfo info;
        info.p_ptr  = ptr;
        info.p_file = p_file;
        info.p_func = p_func;
        info.line   = line;

        VEC_DATA[VEC_INDEX] = info;
        VEC_INDEX++;
    }

    return ptr;
}

/* Updating the data based on pointers. */
void* _ml_vector_update_data(const void* p_src, 
                             void* p_dest, const char* p_file, const char* p_func, ml_size_t line)
{
    pMemoryInfo p_info;

    if(p_dest == NULL)
        return NULL;

    p_info = vector_find_data_by_pointer(p_src);

    if(p_info == NULL)
        invoke_runtime_error("Unexpected error from _ml_vector_update_data.");

    p_info->p_ptr  = p_dest;
    p_info->p_file = p_file;
    p_info->p_func = p_func;
    p_info->line   = line;

    return p_dest;
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
    ml_size_t idx = vector_find_data_by_pointer_and_get_index(ptr);

    if(idx == MAX_ML_SIZE_T)
        invoke_runtime_error("Unexpected error from _ml_vector_first_remove_data.");

    vector_remove_by_index(idx);
}
/* ------------------------------------------------------------------------------------ */

void _ml_exit_message()
{
    ml_size_t i;

    if(VEC_EMPTY())
        printf("Great job! no leaks!\n");
    else
    {
        printf("Found %lu leaks:\n", VEC_SIZE);

        for(i = 0; i < VEC_SIZE; ++i)
        {
            pMemoryInfo p_info = &VEC_DATA[i];
            printf(" #%lu Leaking address: %p, In file: '%s', In function: '%s', In line: %lu\n", 
                    i + 1, p_info->p_ptr, p_info->p_file, p_info->p_func, p_info->line);
        }
    }

    VEC_CLEAR();
}
