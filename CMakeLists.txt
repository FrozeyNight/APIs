cmake_minimum_required(VERSION 3.16)
project(CallingAPIsProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # helpful for editors (clangd, etc.)

# Allow out-of-source builds by default; subprojects live in directories
add_subdirectory(CallingAPIs)
