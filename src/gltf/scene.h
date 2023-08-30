#include "pch.h"

#ifndef VIEWER_GLTF_SCENE_H
#define VIEWER_GLTF_SCENE_H

#include "result.h"

namespace viewer {

    struct gltf_scene {
        std::string m_name;
        std::vector<size_t> m_nodes;

        gltf_result load(nlohmann::json& t_scene_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_SCENE_H
