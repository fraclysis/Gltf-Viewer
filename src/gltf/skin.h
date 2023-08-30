#include "pch.h"

#ifndef VIEWER_GLTF_SKIN_H
#define VIEWER_GLTF_SKIN_H

#include "result.h"

namespace viewer {

    struct gltf_skin {
        size_t m_inverse_bind_matrices;
        size_t m_skeleton;
        std::vector<size_t> m_joints;
        std::string m_name;

        gltf_result load(nlohmann::json& t_skin_json, size_t t_index);
    };

} // namespace viewer

#endif // !VIEWER_GLTF_SKIN_H
