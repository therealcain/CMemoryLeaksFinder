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
