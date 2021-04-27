# CMemoryLeaksFinder
### What is this?
Its a very simple project to find from which lines do we have memory leaks, and if there are any.

### How does it work?
This library is wrapping the `malloc`/`calloc`/`realloc`/`free` in a macro to keep track of the pointers that were allocated and freed.

### What do you need?
Any compiler that supports these extensions:
* `__FILE__`
* `__LINE__`

The code base is also designed to work with C89.

### Code Example
```c
#include <stdlib.h>
#include "memleaks.h"

int main()
{
  INIT_MEMLEAKS; /* We must call it to initialize the library. */
  
  int* p   = malloc(sizeof(int));
  int* arr = malloc(sizeof(int) * 5);
  
  free(p);
  free(arr);
}
```
After compiling it with `clang` we get:
```
Great job! no leaks!
```
Now, let's comment our `free` calls, we then get:
```
Found 2 leaks:
 #1 Leaking address: 0x210b2c0, In file: test.c, In line: 8
 #2 Leaking address: 0x210b2e0, In file: test.c, In line: 9
```

NOTE: This library is not thread safe, I did not test it under multithreaded application.
