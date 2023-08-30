#include "pch.h"

#ifndef VIEWER_GLTF_ACCESSOR_H
#define VIEWER_GLTF_ACCESSOR_H

#include "result.h"

namespace viewer {
    enum gltf_accessor_type {
        gltf_accessor_type_none,
        gltf_accessor_type_scalar,
        gltf_accessor_type_vec2,
        gltf_accessor_type_vec3,
        gltf_accessor_type_vec4,
        gltf_accessor_type_mat2,
        gltf_accessor_type_mat3,
        gltf_accessor_type_mat4,
    };

    gltf_accessor_type gltf_accessor_type_from(std::string& t_type);

    struct gltf_accessor_sparse_indices {
        size_t mo_buffer_view;
        size_t m_byte_offset = 0;
        size_t m_component_type;
    };

    struct gltf_accessor_sparse_values {
        size_t mo_buffer_view;
        size_t m_byte_offset = 0;
    };

    struct gltf_accessor_sparse {
        size_t m_count;
        gltf_accessor_sparse_indices m_indices;
        gltf_accessor_sparse_values m_values;

        bool is_some();
    };

    union gltf_accessor_min_max_container {
        glm::vec1 u_vec1;
        glm::vec2 u_vec2;
        glm::vec3 u_vec3;
        glm::vec4 u_vec4;
        glm::mat2 u_mat2;
        glm::mat3 u_mat3;
        glm::mat4 u_mat4;

        gltf_accessor_min_max_container() = default;

        void load_container(nlohmann::json& t_min_max, gltf_accessor_type t_type);
    };

    struct gltf_accessor {
        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_accessor_bufferview
        size_t mo_buffer_view = -1;
        size_t m_byte_offset = 0;
        size_t m_component_type;
        size_t m_count = 0;
        gltf_accessor_type m_type;
        std::optional<gltf_accessor_min_max_container> mo_max;
        std::optional<gltf_accessor_min_max_container> mo_min;
        std::optional<gltf_accessor_sparse> mo_sparse;
        std::string m_name;
        bool m_normalized = false;

        size_t single_component_size;
        size_t component_count;
        size_t component_size();

        gltf_result load(nlohmann::json& t_accessor_json, size_t t_index);
    };
} // namespace viewer

#endif // !VIEWER_GLTF_ACCESSOR_H
