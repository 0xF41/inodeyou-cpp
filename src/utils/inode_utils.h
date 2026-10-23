// SPDX-License-Identifier: GPL-3.0-only
#ifndef INODE_UTILS_H
#define INODE_UTILS_H
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <ostream>
using Inode = std::uint64_t;
constexpr std::size_t MAX_INVENTORY_ENTRIES = 1000000;
using InodePaths = std::map<Inode, std::set<std::string>>;
std::string join_path(const std::string &parent, const std::string &name);
std::string volume_path(const std::string &mounted, const std::string &mount_root);
bool print_differences(const InodePaths &raw, const InodePaths &mounted, std::ostream &out, bool color = false);
#endif
