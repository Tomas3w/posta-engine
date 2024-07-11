import os, shutil

def copy_shader_to_assets(shader_type, shader_name, project_name):
	shader_type_to_shader_folder = {
		'2d' : '2D',
		'3d' : '3D',
	}
	os.makedirs(f"apps/{project_name}/assets/shader", exist_ok=True)
	shutil.copytree(f"common-assets/common/shaders/{shader_type_to_shader_folder[shader_type]}", f"apps/{project_name}/assets/shader/{shader_name}")

def inc_text(shader_type, shader_name):
	guard_name = ("shader/" + shader_name).replace('/', '_').upper() + '_H'
	shader_class_name = shader_name[shader_name.rfind('/') + 1:]
	if shader_type == '2d':
		return f"""#ifndef {guard_name}
#define {guard_name}
#include "App.h"

namespace shader {{
	class {shader_class_name} : public posta::Shader
	{{
		public:
			{shader_class_name}() :
				posta::Shader("assets/shader/{shader_name}"),
				model(posta::Uniform<glm::mat4>(this, "model")),
				global_color(posta::Uniform<glm::vec4>(this, "global_color"))
			{{}}

			posta::Uniform<glm::mat4> model;
			posta::Uniform<glm::vec4> global_color;
	}};
}}

#endif // {guard_name}
"""
	elif shader_type == '3d':
		return f"""#ifndef {guard_name}
#define {guard_name}
#include "App.h"

namespace shader {{
	class {shader_class_name} : public posta::Shader
	{{
		public:
			{shader_class_name}() :
				posta::Shader("assets/shader/{shader_name}"),
				model(posta::Uniform<glm::mat4>(this, "model")),
				projection_view(posta::Uniform<glm::mat4>(this, "projection_view")),
				normal_model(posta::Uniform<glm::mat3>(this, "normal_model")),
				global_color(posta::Uniform<glm::vec4>(this, "global_color")),
				bones_matrices(posta::Uniform<posta::span<posta::anim_mat4>>(this, "bones_matrices")),
				use_bones(posta::Uniform<bool>(this, "use_bones"))
			{{}}

			posta::Uniform<glm::mat4> model;
			posta::Uniform<glm::mat4> projection_view;
			posta::Uniform<glm::mat3> normal_model;
			posta::Uniform<glm::vec4> global_color;
			posta::Uniform<posta::span<posta::anim_mat4>> bones_matrices;
			posta::Uniform<bool> use_bones;
	}};
}}

#endif // {guard_name}
"""

def src_text(shader_name):
	return f"""#include "shader/{shader_name}.h"
"""

def exists(path):
    try:
        with open(path) as file:
            pass
    except FileNotFoundError:
        return False
    return True

def make_shader(shader_type, shader_name, project_name):
    os.makedirs(f"apps/{project_name}/src/shader", exist_ok=True)
    os.makedirs(f"apps/{project_name}/include/shader", exist_ok=True)
    if '/' in shader_name:
        os.makedirs(f"apps/{project_name}/src/shader/{shader_name[:shader_name.rfind('/')]}", exist_ok=True)
        os.makedirs(f"apps/{project_name}/include/shader/{shader_name[:shader_name.rfind('/')]}", exist_ok=True)

    source_path = f"apps/{project_name}/src/shader/{shader_name}.cpp"
    include_path = f"apps/{project_name}/include/shader/{shader_name}.h"
    if exists(source_path):
        print(f'There is already a source file named {shader_name}.cpp')
        return None
    if exists(include_path):
        print(f'There is already an include file named {shader_name}.h')
        return None
    with open(f'apps/{project_name}/sources.cmake') as file:
        lines = []
        for i, line in enumerate(file.readlines()):
            if line.find(')') != -1:
                lines.append('\t' + f"src/shader/{shader_name}.cpp" + "\n")
            lines.append(line)
    with open(f'apps/{project_name}/sources.cmake', 'w') as file:
        file.write(''.join(lines))
    with open(source_path, 'w') as file:
        file.write(src_text(shader_name))
    with open(include_path, 'w') as file:
        file.write(inc_text(shader_type, shader_name))
    copy_shader_to_assets(shader_type, shader_name, project_name)

if __name__ == '__main__':
    make_shader('2d', 'hola', 'bones')

