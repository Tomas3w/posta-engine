#ifndef POSTAENGINE_TRANSFORM_COMPONENT_H
#define POSTAENGINE_TRANSFORM_COMPONENT_H
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <bullet/btBulletDynamicsCommon.h>

namespace posta::component {
	/// Stores the transformation information of an object; rotation, translation and scale
	class Transform
	{
		public:
			static btTransform tobtTransform(const Transform& transform);
			Transform(btTransform t);
			Transform();
			Transform(glm::vec3 pos);
			void set_position(glm::vec3 pos);
			void set_rotation(glm::quat rot);
			void set_rotation(glm::vec3 rot);
			void set_rotation(glm::vec4 rot);
			void set_scale(glm::vec3 scale);

			const glm::vec3& get_position() const;
			const glm::quat& get_rotation() const;
			const glm::vec3& get_scale() const;

			glm::mat4 get_matrix() const;
			/// Returns the 3x3 matrix of the transpose of the inverse of the matrix returned by get_matrix()
			/* Usually this matrix is used to apply the same transformation of the matrix to the normal vectors */
			glm::mat3 get_normal_matrix() const;
			/// Returns the view matrix for a camera-like object
			/** The matrix returned is calculated using the negated position and inverse rotation, it does not take scale into consideration */
			glm::mat4 get_view_matrix() const;

			/// Returns a vector pointing towards the front (0, 0, 1) of the object in local space
			glm::vec3 front() const;
			/// Returns a vector pointing towards the right (1, 0, 0) of the object in local space
			glm::vec3 right() const;
			/// Returns a vector pointing towards the top (0, 1, 0) of the object in local space
			glm::vec3 up() const;
			
			/// Transform the position vec to a local position (local to this transform)
			glm::vec3 to_local(glm::vec3 vec) const;
			/// Does the opposite of to_local
			glm::vec3 from_local(glm::vec3 vec) const;

			/// Returns a world space transform as if this transform was local to the transform argument
			Transform as_local_to(Transform transform) const;
			/// Returns the local transform of the transform argument as if this transform was the origin
			Transform as_origin_to(Transform transform) const;

			/// Linear interpolation (for translation and scale) and a spherical interpolation (of rotation) of this transform by the transform argument
			/** \param factor factor of the interpolation: 0 = returns this, 1 = returns the transform argument */
			Transform interpolate(Transform transform, float factor);
		private:
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
	};

}

#endif // POSTAENGINE_TRANSFORM_COMPONENT_H
