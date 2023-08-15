#ifndef POSTAENGINE_LUAAPI_LUAAPI_H
#define POSTAENGINE_LUAAPI_LUAAPI_H
#include <posta/LuaAPI/LuaState.h>
#include <posta/Component/Image.h>
#include <posta/Util/Shader.h>
#include <posta/Util/Font.h>
#include <posta/UI/Button.h>
#include <posta/UI/Checkbox.h>

namespace Engine::LuaAPI {

	void load_api(LuaState& lua_state);
}

#endif // POSTAENGINE_LUAAPI_LUAAPI_H
