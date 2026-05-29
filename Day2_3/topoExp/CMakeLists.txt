# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PROJECT_NAME TopoExp)
project(${PROJECT_NAME})

# 设置 OpenCASCADEConfig.cmake 所在目录
set(OpenCASCADE_DIR "../../occt/install/vs2019-x64/cmake")

find_package(OpenCASCADE REQUIRED)

message(STATUS "OpenCASCADE_DIR = ${OpenCASCADE_DIR}")
message(STATUS "OpenCASCADE_INCLUDE_DIR = ${OpenCASCADE_INCLUDE_DIR}")
message(STATUS "OpenCASCADE_LIBRARY_DIR = ${OpenCASCADE_LIBRARY_DIR}")
message(STATUS "OpenCASCADE_BINARY_DIR = ${OpenCASCADE_BINARY_DIR}")
message(STATUS "OpenCASCADE_LIBRARIES = ${OpenCASCADE_LIBRARIES}")

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "./src/main.cpp" "src/dumpshape.cpp" "src/dumpshape.h")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCASCADE_LIBRARIES}
)

# set(FREETYPE_DLL_DIR "../../occt/3rdparty/freetype-2.5.5-vc14-64/bin")

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     "${FREETYPE_DLL_DIR}"
#     "$<TARGET_FILE_DIR:bottle>"
# )

 if (WIN32 AND OpenCASCADE_BINARY_DIR)
     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_directory
         "${OpenCASCADE_BINARY_DIR}"
         "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
     )
 endif()

add_custom_command(TARGET TopoExp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bottle.brep"
        "$<TARGET_FILE_DIR:TopoExp>/bottle.brep"
)

project ("TopoExp")
