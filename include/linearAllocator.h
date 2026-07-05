#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H 

#include <stdbool.h>
#include <stddef.h>

#define MAX_MEMORY_BUFFER_SIZE (4096 * 10)

typedef struct {
	size_t bufferOffset;
	void *ptrToVirtualAddressSpace;
} MemoryBuffer;

bool initMemoryBuffer(MemoryBuffer *buffer);

bool alignMemoryBuffer(MemoryBuffer *buffer, size_t sizeOfType);

bool incrementBufferOffset(MemoryBuffer *buffer, size_t offsetAmount);

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t sizeOfType);

void rlalloc(MemoryBuffer *buffer);

void dlalloc(MemoryBuffer *buffer);

#endif
