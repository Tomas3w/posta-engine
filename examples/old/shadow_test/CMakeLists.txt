add_executable(shadow_test
	src/App.cpp 
	src/main.cpp
)

include_directories(${BULLET_INCLUDE_DIR})

target_include_directories(shadow_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(shadow_test PRIVATE postaengine mingw32 SDL2main SDL2 gdi32 wsock32 ws2_32) # DEBUG, mingw32 SDL2main SDL2, for sockets gdi32 wsock32 ws2_32
target_compile_features(shadow_test PRIVATE cxx_std_17)
target_compile_options(shadow_test PRIVATE -Wall -Wpedantic)

