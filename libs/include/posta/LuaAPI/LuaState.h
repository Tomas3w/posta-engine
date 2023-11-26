#ifndef POSTAENGINE_LUASTATE_LUAAPI_H
#define POSTAENGINE_LUASTATE_LUAAPI_H
#include <lua.hpp>
#include <filesystem>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <cxxabi.h>
#include <tuple>
#include <unordered_map>
#include <SDL2/SDL.h>

namespace posta::LuaAPI
{
	/// Class for having lua objects derived from real types, but that may not be created in lua directly
	/// The class contains a pointer to the actual type but the class as a whole may not be the owner of this pointer
	/// It can also be used to return nil for the functions LuaCFunction::from_method and LuaCFunction::from_func (using the nil static member function)
	template <class T>
	class MayNotBeOwner
	{
	public:
		static MayNotBeOwner<T> nil()
		{
			auto mnbo = MayNotBeOwner<T>(nullptr, false);
			mnbo.nil_state = true;
			return mnbo;
		}
		MayNotBeOwner(T* _t, bool _is_owner)
		{
			t = _t;
			is_owner = _is_owner;
		}
		MayNotBeOwner(const MayNotBeOwner& o)
		{
			if (o.is_owner)
				throw std::logic_error("MayNotBeOwner cannot be copied if the object being copied is a owner of its data (in MayNotBeOwner copy constructor)");
			t = o.t;
			is_owner = o.is_owner;
		}
		MayNotBeOwner& operator=(const MayNotBeOwner& o)
		{
			if (o.is_owner)
				throw std::logic_error("MayNotBeOwner cannot be copied if the object being copied is a owner of its data (in MayNotBeOwner::operator=)");
			else if (is_owner)
				delete t;
			t = o.t;
			is_owner = o.is_owner;
		}
		~MayNotBeOwner()
		{
			if (is_owner)
				delete t;
		}

		T* operator->()
		{
			return t;
		}

		T* get()
		{
			return t;
		}

		bool is_nil() const
		{
			return nil_state;
		}
	private:
		T* t;
		bool is_owner;
		bool nil_state = false;
	};
	/// A lua state class. For internal use
	class LuaState
	{
		public:
			template <class T>
			struct MayNotBeOwnerIsNil
			{
				MayNotBeOwnerIsNil(T* ptr) {}
				operator bool() { return false; }
			};
			template <class T>
			struct MayNotBeOwnerIsNil<MayNotBeOwner<T>>
			{
				MayNotBeOwnerIsNil(MayNotBeOwner<T>* ptr)
				{
					_ptr = ptr;
				}
				operator bool() { return _ptr->is_nil(); }
				MayNotBeOwner<T>* _ptr;
			};
			struct LuaCFunction
			{
				LuaCFunction(std::string _name, lua_CFunction _func) : name(_name), func(_func) {};
				std::string name;
				lua_CFunction func;
				/// Creates a LuaCFunction form a function
				template <class F, F f, class... Args>
				static LuaCFunction from_func(std::string name)
				{
					//std::cout << "Before telling a story... the name: " << name << std::endl;
					return LuaCFunction(name, func_func<F, f, Args...>);
				}
				/// Creates a LuaCFunction from a method
				template <class T, class F, F f, class... Args>
				static LuaCFunction from_method(std::string name)
				{
					return LuaCFunction(name, method_func<T, F, f, Args...>);
				}

