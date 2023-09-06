include(sources.cmake)
add_executable(template ${SOURCES})

include_directories(${BULLET_INCLUDE_DIR})

target_include_directories(template PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(template PRIVATE postaengine mingw32 SDL2main SDL2 gdi32 wsock32 ws2_32) # DEBUG, mingw32 SDL2main SDL2, for sockets gdi32 wsock32 ws2_32
target_compile_features(template PRIVATE cxx_std_17)
target_compile_options(template PRIVATE -Wall -Wpedantic)

