cmake_minimum_required(VERSION 3.28)
project(AI C)

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
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (UNIX)
    set_target_properties(ENGINE PROPERTIES OUTPUT_NAME "engine.so")
elseif (WIN32)
    set_target_properties(ENGINE PROPERTIES OUTPUT_NAME "engine.dll")
endif (UNIX)

set_target_properties(ENGINE PROPERTIES PREFIX "")
set_target_properties(ENGINE PROPERTIES SUFFIX "")

