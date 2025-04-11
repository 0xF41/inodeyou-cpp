cmake_minimum_required(VERSION 3.10)

# Set the project name
project(InodeYou)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(src/filesystem src/tsk src/utils)

# Source files
set(SOURCES
    src/main.cpp
    src/filesystem/fs_walk_path.cpp
    src/filesystem/populate_fs_inodes.cpp
    src/tsk/tsk_walk_path.cpp
    src/tsk/populate_tsk_inodes.cpp
    src/utils/sync_and_cache.cpp
)

# Create the executable
add_executable(inodeyou ${SOURCES})