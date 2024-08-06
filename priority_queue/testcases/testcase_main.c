#include <stdint.h>
#include <stdbool.h>

void testParallelInsertions(uint64_t threadsCount, bool minHeap);
void testMixed();

int main() {
	testParallelInsertions(12, true);
	testParallelInsertions(16, true);
	testMixed();
}