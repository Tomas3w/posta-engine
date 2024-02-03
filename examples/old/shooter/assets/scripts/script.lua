--[[
print("posta.func() = " .. posta.func())
print("posta.func2() = " .. posta.func2())
son = posta.Son()

print("son:get_a_mul() = " .. son:get_a_mul(147))
print("son:get_a_mul() = " .. son:get_a_mul(32))
print("son:impossible() = " .. son:impossible())

mat = posta.mat4()

mat = posta.mat4.inverse(mat)
print(posta.mat4.to_string(mat))
posta.mat4.print_something()
--]]
print("hello from lua!")
imagen = posta.components.Image("assets/button.png")

--imagen.get_matrix()
--imagen.draw()

mat4 = glm.mat4()
mat3 = glm.mat3()
vec4 = glm.vec4(1, 0, 0, 1)
vec3 = glm.vec3()

shader = posta.Shader.current()
shader:set_uniform("global_color", glm.vec4(1, 1, 1, 1))
shader:set_uniform("model", imagen:get_matrix(0, 50))
imagen:draw()

font = posta.Font("assets/OxygenMono-Regular.ttf")
texto = font:render_image("hola mundo!", 24)
shader:set_uniform("global_color", glm.vec4(0, 0, 0, 1))
shader:set_uniform("model", texto:get_matrix(0, 70))
texto:draw()
