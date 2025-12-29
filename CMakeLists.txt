cmake_minimum_required(VERSION 3.16)
project(myWeather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common build flags for curl
set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
# Disable unused curl features
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_RTSP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMB ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)

# Enable HTTPS depending on OS
if(WIN32)
  # Use Windows native SSL (no external DLLs)
  set(CURL_USE_SCHANNEL ON CACHE BOOL "Use Windows SSL backend")
else()
  # Use OpenSSL on Linux
  set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL backend")
endif()

file(GLOB CALLINGAPIS_SOURCES CONFIGURE_DEPENDS CallingAPIs/src/*.cpp)

set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(extern/wxWidgets EXCLUDE_FROM_ALL)
add_subdirectory(extern/json EXCLUDE_FROM_ALL)
add_subdirectory(extern/curl EXCLUDE_FROM_ALL)

# Where executables should be placed
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Where libraries should be placed (if you ever build any)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(myWeather ${CALLINGAPIS_SOURCES})

if(WIN32)
    add_custom_command(TARGET myWeather POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_BINARY_DIR}/extern/wxWidgets/lib/gcc_x64_dll
        $<TARGET_FILE_DIR:myWeather>
    )
endif()

target_link_libraries(myWeather PRIVATE
    wx::core
    wx::base
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

target_include_directories(myWeather PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/wxWidgets/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/curl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/CallingAPIs/include
)

#install(TARGETS ${PROJECT_NAME} DESTINATION bin)