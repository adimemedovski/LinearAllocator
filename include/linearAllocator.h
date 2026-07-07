#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H 

#include <stdbool.h>
#include <stddef.h>

#define MAX_MEMORY_BUFFER_SIZE ((size_t) 4096 * 10)

typedef struct {
	size_t bufferOffset;
	void *ptrToVirtualAddressSpace;
} MemoryBuffer;

bool initMemoryBuffer(MemoryBuffer *buffer);

void *lalloc(MemoryBuffer *buffer, size_t blockSize, size_t alignment);

void rlalloc(MemoryBuffer *buffer);

void dlalloc(MemoryBuffer *buffer);

#endif
