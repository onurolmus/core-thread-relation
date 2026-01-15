cmake_minimum_required(VERSION 3.26)
project(core_thread_relation C)

set(CMAKE_C_STANDARD 11)

add_executable(core_thread_relation
        main.c
        scheduler.c
        data_generator.c
        analysis.c
        core.c
)
