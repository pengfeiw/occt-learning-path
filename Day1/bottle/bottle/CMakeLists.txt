# CMakeList.txt : CMake project for bottle, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# 设置 OpenCASCADEConfig.cmake 所在目录
set(OpenCASCADE_DIR "D:/program/occ/install/vs2019-x64/cmake")

find_package(OpenCASCADE REQUIRED)

message(STATUS "OpenCASCADE_DIR = ${OpenCASCADE_DIR}")
message(STATUS "OpenCASCADE_INCLUDE_DIR = ${OpenCASCADE_INCLUDE_DIR}")
message(STATUS "OpenCASCADE_LIBRARY_DIR = ${OpenCASCADE_LIBRARY_DIR}")
message(STATUS "OpenCASCADE_BINARY_DIR = ${OpenCASCADE_BINARY_DIR}")
message(STATUS "OpenCASCADE_LIBRARIES = ${OpenCASCADE_LIBRARIES}")

# Add source to this project's executable.
add_executable (bottle "bottle.cpp" "bottle.h" "main.cpp")

target_include_directories(bottle PRIVATE
    ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(bottle PRIVATE
    ${OpenCASCADE_LIBRARIES}
)

set(FREETYPE_DLL_DIR
    "D:/program/occ/3dparty/freetype-2.5.5-vc14-64/bin"
)

add_custom_command(TARGET bottle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${FREETYPE_DLL_DIR}"
    "$<TARGET_FILE_DIR:bottle>"
)

if (WIN32 AND OpenCASCADE_BINARY_DIR)
    add_custom_command(TARGET bottle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${OpenCASCADE_BINARY_DIR}"
        "$<TARGET_FILE_DIR:bottle>"
    )
endif()

# TODO: Add tests and install targets if needed.
