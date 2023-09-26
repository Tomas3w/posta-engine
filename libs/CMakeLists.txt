cmake_policy(SET CMP0072 NEW)

add_library(
	postaengine
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Entity/Entity.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/ResourceBag.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/LuaAPI/LuaAPI.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/LuaAPI/LuaState.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/App.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/UI/Textbox.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/UI/Button.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/HingeConstraint.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Constraint.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Skeleton.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Transform.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/StaticMesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/SmoothStaticMesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Image.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Rigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/BoxRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/CylinderRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/CapsuleRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/SphereRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/PlaneRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/MeshStaticRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Component/Texture.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Entity/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Sound.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Assets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Mesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Shader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/General.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Font.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/ConfigFile.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Network/NetworkPackage.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Network/TCPConn.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Network/TCPConnOfServer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Network/TCPConnToServer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Network/TCPConnToClient.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/Framebuffer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/posta/Util/EventEmitter.cpp"
)

target_compile_options(postaengine PUBLIC -I${CMAKE_PREFIX_PATH}/../../include/bullet)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_net REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Lua REQUIRED)

include_directories(${BULLET_INCLUDE_DIR})
find_package(Bullet REQUIRED)

target_include_directories(postaengine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${LUA_INCLUDE_DIR})
target_link_libraries(postaengine PUBLIC SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_net::SDL2_net SDL2_mixer::SDL2_mixer OpenGL::GL OpenGL::GLU GLEW::GLEW ${BULLET_LIBRARIES} ${LUA_LIBRARIES})
target_compile_features(postaengine PUBLIC cxx_std_17)
target_compile_options(postaengine PUBLIC -Wall -Wpedantic -g)