				// Functions listed bellow (public functions) are to be used in lua c functions
				/// Wrapper around pointers to arguments (returned by getarg)
				template <class T>
				struct unique_ptr
				{
					unique_ptr(T* _ptr) : ptr(_ptr) {}
					~unique_ptr() { freearg(ptr); }
					operator bool()
					{
						return ptr != nullptr;
					}
					T* operator->() { return ptr; }
					T& operator*() { return *ptr; }
					T* get()
					{
						return ptr;
					}
				private:
					T* ptr;
				};
				/// Checks if arg == nullptr, raising an error in lua if that is the case
				template <class T>
				static void checkarg(lua_State* L, T* arg, int arg_index)
				{
					if (arg == nullptr)
					{
						std::string full_name = LuaClass::get_fullname_of_type<T>();
						std::string name = LuaClass::get_name_from_fullname_of_type(full_name);
						if constexpr(std::is_same_v<T, std::string>)
							name = "string";
						if constexpr(std::is_arithmetic_v<T>)
							name = "number";
						if constexpr(std::is_same_v<T, bool>)
							name = "boolean";
						luaL_typeerror(L, arg_index, name.c_str());
					}
				}
				/// Pushes any object into the lua stack (copies the object)
				static void push_nil(lua_State* L)
				{
					lua_pushnil(L);
				}
				/// Copies value onto the lua stack
				template <class T>
				static void push_value(lua_State* L, T& t)
				{
					if constexpr(std::is_same<T, bool>::value)
						lua_pushboolean(L, t);
					else if constexpr(std::is_arithmetic_v<T>)
						lua_pushnumber(L, t);
					else if constexpr(std::is_same<T, std::string>::value)
						lua_pushlstring(L, t.c_str(), t.size());
					else
					{
						std::string full_name = LuaClass::get_fullname_of_type<T>();
						*reinterpret_cast<T**>(lua_newuserdatauv(L, sizeof(T*), 0)) = new T(t);
						luaL_setmetatable(L, full_name.c_str());
					}
				}
				/// Moves pointer to type onto the stack (as userdata)
				template <class T>
				static void push_uvalue(lua_State* L, T* t)
				{
					if constexpr(is_of_special_type<T>())
						throw std::logic_error(("push_uvalue only accepts pointers to userdata types, got " + LuaClass::get_fullname_of_type<T>()).c_str());
					std::string full_name = LuaClass::get_fullname_of_type<T>();
					*reinterpret_cast<T**>(lua_newuserdatauv(L, sizeof(T*), 0)) = t;
					luaL_setmetatable(L, full_name.c_str());
				}
				/// Frees an argument, which may imply deleting it, but only if T is a pointer to std::string or double
				template <class T>
				static void freearg(T* arg)
				{
					if constexpr(is_of_special_type<T>())
						delete arg;
				}
				/// Gets an argument, once used the return value must be free with freearg, returns nullptr on error
				template <class T>
				static T* getarg(lua_State* L, int arg_offset)
				{
					//std::cout << "c" << std::endl;
					if constexpr(std::is_same<T, bool>::value)
					{
						if (lua_type(L, arg_offset) == LUA_TBOOLEAN)
							return new T(lua_toboolean(L, arg_offset));
						else
							return nullptr; // on error returns nullptr
					}
					else if constexpr(std::is_arithmetic_v<T>)
					{
						//std::cout << "d " << arg_offset << " " << lua_type(L, arg_offset) << " " << LUA_TNUMBER << std::endl;
						if (lua_type(L, arg_offset) == LUA_TNUMBER)
							return new T(lua_tonumber(L, arg_offset));
						else
						{
							//std::cout << "error" << std::endl;
							return nullptr; // on error returns nullptr
						}
					}
					else if constexpr(std::is_same<T, std::string>::value)
					{
						//std::cout << "e" << std::endl;
						if (lua_type(L, arg_offset) == LUA_TSTRING)
						{
							size_t len;
							const char* value = lua_tolstring(L, arg_offset, &len);
							return new std::string(value, len);
						}
						else
							return nullptr; // on error returns nullptr
					}
					else
					{
						//std::cout << "f" << std::endl;
						if (lua_type(L, arg_offset) == LUA_TUSERDATA)
						{
							void* ud = luaL_testudata(L, arg_offset, LuaClass::get_fullname_of_type<T>().c_str());
							if (ud == nullptr)
								return nullptr;
							return *reinterpret_cast<T**>(ud);
						}
						else
							return nullptr; // on error returns nullptr
					}
					//lua_type(L, arg_offset);
				}
			private:
				/// Returns tuple with the arguments of the function, arg_offset specifies the first argument and nargs specifies the number o arguments
				/// The number of arguments (nargs) should include the first one of a method call too
				/// The return tuple only contains pointers, if a pointer is of type double* or std::string* then it should be deleted after use
				template <class... Ts>
				static std::tuple<Ts*...> function_getargs(lua_State* L, int arg_offset = 1)
				{
					std::tuple<Ts*...> args;
					//std::cout << "a" << std::endl;
					fill_args<0, Ts...>(args, L, arg_offset);
					return args;
				}
				template <class... Ts>
				static std::tuple<Ts&...> function_pargs_to_rargs(std::tuple<Ts*...> tp)
				{
					return function_pargs_to_rargs_aux(std::index_sequence_for<Ts...>{}, tp);
				}
				template <std::size_t... Ns, class... Ts>
				static std::tuple<Ts&...> function_pargs_to_rargs_aux(std::index_sequence<Ns...>, std::tuple<Ts*...> tp)
				{
					return std::tuple<Ts&...>(*std::get<Ns>(tp)...);
				}
				// checks if T is of special type
				template <class T>
				constexpr static bool is_of_special_type()
				{
					return std::is_same_v<T, std::string> || std::is_same_v<T, bool> || std::is_arithmetic_v<T>;
				}
				// free elements of args
				template<size_t arg_offset, std::size_t I = 0, typename... Tp>
				inline typename std::enable_if<I == sizeof...(Tp), void>::type
				static check_args_elements(std::tuple<Tp*...> &, lua_State*)
					{}
				template<size_t arg_offset, std::size_t I = 0, typename... Tp>
				inline typename std::enable_if<I < sizeof...(Tp), void>::type
				static check_args_elements(std::tuple<Tp*...>& t, lua_State* L)
				{
					//std::cout << "ch " << I << " (" << arg_offset << ")" << std::endl;
					if (std::get<I>(t) == nullptr)
					{
						//std::cout << "This is null, right? " << I << " " << arg_offset << std::endl;
						std::string full_name = LuaClass::get_fullname_of_type<typename std::tuple_element<I, std::tuple<Tp...>>::type>();
						std::string name = LuaClass::get_name_from_fullname_of_type(full_name);
						if constexpr(std::is_same_v<typename std::tuple_element<I, std::tuple<Tp...>>::type, std::string>)
							name = "string";
						if constexpr(std::is_arithmetic_v<typename std::tuple_element<I, std::tuple<Tp...>>::type>)
							name = "number";
						if constexpr(std::is_same_v<typename std::tuple_element<I, std::tuple<Tp...>>::type, bool>)
							name = "boolean";
						luaL_typeerror(L, I + arg_offset, name.c_str());
					}
					check_args_elements<arg_offset, I + 1, Tp...>(t, L);
				}
				template<size_t arg_offset, std::size_t I = 0, typename... Tp>
				inline typename std::enable_if<I == sizeof...(Tp), void>::type
				static free_args_elements(std::tuple<Tp*...> &, lua_State*)
					{}
				template<size_t arg_offset, std::size_t I = 0, typename... Tp>
				inline typename std::enable_if<I < sizeof...(Tp), void>::type
				static free_args_elements(std::tuple<Tp*...>& t, lua_State* L)
				{
					//std::cout << "g " << I << " (" << arg_offset << ")" << std::endl;
					auto& e = std::get<I>(t);
					if constexpr(is_of_special_type<typename std::tuple_element<I, std::tuple<Tp...>>::type>())//std::is_same_v<typename std::tuple_element<I, std::tuple<Tp...>>::type, std::string> || std::is_arithmetic_v<typename std::tuple_element<I, std::tuple<Tp...>>::type>)
					{
						//std::cout << "you deleted, right? " << I << " " << LuaClass::get_fullname_of_type<typename std::tuple_element<I, std::tuple<Tp...>>::type>() << std::endl;
						delete e;
					}
					free_args_elements<arg_offset, I + 1, Tp...>(t, L);
				}
				template <class T, class F, F f, class... Args>
				static int method_func(lua_State* L)
				{
					T* self = LuaState::LuaClass::method_checkself<T>(L);

					auto args = LuaState::LuaCFunction::function_getargs<Args...>(L, 2);
					LuaState::LuaCFunction::check_args_elements<2>(args, L);
					auto reference_tuple = LuaState::LuaCFunction::function_pargs_to_rargs<Args...>(args);
					if constexpr(std::is_same_v<std::invoke_result_t<F, T, Args&...>, void>)
					{
						std::apply(f, std::tuple_cat(std::forward_as_tuple(*self), reference_tuple));
						return 0;
					}
					else
					{
						auto rvalue = std::apply(f, std::tuple_cat(std::forward_as_tuple(*self), reference_tuple));
						LuaState::LuaCFunction::free_args_elements<2>(args, L);
						if (MayNotBeOwnerIsNil<decltype(rvalue)>(&rvalue))
						{
							//std::cout << "is this happening (method_func)?" << std::endl;
							LuaState::LuaCFunction::push_nil(L);
						}
						else
							LuaState::LuaCFunction::push_value(L, rvalue);
						return 1;
					}
				}
				template <class F, F f, class... Args>
				static int func_func(lua_State* L)
				{
					auto args = LuaState::LuaCFunction::function_getargs<Args...>(L, 1);
					LuaState::LuaCFunction::check_args_elements<1>(args, L);
					auto reference_tuple = LuaState::LuaCFunction::function_pargs_to_rargs<Args...>(args);
					if constexpr(std::is_same_v<std::invoke_result_t<F, Args&...>, void>)
					{
						std::apply(f, reference_tuple);
						return 0;
					}
					else
					{
						auto rvalue = std::apply(f, reference_tuple);
						LuaState::LuaCFunction::free_args_elements<1>(args, L);
						if (MayNotBeOwnerIsNil<decltype(rvalue)>(&rvalue))
						{
							//std::cout << "is this happening (func_func)?" << std::endl;
							LuaState::LuaCFunction::push_nil(L);
						}
						else
							LuaState::LuaCFunction::push_value(L, rvalue);
						return 1;
					}
				}
				template <const int i, class... Ts>
				static void fill_args(std::tuple<Ts*...>& args, lua_State* L, int arg_offset)
				{
					if constexpr(i == std::tuple_size_v<std::tuple<Ts*...>>)
						return;
					else
					{
						//std::cout << "b" << std::endl;
						std::get<i>(args) = getarg<typename std::tuple_element<i, std::tuple<Ts...>>::type>(L, arg_offset + i);
						fill_args<i + 1>(args, L, arg_offset);
						//if (std::get<i>(args)) // stops on error
						//	return;
					}
				}
			};
			struct LuaClass
			{
				static std::unordered_map<std::string, std::string> type_lua_names;
				template <class T>
				static std::string get_fullname_of_type()
				{
					int status;
					std::string r = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);

