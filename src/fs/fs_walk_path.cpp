#include <iostream>
#include <cstring>
#include <dirent.h>
#include <unordered_set>
#include <string>

using namespace std;

/**
 * Recursively walks through the directory structure starting from the given inode number
 * and populates the provided set with inode numbers of all directories encountered.
 *
 * @param inode_fpn Path to the directory to start walking from
 * @param fs_inode_set Set to populate with inode numbers of all directories encountered
 */
void fs_walk_path(const char *inode_fpn, unordered_set<int> &fs_inode_set)
{
    DIR *folder = opendir(inode_fpn);
    if (folder == nullptr)
    {
        cerr << "[!] fs_walk_path: Error: Failed to read directory " << inode_fpn << ". Skipping." << endl;
        return;
    }

    struct dirent *entry = nullptr;

    while ((entry = readdir(folder)) != nullptr)
    {
        // Directory entry type
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            fs_inode_set.insert(entry->d_ino);

            // Recursively walk the subdirectory
            string path = string(inode_fpn) + "/" + entry->d_name;
            fs_walk_path(path.c_str(), fs_inode_set);
        }
        // Regular file
        else if (entry->d_type == DT_REG)
        {
            fs_inode_set.insert(entry->d_ino);
        }
    }
    closedir(folder);
}