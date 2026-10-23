// SPDX-License-Identifier: GPL-3.0-only
#include "populate_tsk_inodes.h"
#include "tsk_walk_path.h"
#include <memory>
#include <stdexcept>
#include <vector>

void populate_tsk_inodes(const char *volume, const std::string &inside,
                         const std::string &mounted, Inode expected_inode, InodePaths &paths)
{
    std::unique_ptr<TSK_IMG_INFO, decltype(&tsk_img_close)> img(
        tsk_img_open_utf8_sing(volume, TSK_IMG_TYPE_RAW, 0), tsk_img_close);
    if (!img) throw std::runtime_error("TSK could not open volume");
    std::unique_ptr<TSK_FS_INFO, decltype(&tsk_fs_close)> fs(
        tsk_fs_open_img(img.get(), 0, TSK_FS_TYPE_DETECT), tsk_fs_close);
    if (!fs) throw std::runtime_error("TSK could not open filesystem at volume offset zero");
    TSK_INUM_T inode = 0;
    std::vector<char> lookup(inside.begin(), inside.end());
    lookup.push_back('\0');
    if (tsk_fs_ifind_path(fs.get(), lookup.data(), &inode) != 0)
        throw std::runtime_error("TSK could not resolve starting directory inside volume: " + inside);
    if (static_cast<Inode>(inode) != expected_inode)
        throw std::runtime_error("Raw/mounted starting inode mismatch; check volume and --mount-root");
    std::set<Inode> ancestors;
    std::size_t entries = 0;
    tsk_walk_path(fs.get(), inode, mounted, paths, ancestors, entries);
}
