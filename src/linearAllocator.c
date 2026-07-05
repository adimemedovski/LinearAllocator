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

bool memoryBufferValid(MemoryBuffer *buffer) {
  if (buffer == NULL) {
    printf("Error: Buffer was not initialised correctly or is invalid.\n");
    return false;
  } else if (buffer -> ptrToVirtualAddressSpace == NULL) {
    printf("Error: Buffer not valid as ptr to virtual address space is NULL.\n");
    return false;
  }

  return true;
}

bool allignMemoryBuffer(MemoryBuffer *buffer, size_t allignment) {
  if (!memoryBufferValid(buffer)) {
    printf("Failed to call allignMemoryBuffer as buffer is invalid.\n");
    return false;
  } else if (allignment == 0) {
		printf("Error: Division by zero error.\n"); 
		return false;
	}

	while (buffer -> bufferOffset % allignment != 0) {
		if (buffer -> bufferOffset + 1 > MAX_MEMORY_BUFFER_SIZE) {
			printf("Error: Cannot align memory buffer due to overflow.\n"); 
			return false;
		}

		buffer -> bufferOffset += 1;
	}

  return true;
}

bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
  if (!memoryBufferValid(buffer)) {
    printf("Error; Failed to call incrementBufferOffset as buffer is invalid.\n");
    return false;
  } else if (buffer -> bufferOffset > MAX_MEMORY_BUFFER_SIZE - offsetAmount) {
		printf("Error: Cannot increment buffer offset due to overflow.\n"); 
		return false;
	}

	buffer -> bufferOffset += offsetAmount;

	return true;
}

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t allignment) {
  if (!memoryBufferValid(buffer)) {
    printf("Error: Failed to call lalloc due to buffer being invalid.\n");
    return NULL;
  } else if (blockSize == 0) {
    printf("Error: Cannot call lalloc due to block size being 0.\n");
    return NULL;
  } else if (allignment == 0) {
    printf("Error: Cannot call lalloc due to allignment being 0.\n");
    return NULL;
  } else if (!allignMemoryBuffer(buffer, allignment)) {
    printf("Error: Failed to align memory buffer when executing lalloc.\n");
    return NULL;
  } else if (buffer -> bufferOffset + blockSize > MAX_MEMORY_BUFFER_SIZE) {
    printf("Error: Failed to call lalloc due to memory buffer overflow.\n");
    return NULL;
  }

  char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
  ptr += buffer -> bufferOffset;  

  if (!incrementBufferOffset(buffer, blockSize)) {
    printf("Error: Cannot call lalloc due to increment buffer offset failing.\n");
    return NULL;
  }

	return (void*) ptr;
}

void rlalloc(MemoryBuffer *buffer) {
	buffer -> bufferOffset = 0;
}

void dlalloc(MemoryBuffer *buffer) {
  if (buffer == NULL) {
    return;
  } else if (buffer -> ptrToVirtualAddressSpace == NULL) {
    buffer -> bufferOffset = 0; 
    return;
  }	
  
  munmap(buffer -> ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
  buffer -> ptrToVirtualAddressSpace = NULL;	
  buffer -> bufferOffset = 0;
}
