# CMemoryLeaksFinder
### What is this?
C++ have smart pointers and they should be used almost always, but C doesn't have that, so it's easy to forget about dynamic memory allocations, so this library simply finds memory leaks that were allocated with `malloc`/`calloc`/`realloc` and tell you information about that allocation.
If you lazy to to download and install `valgrind` for some reason, this may be a nice code alternative to finding leaks.

### How does it work?
This library is wrapping the `malloc`/`calloc`/`realloc`/`free` in a macro to keep track of the pointers that were allocated and freed.

### Code Example
```c
#include <stdlib.h>
#include "memleaks.h"

int main()
{
  INIT_MEMLEAKS; /* We must call it to initialize the library. */
  
  int* p   = (int*)malloc(sizeof(int));
  int* arr = (int*)malloc(sizeof(int) * 5);
  
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
 #1 Leaking address: 0x210b2c0, In file: 'test.c', In function: 'int main()', In line: 8
 #2 Leaking address: 0x210b2e0, In file: 'test.c', In function: 'int main()', In line: 9
```

### Some Notes.
* This library probably not thread safety, I did not test it under multithreaded application.
* It does not cover operators new/delete of C++.
* It was designed to work with Ansi C aswell.
