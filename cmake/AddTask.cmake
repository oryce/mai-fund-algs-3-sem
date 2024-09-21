function(add_task task_name task_source_dir)
    file(GLOB_RECURSE lib_src "${CMAKE_SOURCE_DIR}/src/lib/*")
    file(GLOB_RECURSE task_src "${task_source_dir}/*")

    add_executable(${task_name} ${task_src} ${lib_src})

    target_include_directories(${task_name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
endfunction()