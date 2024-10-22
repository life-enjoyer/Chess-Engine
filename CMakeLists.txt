cmake_minimum_required(VERSION 3.28)
project(ENGINE C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_library(ENGINE SHARED
        game.c
        memoryUtils.c
        memoryUtils.h
        movementManager.c
        movementManager.h
        positionManager.c
        positionManager.h
        UIManager.c
        UIManager.h
        API.c
        game.h
        API.h
        wrapper.c
        consts.h
        main.c)

target_link_libraries(ENGINE -lm)


if (UNIX)
    set_target_properties(ENGINE PROPERTIES OUTPUT_NAME "engine.so")
elseif (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set_target_properties(ENGINE PROPERTIES OUTPUT_NAME "engine.dll")
    set(CMAKE_CXX_FLAGS "-s --shared")
endif (UNIX)

set_target_properties(ENGINE PROPERTIES PREFIX "")
set_target_properties(ENGINE PROPERTIES SUFFIX "")

