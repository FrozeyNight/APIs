cmake_minimum_required(VERSION 3.16)
project(myWeather LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This prevents CMake from finding libraries in C:/msys64/mingw64/lib and forcing Curl to use them.
set(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 ON)
set(CMAKE_DISABLE_FIND_PACKAGE_libssh2 ON)
if(WIN32)
    set(CMAKE_DISABLE_FIND_PACKAGE_OpenSSL ON)
endif()
set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB ON)
set(CMAKE_DISABLE_FIND_PACKAGE_NGHTTP2 ON) # Blocks HTTP/2
set(CMAKE_DISABLE_FIND_PACKAGE_nghttp2 ON)
set(CMAKE_DISABLE_FIND_PACKAGE_LibIDN2 ON) # Blocks IDN (u8_strlen errors)
set(CMAKE_DISABLE_FIND_PACKAGE_Brotli ON)
set(CMAKE_DISABLE_FIND_PACKAGE_Zstd ON)

# Prevent Curl from using pkg-config to accidentally find system libraries
set(CURL_USE_PKGCONFIG OFF CACHE BOOL "" FORCE)

set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
set(BUILD_TESTING OFF CACHE INTERNAL "")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")

# Force disable features to prevent linker errors
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
set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_MQTT ON CACHE BOOL "" FORCE)

# Disable the libraries causing errors
set(CURL_DISABLE_SSH ON CACHE BOOL "No SSH" FORCE)
set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "No LibSSH2" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "No HTTP/2" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "No IDN" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "No Zstd" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "No Brotli" FORCE)

# Enable HTTPS depending on OS
if(WIN32)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "Use Windows SSL" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "No OpenSSL" FORCE)
else()
    set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL backend")
endif()

if(MINGW OR CYGWIN OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # Ensure the program doesn't depend on libgcc_s_seh-1.dll or libstdc++-6.dll
    add_link_options(-static-libgcc -static-libstdc++)
    if(WIN32)
        add_link_options(-static)
    else()
        # Force wxWidgets to use its bundled PCRE2
        set(wxUSE_REGEX builtin CACHE STRING "Use builtin PCRE2" FORCE)
    endif()
endif()

add_subdirectory(extern/wxWidgets EXCLUDE_FROM_ALL)
add_subdirectory(extern/json EXCLUDE_FROM_ALL)
add_subdirectory(extern/curl EXCLUDE_FROM_ALL)

file(GLOB CALLINGAPIS_SOURCES CONFIGURE_DEPENDS CallingAPIs/src/*.cpp)

if(WIN32)
    add_executable(myWeather WIN32 ${CALLINGAPIS_SOURCES})
    target_sources(myWeather PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CallingAPIs/src/app.rc)
else()
    add_executable(myWeather ${CALLINGAPIS_SOURCES})
endif()

target_include_directories(myWeather PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/wxWidgets/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/curl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/CallingAPIs/include
)

target_link_libraries(myWeather PRIVATE
    wx::core
    wx::base
    nlohmann_json::nlohmann_json
    CURL::libcurl
)

if(WIN32)
    target_link_libraries(myWeather PRIVATE
        ws2_32   # Winsock
        crypt32  # Crypto API (Needed for Schannel/Native SSL)
        wldap32  # Often needed by Curl internally
        advapi32
        user32
        bcrypt   # Sometimes needed by newer Curl/Windows Crypto
    )
endif()

# Console version for Windows (.com file)
if(WIN32)
    add_executable(myWeatherConsole ${CALLINGAPIS_SOURCES})
    set_target_properties(myWeatherConsole PROPERTIES OUTPUT_NAME "myWeather" SUFFIX ".com")
    if(MSVC)
        target_link_options(myWeatherConsole PRIVATE "/ENTRY:WinMainCRTStartup")
    endif()
    target_sources(myWeatherConsole PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/CallingAPIs/src/app.rc)
    target_include_directories(myWeatherConsole PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/wxWidgets/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/curl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/json/include
        ${CMAKE_CURRENT_SOURCE_DIR}/CallingAPIs/include
    )
    target_link_libraries(myWeatherConsole PRIVATE
        wx::core
        wx::base
        nlohmann_json::nlohmann_json
        CURL::libcurl
        ws2_32
        crypt32
        wldap32
        advapi32
        user32
        bcrypt
    )
    target_compile_definitions(myWeatherConsole PRIVATE RUNNING_AS_CONSOLE)
else()
    # Creating the AppImage
    find_program(LINUXDEPLOY_CMD 
        NAMES linuxdeploy-x86_64.AppImage linuxdeploy
        PATHS ${CMAKE_SOURCE_DIR}
    )

    if(LINUXDEPLOY_CMD)
        message(STATUS "linuxdeploy found: Enabling 'appimage' build target.")
        
        set(DESKTOP_FILE "${CMAKE_SOURCE_DIR}/myWeather.desktop")
        set(ICON_FILE "${CMAKE_SOURCE_DIR}/myWeatherIcon.png")

        add_custom_target(appimage
            DEPENDS myWeather
            
            COMMAND ${LINUXDEPLOY_CMD} 
                --appdir=${CMAKE_BINARY_DIR}/AppDir 
                --executable=$<TARGET_FILE:myWeather> 
                --desktop-file=${DESKTOP_FILE} 
                --icon-file=${ICON_FILE} 
                --plugin gtk
                --output appimage
                
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Bundling dynamic libraries and building the AppImage..."
        )
    else()
        message(WARNING "linuxdeploy not found. The 'appimage' target will not be available.")
    endif()
endif()