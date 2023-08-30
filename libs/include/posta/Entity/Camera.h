#ifndef POSTAENGINE_CAMERA_ENTITY_H
#define POSTAENGINE_CAMERA_ENTITY_H
#include <memory>
#include <posta/Entity/Entity.h>
#include <posta/Component/Camera.h>
#include <posta/Component/Transform.h>
#include <glm/gtx/string_cast.hpp>

namespace posta::entity {
	class Camera : public Entity
	{
		public:
			Camera(posta::component::Camera* camera, int width, int height);
			void set_resolution(int width, int height);
			void get_resolution(int& width, int& height);

			/// Returns the coordinates of a point on the screen, the z coordinate is the depth, values outside the range [-1, 1] are invalid points
			glm::vec3 point_on_screen(glm::vec3 position);
			
			void update_projection_matrix();
			/// Returns projection * view matrix, projection comes from the camera component and view represents the position and rotation of the transform
			glm::mat4 get_projection_view_matrix();
			/// Returns the projection matrix
			glm::mat4 get_projection_matrix();

			/// Returns the near_plane position in world-space from a screen-space position
			glm::vec3 get_point_on_near_plane(glm::vec2 pixel_position);

			/// Returns the far_plane position in world-space from a screen-space position
			glm::vec3 get_point_on_far_plane(glm::vec2 pixel_position);

			// This is a unique_ptr so the camera can use an orthographic or projection view
			std::unique_ptr<posta::component::Camera> camera;
			posta::component::Transform transform;
		private:
			//glm::vec3 position;
			//glm::quat rotation;
			int width, height;
	};
}

#endif // POSTAENGINE_CAMERA_ENTITY_H
