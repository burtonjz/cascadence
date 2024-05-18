cmake_minimum_required(VERSION 3.12)
project(Cascadence)

# Compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -O2 -fPIC -ffast-math")

# Find and include libraries
find_package(PkgConfig REQUIRED)

pkg_check_modules(LV2 REQUIRED lv2)
#pkg_check_modules(CAIRO REQUIRED cairo)
#pkg_check_modules(X11 REQUIRED x11)

#========================================================================
#=========================== SYNTHESTHESIA.SO ===========================
#========================================================================

# Source Files
set(SRC_DIR src)
file(GLOB SRC "${SRC_DIR}/*.cpp")

# Set SO
add_library(Cascadence SHARED ${SRC})
set_target_properties(Cascadence PROPERTIES PREFIX "")

# add package cflags
target_compile_options(Cascadence PRIVATE
    ${LV2_CFLAGS}
)

# Include Directories
target_include_directories(Cascadence PRIVATE 
    ${LV2_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(Cascadence PRIVATE 
    ${LV2_LIBRARIES}
    m
    stdc++
)

# Linker Flags
target_link_options(Cascadence PRIVATE
    -fvisibility=hidden
    -Wl,-Bstatic
    -Wl,-Bdynamic
    -Wl,--as-needed
    -shared
    -pthread
)

#========================================================================
#============================= MAKE PACKAGE =============================
#========================================================================

set(PACKAGE_FILES
    "${CMAKE_SOURCE_DIR}/src/manifest.ttl"
    "${CMAKE_SOURCE_DIR}/src/Cascadence.ttl"
    "${CMAKE_BINARY_DIR}/Cascadence.so"
    # "${CMAKE_BINARY_DIR}/Cascadence-ui.so"
)
# file(GLOB ASSETS_FILES "${CMAKE_SOURCE_DIR}/assets/*.png")

set(INSTALL_DIR "$ENV{HOME}/.lv2")

add_custom_target(package
    DEPENDS ${PACKAGE_FILES}
    COMMENT "Packaging the LV2 Plugin"
)

add_custom_command(
    TARGET package
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/Cascadence.lv2
    COMMAND ${CMAKE_COMMAND} -E copy ${PACKAGE_FILES} ${CMAKE_BINARY_DIR}/Cascadence.lv2/ 
    # COMMAND $(CMAKE_COMMAND) -E copy ${ASSETS_FILES} ${CMAKE_BINARY_DIR}/Cascadence.lv2/assets/
)

# Install

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/Cascadence.lv2
    DESTINATION ${INSTALL_DIR}
)