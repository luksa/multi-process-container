#include <iostream>
#include <cstdlib>     // for malloc()
#include <cstring>     // for memset()
#include <unistd.h>    // for sleep() (POSIX systems)

constexpr size_t ALLOCATION_SIZE = 80 * 1024 * 1024;

int main() {
    std::cout << "Allocating 80MB of memory" << std::endl;
    // Allocate memory
    char* leak = (char*)malloc(ALLOCATION_SIZE);
    if (leak) {
        // Use the memory to store data (write something into it)
        memset(leak, 'A', ALLOCATION_SIZE - 1); // Fill with 'A's
        leak[ALLOCATION_SIZE - 1] = '\0'; // Null-terminate for safety
    } else {
        std::cerr << "Memory allocation failed!" << std::endl;
        return 1;
    }

    while (true) {
        sleep(1);
    }
    return 0;
}