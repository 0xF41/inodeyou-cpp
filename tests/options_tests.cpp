// SPDX-License-Identifier: GPL-3.0-only
#include "options.h"
#include <iostream>
#include <stdexcept>

namespace {

bool throws(int argc, char *argv[])
{
    try {
        parse_options(argc, argv);
    } catch (const std::runtime_error &) {
        return true;
    }
    return false;
}

} // namespace

int main()
{
    try {
        {
            char *argv[] = {const_cast<char *>("inodeyou"),
                            const_cast<char *>("/dev/sda1")};
            const Options options = parse_options(2, argv);
            if (options.volume != "/dev/sda1" || options.root != "/" ||
                options.start != "/" || options.color != "auto" ||
                options.fs_type != "auto" || options.have_start)
                throw std::runtime_error("default options mismatch");
        }
        {
            char *argv[] = {const_cast<char *>("inodeyou"),
                            const_cast<char *>("/dev/sda1"),
                            const_cast<char *>("/mnt/data/demo"),
                            const_cast<char *>("--mount-root"),
                            const_cast<char *>("/mnt/data"),
                            const_cast<char *>("--fs-type"),
                            const_cast<char *>("ext4"),
                            const_cast<char *>("--color"),
                            const_cast<char *>("never")};
            const Options options = parse_options(9, argv);
            if (!options.have_start || options.start != "/mnt/data/demo" ||
                options.root != "/mnt/data" || options.fs_type != "ext4" ||
                options.color != "never")
                throw std::runtime_error("explicit options mismatch");
        }
        {
            char *argv[] = {const_cast<char *>("inodeyou"),
                            const_cast<char *>("/dev/sda1"),
                            const_cast<char *>("--"),
                            const_cast<char *>("--directory")};
            const Options options = parse_options(4, argv);
            if (!options.have_start || options.start != "--directory")
                throw std::runtime_error("end-of-options mismatch");
        }
        {
            char *argv[] = {const_cast<char *>("inodeyou"),
                            const_cast<char *>("/dev/sda1"),
                            const_cast<char *>("--fs-type"),
                            const_cast<char *>("unknown")};
            if (!throws(4, argv))
                throw std::runtime_error("invalid fs type accepted");
        }
        {
            char *argv[] = {const_cast<char *>("inodeyou"),
                            const_cast<char *>("/dev/sda1"),
                            const_cast<char *>("one"),
                            const_cast<char *>("two")};
            if (!throws(4, argv))
                throw std::runtime_error("duplicate directory accepted");
        }
        return 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
