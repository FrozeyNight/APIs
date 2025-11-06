# CallingAPIs/CMakeLists.txt
# ==========================
# Build the myWeather executable and automatically fetch dependencies.
# Cross-platform: works on Linux and Windows (with HTTPS support).

cmake_minimum_required(VERSION 3.16)
project(myWeather LANGUAGES CXX)

# Always use at least C++17 for modern libraries
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect all .cpp files automatically
file(GLOB MYWEATHER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(myWeather ${MYWEATHER_SOURCES})

# --- Include FetchContent for dependency management ---
include(FetchContent)

# ======================
# nlohmann/json (JSON)
# ======================
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3   # newer tag to avoid CMake 3.5 issue
)
FetchContent_MakeAvailable(nlohmann_json)

# ======================
# libcurl (networking)
# ======================
FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG curl-8_10_1
)

# Common build flags for curl
set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(CURL_DISABLE_LDAP ON CACHE INTERNAL "")

# Enable HTTPS depending on OS
if(WIN32)
  # Use Windows native SSL (no external DLLs)
  set(CURL_USE_SCHANNEL ON CACHE BOOL "Use Windows SSL backend")
else()
  # Use OpenSSL on Linux
  set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL backend")
endif()

FetchContent_MakeAvailable(curl)

# ======================
# Target configuration
# ======================
target_include_directories(myWeather PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link dependencies
target_link_libraries(myWeather
  PRIVATE
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

# Optional: nicer warnings
if(MSVC)
  target_compile_options(myWeather PRIVATE /W4)
else()
  target_compile_options(myWeather PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
