# CallingAPIs/CMakeLists.txt
# Build the callAPI executable and demonstrate FetchContent usage.

# If you have multiple .cpp files, adjust SOURCES accordingly.
cmake_minimum_required(VERSION 3.16)

file(GLOB CALLAPI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(callAPI ${CALLAPI_SOURCES})

target_include_directories(callAPI PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- Example: fetch fmt and nlohmann/json using FetchContent ---
include(FetchContent)

# fmt (example)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.0.1
)
FetchContent_MakeAvailable(fmt)

# nlohmann/json (example)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# --- Fetch libcurl automatically ---
FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG curl-8_10_1  # You can change this to a newer tag later
)

set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# Fetch and make it available
FetchContent_MakeAvailable(curl)

# Link your program against it
target_link_libraries(callAPI PRIVATE libcurl)

target_link_libraries(callAPI PRIVATE CURL::libcurl)

# link libraries if your code uses them
# target_link_libraries(callAPI PRIVATE fmt::fmt nlohmann_json::nlohmann_json)
