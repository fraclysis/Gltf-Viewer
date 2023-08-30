#include "pch.h"

#include "accessor.h"
#include "mesh.h"
#include <glm/gtc/type_ptr.hpp>

namespace viewer {

    using json = nlohmann::json;

    gltf_result gltf_primitive::load_primitive(nlohmann::json& t_primitive_json, size_t t_index) {
        if (t_primitive_json.contains("attributes")) {
            json& attributes = t_primitive_json["attributes"];

            gltf_result res = m_attributes.load_attributes(attributes);
            if (res) {
                return res;
            }
        } else {
            return gltf_result("Primitive must contain attributes.");
        }

        if (t_primitive_json.contains("indices")) {
            mo_indices = t_primitive_json["indices"];
        } else {
            mo_indices = -1;
        }

        if (t_primitive_json.contains("material")) {
            mo_material = t_primitive_json["material"];
        } else {
            mo_material = -1;
        }

        if (t_primitive_json.contains("mode")) {
            m_mode = t_primitive_json["mode"];
        } else {
            m_mode = gltf_primitive_target_triangles;
        }

        if (t_primitive_json.contains("targets")) {
            json& targets = t_primitive_json["targets"];

            size_t size = targets.size();
            m_targets.reserve(size);

            for (json& target : targets) {
                m_targets.emplace_back();
                gltf_attributes& attribute_target = m_targets.back();

                gltf_result res = attribute_target.load_attributes(target);
                if (res)
                    return res;
            }
        }

        return gltf_result();
    }

    gltf_result gltf_mesh::load(nlohmann::json& t_mesh_json, size_t t_index) {
        if (t_mesh_json.contains("name")) {
            m_name = t_mesh_json["name"];
        } else {
            m_name = "Mesh " + std::to_string(t_index);
        }

        if (t_mesh_json.contains("primitives")) {
            json& primitives = t_mesh_json["primitives"];
            size_t size = primitives.size();

            m_primitives.reserve(size);

            size_t i = 0;
            for (json& primitive : primitives) {
                m_primitives.emplace_back();
                gltf_primitive& prim = m_primitives.back();

                gltf_result res = prim.load_primitive(primitive, i);
                if (res) {
                    return res;
                }

                i++;
            }
        } else {
            return gltf_result("Mesh must contain a primitive.");
        }

        if (t_mesh_json.contains("weights")) {
            json& weights = t_mesh_json["weights"];
            size_t size = weights.size();

            m_weights.reserve(size);

            for (float weight : weights) {
                m_weights.push_back(std::pair<float, float>(weight, 0.0f));
            }
        }

        return gltf_result();
    }

    gltf_result gltf_attributes::load_attributes(nlohmann::json& t_attributes_json) {
        if (t_attributes_json.contains("POSITION")) {
            mo_POSITION = t_attributes_json["POSITION"];
        }

        if (t_attributes_json.contains("NORMAL")) {
            mo_NORMAL = t_attributes_json["NORMAL"];
        }

        if (t_attributes_json.contains("TANGENT")) {
            mo_TANGENT = t_attributes_json["TANGENT"];
        }

        if (t_attributes_json.contains("TEXCOORD_0")) {
            mo_TEXCOORD_0 = t_attributes_json["TEXCOORD_0"];
        }

        if (t_attributes_json.contains("TEXCOORD_1")) {
            mo_TEXCOORD_1 = t_attributes_json["TEXCOORD_1"];
        }

        if (t_attributes_json.contains("COLOR_0")) {
            mo_COLOR_0 = t_attributes_json["COLOR_0"];
        }

        if (t_attributes_json.contains("JOINTS_0")) {
            mo_JOINTS_0 = t_attributes_json["JOINTS_0"];
        }

        if (t_attributes_json.contains("WEIGHTS_0")) {
            mo_WEIGHTS_0 = t_attributes_json["WEIGHTS_0"];
        }

        return gltf_result();
    }

} // namespace viewer