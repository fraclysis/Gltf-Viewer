#include "pch.h"

#ifndef VIEWER_GLTF_BUFFER_VIEW_H
#define VIEWER_GLTF_BUFFER_VIEW_H

#include "result.h"

namespace viewer {

    enum gltf_buffer_view_target : size_t {
        gltf_buffer_view_target_none,
        gltf_buffer_view_target_array_buffer = 0x8892,
        gltf_buffer_view_target_element_array_buffer = 0x8893,
    };

    struct gltf_buffer_view {
        size_t m_buffer;
        size_t m_byte_offset = 0;
        size_t m_byte_length;
        size_t mo_byte_stride = 0;
        gltf_buffer_view_target m_target;
        std::string m_name;

        gltf_result load(nlohmann::json& t_buffer_view_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_BUFFER_VIEW_H
