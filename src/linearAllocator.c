#define _GNU_SOURCE

#include "linearAllocator.h"
#include <stdio.h> // For debugging information.
#include <stddef.h> // For size_t type.
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

static bool memoryBufferValid(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Buffer was not initialised correctly or is invalid.\n");
        return false;
    } 
   
    if (buffer -> ptrToVirtualAddressSpace == NULL) {
        fprintf(stderr, "Error: Buffer not valid as ptr to virtual address space is NULL.\n"); 
        return false; 
    }

    return true;
}

bool initMemoryBuffer(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Failed to initialise memory buffer due to buffer == NULL.\n"); 
        return false; 
    }

    buffer -> ptrToVirtualAddressSpace = (void*) mmap(NULL, MAX_MEMORY_BUFFER_SIZE,
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);

    if (buffer -> ptrToVirtualAddressSpace == MAP_FAILED) {
        buffer -> ptrToVirtualAddressSpace = NULL;	
        buffer -> bufferOffset = 0;	
        fprintf(stderr, "Failed to initialise memory buffer.\n");

        return false; 
    }

    buffer -> bufferOffset = 0;

    return true;

}

static int getAlignmentPadding(MemoryBuffer *buffer, size_t alignment) {
    size_t address = (size_t) buffer -> ptrToVirtualAddressSpace + buffer -> bufferOffset; 

    if (alignment == 0) {
        fprintf(stderr, "Error: Failed to call getAlignmentPadding as alignment cannot be zero -- Division by zero error.\n");
        return -1; 
    }   
    
    if (address % alignment == 0) {
        return 0;
    } 

    return alignment - (address % alignment);
}

static bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount) {
    if (!memoryBufferValid(buffer)) {
        fprintf(stderr, "Error; Failed to call incrementBufferOffset as buffer is invalid.\n");
        return false;
    }

    if (buffer -> bufferOffset >= MAX_MEMORY_BUFFER_SIZE) {
        fprintf(stderr, "Error: Failed to call incrementBufferOffet as bufferOffset exceeds buffer size.\n");
        return false;
    }
    
    if (offsetAmount > MAX_MEMORY_BUFFER_SIZE) {
        fprintf(stderr, "Error: Failed to call incrementBufferOffset as offsetAmount exceeds buffer size.\n");
        return false;
    }

    size_t remainder = MAX_MEMORY_BUFFER_SIZE - buffer -> bufferOffset;
    
    if (offsetAmount > remainder) {
        fprintf(stderr, "Error: Failed to call incrementBufferOffset as offset amount exceeds buffer size.\n");
        return false;
    }

    buffer -> bufferOffset += offsetAmount;

    return true;
}

static bool validateStateOfLalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!memoryBufferValid(buffer)) {
        fprintf(stderr, "Error: Failed to call lalloc due to buffer being invalid.\n");
        return false;
    } 

    if (blockSize == 0) {
        fprintf(stderr, "Error: Cannot call lalloc due to block size being 0.\n");
        return false;
    } 
   
    if (alignment == 0) {
        fprintf(stderr, "Error: Cannot call lalloc due to alignment being 0.\n");
        return false;
    }

    return true;
}

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment) {
    if (!validateStateOfLalloc(buffer, blockSize, alignment)) {
        fprintf(stderr, "Failed to call lalloc during validation of state check.\n");
        return NULL;
    }

    int alignmentPadding = getAlignmentPadding(buffer, alignment);

    if (alignmentPadding == -1) {
        fprintf(stderr, "Error: Failed to call lalloc due to invalid alignment padding.\n");
        return NULL;
        // Need to add overflow checking.
    } 
    
    size_t padding = (size_t) alignmentPadding;
    
    if (padding > SIZE_MAX - blockSize) {
        fprintf(stderr, "Error: Failed to call lalloc due to overflow error.\n");
        return NULL;
    }

    char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
    ptr += (buffer -> bufferOffset) + alignmentPadding; 
   
    if (!incrementBufferOffset(buffer, padding + blockSize)) {
        fprintf(stderr, "Error: Failed to call lalloc due to failure of incrementing buffer offset.\n");
        return NULL;
    }
    
    return (void*) ptr;
}

void rlalloc(MemoryBuffer *buffer) {
    if (buffer == NULL) {
        fprintf(stderr, "Error: Failed to call rlalloc due to buffer == NULL.\n");
        return;
    }	

    buffer -> bufferOffset = 0;
}

void dlalloc(MemoryBuffer *buffer) {
    if (buffer == NULL)  {
        return;
    } 
   
    if (buffer -> ptrToVirtualAddressSpace == NULL) {
        buffer -> bufferOffset = 0; 
        return;
    }	

    munmap(buffer -> ptrToVirtualAddressSpace, MAX_MEMORY_BUFFER_SIZE);
    buffer -> ptrToVirtualAddressSpace = NULL;	
    buffer -> bufferOffset = 0;
}
