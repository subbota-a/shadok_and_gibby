set(_target shadok_and_gibby)
add_executable(${_target}
    main.cpp
)

target_link_libraries(${_target}
    PRIVATE
    logic
    domain
    render
)
