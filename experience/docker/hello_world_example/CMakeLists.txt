cmake_minimum_required(VERSION 3.10)
project(testCmake)
add_executable (say_hello hello_world.c)
install(TARGETS say_hello DESTINATION bin)
