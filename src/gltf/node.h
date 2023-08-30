#include "pch.h"

#ifndef VIEWER_GLTF_NODE_H
#define VIEWER_GLTF_NODE_H

#include "result.h"

#include <glm/gtx/quaternion.hpp>

namespace viewer {

    struct gltf;

    struct gltf_node {
        glm::vec3 m_translation;
        glm::quat m_rotation;
        glm::vec3 m_scale = glm::vec3(1.0f);
        glm::mat4 m_matrix = glm::mat4(1.0f);
        glm::mat4 m_world_matrix = glm::mat4(1.0f);

        size_t m_camera = -1;
        size_t m_mesh = -1;
        size_t m_skin = -1;

        std::string m_name;
        std::vector<size_t> m_children;
        std::vector<size_t> m_weights;

        gltf_result load(nlohmann::json& t_node_json, size_t index);

        inline void update_local_matrix() {
            m_matrix = glm::mat4(1.0f);
            m_matrix = glm::translate(m_matrix, m_translation);
            m_matrix = m_matrix * glm::toMat4(m_rotation);
            m_matrix = glm::scale(m_matrix, m_scale);
        }
    };

} // namespace viewer
#endif // !VIEWER_GLTF_NODE_H
