cmake_minimum_required(VERSION 3.16)
project(SocialSteps)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base richtext html xml media webview)
include(${wxWidgets_USE_FILE})

# Find libcurl
find_package(CURL REQUIRED)
add_compile_definitions(CURL_STATICLIB)
include_directories(${CMAKE_SOURCE_DIR}/external)
# Find PortAudio
find_library(PORTAUDIO_LIB portaudio REQUIRED)
# Find libsndfile
find_library(SNDFILE_LIB sndfile REQUIRED)
# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
# Include nlohmann/json (you can also just add it as a header if not installed)
# Either use a system package or a vendored copy
# Include wxWidgets
include(${wxWidgets_USE_FILE})

# Source files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

# Create executable
add_executable(SocialSteps ${SOURCES})

# Link libraries
if(CMAKE_CROSSCOMPILING)
    target_link_libraries(SocialSteps
        ${wxWidgets_LIBRARIES}
        ${CURL_LIBRARIES}
        ssh2
        ssl
        crypto
        z
        wldap32
        crypt32
        advapi32
        bcrypt.lib
        psl
        brotlidec
        brotlienc      
        brotlicommon
        nghttp2
        idn2
        iconv         
        unistring
        ws2_32
    )
else()
    target_link_libraries(SocialSteps ${wxWidgets_LIBRARIES} ${CURL_LIBRARIES} -lpiper -lSDL2 -lportaudio -lsndfile) 
endif()

# If nlohmann_json was found, link it
if (nlohmann_json_FOUND)
    target_link_libraries(SocialSteps nlohmann_json::nlohmann_json)
endif()
