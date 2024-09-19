cmake_minimum_required(VERSION 3.27)

project(mai_labs_3_sem)

file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/src/lib/*")

add_executable(lab_1_1 main.c tasks.c tasks.h)

target_sources(lab_1_1 PRIVATE ${libs})
target_include_directories(lab_1_1 PRIVATE "${CMAKE_SOURCE_DIR}/include")
