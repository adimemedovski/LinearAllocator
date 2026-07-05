#define _GNU_SOURCE

#include "linearAllocator.h"
#include <stdio.h> // For debugging information.
#include <stddef.h> // For size_t type.
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>

static bool memoryBufferValid(MemoryBuffer *buffer) {
  if (buffer == NULL) {
    printf("Error: Buffer was not initialised correctly or is invalid.\n");
    return false;
  } else if (buffer -> ptrToVirtualAddressSpace == NULL) {
    printf("Error: Buffer not valid as ptr to virtual address space is NULL.\n");
    return false;
  }

  return true;
}

bool initMemoryBuffer(MemoryBuffer *buffer) {
  if (buffer == NULL) {
    printf("Error: Failed to initialise memory buffer due to buffer == NULL.\n"); 
    return false; 
  } 
  
  buffer -> ptrToVirtualAddressSpace = (void*) mmap(NULL, MAX_MEMORY_BUFFER_SIZE,
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);

	if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED) {
	  buffer -> ptrToVirtualAddressSpace = NULL;	
	  buffer -> bufferOffset = 0;	
    printf("Failed to initialise memory buffer.\n");

    return false; 
	}

	buffer -> bufferOffset = 0;

	return true;
}

/*
 * Need to align the actual memory address of a pointer allocation,
 * not only the relative buffer offset.
 *
 * To get final pointer, ptrToVirtualAddressSpace + bufferOffset. Perform
 * pointer arithmetic to correct align this pointer. No need to align 
 * bufferOffset, as bufferOffset only tells us how many bytes we are into 
 * the buffer. 
 */
static bool alignMemoryBuffer(MemoryBuffer *buffer, size_t alignment) {
  if (!memoryBufferValid(buffer)) {
    printf("Failed to call alignMemoryBuffer as buffer is invalid.\n");
    return false;
  } else if (alignment == 0) {
		printf("Error: Division by zero error.\n"); 
		return false;
	}
 
  if (buffer -> bufferOffset % alignment == 0) {
    return true;
  } else {
    size_t quotient = (buffer -> bufferOffset - (buffer -> bufferOffset % alignment)) / alignment; 
    size_t offsetAmount = alignment * (quotient + 1) - buffer -> bufferOffset;

    if (buffer -> bufferOffset > MAX_MEMORY_BUFFER_SIZE - offsetAmount) {
      printf("Error: Cannot align memory due to buffer overflow.\n"); 
      return false;
    }

    buffer -> bufferOffset += offsetAmount;
  }

  return true;
}

static size_t getPadding(size_t alignment) {

}

static bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
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

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
  if (!memoryBufferValid(buffer)) {
    printf("Error: Failed to call lalloc due to buffer being invalid.\n");
    return NULL;
  } else if (blockSize == 0) {
    printf("Error: Cannot call lalloc due to block size being 0.\n");
    return NULL;
  } else if (alignment == 0) {
    printf("Error: Cannot call lalloc due to alignment being 0.\n");
    return NULL;
  } else if (!alignMemoryBuffer(buffer, alignment)) {
    printf("Error: Failed to align memory buffer when executing lalloc.\n");
    return NULL;
  } else if (buffer -> bufferOffset > MAX_MEMORY_BUFFER_SIZE - blockSize) {
    printf("Error: Failed to call lalloc due to memory buffer overflow.\n");
    return NULL;
  }

  char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
  ptr += buffer -> bufferOffset;  
  
  size_t ptrPadding = getPadding(alignment);
  
  if (ptrPadding + buffer.bufferOffset > MAX_MEMORY_BUFFER_SIZE) {
    printf("Error: Cannot call lalloc due to buffer overflow.\n");
    return NULL;
  }
  
  ptr += ptrPadding;

  if (!incrementBufferOffset(buffer, blockSize)) {
    printf("Error: Cannot call lalloc due to increment buffer offset failing.\n");
    return NULL;
  }

	return (void*) ptr;
}

void rlalloc(MemoryBuffer *buffer) {
  if (buffer == NULL) {
    printf("Error: Failed to call rlalloc due to buffer == NULL.\n");
    return;
  }	
 
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
