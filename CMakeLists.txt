cmake_minimum_required(VERSION 3.15)
project(learning-llvm)
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

message(STATUS "Found C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Found CXX Compiler: ${CMAKE_CXX_COMPILER}")

find_package(LLVM CONFIG REQUIRED)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVM include directory: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Using LLVM link directory: ${LLVM_LIBRARY_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LINK_DIRECTORIES})
add_definitions(${LLVM_DEFINITIONS})

# find_package(Clang CONFIG REQUIRED)
# message(STATUS "Using ClangConfig.cmake in: ${CLANG_CMAKE_DIR}")
# message(STATUS "Using Clang include directory: ${CLANG_INCLUDE_DIRS}")
# message(STATUS "Using Clang link directory: ${CLANG_CMAKE_DIR}/../../")
include_directories(${Clang_INCLUDE_DIRS})
# Currently I don't know how to set clang link directory directly
link_directories("${CLANG_CMAKE_DIR}/../../")

add_compile_options(-Wall -Wextra)
# connected with virtuals
# - Or use llvm-config -cxxflags
add_compile_options(-fno-exceptions -fno-rtti)
# llvm::cl depend on pthread
# - Or use llvm-config --system-libs 

add_subdirectory(cl)
add_subdirectory(clang)
