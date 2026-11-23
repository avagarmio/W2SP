cmake_minimum_required(VERSION 3.16)
project(ringbuf C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -fsanitize=address,undefined)
add_link_options(-fsanitize=address,undefined)

add_library(ringbuf SHARED ringbuf.c)

add_executable(rb_test main.c)

target_link_libraries(rb_test PRIVATE ringbuf)
