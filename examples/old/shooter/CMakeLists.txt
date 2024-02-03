add_executable(shooter
	src/main.cpp
	include/Player.h
	src/Player.cpp
	include/Meat.h
	src/Meat.cpp
	include/Weapon.h
	src/Weapon.cpp
	include/WeaponItem.h
	src/WeaponItem.cpp
	include/Mapa.h
	src/Mapa.cpp
	include/Text.h
	src/Text.cpp
	include/PackagesTypes.h
)

include_directories(${BULLET_INCLUDE_DIR})

target_include_directories(shooter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(shooter PUBLIC postaengine mingw32 SDL2main SDL2 gdi32 wsock32 ws2_32) # DEBUG, mingw32 SDL2main SDL2, for sockets gdi32 wsock32 ws2_32
target_compile_features(shooter PRIVATE cxx_std_17)
target_compile_options(shooter PRIVATE -Wall -Wpedantic)
