cmake_minimum_required(VERSION 3.27)

project(mai_labs_3_sem)

set(CMAKE_C_STANDARD "99")
set(CMAKE_C_FLAGS "-Wall -pedantic")

if (CMAKE_BUILD_TYPE STREQUAL "ASAN")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address,undefined")
endif ()

include(cmake/AddTask.cmake)

add_subdirectory("src/labs/lab-1/task-1" "src/labs/lab-1/task-1/output")
add_subdirectory("src/labs/lab-1/task-2" "src/labs/lab-1/task-2/output")
add_subdirectory("src/labs/lab-1/task-3" "src/labs/lab-1/task-3/output")
add_subdirectory("src/labs/lab-1/task-4" "src/labs/lab-1/task-4/output")
add_subdirectory("src/labs/lab-1/task-5" "src/labs/lab-1/task-5/output")
add_subdirectory("src/labs/lab-1/task-6" "src/labs/lab-1/task-6/output")
add_subdirectory("src/labs/lab-1/task-7" "src/labs/lab-1/task-7/output")
add_subdirectory("src/labs/lab-1/task-8" "src/labs/lab-1/task-8/output")
add_subdirectory("src/labs/lab-1/task-9-1" "src/labs/lab-1/task-9-1/output")
add_subdirectory("src/labs/lab-1/task-9-2" "src/labs/lab-1/task-9-2/output")
add_subdirectory("src/labs/lab-1/task-10" "src/labs/lab-1/task-10/output")
