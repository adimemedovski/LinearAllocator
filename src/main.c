#include <stdio.h>
#include <stdlib.h>
#include "linearAllocator.h"
#include <stddef.h>

int main(void) {
  
  printf("_Alignof(size_t): %zu\n", _Alignof(size_t));
  printf("_Alignof(char): %zu\n", _Alignof(char));

}
