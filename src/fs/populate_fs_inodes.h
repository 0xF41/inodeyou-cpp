// SPDX-License-Identifier: GPL-3.0-only
#ifndef POPULATE_FS_INODES_H
#define POPULATE_FS_INODES_H
#include "../utils/inode_utils.h"
#include <string>
#include <sys/types.h>

void populate_fs_inodes(const char *path, InodePaths &paths);

// Walk from a verified descriptor; takes ownership of fd.
void populate_fs_inodes(int fd, const std::string &path, dev_t device,
                        ino_t inode, InodePaths &paths);

#endif
