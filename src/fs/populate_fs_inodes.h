#ifndef POPULATE_FS_INODES_H
#define POPULATE_FS_INODES_H

#include <unordered_set>

void populate_fs_inodes(const char *mount_point, std::unordered_set<int> &fs_inode_set);

#endif // POPULATE_FS_INODES_H