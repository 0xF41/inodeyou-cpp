#include <iostream>
#include <cstring>
#include <unordered_set>
#include <sys/statfs.h>
#include "fs_walk_path.h"
#include "populate_fs_inodes.h"

using namespace std;

void populate_fs_inodes(const char *mount_point, const char *root, unordered_set<int> &fs_inode_set)
{
    struct statfs stat;
    if (statfs(mount_point, &stat) == -1)
    {
        cerr << "Error: Failed to get filesystem statistics." << endl;
        return;
    }

    fs_walk_path(mount_point, root, fs_inode_set);
}