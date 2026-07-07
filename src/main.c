#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int main(void) {
  char *ptr = (char*) malloc(sizeof(char));

  size_t remainder = (&ptr) % 8;
  
  printf("Remainder: %zu", remainder);
}
