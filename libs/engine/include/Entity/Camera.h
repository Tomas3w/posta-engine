#ifndef POSTAENGINE_CAMERA_ENTITY_H
#define POSTAENGINE_CAMERA_ENTITY_H
#include <memory>
#include <engine/include/Entity/Entity.h>
#include <engine/include/Component/Camera.h>
#include <engine/include/Component/Transform.h>
#include <glm/gtx/string_cast.hpp>

namespace Engine::Entity {
	class Camera : public Entity
	{
		public:
			Camera(Engine::Component::Camera* camera, int width, int height);
			void set_resolution(int width, int height);
			void get_resolution(int& width, int& height);

			void loop();
			void draw();
			
			void update_projection_matrix();
			/// Returns projection * view matrix, projection comes from the camera component and view represents the position and rotation of the transform
			glm::mat4 get_projection_view_matrix();
			/// Returns the projection matrix
			glm::mat4 get_projection_matrix();

			std::unique_ptr<Engine::Component::Camera> camera; // This is a unique_ptr just to be able to use an orthographic view or projection view
			Engine::Component::Transform transform;
		private:
			//glm::vec3 position;
			//glm::quat rotation;
			int width, height;
	};
}

#endif // POSTAENGINE_CAMERA_ENTITY_H
