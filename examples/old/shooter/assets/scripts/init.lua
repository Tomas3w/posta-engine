
button_image = posta.components.Image("assets/button.png")
button = posta.UI.Button(posta.UI.Rect(0, 50, button_image:get_w(), button_image:get_h()))

checkbox_image_off = posta.components.Image("assets/checkbox.png")
checkbox_image_on = posta.components.Image("assets/checkbox2.png")
checkbox = posta.UI.Checkbox(posta.UI.Rect(0, 70, checkbox_image_on:get_w(), checkbox_image_on:get_h()))

checkbox_group = posta.UI.CheckboxGroup()
for i=1, 4 do
	checkbox_group:add_checkbox(posta.UI.Checkbox(posta.UI.Rect(i * checkbox_image_on:get_w(), 100, checkbox_image_on:get_w(), checkbox_image_on:get_h())))
end


