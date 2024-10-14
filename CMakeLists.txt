cmake_minimum_required(VERSION 3.29)
project(ShadokAndGibby)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(paths)
add_subdirectory(domain)
add_subdirectory(logic)
add_subdirectory(render)
add_subdirectory(app)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    install(DIRECTORY assets DESTINATION . )
    install(FILES README.md DESTINATION .)
    install(TARGETS shadok_and_gibby RUNTIME DESTINATION .)
    install(FILES
        sdl2/win/lib/x64/SDL2.dll
        sdl2/win/lib/x64/SDL2_image.dll
        sdl2/win/lib/x64/SDL2_mixer.dll
        sdl2/win/lib/x64/SDL2_ttf.dll
        DESTINATION .)
else()
    set(app_name shadok_and_gibby)
    set(_desktop_file "${CMAKE_BINARY_DIR}/${app_name}.desktop")
    configure_file(
        app/linux/app.desktop.in
        ${_desktop_file}
    )
    install(TARGETS shadok_and_gibby RUNTIME DESTINATION bin)
    install(DIRECTORY assets DESTINATION share/${CMAKE_PROJECT_NAME} )
    install(PROGRAMS ${_desktop_file} DESTINATION share/applications )
endif ()
