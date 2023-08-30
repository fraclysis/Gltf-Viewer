#include "pch.h"

#ifndef VIEWER_GLTF_BUFFER_H
#define VIEWER_GLTF_BUFFER_H

#include "result.h"
#include <filesystem>

namespace viewer {

    struct gltf_buffer {
        std::string m_name;
        std::string m_uri;
        size_t m_byte_length;

        gltf_result load(nlohmann::json& t_buffer_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_BUFFER_H
