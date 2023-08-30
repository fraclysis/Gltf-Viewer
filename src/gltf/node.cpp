#include "pch.h"

#include "node.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "gltf.h"
#include "helper.h"

namespace viewer {

    using json = nlohmann::json;

    gltf_result gltf_node::load(nlohmann::json& t_node_json, size_t index) {
        if (t_node_json.contains("name")) {
            m_name = t_node_json["name"];
        } else {
            m_name = "Node " + std::to_string(index);
        }

        if (t_node_json.contains("camera")) {
            m_camera = t_node_json["camera"];
        }

        if (t_node_json.contains("skin")) {
            m_skin = t_node_json["skin"];
        }

        if (t_node_json.contains("mesh")) {
            m_mesh = t_node_json["mesh"];
        }

        if (t_node_json.contains("matrix")) {
            json& matrix = t_node_json["matrix"];
            m_matrix = json_to_matrix4(matrix);
        }

        if (t_node_json.contains("translation")) {
            json& translation = t_node_json["translation"];
            m_translation.x = translation[0];
            m_translation.y = translation[1];
            m_translation.z = translation[2];
        }

        if (t_node_json.contains("rotation")) {
            json& rotation = t_node_json["rotation"];
            m_rotation.w = rotation[3];
            m_rotation.x = rotation[0];
            m_rotation.y = rotation[1];
            m_rotation.z = rotation[2];
        }

        if (t_node_json.contains("scale")) {
            json& scale = t_node_json["scale"];
            m_scale.x = scale[0];
            m_scale.y = scale[1];
            m_scale.z = scale[2];
        }

        if (t_node_json.contains("weights")) {
            json& weights = t_node_json["weights"];

            for (json& weight : weights) {
                size_t weightId = weight;
                m_weights.push_back(weightId);
            }
        }

        if (t_node_json.contains("children")) {
            json& children = t_node_json["children"];

            for (json& childId : children) {
                size_t childNodeId = childId;
                m_children.push_back(childNodeId);
            }
        }

        return gltf_result();
    }

} // namespace viewer