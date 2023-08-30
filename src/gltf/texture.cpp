#include "pch.h"

#include "texture.h"

namespace viewer {

    gltf_result gltf_texture::load(nlohmann::json& t_texture_json,
                                   size_t t_index) {
        if (t_texture_json.contains("name")) {
            m_name = t_texture_json["name"];
        } else {
            m_name = "Texture " + std::to_string(t_index);
        }

        if (t_texture_json.contains("sampler")) {
            m_sampler = t_texture_json["sampler"];
        }

        if (t_texture_json.contains("source")) {
            m_source = t_texture_json["source"];
        }

        return gltf_result();
    }

} // namespace viewer