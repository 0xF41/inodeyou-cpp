// SPDX-License-Identifier: GPL-3.0-only
#ifndef INODE_UTILS_H
#define INODE_UTILS_H
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <ostream>
#include <stdexcept>
using Inode = std::uint64_t;
constexpr std::size_t MAX_INVENTORY_ENTRIES = 1000000;
constexpr std::size_t MAX_INVENTORY_PATH_BYTES = 256 * 1024 * 1024;

struct InventoryBudget {
    std::size_t max_entries = MAX_INVENTORY_ENTRIES;
    std::size_t max_path_bytes = MAX_INVENTORY_PATH_BYTES;
    std::size_t entries = 0;
    std::size_t path_bytes = 0;

    void add(const std::string &path)
    {
        if (++entries > max_entries)
            throw std::runtime_error("Inventory entry limit exceeded");
        path_bytes += path.size();
        if (path_bytes > max_path_bytes)
            throw std::runtime_error("Inventory path-byte limit exceeded");
    }
};
using InodePaths = std::map<Inode, std::set<std::string>>;
std::string join_path(const std::string &parent, const std::string &name);
std::string volume_path(const std::string &mounted, const std::string &mount_root);
bool print_differences(const InodePaths &raw, const InodePaths &mounted, std::ostream &out, bool color = false);
#endif
