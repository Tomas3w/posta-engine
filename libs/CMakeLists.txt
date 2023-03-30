cmake_policy(SET CMP0072 NEW)

add_library(
	postaengine
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/ResourceBag.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/LuaAPI/LuaAPI.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/LuaAPI/LuaAPI.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/LuaAPI/LuaState.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/LuaAPI/LuaState.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/App.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/App.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/UI/Textbox.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/UI/Textbox.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/UI/Rect.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/UI/Checkbox.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/UI/Button.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/UI/Button.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Skeleton.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Skeleton.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Transform.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Transform.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/StaticMesh.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/StaticMesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Camera.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Image.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Image.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Rigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Rigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/BoxRigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/BoxRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/CapsuleRigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/CapsuleRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/SphereRigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/SphereRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/PlaneRigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/PlaneRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/MeshStaticRigidbody.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/MeshStaticRigidbody.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Component/Texture.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Component/Texture.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Entity/Entity.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Entity/Camera.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Entity/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Sound.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Sound.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Thread.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Assets.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Assets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Mesh.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Mesh.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Shader.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Shader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/General.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/General.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Font.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Font.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/ConfigFile.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/ConfigFile.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/Logging.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/LoggingMacro.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/NetworkPackage.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/NetworkPackage.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/TCPConn.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/TCPConn.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/TCPConnOfServer.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/TCPConnOfServer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/TCPConnToServer.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/TCPConnToServer.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/include/Util/TCPConnToClient.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/TCPConnToClient.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/engine/src/Util/Framebuffer.cpp"
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

target_include_directories(postaengine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${LUA_INCLUDE_DIR})
target_link_libraries(postaengine PUBLIC SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_net::SDL2_net SDL2_mixer::SDL2_mixer OpenGL::GL OpenGL::GLU GLEW::GLEW ${BULLET_LIBRARIES} ${LUA_LIBRARIES})
target_compile_features(postaengine PUBLIC cxx_std_17)
target_compile_options(postaengine PUBLIC -Wall -Wpedantic)

