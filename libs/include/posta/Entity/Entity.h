#ifndef POSTAENGINE_ENTITY_H
#define POSTAENGINE_ENTITY_H
#include <posta/Component/Camera.h>
#include <posta/Util/Framebuffer.h>
#include <posta/Util/Assets.h>
#include <posta/Component/Texture.h>
#include <posta/Component/StaticMesh.h>
#include <posta/Component/DrawableMesh.h>
#include <corecrt.h>
#include <posta/Component/Transform.h>
#include <posta/Component/Rigidbody.h>
#include <posta/Util/Shader.h>
#include <unordered_set>
#ifndef POSTA_EDITOR_DISABLED
#define POSTA_EDITOR_AVAILABILITY 1
#else
#define POSTA_EDITOR_AVAILABILITY 0
#endif
#define POSTA_REGISTER_ENTITY(entity_object) if constexpr(POSTA_EDITOR_AVAILABILITY) { this->__register(entity_object); }

namespace posta::entity {
	#ifndef POSTA_EDITOR_DISABLED
	/// Parent of all types of entities
	class Type
	{
		public:
			virtual ~Type() = default;
			virtual void draw_front() {}
			virtual void draw_back() {}
	};

	/// Simple entity type, it only contains a transform
	class EmptyType : public Type
	{
		private:
			posta::component::Transform* transform = nullptr;
		public:
			EmptyType(posta::component::Transform* _transform);
			void draw_front() override;
	};

	/// Entity type for a camera that doesn't have a framework attached
	class CameraType : public Type
	{
		private:
			posta::component::Transform* transform = nullptr;
			std::unique_ptr<posta::component::Camera>* camera = nullptr;
			int* width = nullptr;
			int* height = nullptr;
		public:
			posta::Framebuffer* framebuffer = nullptr;
			CameraType(posta::component::Transform* _transform, std::unique_ptr<posta::component::Camera>* _camera, int* width, int* height);
			void draw_front() override;
	};

	class RigidbodyType : public Type
	{
		private:
			posta::component::Rigidbody* rigidbody = nullptr;
		public:
			std::vector<posta::component::DrawableMesh*> drawable_meshes;
			RigidbodyType(posta::component::Rigidbody* _rigidbody, posta::component::DrawableMesh* _drawable_mesh = nullptr);
			void draw_back() override;
			void draw_front() override;
	};
	#endif

	/** Base class for every entity in the editor
	 * Ensure to enclose all code relevant to this parent class with an #ifndef POSTA_EDITOR_DISABLED preprocessor directive. This can be achieved using the macro functions POSTA_REGISTER_ENTITY_*() */
	class Entity
	{
		#ifndef POSTA_EDITOR_DISABLED
		public:
			class EditorGraphics
			{
				public:
					EditorGraphics();

					friend class Entity;

					posta::component::StaticMesh arrow;
					posta::component::Texture blank_texture;
					class EditorShader : public posta::Shader
					{
						public:
							EditorShader() :
								posta::Shader("common/editor/shaders/3DEditor"),
								model(posta::Uniform<glm::mat4>(this, "model")),
								projection_view(posta::Uniform<glm::mat4>(this, "projection_view")),
								normal_model(posta::Uniform<glm::mat3>(this, "normal_model")),
								global_color(posta::Uniform<glm::vec4>(this, "global_color")),
								line(posta::Uniform<bool>(this, "line")),
								linei(posta::Uniform<glm::vec2>(this, "linei")),
								linef(posta::Uniform<glm::vec2>(this, "linef")),
								line_width(posta::Uniform<float>(this, "line_width")),
								width(posta::Uniform<float>(this, "width")),
								height(posta::Uniform<float>(this, "height"))
							{}

							posta::Uniform<glm::mat4> model;
							posta::Uniform<glm::mat4> projection_view;
							posta::Uniform<glm::mat3> normal_model;
							posta::Uniform<glm::vec4> global_color;
							posta::Uniform<bool> line;
							posta::Uniform<glm::vec2> linei;
							posta::Uniform<glm::vec2> linef;
							posta::Uniform<float> line_width;
							posta::Uniform<float> width;
							posta::Uniform<float> height;
					} shader;
			};
			Entity();
			virtual ~Entity();

			/// Draws the entity, this function is called internally to draw the entity in the editor
			void __draw_entity(bool front);
			/// Do not use directly, instead use the macro POSTA_REGISTER_ENTITY.
			/// Registers the entity with their respective properties
			void __register(Type* type);

			/// For internal use only. Frees the editor graphics
			static void __free_graphics();

			/// Returns the entity type in case further configuration is needed for this entity, remember to enclose modifications to the entity using the #ifndef POSTA_EDITOR_DISABLED preprocessor directive
			Type* get_internal_entity_type();
			
			friend class EmptyType;
			friend class CameraType;
			friend class RigidbodyType;
		private:
			static EditorGraphics* graphics();
			static std::unique_ptr<EditorGraphics> __graphics;

			static void draw_outline(component::DrawableMesh* drawable_mesh, posta::component::Transform transform);
			static void draw_line(glm::vec3 i, glm::vec3 f, glm::vec4 color, float line_width);
			static void draw_transform(posta::component::Transform transform);
			static void draw_camera_outline(posta::component::Transform transform, posta::component::Camera* camera, posta::Framebuffer* framebuffer, int width, int height);

			std::unique_ptr<Type> type;

			/*
			posta::component::Rigidbody* __entity_rigidbody = nullptr;
			posta::component::DrawableMesh* __entity_drawable_mesh = nullptr;
			// In case the entity doesn't have a rigidbody, then it is created from a mesh object
			std::unique_ptr<posta::component::Rigidbody> __entity_rigidbody_artificial;
			*/

			// Name of the entity, by default the name is just to_string(this)
			std::string __entity_name;
			
		#else
		public:
			virtual ~Entity() = default;
		#endif

	};

	// Editor section
	extern std::unordered_set<Entity*> entities;
}

#endif // POSTAENGINE_ENTITY_H
