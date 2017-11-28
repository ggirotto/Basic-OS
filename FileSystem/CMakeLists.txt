cmake_minimum_required(VERSION 3.7)
project(FileSystem)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c shell/utils.h shell/utils.c dir/dir_entry.h cluster/data_cluster.h shell/shell.h shell/shell.c shell/init/init_file_system.c shell/load/load_file_system.c shell/init/init_file_system.h shell/load/load_file_system.h shell/mkdir/make_directory.h shell/mkdir/make_directory.c shell/unlink/delete_directory.c shell/unlink/delete_directory.h shell/write/write_file.c shell/write/write_file.h shell/append/append_file.c shell/append/append_file.h shell/ls/list_directories.c shell/ls/list_directories.h shell/create/make_file.c shell/create/make_file.h shell/read/read_file.c shell/read/read_file.h)
add_executable(FileSystem ${SOURCE_FILES})
