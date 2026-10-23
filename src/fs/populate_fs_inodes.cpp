// SPDX-License-Identifier: GPL-3.0-only
#include "populate_fs_inodes.h"
#include "fs_walk_path.h"

void populate_fs_inodes(const char *path, InodePaths &paths)
{
    fs_walk_root(path, paths);
}

void populate_fs_inodes(int fd, const std::string &path, dev_t device,
                        ino_t inode, InodePaths &paths)
{
    fs_walk_fd(fd, path, device, inode, paths);
}
