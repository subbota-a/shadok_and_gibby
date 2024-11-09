find_package(tomlplusplus REQUIRED)
find_package(SDL2 REQUIRED)

set(_target ${APP_ID})

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${_target} WIN32)
    target_sources(${_target} PUBLIC
        windows/resources.rc windows/manifest.manifest
    )
else()
    add_executable(${_target})
endif ()

target_sources(${_target} PRIVATE
    main.cpp
    config.cpp
    config.h
)

target_link_libraries(${_target}
    PRIVATE
    logic
    domain
    ui
    paths
    tomlplusplus::tomlplusplus
    SDL2::SDL2main
    SDL2::SDL2
)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${APP_ID}")
