#include "pch.h"

#include "scene.h"

namespace viewer {

    gltf_result gltf_scene::load(nlohmann::json& t_scene_json, size_t t_index) {
        if (t_scene_json.contains("name")) {
            m_name = t_scene_json["name"];
        } else {
            m_name = "Scene " + std::to_string(t_index);
        }

        if (t_scene_json.contains("nodes")) {
            auto& nodes = t_scene_json["nodes"];

            size_t size = nodes.size();
            m_nodes.reserve(size);

            for (size_t node : nodes) {
                m_nodes.push_back(node);
            }
        }

        return gltf_result();
    }

} // namespace viewer