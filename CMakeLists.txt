cmake_minimum_required(VERSION 3.25)
project(ShadokAndGibby VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_ID shadok_and_gibby CACHE STRING "Executable name of the application")
if (DEFINED ENV{FLATPAK_ID})
    set(APP_ID $ENV{FLATPAK_ID} CACHE STRING "Executable name of the application" FORCE)
endif ()
message(STATUS "APP_ID=${APP_ID}")

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
    set(_desktop_file "${CMAKE_BINARY_DIR}/${APP_ID}.desktop")
    set(_metainfo_file "${CMAKE_BINARY_DIR}/${APP_ID}.metainfo.xml")
    configure_file(
        app/linux/app.desktop
        ${_desktop_file}
    )
    configure_file(
        app/linux/metainfo.xml
        ${_metainfo_file}
    )

    install(TARGETS ${APP_ID} RUNTIME DESTINATION bin)
    install(DIRECTORY assets DESTINATION share/${APP_ID} )
    install(PROGRAMS ${_desktop_file} DESTINATION share/applications )
    install(FILES ${_metainfo_file} DESTINATION share/metainfo)

    file(GLOB_RECURSE ICON_FILES LIST_DIRECTORIES false RELATIVE "${CMAKE_SOURCE_DIR}/app/linux/icons" "icon.*" )
    foreach(icon_file ${ICON_FILES})
        cmake_path(GET icon_file PARENT_PATH rel_dir)
        cmake_path(GET icon_file EXTENSION LAST_ONLY extension)
        install(FILES "app/linux/icons/${icon_file}" DESTINATION "share/icons/${rel_dir}" RENAME "${APP_ID}${extension}")
    endforeach()

endif ()
