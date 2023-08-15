#ifndef POSTAENGINE_LUAAPI_LUAAPI_H
#define POSTAENGINE_LUAAPI_LUAAPI_H
#include <engine/include/LuaAPI/LuaState.h>
#include <engine/include/Component/Image.h>
#include <engine/include/Util/Shader.h>
#include <engine/include/Util/Font.h>
#include <engine/include/UI/Button.h>
#include <engine/include/UI/Checkbox.h>

namespace Engine::LuaAPI {

	void load_api(LuaState& lua_state);
}

#endif // POSTAENGINE_LUAAPI_LUAAPI_H
