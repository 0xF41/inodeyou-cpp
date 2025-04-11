#include <iostream>
#include <unordered_set>
#include <tsk/libtsk.h>
#include "tsk_walk_path.h"
#include <string.h>

void tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_inode_num, std::unordered_set<int> &tsk_inode_set)
{
    TSK_FS_DIR *fs_dir = tsk_fs_dir_open_meta(fs, dir_inode_num);
    if (fs_dir == nullptr)
    {
        std::cerr << "Error: Failed to open directory." << std::endl;
        return;
    }

    TSK_FS_FILE *fs_file = nullptr;
    TSK_INUM_T inode_num;
    for (size_t i = 0, n = tsk_fs_dir_getsize(fs_dir); i < n; i++)
    {
        inode_num = -1;
        fs_file = tsk_fs_dir_get(fs_dir, i);
        if (fs_file == nullptr)
        {
            std::cerr << "Error: Failed to get file from directory." << std::endl;
            exit(1);
        }

        if (fs_file->name->flags & TSK_FS_NAME_FLAG_UNALLOC)
        {
            tsk_fs_file_close(fs_file);
            continue;
        }

        if (!strcmp(fs_file->name->name, ".") || !strcmp(fs_file->name->name, ".."))
        {
            tsk_fs_file_close(fs_file);
            continue;
        }

        if (fs_file->meta->type == TSK_FS_META_TYPE_DIR && strcmp(fs_file->name->name, "$OrphanFiles") != 0)
        {
            tsk_inode_set.insert(fs_file->meta->addr);
            tsk_walk_path(fs, fs_file->meta->addr, tsk_inode_set);
        }
    }

    tsk_fs_dir_close(fs_dir);
}