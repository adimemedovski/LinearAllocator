#include "unity.h"
#include "linearAllocator.h"
#include <stddef.h>
#include <stdbool.h>

void setUp(void) {

}

void tearDown(void) {

}

void testInitMemoryBuffer(void) {
	MemoryBuffer buffer;
	initMemoryBuffer(&buffer);

	TEST_ASSERT_EQUAL_size_t(0, buffer.bufferOffset);
	TEST_ASSERT_FALSE(buffer.ptrToVirtualAddressSpace == NULL);
}

void testLalloc(void) {
	MemoryBuffer buffer;
	initMemoryBuffer(&buffer);

	/*
	 * Test one:
	 *     - Testing for alignment;
	 *     - and correct padding.
	 */

	typedef struct {
		size_t x;
		char y;
		size_t z;
	} Test;

	size_t blockSizeOfTest = sizeof(Test);

	size_t *ptrOne = (size_t*) lalloc(&buffer, sizeof(size_t), _Alignof(size_t));
	char *ptrTwo = (char*) lalloc(&buffer, sizeof(char), _Alignof(char));
	size_t *ptrThree = (size_t*) lalloc(&buffer, sizeof(size_t), _Alignof(size_t));

	TEST_ASSERT_EQUAL_size_t(24, buffer.bufferOffset);

	/*
	 * Test two:
	 *     - Test for edge cases.
	 */

	size_t snapShotOfBufferOffsetOne = buffer.bufferOffset;

	size_t *ptrFour = (size_t*) lalloc(&buffer, sizeof(size_t), 0);

	TEST_ASSERT_TRUE(ptrFour == NULL);
	TEST_ASSERT_EQUAL_size_t(snapShotOfBufferOffsetOne, buffer.bufferOffset);

	size_t *ptrFive = (size_t*) lalloc(&buffer, 0, _Alignof(size_t));

	TEST_ASSERT_TRUE(ptrFive == NULL);
	TEST_ASSERT_EQUAL_size_t(snapShotOfBufferOffsetOne, buffer.bufferOffset);

	size_t *ptrSix = (size_t*) lalloc(&buffer, 0, 0);

	TEST_ASSERT_TRUE(ptrSix == NULL);
	TEST_ASSERT_EQUAL_size_t(snapShotOfBufferOffsetOne, buffer.bufferOffset);

	/*
	 * Test Three: 
	 *     - Overflow handling.
	 */

	size_t overFlowByteAmount = MAX_MEMORY_BUFFER_SIZE - buffer.bufferOffset + 1;
	size_t overFlowBlockSize = overFlowByteAmount / sizeof(size_t) + 1;

	size_t *ptrSeven = (size_t*) lalloc(&buffer, sizeof(size_t) * overFlowBlockSize, _Alignof(size_t));

	TEST_ASSERT_TRUE(ptrSeven == NULL);

	/*
	 * Test Four:
	 *     - More testing on padding and alignment.
	 */

	size_t snapShotOfBufferOffsetTwo = buffer.bufferOffset;

	size_t *ptrEight = (size_t*) lalloc(&buffer, sizeof(size_t) * 3, _Alignof(size_t));

	/*
	 * This test depends on whether the previous lalloc call caused a disruption to the alignment of 
	 * the second lalloc call. Hence we must test both cases. In this first case, the test passed
	 * so we can assume the previous lalloc call did not cause any disruption. Thus, in our next 
	 * test case we will purposely disrupt the alignment of a size_t lalloc call.
	 */
	TEST_ASSERT_EQUAL_size_t(snapShotOfBufferOffsetTwo + 24, buffer.bufferOffset); 

	char *ptrNine = (char*) lalloc(&buffer, sizeof(char), sizeof(char));

	size_t snapShotOfBufferOffsetThree = buffer.bufferOffset;

	size_t *ptrTen = (size_t*) lalloc(&buffer, sizeof(size_t) * 2, sizeof(size_t));


	TEST_ASSERT_EQUAL_size_t(snapShotOfBufferOffsetThree + 7 + sizeof(size_t) * 2, buffer.bufferOffset);
  
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(testInitMemoryBuffer);
	RUN_TEST(testLalloc);

	return UNITY_END();
}
