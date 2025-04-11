#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstring>
#include <unordered_set>

const int MAX_BUF = 256;

// Function prototypes for common utility functions
void sync_filesystem();
void drop_caches();
bool is_valid_path(const char *path);
bool checkArgv(char param[MAX_BUF], char *arg, char *err_msg);

#endif // COMMON_H