#include "inode_utils.h"
#include <iostream>
using namespace std;

/**
 * Find the differences between TSK and filesystem inodes.
 * @param tsk_inodes Set of inodes from TSK
 * @param fs_inodes Set of inodes from the filesystem
 * @return Set of inodes that are different between TSK and filesystem
 */
std::unordered_set<int> find_differences(const std::unordered_set<int> &tsk_inodes, const std::unordered_set<int> &fs_inodes)
{
    std::unordered_set<int> diff_inodes; // Set to store differences between TSK and filesystem inodes
    for (const auto &inode : tsk_inodes)
    {
        if (fs_inodes.find(inode) == fs_inodes.end())
        {
            diff_inodes.insert(inode);
        }
    }
    for (const auto &inode : fs_inodes)
    {
        if (tsk_inodes.find(inode) == tsk_inodes.end())
        {
            diff_inodes.insert(inode);
        }
    }
    return diff_inodes;
}

/**
 * Print the result of inode differences.
 * @param diff_inodes Set of inodes that are different between TSK and filesystem
 */
void print_result(const std::unordered_set<int> &diff_inodes)
{
    if (diff_inodes.empty())
    {
        cout << "[*] No differences found between TSK and filesystem inodes." << endl;
    }
    else
    {
        cout << "[!!] The following files may be hidden by malware:" << endl;
        for (const auto &inode : diff_inodes)
        {
            cout << inode << endl;
        }
    }
}