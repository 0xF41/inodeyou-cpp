// SPDX-License-Identifier: GPL-3.0-only
#include "options.h"
#include <stdexcept>

const char *const USAGE =
    "Usage: inodeyou <volume> [directory] [--mount-root <mountpoint>] "
    "[--fs-type auto|ext4|ext3|ext2|ntfs|fat|exfat|hfs|ufs|yaffs2|apfs] "
    "[--color auto|always|never]\n"
    "Exit: 0=no differences, 1=differences, 2=incomplete/error.\n";

namespace {

bool valid_fs_type(const std::string &value)
{
    static const char *const names[] = {
        "auto", "ext4", "ext3", "ext2", "ntfs", "fat", "exfat",
        "hfs", "ufs", "yaffs2", "apfs",
    };
    for (const char *name : names)
        if (value == name)
            return true;
    return false;
}

} // namespace

Options parse_options(int argc, char *argv[])
{
    if (argc < 2)
        throw std::runtime_error("Missing volume; use --help for usage");
    Options options;
    options.volume = argv[1];
    bool have_root = false;
    bool have_color = false;
    bool have_fs_type = false;

    for (int i = 2; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--mount-root" && !have_root && i + 1 < argc) {
            options.root = argv[++i];
            have_root = true;
        } else if (argument == "--color" && !have_color && i + 1 < argc) {
            options.color = argv[++i];
            have_color = true;
            if (options.color != "auto" && options.color != "always" &&
                options.color != "never")
                throw std::runtime_error("--color must be auto, always, or never");
        } else if (argument == "--fs-type" && !have_fs_type && i + 1 < argc) {
            options.fs_type = argv[++i];
            have_fs_type = true;
            if (!valid_fs_type(options.fs_type))
                throw std::runtime_error("Unsupported --fs-type: " + options.fs_type);
        } else if (argument == "--" && !options.have_start && i + 1 < argc) {
            options.start = argv[++i];
            options.have_start = true;
        } else if (!options.have_start && argument.compare(0, 2, "--") != 0) {
            options.start = argument;
            options.have_start = true;
        } else {
            throw std::runtime_error("Invalid or duplicate argument: " + argument);
        }
    }
    return options;
}
