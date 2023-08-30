#include "pch.h"
#include "pch.h"

#include "image.h"

namespace viewer {

    gltf_result gltf_image::load(nlohmann::json& t_image_json, size_t t_index) {
        if (t_image_json.contains("name")) {
            m_name = t_image_json["name"];
        } else {
            m_name = "Image " + std::to_string(t_index);
        }

        if (t_image_json.contains("uri")) {
            m_uri = t_image_json["uri"];
        } else {
            __debugbreak();
            return "Images without an uri not supported for now.";
        }

        return gltf_result();
    }

} // namespace viewer