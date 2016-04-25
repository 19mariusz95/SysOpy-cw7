cmake_minimum_required(VERSION 3.4)
project(zad2 C)

set(CMAKE_C_FLAGS "-Wall -lrt -lpthread")

add_executable(host host.c)
add_executable(writer writer.c)
add_executable(reader reader.c)
