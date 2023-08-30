#include "pch.h"

#ifndef VIEWER_GLTF_MESH_H
#define VIEWER_GLTF_MESH_H

#include "result.h"

namespace viewer {

    struct gltf_attributes {
        size_t mo_POSITION = -1;
        size_t mo_NORMAL = -1;
        size_t mo_TANGENT = -1;
        size_t mo_TEXCOORD_0 = -1;
        size_t mo_TEXCOORD_1 = -1;
        size_t mo_COLOR_0 = -1;
        size_t mo_JOINTS_0 = -1;
        size_t mo_WEIGHTS_0 = -1;

        gltf_result load_attributes(nlohmann::json& t_attributes_json);
    };

    enum gltf_primitive_target : size_t {
        gltf_primitive_target_points,
        gltf_primitive_target_lines,
        gltf_primitive_target_line_loop,
        gltf_primitive_target_line_strip,
        gltf_primitive_target_triangles,
        gltf_primitive_target_triangle_strip,
        gltf_primitive_target_triangle_fan,
    };

    struct gltf_primitive {
        gltf_attributes m_attributes;
        size_t mo_indices = -1;
        size_t mo_material = -1;
        gltf_primitive_target m_mode = gltf_primitive_target_triangles;
        std::vector<gltf_attributes> m_targets;

        gltf_result load_primitive(nlohmann::json& t_primitive_json, size_t t_index);
    };

    struct gltf_mesh {
        std::string m_name;
        std::vector<std::pair<float, float>> m_weights;
        std::vector<gltf_primitive> m_primitives;

        gltf_result load(nlohmann::json& t_mesh_json, size_t t_index);
    };

} // namespace viewer

#endif // !VIEWER_GLTF_MESH_H
