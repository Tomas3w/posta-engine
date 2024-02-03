
shader = posta.Shader.current()

-- button
button_color = glm.vec4(0.75, 0.75, 0.75, 1)
if button:is_highlighted() then
	button_color = glm.vec4(1, 1, 1, 1)
end
if button:is_pressed() then
	button_color = glm.vec4(0.5, 0.5, 0.5, 1)
end
if button:loop() then
	print("checkbox selected: " .. checkbox_group:get_checked_index())
end

shader:set_uniform("model", button_image:get_matrix(0, 50))
shader:set_uniform("global_color", button_color)
button_image:draw()

-- checkbox
checkbox:loop()
checkbox_color = glm.vec4(0.75, 0.75, 0.75, 1)
if checkbox:is_highlighted() then
	checkbox_color = glm.vec4(1, 1, 1, 1)
end
shader:set_uniform("model", checkbox_image_on:get_matrix(0, 70))
shader:set_uniform("global_color", checkbox_color)
if checkbox:is_checked() then
	checkbox_image_on:draw()
else
	checkbox_image_off:draw()
end

-- checkbox_group
checkbox_group:loop()
for i=1, checkbox_group:size() do
	checkbox_color = glm.vec4(0.75, 0.75, 0.75, 1)
	if checkbox_group:get_checkbox(i):is_highlighted() then
		checkbox_color = glm.vec4(1, 1, 1, 1)
	end
	shader:set_uniform("model", checkbox_image_on:get_matrix(checkbox_group:get_checkbox(i):get_rect():get_x(), checkbox_group:get_checkbox(i):get_rect():get_y()))
	shader:set_uniform("global_color", checkbox_color)
	if checkbox_group:get_checkbox(i):is_checked() then
		checkbox_image_on:draw()
	else
		checkbox_image_off:draw()
	end
end

