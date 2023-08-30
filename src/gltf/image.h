#include "pch.h"

#ifndef VIEWER_GLTF_IMAGE_H
#define VIEWER_GLTF_IMAGE_H

#include <filesystem>

#include "result.h"

namespace viewer {
    struct gltf;

    struct gltf_image {
        size_t mo_buffer_view;   // Unused
        std::string m_mime_type; // Unused
        std::string m_uri;
        std::string m_name;

        gltf_result load(nlohmann::json& t_image_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_IMAGE_H
