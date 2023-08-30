#include "pch.h"

#include "buffer.h"

namespace viewer {

    gltf_result gltf_buffer::load(nlohmann::json& t_buffer_json, size_t t_index) {
        if (t_buffer_json.contains("name")) {
            m_name = t_buffer_json["name"];
        } else {
            m_name = "Buffer " + std::to_string(t_index);
        }

        if (t_buffer_json.contains("uri")) {
            m_uri = t_buffer_json["uri"];
        } else {
            return "Buffers without an uri not supported.";
        }

        m_byte_length = t_buffer_json["byteLength"];

        return gltf_result();
    }

} // namespace viewer
