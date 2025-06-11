set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(cruzadinha-pp main.cpp)
target_link_libraries(cruzadinha-pp
    PRIVATE
    SDL3::SDL3
)
