#ifndef POSTAENGINE_ASSETS_H
#define POSTAENGINE_ASSETS_H
#include <fstream>
#include <filesystem>
#include <array>
#include <algorithm>
#include <posta/Util/Mesh.h>
#include <glm/gtc/matrix_transform.hpp>

namespace posta::assets {
	/// Basic vertex properties
	/** Vertex properties representing three components:
	 *  - position: x, y, z
	 *  - normal: nx, ny, nz
	 *  - uv: u, v
	 * */
	const VertexProperties basic_vertex_props = {{"position", "normal", "uv"}, {VertexProperties::INTERPOLATION_TYPE::LERP, VertexProperties::INTERPOLATION_TYPE::SLERP, VertexProperties::INTERPOLATION_TYPE::LERP}, {3, 3, 2}, 8};
	/// Basic vertex properties with bone weights (and indices)
	/** Properties:
	 * - position: x, y, z
	 * - normal: nx, ny, nz
	 * - uv: u, v
	 * - weights: b1, b2, b3, b4 */
	const VertexProperties bone_vertex_props = {{"position", "normal", "uv", "weights"}, {VertexProperties::INTERPOLATION_TYPE::LERP, VertexProperties::INTERPOLATION_TYPE::SLERP, VertexProperties::INTERPOLATION_TYPE::LERP, VertexProperties::INTERPOLATION_TYPE::LERP}, {3, 3, 2, 4}, 12};

	/// loads an .obj file into a Mesh
	Mesh load_obj(std::filesystem::path path);

	/// loads an .obj file into a Mesh and caches it for next loading, the cache will be stored at path + '.cache'
	Mesh load_obj_and_cache(std::filesystem::path path, bool smooth = false);
	
	/// loads an .obj file and a .bones file containing bones weights and indices
	/** A .bones contains the following format:
	 * vertex_index1 bone_index1|bone_weight1,bone_index2|bone_weight2,bone_indexN|bone_weightN, \n
	 * vertex_index2 ...
	 * The vertex index corresponds with the .obj vertices indices, except that they start at 0 and not 1, this also applies to the bone_indices
	 *
	 * \param path a path pointing to two files with diferent extension but with the same name, i.e. path.obj and path.bones
	 * */
	Mesh load_obj_with_bones(std::filesystem::path path);

}

#endif // POSTAENGINE_ASSETS_H
