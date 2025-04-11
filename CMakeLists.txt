cmake_minimum_required(VERSION 3.10)

# Set the project name
project(InodeYou)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(src/fs src/tsk src/utils)

# Source files
set(SOURCES
    src/main.cpp
    src/fs/fs_walk_path.cpp
    src/fs/populate_fs_inodes.cpp
    src/tsk/tsk_walk_path.cpp
    src/tsk/populate_tsk_inodes.cpp
    src/utils/sync_and_cache.cpp
    src/utils/common.cpp
    src/utils/inode_utils.cpp
)

# Find and link the Sleuth Kit library
find_library(TSK_LIB tsk REQUIRED)
if (NOT TSK_LIB)
    message(FATAL_ERROR "Sleuth Kit library not found. Please install libtsk-dev.")
endif()

# Create the executable
add_executable(inodeyou ${SOURCES})

# Link the Sleuth Kit library
target_link_libraries(inodeyou ${TSK_LIB})