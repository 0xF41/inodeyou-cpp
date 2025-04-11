#include <iostream>
#include <cstring>
#include <dirent.h>
#include <unordered_set>

using namespace std;

const int MAX_BUF = 256;

void fs_walk_path(const char *mount_point, const char *root, unordered_set<int> &fs_inode_set)
{
    DIR *folder = opendir(mount_point);
    if (folder == nullptr)
    {
        cerr << "Error: Failed to open directory." << endl;
        return;
    }

    struct dirent *entry = nullptr;
    int inode_num = 0;

    while ((entry = readdir(folder)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            char path[MAX_BUF];
            snprintf(path, sizeof(path), "%s/%s", mount_point, entry->d_name);
            fs_walk_path(path, root, fs_inode_set);
        }
        else if (entry->d_type == DT_REG || entry->d_type == DT_LNK)
        {
            inode_num = entry->d_ino;
            fs_inode_set.insert(inode_num);
        }
    }
    closedir(folder);
}