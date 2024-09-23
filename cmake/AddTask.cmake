function(add_task task_name task_source_dir)
    file(GLOB_RECURSE lib_src "${CMAKE_SOURCE_DIR}/src/lib/*")
    file(GLOB_RECURSE task_src "${task_source_dir}/*")

    add_executable(${task_name} ${task_src} ${lib_src})

    if (APPLE AND CMAKE_BUILD_TYPE STREQUAL "ASAN")
        add_custom_command(TARGET ${task_name} POST_BUILD
                COMMAND dsymutil $<TARGET_FILE:${task_name}>
                COMMENT Generate dSYM files for ${task_name}
        )
    endif ()

    target_include_directories(${task_name} PRIVATE "${CMAKE_SOURCE_DIR}/include")
endfunction()