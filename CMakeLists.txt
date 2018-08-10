cmake_minimum_required(VERSION 3.0)
project(RCX8)
set(libsrc src/program.c src/loader.c)
add_library(objlib OBJECT ${libsrc})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

add_executable(rcx8 src/main.c ${libsrc})
add_library(rcx8_shared SHARED $<TARGET_OBJECTS:objlib>)
add_library(rcx8_static STATIC $<TARGET_OBJECTS:objlib>)