					std::unordered_map<std::string, std::string>::iterator it;
					if ((it = type_lua_names.find(r)) != type_lua_names.end())
						return it->second;
					return r;
				}
				template <class T>
				static void add_alias_to_class(std::string alias)
				{
					type_lua_names[get_fullname_of_type<T>()] = alias;
				}
				static std::string get_name_from_fullname_of_type(std::string& full_name)
				{
					return full_name.substr(full_name.find_last_of(':') + 1);
				}
				template <class T>
				static int dtor_func(lua_State* L)
				{
					//std::cout << "deleting " << get_fullname_of_type<T>() << "..." << std::endl;
					delete *reinterpret_cast<T**>(lua_touserdata(L, 1));
					return 0;
				}
				template <class T, T* (*ctor) (lua_State*), const bool has_static_functions>
				static int ctor_func(struct lua_State* L)
				{
					std::string full_name = get_fullname_of_type<T>();
					std::string name = get_name_from_fullname_of_type(full_name);
					//std::cout << "creating " << name << "(" << full_name << ")..." << std::endl;

					// removes the first argument if the class has static functions
					// this is used to remove the element that is automatically added when using a table instead of a cfunction as a constructor
					if constexpr(has_static_functions)
						lua_remove(L, 1);
					*reinterpret_cast<T**>(lua_newuserdatauv(L, sizeof(T*), 0)) = ctor(L);
					luaL_setmetatable(L, full_name.c_str());
					return 1;
				}
				template <class T, T* (*ctor) (lua_State*), const bool has_static_functions>
				static LuaClass from_class()
				{
					return LuaState::LuaClass::from_class_aux<T>(LuaState::LuaClass::ctor_func<T, ctor, has_static_functions>);
				}
				template <class T>
				static LuaClass from_class_aux(int (*ctor)(lua_State*))
				{
					std::string full_name = get_fullname_of_type<T>();
					std::string name = get_name_from_fullname_of_type(full_name);
					//std::cout << "creating lua class: " << name << "(" << full_name << ")" << std::endl;
					return LuaClass(name, full_name, ctor, dtor_func<T>);
				}
				/// Methods magic
				template <class T>
				static T* method_checkself(lua_State* L)
				{
					std::string full_name = get_fullname_of_type<T>();
					void *ud = luaL_checkudata(L, 1, full_name.c_str());
					luaL_argcheck(L, ud != nullptr, 1, (std::string("'") + get_name_from_fullname_of_type(full_name) + "' expected").c_str());
					return *reinterpret_cast<T**>(ud);
				}
				LuaClass(std::string _name, std::string _metatable_name, lua_CFunction _ctor, lua_CFunction _dtor) : name(_name), metatable_name(_metatable_name), ctor(_ctor), dtor(_dtor) {}
				std::string name;
				std::string metatable_name;
				lua_CFunction ctor;
				lua_CFunction dtor;
				std::vector<LuaCFunction> methods;
				std::vector<LuaCFunction> static_functions;
			};
			struct LuaLibrary
			{
				LuaLibrary(std::string _name) : name(_name) {}
				std::string name;
				std::vector<LuaClass> classes;
				std::vector<LuaCFunction> functions;
				std::vector<LuaLibrary> sublibraries;
			};
			/// Creates a lua state
			LuaState();
			~LuaState();
			LuaState(const LuaState&) = delete;
			LuaState& operator=(const LuaState&) = delete;

			/// Runs file in the lua state
			void run_file(std::filesystem::path path);

			void add_lib(LuaLibrary& lib);
		private:
			/// Pushes a library in the lua stack (a table with classes and functions)
			void push_sublib(LuaLibrary& lib);
			lua_State* L;
	};
}

#endif // POSTAENGINE_LUASTATE_LUAAPI_H
