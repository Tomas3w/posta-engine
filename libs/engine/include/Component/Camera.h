#ifndef POSTAENGINE_CAMERA_COMPONENT_H
#define POSTAENGINE_CAMERA_COMPONENT_H
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Engine::Component {
	/// Abstract class for all camera-like components
	class Camera
	{
		public:
			Camera() = default;
			virtual ~Camera() = default;
			/** Updates projection matrix based on type of camera */
			virtual void update_projection_matrix(const int width, const int height) = 0;

			glm::mat4 projection;
	};

	class CameraNearFar : public Camera
	{
		public:
			/** Near and far planes */
			float __near, __far;
	};

	/// Projection camera component
	class PCamera : public CameraNearFar
	{
		public:
			PCamera(float _near, float _far);
			PCamera(float _near, float _far, float _fov);
			/** Should be run every time projection parameters for the
			 *  camera are changed or the screen resolution is changed */
			void update_projection_matrix(const int width, const int height);

			/** Field of view */
			float __fov;
	};

	class OCamera : public CameraNearFar
	{
		public:
			OCamera(float _near, float _far);
			OCamera(float _near, float _far, float _size);
			void update_projection_matrix(const int width, const int height);

			/** Size */
			float __size;
	};
}

#endif // POSTAENGINE_CAMERA_COMPONENT_H
