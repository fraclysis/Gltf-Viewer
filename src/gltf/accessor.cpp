#include "pch.h"

#include "accessor.h"

namespace viewer {
    using json = nlohmann::json;

    static size_t get_single_component_size(gltf_accessor& t_accessor) {
        switch (t_accessor.m_component_type) {
        case 5120: // GL_BYTE
        case 5121: // GL_UNSIGNED_BYTE
            return 1;
        case 5122: // GL_SHORT
        case 5123: // GL_UNSIGNED_SHORT
            return 2;
        case 5125: // GL_UNSIGNED_INT
        case 5126: // GL_FLOAT
            return 4;
        }

        return 0;
    }

    size_t gltf_accessor::component_size() { return single_component_size * component_count; }

    static size_t get_component_count(gltf_accessor& t_accessor) {
        switch (t_accessor.m_type) {
        case gltf_accessor_type_none:
            return 0;
        case gltf_accessor_type_scalar:
            return 1;
        case gltf_accessor_type_vec2:
            return 2;
        case gltf_accessor_type_vec3:
            return 3;
        case gltf_accessor_type_vec4:
            return 4;
        case gltf_accessor_type_mat2:
            return 4;
        case gltf_accessor_type_mat3:
            return 9;
        case gltf_accessor_type_mat4:
            return 16;
        default:
            return 0;
        }
    }

    gltf_result gltf_accessor::load(nlohmann::json& t_accessor_json, size_t t_index) {
        if (t_accessor_json.contains("name")) {
            m_name = t_accessor_json["name"];
        } else {
            m_name = "Accessor " + std::to_string(t_index);
        }

        if (t_accessor_json.contains("bufferView")) {
            mo_buffer_view = t_accessor_json["bufferView"];
        }

        if (t_accessor_json.contains("byteOffset")) {
            if (mo_buffer_view == -1) {
                return "accessor.byteOffset must not be defined when bufferView is undefined.";
            }
            m_byte_offset = t_accessor_json["byteOffset"];
        }

        if (t_accessor_json.contains("componentType")) {
            m_component_type = t_accessor_json["componentType"];

            switch (m_component_type) {
            case 5120:
            case 5121:
            case 5122:
            case 5123:
            case 5125:
            case 5126:
                break;
            default:
                // TODO gramer
                return "accessor.componentType is not one of BYTE, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, UNSIGNED_INT "
                       "or FLOAT.";
                break;
            }
        } else {
            return "accessor.componentType must not undefined.";
        }

        if (t_accessor_json.contains("normalized")) {
            m_normalized = t_accessor_json["normalized"];
        }

        if (t_accessor_json.contains("count")) {
            m_count = t_accessor_json["count"];
        } else {
            return "accessor.count must not undefined.";
        }

        if (t_accessor_json.contains("type")) {
            std::string type = t_accessor_json["type"];
            m_type = gltf_accessor_type_from(type);
            if (m_type == gltf_accessor_type_none) {
                return "Unsupported accessor.type.";
            }
        } else {
            return "accessor.type must not undefined.";
        }

        if (t_accessor_json.contains("max")) {
            json& max = t_accessor_json["max"];
            gltf_accessor_min_max_container max_container;
            max_container.load_container(max, m_type);
            mo_max = max_container;
        }

        if (t_accessor_json.contains("min")) {
            json& min = t_accessor_json["min"];
            gltf_accessor_min_max_container min_container;
            min_container.load_container(min, m_type);
            mo_min = min_container;
        }

        if (t_accessor_json.contains("sparse")) {
            json& sparse = t_accessor_json["sparse"];

            gltf_accessor_sparse sparse_;

            sparse_.m_count = sparse["count"];
            json& indices = sparse["indices"];
            sparse_.m_indices.mo_buffer_view = indices["bufferView"];
            sparse_.m_indices.m_component_type = indices["componentType"];
            if (indices.contains("byteOffset")) {
                sparse_.m_indices.m_byte_offset = indices["byteOffset"];
            }

            json& values = sparse["values"];
            sparse_.m_values.mo_buffer_view = values["bufferView"];
            if (values.contains("byteOffset")) {
                sparse_.m_values.m_byte_offset = values["byteOffset"];
            }

            mo_sparse = sparse_;
        }

        single_component_size = get_single_component_size(*this);
        component_count = get_component_count(*this);

        return gltf_result();
    }

    gltf_accessor_type gltf_accessor_type_from(std::string& t_type) {
        if (t_type == "SCALAR")
            return gltf_accessor_type_scalar;
        if (t_type == "VEC2")
            return gltf_accessor_type_vec2;
        if (t_type == "VEC3")
            return gltf_accessor_type_vec3;
        if (t_type == "VEC4")
            return gltf_accessor_type_vec4;
        if (t_type == "MAT2")
            return gltf_accessor_type_mat2;
        if (t_type == "MAT3")
            return gltf_accessor_type_mat3;
        if (t_type == "MAT4")
            return gltf_accessor_type_mat4;

        log_warn("Unkown accessor type: %s.\n", t_type.c_str());
        log_flush();

        return gltf_accessor_type_none;
    }

    void gltf_accessor_min_max_container::load_container(nlohmann::json& t_min_max, gltf_accessor_type t_type) {
        // std::stringstream ss;
        // ss << t_min_max;
        // log_trace("Load container min max: %s", ss.str().c_str());

        float f0 = t_min_max[0];

        if (t_type == gltf_accessor_type_scalar) {
            u_vec1.x = f0;
            return;
        }

        float f1 = t_min_max[1];

        if (t_type == gltf_accessor_type_vec2) {
            u_vec2.x = f0;
            u_vec2.y = f1;
            return;
        }

        float f2 = t_min_max[2];

        if (t_type == gltf_accessor_type_vec3) {
            u_vec3.x = f0;
            u_vec3.y = f1;
            u_vec3.z = f2;
            return;
        }

        float f3 = t_min_max[3];

        if (t_type == gltf_accessor_type_vec4) {
            u_vec4.x = f0;
            u_vec4.y = f1;
            u_vec4.z = f2;
            u_vec4.w = f3;
            return;
        }
    }

    bool gltf_accessor_sparse::is_some() { return m_count != -1; }
} // namespace viewer