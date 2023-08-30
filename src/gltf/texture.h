#include "pch.h"

#ifndef VIEWER_GLTF_TEXTURE_H
#define VIEWER_GLTF_TEXTURE_H

#include "result.h"

namespace viewer {

    struct gltf_texture {
        size_t m_sampler = -1;
        size_t m_source = -1;
        std::string m_name;

        gltf_result load(nlohmann::json& t_texture_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_TEXTURE_H
