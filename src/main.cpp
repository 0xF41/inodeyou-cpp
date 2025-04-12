#include <iostream>
#include <unordered_set>
#include <string>
#include "tsk/populate_tsk_inodes.h"
#include "fs/populate_fs_inodes.h"
#include "utils/common.h"
#include "utils/inode_utils.h"

using namespace std;

/**
 * Main function to check for differences between TSK and filesystem inodes.
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 */
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

    string volume;
    string mount_point;
    string start_dir = "/"; // Default to root directory

    if (argc > 1 && !checkArgv(volume, argv[1], "volume"))
        return 1;
    if (argc > 2 && !checkArgv(mount_point, argv[2], "mount point"))
        return 1;
    if (argc > 3 && !checkArgv(start_dir, argv[3], "starting directory"))
        return 1;

    sync_filesystem(); // Sync the filesystem to ensure all changes are written
    drop_caches();     // Drop caches to free up memory

    unordered_set<int> tsk_inode_set; // Set to store TSK inodes
    unordered_set<int> fs_inode_set;  // Set to store filesystem inodes

    populate_tsk_inodes(volume.c_str(), start_dir.c_str(), tsk_inode_set); // Populate TSK inodes
    populate_fs_inodes(start_dir.c_str(), fs_inode_set);   // Populate filesystem inodes

    unordered_set<int> diff_inodes = find_differences(tsk_inode_set, fs_inode_set);
    print_result(diff_inodes);

    return 0;
}