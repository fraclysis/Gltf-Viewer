#include "pch.h"

#include "animation.h"

namespace viewer {
    using json = nlohmann::json;

    gltf_result gltf_animation::load(nlohmann::json& t_animation_json, size_t t_index) {

        if (t_animation_json.contains("name")) {
            m_name = t_animation_json["name"];
        } else {
            m_name = "Animation " + std::to_string(t_index);
        }

        json& channels = t_animation_json["channels"];
        size_t channels_size = channels.size();
        m_channels.reserve(channels_size);

        size_t ii = 0;
        for (json& channel : channels) {
            m_channels.emplace_back();
            gltf_animation_channel& gltf_channel = m_channels.back();
            gltf_channel.m_sampler = channel["sampler"];
            json& target = channel["target"];

            gltf_channel.m_target.m_node = target["node"];
            std::string path = target["path"];

            if (path == "translation") {
                gltf_channel.m_target.m_path = gltf_animation_channel_target_path_translation;
            } else if (path == "rotation") {
                gltf_channel.m_target.m_path = gltf_animation_channel_target_path_rotation;
            } else if (path == "scale") {
                gltf_channel.m_target.m_path = gltf_animation_channel_target_path_scale;
            } else if (path == "weights") {
                gltf_channel.m_target.m_path = gltf_animation_channel_target_path_weights;
            } else {
                log_warn("Channel %zd in %s is skipped due to having unsupported target path (%s).", ii, m_name.c_str(),
                         path.c_str());
                log_flush();
            }

            ii++;
        }

        json& samplers = t_animation_json["samplers"];
        size_t samplers_size = samplers.size();
        m_samplers.reserve(samplers_size);

        for (json& sampler : samplers) {
            m_samplers.emplace_back();
            gltf_animation_sampler& gltf_sampler = m_samplers.back();

            gltf_sampler.m_input = sampler["input"];
            gltf_sampler.m_output = sampler["output"];

            if (sampler.contains("interpolation")) {
                std::string interpolation = sampler["interpolation"];

                if (interpolation == "STEP") {
                    gltf_sampler.m_interpolation = gltf_animation_sampler_interpolation_step;
                } else if (interpolation == "CUBICSPLINE") {
                    gltf_sampler.m_interpolation = gltf_animation_sampler_interpolation_cubicspline;
                } else {
                    gltf_sampler.m_interpolation = gltf_animation_sampler_interpolation_linear;
                }
            }
        }

        return gltf_result();
    }
} // namespace viewer