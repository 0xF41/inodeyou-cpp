#include "inode_utils.h"
#include "output.h"
#include <iomanip>
#include <stdexcept>

std::string join_path(const std::string &parent, const std::string &name)
{
    return (parent == "/" ? "/" : parent + "/") + name;
}

std::string volume_path(const std::string &mounted, const std::string &root)
{
    if (root == "/") return mounted;
    if (mounted == root) return "/";
    if (mounted.compare(0, root.size() + 1, root + "/") != 0)
        throw std::runtime_error("Starting directory is outside --mount-root");
    return mounted.substr(root.size());
}

static void print_paths(std::ostream &out, const char *label, Inode inode,
                        const std::set<std::string> &paths, bool color, const char *tone)
{
    out << paint(std::string("[!] ") + label, tone, color)
        << " | " << paint("inode=" + std::to_string(inode), "1;36", color) << '\n';
    for (const auto &path : paths) {
        // Escape control bytes so filenames cannot inject terminal commands/lines.
        out << "    " << paint("Path:", "36", color) << " \"";
        for (unsigned char c : path) {
            if (c == '\\' || c == '"') out << '\\' << c;
            else if (c < 32 || c >= 127)
                out << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<unsigned>(c) << std::dec;
            else out << c;
        }
        out << "\"\n";
    }
}

bool print_differences(const InodePaths &raw, const InodePaths &mounted, std::ostream &out, bool color)
{
    bool different = false;
    for (const auto &item : raw) {
        auto found = mounted.find(item.first);
        if (found == mounted.end()) {
            different = true;
            print_paths(out, "RAW-ONLY: hidden-entry candidate", item.first, item.second, color, "1;31");
        } else {
            std::set<std::string> raw_only, mounted_only;
            for (const auto &p : item.second)
                if (!found->second.count(p)) raw_only.insert(p);
            for (const auto &p : found->second)
                if (!item.second.count(p)) mounted_only.insert(p);
            if (!raw_only.empty() || !mounted_only.empty()) different = true;
            if (!raw_only.empty()) print_paths(out, "NAME-ONLY IN RAW VIEW", item.first, raw_only, color, "1;31");
            if (!mounted_only.empty()) print_paths(out, "NAME-ONLY IN MOUNTED VIEW", item.first, mounted_only, color, "1;33");
        }
    }
    for (const auto &item : mounted) {
        if (!raw.count(item.first)) {
            different = true;
            print_paths(out, "MOUNTED-ONLY: reverse discrepancy", item.first, item.second, color, "1;33");
        }
    }
    if (!different) out << paint("[*] No differences found between TSK and filesystem inode/path inventories.", "1;32", color) << '\n';
    return different;
}
