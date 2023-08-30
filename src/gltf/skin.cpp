#include "pch.h"

#include "skin.h"

namespace viewer {

    using json = nlohmann::json;

    gltf_result gltf_skin::load(nlohmann::json& t_skin_json, size_t t_index) {
        if (t_skin_json.contains("inverseBindMatrices")) {
            m_inverse_bind_matrices = t_skin_json["inverseBindMatrices"];
        } else {
            m_inverse_bind_matrices = -1;
        }

        if (t_skin_json.contains("skeleton")) {
            m_skeleton = t_skin_json["skeleton"];
        } else {
            m_skeleton = -1;
        }

        json& joints = t_skin_json["joints"];
        m_joints.reserve(joints.size());

        for (int joint : joints) {
            m_joints.push_back(joint);
        }

        return gltf_result();
    }

} // namespace viewer