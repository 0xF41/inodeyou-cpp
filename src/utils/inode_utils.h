#ifndef INODE_UTILS_H
#define INODE_UTILS_H

#include <unordered_set>

// Function prototypes for inode utilities
std::unordered_set<int> find_differences(const std::unordered_set<int> &tsk_inodes, const std::unordered_set<int> &fs_inodes);
void print_result(const std::unordered_set<int> &diff_inodes);

#endif // INODE_UTILS_H