#include "pch.h"

#include "buffer_view.h"

namespace viewer {

    gltf_result gltf_buffer_view::load(nlohmann::json& t_buffer_view_json, size_t t_index) {
        if (t_buffer_view_json.contains("buffer")) {
            m_buffer = t_buffer_view_json["buffer"];
        } else {
            return "Must contain a buffer.";
        }

        if (t_buffer_view_json.contains("byteLength")) {
            m_byte_length = t_buffer_view_json["byteLength"];
        } else {
            return "Must contain a byte length.";
        }

        if (t_buffer_view_json.contains("byteOffset")) {
            m_byte_offset = t_buffer_view_json["byteOffset"];
        } else {
            m_byte_offset = 0;
        }

        if (t_buffer_view_json.contains("byteStride")) {
            mo_byte_stride = t_buffer_view_json["byteStride"];
        }

        if (t_buffer_view_json.contains("target")) {
            m_target = t_buffer_view_json["target"];
        } else {
            m_target = gltf_buffer_view_target_none;
        }

        if (t_buffer_view_json.contains("name")) {
            m_name = t_buffer_view_json["name"];
        } else {
            m_name = "Buffer view " + std::to_string(t_index);
        }

        return gltf_result();
    }

} // namespace viewer