#include <posta/LuaAPI/LuaState.h>

using posta::LuaAPI::LuaState;

std::unordered_map<std::string, std::string> LuaState::LuaClass::type_lua_names;

LuaState::LuaState()
{
	L = luaL_newstate();
	luaL_openlibs(L);
}

LuaState::~LuaState()
{
	lua_close(L);
}

void LuaState::run_file(std::filesystem::path path)
{
	if (luaL_dofile(L, path.string().c_str()))
		std::cout << lua_tostring(L, -1) << std::endl;
}

void LuaState::add_lib(LuaLibrary& lib)
{
	push_sublib(lib);
	lua_setglobal(L, lib.name.c_str());
}

void LuaState::push_sublib(LuaLibrary& lib)
{
	lua_newtable(L);
	// loading functions into the lib table
	for (auto& func : lib.functions)
	{
		lua_pushcfunction(L, func.func);
		lua_setfield(L, -2, func.name.c_str());
	}
	// loading classes
	for (auto& cls : lib.classes)
	{
		// loading the class constructor in the sublib
		if (cls.static_functions.empty()) // an class name is just a function if there are no static functions
			lua_pushcfunction(L, cls.ctor);
		else // if static functions exists for this type, then the class name acts as a table with functions, and as a callable table
		{
			lua_newtable(L); 
			// filling main table with the static functions
			for (auto& func : cls.static_functions)
			{
				lua_pushcfunction(L, func.func);
				lua_setfield(L, -2, func.name.c_str());
			}
			// adding the constructor to the table
			lua_newtable(L); // metatable
			lua_pushcfunction(L, cls.ctor);
			lua_setfield(L, -2, "__call"); // sets the field of the metatable with the constructor
			lua_setmetatable(L, -2);
		}
		lua_setfield(L, -2, cls.name.c_str());
		// creating metatable to add the methods and the destructor
		luaL_newmetatable(L, cls.metatable_name.c_str());
		lua_pushcfunction(L, cls.dtor);
		lua_setfield(L, -2, "__gc");
		lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
		for (auto& func : cls.methods)
		{
			lua_pushcfunction(L, func.func);
			lua_setfield(L, -2, func.name.c_str());
		}
		lua_pop(L, 1);
	}
	// loading sublibraries into the library
	for (auto& l : lib.sublibraries)
	{
		push_sublib(l);
		lua_setfield(L, -2, l.name.c_str());
	}
}

