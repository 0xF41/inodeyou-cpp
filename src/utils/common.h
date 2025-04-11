#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstring>
#include <unordered_set>

const int MAX_BUF = 256;

// Function prototypes for common utility functions
void sync_filesystem();
void drop_caches();

#endif // COMMON_H