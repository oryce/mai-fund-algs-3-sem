cmake_minimum_required(VERSION 3.27)

project(mai_labs_3_sem)

file(GLOB_RECURSE libs "${CMAKE_SOURCE_DIR}/src/lib/*")

add_executable(lab_1_2 main.c tasks.h exp.c pi.c ln_2.c sqrt_2.c gamma.c)

target_sources(lab_1_2 PRIVATE ${libs})
target_include_directories(lab_1_2 PRIVATE "${CMAKE_SOURCE_DIR}/include")
