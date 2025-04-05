cmake_minimum_required(VERSION 3.22)
project(VoxelGame)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/include
)
file(GLOB_RECURSE SOURCES src/*.cpp)

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    OpenGL::GL 
    glad::glad 
    glfw 
    glm::glm
    fmt::fmt
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
    DEPENDS ${CMAKE_SOURCE_DIR}/res
)

add_dependencies(${PROJECT_NAME} copy_resources)