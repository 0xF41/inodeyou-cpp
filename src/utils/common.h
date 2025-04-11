#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <unordered_set>

// Function prototypes for common utility functions
void sync_filesystem();
void drop_caches();
bool is_valid_path(const std::string &path);
bool checkArgv(std::string &param, const char *arg, const std::string &err_msg);

#endif // COMMON_H