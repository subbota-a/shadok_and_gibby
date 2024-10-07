cmake_minimum_required(VERSION 3.29)
project(ShadokAndGibby)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(domain)
add_subdirectory(logic)
add_subdirectory(render)
add_subdirectory(app)

install(DIRECTORY assets DESTINATION share/${CMAKE_PROJECT_NAME} )
