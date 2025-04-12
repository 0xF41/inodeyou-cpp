#ifndef FS_WALK_PATH_H
#define FS_WALK_PATH_H

#include <unordered_set>

void fs_walk_path(const char *path, std::unordered_set<int> &fs_inode_set);
void populate_fs_inodes(const char *path, std::unordered_set<int> &fs_inode_set);

#endif // FS_WALK_PATH_H