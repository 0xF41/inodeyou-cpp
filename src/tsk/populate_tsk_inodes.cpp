#include <iostream>
#include <unordered_set>
#include <tsk/libtsk.h>
#include "tsk_walk_path.h"
#include "populate_tsk_inodes.h"

using namespace std;

/**
 * Recursively walks through the directory structure starting from the given inode number
 *
 * @param fs Filesystem information
 * @param dir_inode_num Directory inode number to start walking from
 * @param tsk_inode_set Set to populate with inode numbers of all directories encountered
 */
void populate_tsk_inodes(const char *mount_point, const char *root, unordered_set<int> &tsk_inode_set)
{
    TSK_IMG_INFO *img = tsk_img_open_utf8_sing(mount_point, TSK_IMG_TYPE_DETECT, 0);
    if (img == nullptr)
    {
        cerr << "Error: Failed to open image." << endl;
        return;
    }

    TSK_FS_INFO *fs = tsk_fs_open_img(img, 0, TSK_FS_TYPE_DETECT);
    if (fs == nullptr)
    {
        cerr << "Error: Failed to open filesystem." << endl;
        tsk_img_close(img);
        return;
    }

    tsk_walk_path(fs, fs->root_inum, tsk_inode_set); // Start walking from the root inode

    tsk_fs_close(fs);
    tsk_img_close(img);
}