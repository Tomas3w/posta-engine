#ifndef POSTAENGINE_GENERAL_H
#define POSTAENGINE_GENERAL_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <posta/Util/span.h>
#include <GL/glew.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace posta {
	std::string read_file(std::filesystem::path path);

	btVector3 to_btVector3(glm::vec3 vec);
	glm::vec3 from_btVector3(btVector3 vec);

	std::string strip(std::string str);

	std::vector<std::string> split(std::string str, char separator);

	std::string get_glError(GLenum error);
}

#endif // POSTAENGINE_GENERAL_H
