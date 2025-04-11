#include <iostream>
#include <unordered_set>
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
int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <volume> [mount_point] [starting_dir]" << endl;
        return 1;
    }

    if (geteuid() != 0) {
        cerr << "Error: This program must be run as root." << endl;
        return 1;
    }

    char volume[MAX_BUF] = {0};
    char mount_point[MAX_BUF] = {0};
    char root[MAX_BUF] = {0};

    if (argc > 1 && checkArgv(volume, argv[1], "volume")) return 1;
    if (argc > 2 && checkArgv(mount_point, argv[2], "mount point")) return 1;
    if (argc > 3 && checkArgv(root, argv[3], "starting directory")) return 1;
    if (argc <= 3 && checkArgv(root, "/", "starting directory")) return 1;

    sync_filesystem(); // Sync the filesystem to ensure all changes are written
    drop_caches();     // Drop caches to free up memory

    unordered_set<int> tsk_inode_set; // Set to store TSK inodes
    unordered_set<int> fs_inode_set;  // Set to store filesystem inodes

    populate_tsk_inodes(mount_point, root, tsk_inode_set); // Populate TSK inodes
    populate_fs_inodes(mount_point, root, fs_inode_set);   // Populate filesystem inodes

    unordered_set<int> diff_inodes = find_differences(tsk_inode_set, fs_inode_set);
    print_result(diff_inodes);

    return 0;
}