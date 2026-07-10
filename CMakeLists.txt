cmake_minimum_required(VERSION 3.10)

project(elfinspect
    VERSION 0.1.0
    LANGUAGES C
)

set(APP_NAME elfinspect)

file(GLOB_RECURSE ELFINSPECT_SRC
    ${PROJECT_SOURCE_DIR}/src/*.c
)

add_executable(${APP_NAME}
    main.c
    ${ELFINSPECT_SRC}
)

target_include_directories(${APP_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

