#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <unordered_set>
#include <sys/stat.h>
#include "tsk/populate_tsk_inodes.h"
#include "fs/populate_fs_inodes.h"
#include "utils/common.h"

using namespace std;

// Helper function to validate if a path exists
bool is_valid_path(const char *path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <volume> [mount_point] [starting_dir]" << endl;
        return 1;
    }

    if (geteuid() != 0)
    {
        cerr << "Error: This program must be run as root." << endl;
        return 1;
    }

    char volume[MAX_BUF] = {0};
    char mount_point[MAX_BUF] = {0};
    char root[MAX_BUF] = {0};

    if (argc > 1)
    {
        strncpy(volume, argv[1], MAX_BUF - 1);
        volume[MAX_BUF - 1] = '\0';
        if (!is_valid_path(volume))
        {
            cerr << "Error: Invalid volume path provided: " << volume << endl;
            return 1;
        }
    }
    if (argc > 2)
    {
        strncpy(mount_point, argv[2], MAX_BUF - 1);
        mount_point[MAX_BUF - 1] = '\0';
        if (!is_valid_path(mount_point))
        {
            cerr << "Error: Invalid mount point path provided: " << mount_point << endl;
            return 1;
        }
    }
    if (argc > 3)
    {
        strncpy(root, argv[3], MAX_BUF - 1);
        root[MAX_BUF - 1] = '\0';
        if (!is_valid_path(root))
        {
            cerr << "Error: Invalid root path provided: " << root << endl;
            return 1;
        }
    }
    else
    {
        strncpy(root, "/", MAX_BUF - 1);
        root[MAX_BUF - 1] = '\0';
    }

    sync_filesystem(); // Sync the filesystem to ensure all changes are written
    drop_caches(); // Drop caches to free up memory

    unordered_set<int> tsk_inode_set; // Set to store TSK inodes
    unordered_set<int> fs_inode_set; // Set to store filesystem inodes

    populate_tsk_inodes(mount_point, root, tsk_inode_set); // Populate TSK inodes
    populate_fs_inodes(mount_point, root, fs_inode_set); // Populate filesystem inodes

    unordered_set<int> diff_inodes; // Set to store differences between TSK and filesystem inodes (anomalies)
    for (const auto &inode : tsk_inode_set)
    {
        if (fs_inode_set.find(inode) == fs_inode_set.end())
        {
            diff_inodes.insert(inode);
        }
    }
    for (const auto &inode : fs_inode_set)
    {
        if (tsk_inode_set.find(inode) == tsk_inode_set.end())
        {
            diff_inodes.insert(inode);
        }
    }

    if (diff_inodes.empty())
    {
        cout << "No differences found between TSK and filesystem inodes." << endl;
    }
    else
    {
        cout << "The following inodes may be hidden by a malware:" << endl;
        for (const auto &inode : diff_inodes)
        {
            cout << inode << endl;
        }
    }

    return 0;
}