cmake_minimum_required(VERSION 3.10.2)
project(PrakBS22 C)

set(CMAKE_C_STANDARD 99)

add_executable(PrakBS22 sub.c keyValueStore.c main.c subscriberStore.c)

target_link_libraries(PrakBS22 pthread rt)