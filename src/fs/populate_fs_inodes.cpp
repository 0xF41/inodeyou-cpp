#include <iostream>
#include <cstring>
#include <unordered_set>
#include <sys/statfs.h>
#include "fs_walk_path.h"
#include "populate_fs_inodes.h"

using namespace std;

/**
 * Recursively walks through the directory structure starting from the given inode number
 * and populates the provided set with inode numbers of all directories encountered.
 *
 * @param inode_fpn Path to the directory to start walking from
 * @param fs_inode_set Set to populate with inode numbers of all directories encountered
 */
void populate_fs_inodes(const char *mount_point, unordered_set<int> &fs_inode_set)
{
    struct statfs stat;
    if (statfs(mount_point, &stat) == -1)
    {
        cerr << "Error: Failed to get filesystem statistics." << endl;
        return;
    }

    fs_walk_path(mount_point, fs_inode_set); // Recursively walk the directory structure
}