#define _GNU_SOURCE

#include "linearAllocator.h"
#include <stdio.h> // For debugging information.
#include <stddef.h> // For size_t type.
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>

bool initMemoryBuffer(MemoryBuffer *buffer) {
  buffer -> ptrToVirtualAddressSpace = (void*) mmap(NULL, MAX_MEMORY_BUFFER_SIZE,
      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
      -1, 0);

  if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED) {
    printf("Failed to initialise memory buffer.\n");
    return false; 
  }

  buffer -> bufferOffset = 0;
  
  return true;
}

bool alignMemoryBuffer(MemoryBuffer *buffer, size_t sizeOfType) {
  if (sizeOfType == 0) {
    printf("Error: Division by zero error.\n"); 
    return false;
  } 
  
  while (buffer -> bufferOffset % sizeOfType != 0) {
    if (buffer -> bufferOffset + 1 > MAX_MEMORY_BUFFER_SIZE) {
      printf("Error: Cannot align memory buffer due to overflow.\n"); 
      return false;
    }

    buffer -> bufferOffset += 1;
  }
  
  return true;
}

bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
  if (buffer -> bufferOffset + offsetAmount > MAX_MEMORY_BUFFER_SIZE) {
    printf("Error: Cannot increment buffer offset due to overflow.\n"); 
    return false;
  }

  buffer -> bufferOffset += offsetAmount;
  
  return true;
}

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t sizeOfType) {
  if (!alignMemoryBuffer(buffer, sizeOfType)) {
    printf("Error: Failed to align memory buffer when executing lalloc.\n");
    return NULL;
  } else if (buffer -> ptrToVirtualAddressSpace == NULL) {
    printf("Error: Failed to call lalloc due to invalid virtual address space.\n");
    return NULL;
  } else if (buffer -> bufferOffset + blockSize > MAX_MEMORY_BUFFER_SIZE) {
    printf("Error: Failed to call lalloc due to memory buffer overflow.\n");
    return NULL;
  } else if (blockSize == 0) {
    printf("Error: Cannot call lalloc with a blocksize of 0.\n");
    return NULL;
  } 

  char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
  ptr += buffer -> bufferOffset;  
  
  buffer -> bufferOffset += blockSize;
  
  return (void*) ptr;
}

void rlalloc(MemoryBuffer *buffer) {
  buffer -> bufferOffset = 0;
}

void dlalloc(MemoryBuffer *buffer) {
  munmap(buffer -> ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
  buffer -> bufferOffset = 0;
}
