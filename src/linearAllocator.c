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

static int getAlignmentPadding(MemoryBuffer *buffer, size_t alignment) {
    size_t address = (size_t) buffer -> ptrToVirtualAddressSpace + buffer -> bufferOffset; 

    if (alignment == 0) {
        printf("Error: Failed to call getAlignmentPadding as alignment cannot be zero -- Division by zero error.\n");
        return -1; 
    }   
    else if (address % alignment == 0) {
        return 0;
    } else {
        size_t quotient = (address - (address % alignment)) / alignment; 
        size_t offsetAmount = alignment * (quotient + 1) - address;
       
        return offsetAmount;
    }

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
    } else if (buffer -> bufferOffset > MAX_MEMORY_BUFFER_SIZE - blockSize) {
        printf("Error: Failed to call lalloc due to memory buffer overflow.\n");
        return NULL;
    }
    
    int alignmentPadding = getAlignmentPadding(buffer, alignment);
    
    if (alignmentPadding == -1) {
        printf("Error");
    }

    char *ptr = (char*) buffer -> ptrToVirtualAddressSpace;
    ptr += (buffer -> bufferOffset) + alignmentPadding; 
    
    if (ptr == NULL) {
        printf("Error: Failed to call lalloc. ptr is invalid.\n");
        return NULL;
    }
    else if (!incrementBufferOffset(buffer, alignmentPadding + blockSize)) {
        printf("Error: Failed to add alignment padding and block size to buffer offset. Buffer overflow.\n");
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
