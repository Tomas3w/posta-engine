#include <engine/include/LuaAPI/LuaAPI.h>
// Test
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <engine/include/App.h>

using Engine::LuaAPI::LuaState;
using Engine::LuaAPI::MayNotBeOwner;
/*using Engine::LuaAPI::Son;

static int func(lua_State* L)
{
	lua_pushnumber(L, 77.5);
	return 1;
}

static std::string func2()
{
	return "hola!";
}

static Son* son_new(lua_State* L)
{
	return new Son;
}

int Son::get_a_mul(int n)
{
	a++;
	return a * n;
}

int Son::impossible()
{
	return 348;
}

static glm::mat4* glm__mat4_new(lua_State* L)
{
	return new glm::mat4(1.0f);
}

void print_something()
{
	std::cout << "just printing something..." << std::endl;
}*/

#define LUA_METHOD_MAKE(lib, t, m, ...) \
	(lib.classes.back().methods.push_back(LuaState::LuaCFunction::from_method<t, decltype(t::m), &t::m, __VA_ARGS__>(#m)))
#define LUA_METHOD_MAKE_NARGS(lib, t, m) \
	(lib.classes.back().methods.push_back(LuaState::LuaCFunction::from_method<t, decltype(t::m), &t::m>(#m)))
#define LUA_METHOD_MAKE_FROM_LUAFUNC(lib, name, f) \
	(lib.classes.back().methods.push_back(LuaState::LuaCFunction(name, f)))

/// This define is only meant for classes that do not have static functions
#define LUA_CLASS_MAKE(lib, t, ctor) \
	(lib.classes.push_back(LuaState::LuaClass::from_class<t, ctor, false>()))
/// This define is only meant for classes that do have static functions
#define LUA_CLASS_MAKE_S(lib, t, ctor) \
	(lib.classes.push_back(LuaState::LuaClass::from_class<t, ctor, true>()))

#define LUA_FUNC_MAKE(lib, f, ...) \
	(lib.functions.push_back(LuaState::LuaCFunction::from_func<decltype(f), &f, __VA_ARGS__>(#f)))
#define LUA_FUNC_MAKE_NARGS(lib, f) \
	(lib.functions.push_back(LuaState::LuaCFunction::from_func<decltype(f), &f>(#f)))

#define LUA_USE_THIS_ALIAS_FOR_CLASS(x) \
	(LuaState::LuaClass::add_alias_to_class<x>(#x))
#define LUA_USE_THIS_ALIAS_FOR_CLASS_N(x, y) \
	(LuaState::LuaClass::add_alias_to_class<x>(#y))

#define LUA_STATIC_FUNC_MAKE(lib, f, name, ...) \
	(lib.classes.back().static_functions.push_back(LuaState::LuaCFunction::from_func<decltype(f), &f, __VA_ARGS__>(name)))
#define LUA_STATIC_FUNC_MAKE_NARGS(lib, f, name) \
	(lib.classes.back().static_functions.push_back(LuaState::LuaCFunction::from_func<decltype(f), &f>(name)))

/// Returns a new glm 4x4 matrix (glm::mat4) with the value of the identity
static glm::mat4* glm_mat4_new(lua_State* L)
{
	return new glm::mat4(1.0f);
}

/// Same as glm_mat4_new but for mat3
static glm::mat3* glm_mat3_new(lua_State* L)
{
	return new glm::mat3(1.0f);
}

/// Same as glm_mat4_new but for vec4
static glm::vec4* glm_vec4_new(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<float> x(LuaState::LuaCFunction::getarg<float>(L, 1));
		LuaState::LuaCFunction::checkarg(L, x.get(), 1);
		LuaState::LuaCFunction::unique_ptr<float> y(LuaState::LuaCFunction::getarg<float>(L, 2));
		LuaState::LuaCFunction::checkarg(L, y.get(), 2);
		LuaState::LuaCFunction::unique_ptr<float> z(LuaState::LuaCFunction::getarg<float>(L, 3));
		LuaState::LuaCFunction::checkarg(L, z.get(), 3);
		LuaState::LuaCFunction::unique_ptr<float> w(LuaState::LuaCFunction::getarg<float>(L, 4));
		LuaState::LuaCFunction::checkarg(L, w.get(), 4);
		return new glm::vec4(*x, *y, *z, *w);
	}
	return new glm::vec4(1.0f);
}

/// Same as glm_mat4_new but for vec3
static glm::vec3* glm_vec3_new(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<float> x(LuaState::LuaCFunction::getarg<float>(L, 1));
		LuaState::LuaCFunction::checkarg(L, x.get(), 1);
		LuaState::LuaCFunction::unique_ptr<float> y(LuaState::LuaCFunction::getarg<float>(L, 2));
		LuaState::LuaCFunction::checkarg(L, y.get(), 2);
		LuaState::LuaCFunction::unique_ptr<float> z(LuaState::LuaCFunction::getarg<float>(L, 3));
		LuaState::LuaCFunction::checkarg(L, z.get(), 3);
		return new glm::vec3(*x, *y, *z);
	}
	return new glm::vec3(1.0f);
}

/// Same as glm_mat4_new but for vec2
static glm::vec2* glm_vec2_new(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<float> x(LuaState::LuaCFunction::getarg<float>(L, 1));
		LuaState::LuaCFunction::checkarg(L, x.get(), 1);
		LuaState::LuaCFunction::unique_ptr<float> y(LuaState::LuaCFunction::getarg<float>(L, 2));
		LuaState::LuaCFunction::checkarg(L, y.get(), 2);
		return new glm::vec2(*x, *y);
	}
	return new glm::vec2(1.0f);
}

/// Takes an path (a string) to an image file and returns an Image object
static Engine::Component::Image* Image_new(lua_State* L)
{
	std::string path = luaL_checkstring(L, 1);
	return new Engine::Component::Image(path);
}

/// Shader constructor
/// Takes an path (a string) to an shader folder(folder containing a fragment.glsl and a vertex.glsl) and returns an Shader object in a MayNotBeOwner wrapper
static MayNotBeOwner<Engine::Shader>* Shader_new(lua_State* L)
{
	std::string path = luaL_checkstring(L, 1);
	return new MayNotBeOwner<Engine::Shader>(new Engine::Shader(path), true);
}

/// Returns current binded shader, if no shader has been bind, returns nil
static MayNotBeOwner<Engine::Shader> Shader__current()
{
	if (Engine::App::app->current_shader == nullptr)
		return MayNotBeOwner<Engine::Shader>::nil();
	return MayNotBeOwner<Engine::Shader>(Engine::App::app->current_shader, false);
}

#define CHECK_FOR_THIRD_ARG_IN_UNIFORM(t) \
	if (self->uniforms_##t.count(name_of_uniform)) \
	{ \
		std::string full_name = LuaState::LuaClass::get_fullname_of_type<t>(); \
		std::string name = LuaState::LuaClass::get_name_from_fullname_of_type(full_name); \
		luaL_typeerror(L, 3, name.c_str()); \
	}
#define CHECK_FOR_THIRD_ARG_IN_UNIFORM_GLM(t) \
	if (self->uniforms_##t.count(name_of_uniform)) \
	{ \
		std::string full_name = LuaState::LuaClass::get_fullname_of_type<glm::t>(); \
		std::string name = LuaState::LuaClass::get_name_from_fullname_of_type(full_name); \
		luaL_typeerror(L, 3, name.c_str()); \
	}
#define DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(FUNC) \
	FUNC(bool); \
	FUNC(int); \
	FUNC(float); \
	FUNC##_GLM(mat4); \
	FUNC##_GLM(mat3); \
	FUNC##_GLM(vec4); \
	FUNC##_GLM(vec3); \
	FUNC##_GLM(vec2);
#define CHECK_THIRD_ARG_AND_IF_SO_SEARCH_FOR_UNIFORM_AND_THEN_SET_IT(t) \
	{ \
		LuaState::LuaCFunction::unique_ptr<t> arg_value(LuaState::LuaCFunction::getarg<t>(L, 3)); \
		if (arg_value) \
		{ \
			if (self->uniforms_##t.count(*name)) \
				self->uniforms_##t[*name].set_value(*arg_value); \
			else \
			{ \
				CHECK_FOR_THIRD_ARG_IN_ALL_OF_THE_UNIFORMS<t>(self, *name, L); \
				luaL_error(L, (std::string("uniform name('") + (*name) + "') was not found in shader (with type " # t + ")").c_str()); \
			} \
			return 0; \
		} \
	}
#define CHECK_THIRD_ARG_AND_IF_SO_SEARCH_FOR_UNIFORM_AND_THEN_SET_IT_GLM(t) \
	{ \
		LuaState::LuaCFunction::unique_ptr<glm::t> arg_value(LuaState::LuaCFunction::getarg<glm::t>(L, 3)); \
		if (arg_value) \
		{ \
			if (self->uniforms_##t.count(*name)) \
				self->uniforms_##t[*name].set_value(*arg_value); \
			else \
			{ \
				CHECK_FOR_THIRD_ARG_IN_ALL_OF_THE_UNIFORMS<glm::t>(self, *name, L); \
				luaL_error(L, (std::string("uniform name('") + (*name) + "') was not found in shader (with type " # t + ")").c_str()); \
			} \
			return 0; \
		} \
	}
template<class T>
static void CHECK_FOR_THIRD_ARG_IN_ALL_OF_THE_UNIFORMS(MayNotBeOwner<Engine::Shader>& self, std::string name_of_uniform, lua_State* L)
{
	DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(CHECK_FOR_THIRD_ARG_IN_UNIFORM)
}

static int Shader_set_uniform(lua_State* L)
{
	MayNotBeOwner<Engine::Shader>* _self = LuaState::LuaClass::method_checkself<MayNotBeOwner<Engine::Shader>>(L);
	MayNotBeOwner<Engine::Shader>& self = *_self;
	//std::cout << "aqui estoy1!" << std::endl;
	LuaState::LuaCFunction::unique_ptr<std::string> name(LuaState::LuaCFunction::getarg<std::string>(L, 2));
	LuaState::LuaCFunction::checkarg(L, name.get(), 2);
	DO_DEFINES_OVER_EACH_TYPE_USED_BY_UNIFORMS(CHECK_THIRD_ARG_AND_IF_SO_SEARCH_FOR_UNIFORM_AND_THEN_SET_IT)
	// else of all the previous ifs
	luaL_typeerror(L, 3, "boolean, number, glm.mat4, glm.mat3, glm.vec4, glm.vec3, or glm.vec2");
	return 0;
}

/// Font
static Engine::Font* Font_new(lua_State* L)
{
	std::string path = luaL_checkstring(L, 1);
	return new Engine::Font(path);
}

static int Font_render_image(lua_State* L)
{
	Engine::Font* self = LuaState::LuaClass::method_checkself<Engine::Font>(L);
	LuaState::LuaCFunction::unique_ptr<std::string> text(LuaState::LuaCFunction::getarg<std::string>(L, 2));
	LuaState::LuaCFunction::checkarg(L, text.get(), 2);
	LuaState::LuaCFunction::unique_ptr<size_t> font_size(LuaState::LuaCFunction::getarg<size_t>(L, 3));
	LuaState::LuaCFunction::checkarg(L, font_size.get(), 3);
	LuaState::LuaCFunction::push_uvalue(L, self->render_image(*text, *font_size));
	return 1;
}

static int Font_get_text_size(lua_State* L)
{
	Engine::Font* self = LuaState::LuaClass::method_checkself<Engine::Font>(L);
	LuaState::LuaCFunction::unique_ptr<std::string> text(LuaState::LuaCFunction::getarg<std::string>(L, 2));
	LuaState::LuaCFunction::checkarg(L, text.get(), 2);
	LuaState::LuaCFunction::unique_ptr<size_t> font_size(LuaState::LuaCFunction::getarg<size_t>(L, 3));
	LuaState::LuaCFunction::checkarg(L, font_size.get(), 3);
	int w, h;
	int r = self->get_text_size(*text.get(), *font_size, w, h);
	if (r == -1)
	{
		LuaState::LuaCFunction::push_nil(L);
		return 1;
	}
	LuaState::LuaCFunction::push_value(L, w);
	LuaState::LuaCFunction::push_value(L, h);
	return 2;
}

/// Rect constructor
static Engine::UI::Rect* Rect_new(lua_State* L)
{
	LuaState::LuaCFunction::unique_ptr<int> x(LuaState::LuaCFunction::getarg<int>(L, 1));
	LuaState::LuaCFunction::checkarg(L, x.get(), 1);
	LuaState::LuaCFunction::unique_ptr<int> y(LuaState::LuaCFunction::getarg<int>(L, 2));
	LuaState::LuaCFunction::checkarg(L, y.get(), 2);
	LuaState::LuaCFunction::unique_ptr<int> w(LuaState::LuaCFunction::getarg<int>(L, 3));
	LuaState::LuaCFunction::checkarg(L, w.get(), 3);
	LuaState::LuaCFunction::unique_ptr<int> h(LuaState::LuaCFunction::getarg<int>(L, 4));
	LuaState::LuaCFunction::checkarg(L, h.get(), 4);
	return new Engine::UI::Rect(*x, *y, *w, *h);
}

/// Button constructor
static Engine::UI::Button* Button_new(lua_State* L)
{
	LuaState::LuaCFunction::unique_ptr<Engine::UI::Rect> rect(LuaState::LuaCFunction::getarg<Engine::UI::Rect>(L, 1));
	LuaState::LuaCFunction::checkarg(L, rect.get(), 1);
	return new Engine::UI::Button(*rect);
}

static int Button_loop(lua_State* L) // int x_offset, int y_offset, Rect _rect
{
	Engine::UI::Button* self = LuaState::LuaClass::method_checkself<Engine::UI::Button>(L);
	bool r;
	if (lua_type(L, 2) == LUA_TNONE)
	{
		r = self->loop();
		LuaState::LuaCFunction::push_value(L, r);
		return 1;
	}
	LuaState::LuaCFunction::unique_ptr<int> x_offset(LuaState::LuaCFunction::getarg<int>(L, 2)); 
	LuaState::LuaCFunction::checkarg(L, x_offset.get(), 2);
	LuaState::LuaCFunction::unique_ptr<int> y_offset(LuaState::LuaCFunction::getarg<int>(L, 3)); 
	LuaState::LuaCFunction::checkarg(L, y_offset.get(), 3);
	if (lua_type(L, 4) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<Engine::UI::Rect> _rect(LuaState::LuaCFunction::getarg<Engine::UI::Rect>(L, 4)); 
		LuaState::LuaCFunction::checkarg(L, _rect.get(), 4);
		r = self->loop(*x_offset, *y_offset, *_rect);
	}
	else
		r = self->loop(*x_offset, *y_offset);
	LuaState::LuaCFunction::push_value(L, r);
	return 1;
}

/// Checkbox constructor
static Engine::UI::Checkbox* Checkbox_new(lua_State* L)
{
	LuaState::LuaCFunction::unique_ptr<Engine::UI::Rect> rect(LuaState::LuaCFunction::getarg<Engine::UI::Rect>(L, 1));
	LuaState::LuaCFunction::checkarg(L, rect.get(), 1);
	return new Engine::UI::Checkbox(*rect);
}

static int Checkbox_loop(lua_State* L) // int x_offset, int y_offset, Rect _rect
{
	Engine::UI::Checkbox* self = LuaState::LuaClass::method_checkself<Engine::UI::Checkbox>(L);
	bool r;
	if (lua_type(L, 2) == LUA_TNONE)
	{
		r = self->loop();
		LuaState::LuaCFunction::push_value(L, r);
		return 1;
	}
	LuaState::LuaCFunction::unique_ptr<int> x_offset(LuaState::LuaCFunction::getarg<int>(L, 2)); 
	LuaState::LuaCFunction::checkarg(L, x_offset.get(), 2);
	LuaState::LuaCFunction::unique_ptr<int> y_offset(LuaState::LuaCFunction::getarg<int>(L, 3)); 
	LuaState::LuaCFunction::checkarg(L, y_offset.get(), 3);
	if (lua_type(L, 4) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<Engine::UI::Rect> _rect(LuaState::LuaCFunction::getarg<Engine::UI::Rect>(L, 4)); 
		LuaState::LuaCFunction::checkarg(L, _rect.get(), 4);
		r = self->loop(*x_offset, *y_offset, *_rect);
	}
	else
		r = self->loop(*x_offset, *y_offset);
	LuaState::LuaCFunction::push_value(L, r);
	return 1;
}

/// CheckboxGroup constructor
static Engine::UI::CheckboxGroup* CheckboxGroup_new(lua_State* L)
{
	return new Engine::UI::CheckboxGroup;
}

static int CheckboxGroup_loop(lua_State* L) // int x_offset, int y_offset, Rect _rect
{
	Engine::UI::CheckboxGroup* self = LuaState::LuaClass::method_checkself<Engine::UI::CheckboxGroup>(L);
	if (lua_type(L, 2) == LUA_TNONE)
	{
		self->loop();
		return 0;
	}
	LuaState::LuaCFunction::unique_ptr<int> x_offset(LuaState::LuaCFunction::getarg<int>(L, 2)); 
	LuaState::LuaCFunction::checkarg(L, x_offset.get(), 2);
	LuaState::LuaCFunction::unique_ptr<int> y_offset(LuaState::LuaCFunction::getarg<int>(L, 3)); 
	LuaState::LuaCFunction::checkarg(L, y_offset.get(), 3);
	if (lua_type(L, 4) != LUA_TNONE)
	{
		LuaState::LuaCFunction::unique_ptr<Engine::UI::Rect> _rect(LuaState::LuaCFunction::getarg<Engine::UI::Rect>(L, 4)); 
		LuaState::LuaCFunction::checkarg(L, _rect.get(), 4);
		self->loop(*x_offset, *y_offset, *_rect);
	}
	else
		self->loop(*x_offset, *y_offset);
	return 0;
}

static int CheckboxGroup_get_checked_index(lua_State* L)
{
	Engine::UI::CheckboxGroup* self = LuaState::LuaClass::method_checkself<Engine::UI::CheckboxGroup>(L);
	auto index = self->get_checked_index();
	
	int r = 0;
	if (index)
		r = (*index) + 1;
	LuaState::LuaCFunction::push_value(L, r);
	return 1;
}

// copies the checkbox in the index
static int CheckboxGroup_get_checkbox(lua_State* L)
{
	Engine::UI::CheckboxGroup* self = LuaState::LuaClass::method_checkself<Engine::UI::CheckboxGroup>(L);
	LuaState::LuaCFunction::unique_ptr<int> index(LuaState::LuaCFunction::getarg<int>(L, 2)); 
	LuaState::LuaCFunction::checkarg(L, index.get(), 2);
	if ((*index) < 1 || (*index) > static_cast<int>(self->checkboxes.size()))
	{
		LuaState::LuaCFunction::push_nil(L);
		return 1;
	}
	LuaState::LuaCFunction::push_value(L, self->checkboxes[(*index) - 1]);
	return 1;
}

static int CheckboxGroup_size(lua_State* L)
{
	Engine::UI::CheckboxGroup* self = LuaState::LuaClass::method_checkself<Engine::UI::CheckboxGroup>(L);
	size_t r = self->checkboxes.size();
	LuaState::LuaCFunction::push_value(L, r);
	return 1;
}

void Engine::LuaAPI::load_api(LuaState& lua_state)
{
	// Aliases
	LUA_USE_THIS_ALIAS_FOR_CLASS_N(MayNotBeOwner<Engine::Shader>, Engine::Shader);
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::mat4);
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::mat3);
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::vec4);
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::vec3);
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::vec2);
	
	// glm
	LuaState::LuaLibrary _glm("glm");
	LUA_CLASS_MAKE(_glm, glm::mat4, glm_mat4_new);
	LUA_CLASS_MAKE(_glm, glm::mat3, glm_mat3_new);
	LUA_CLASS_MAKE(_glm, glm::vec4, glm_vec4_new);
	LUA_CLASS_MAKE(_glm, glm::vec3, glm_vec3_new);
	LUA_CLASS_MAKE(_glm, glm::vec2, glm_vec2_new);
	lua_state.add_lib(_glm);

	// posta
	LuaState::LuaLibrary posta("posta");
	posta.sublibraries.emplace_back("components");
	posta.sublibraries.emplace_back("UI");
	LuaState::LuaLibrary& components = posta.sublibraries[0];
	LuaState::LuaLibrary& UI = posta.sublibraries[1];
	
	// Test
	/*posta.functions.push_back(LuaState::LuaCFunction("func", func));
	LUA_FUNC_MAKE_NARGS(posta, func2);
	LUA_CLASS_MAKE(posta, Son, son_new);
	LUA_METHOD_MAKE(posta, Son, get_a_mul, int);
	LUA_METHOD_MAKE_NARGS(posta, Son, impossible);

	// Test 2
	LUA_USE_THIS_ALIAS_FOR_CLASS(glm::mat4);
	LUA_CLASS_MAKE_S(posta, glm::mat4, glm__mat4_new);
	LUA_STATIC_FUNC_MAKE(posta, (glm::inverse<4, 4, float, glm::packed_highp>), "inverse", glm::mat4);
	LUA_STATIC_FUNC_MAKE(posta, (glm::to_string<glm::mat4>), "to_string", glm::mat4);
	LUA_STATIC_FUNC_MAKE_NARGS(posta, print_something, "print_something");*/

	// Util
	LUA_CLASS_MAKE(posta, Engine::Font, Font_new);
	LUA_METHOD_MAKE_FROM_LUAFUNC(posta, "render_image", Font_render_image);
	LUA_METHOD_MAKE_FROM_LUAFUNC(posta, "get_text_size", Font_get_text_size);

	LUA_CLASS_MAKE_S(posta, MayNotBeOwner<Engine::Shader>, Shader_new);
	LUA_STATIC_FUNC_MAKE_NARGS(posta, Shader__current, "current");
	LUA_METHOD_MAKE_FROM_LUAFUNC(posta, "set_uniform", Shader_set_uniform);

	// Components
	LUA_CLASS_MAKE(components, Engine::Component::Image, Image_new);
	LUA_METHOD_MAKE(components, Engine::Component::Image, get_matrix, int, int);
	LUA_METHOD_MAKE_NARGS(components, Engine::Component::Image, draw);
	LUA_METHOD_MAKE_NARGS(components, Engine::Component::Image, get_w);
	LUA_METHOD_MAKE_NARGS(components, Engine::Component::Image, get_h);
	LUA_METHOD_MAKE(components, Engine::Component::Image, set_w, int);
	LUA_METHOD_MAKE(components, Engine::Component::Image, set_h, int);

	// UI
	LUA_CLASS_MAKE(UI, Engine::UI::Rect, Rect_new);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Rect, get_x);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Rect, get_y);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Rect, get_w);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Rect, get_h);
	LUA_METHOD_MAKE(UI, Engine::UI::Rect, set_x, int);
	LUA_METHOD_MAKE(UI, Engine::UI::Rect, set_y, int);
	LUA_METHOD_MAKE(UI, Engine::UI::Rect, set_w, int);
	LUA_METHOD_MAKE(UI, Engine::UI::Rect, set_h, int);

	LUA_CLASS_MAKE(UI, Engine::UI::Button, Button_new);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "loop", Button_loop);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Button, get_rect);
	LUA_METHOD_MAKE(UI, Engine::UI::Button, set_rect, Engine::UI::Rect);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Button, is_pressed);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Button, is_highlighted);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Button, press);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Button, release);

	LUA_CLASS_MAKE(UI, Engine::UI::Checkbox, Checkbox_new);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "loop", Checkbox_loop);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Checkbox, get_rect);
	LUA_METHOD_MAKE(UI, Engine::UI::Checkbox, set_rect, Engine::UI::Rect);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Checkbox, is_checked);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Checkbox, is_highlighted);
	LUA_METHOD_MAKE_NARGS(UI, Engine::UI::Checkbox, toggle);

	LUA_CLASS_MAKE(UI, Engine::UI::CheckboxGroup, CheckboxGroup_new);
	LUA_METHOD_MAKE(UI, Engine::UI::CheckboxGroup, add_checkbox, Engine::UI::Checkbox);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "loop", CheckboxGroup_loop);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "get_checked_index", CheckboxGroup_get_checked_index);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "get_checkbox", CheckboxGroup_get_checkbox);
	LUA_METHOD_MAKE_FROM_LUAFUNC(UI, "size", CheckboxGroup_size);


	// Loading the api into the lua state
	lua_state.add_lib(posta);
}

