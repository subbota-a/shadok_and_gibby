cmake_minimum_required(VERSION 3.25)
project(ShadokAndGibby VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(paths)
add_subdirectory(domain)
add_subdirectory(logic)
add_subdirectory(ui)
add_subdirectory(app)

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    string(REPLACE "/" "\\" _setup_output_dir "${CMAKE_BINARY_DIR}")
    string(REPLACE "/" "\\" _setup_binary_dir "${CMAKE_BINARY_DIR}")
    string(REPLACE "/" "\\" _setup_source_dir "${CMAKE_SOURCE_DIR}")
    configure_file(
        "${CMAKE_SOURCE_DIR}/app/windows/setup.iss.in"
        "${CMAKE_BINARY_DIR}/setup.iss"
    )
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
    install(DIRECTORY app/linux/icons DESTINATION share )
    install(PROGRAMS ${_desktop_file} DESTINATION share/applications )
    install(FILES app/linux/io.github.subbota_a.shadok_and_gibby.metainfo.xml DESTINATION share/metainfo)
endif ()
