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
else()
    install(DIRECTORY assets DESTINATION share/${CMAKE_PROJECT_NAME} )
endif ()
