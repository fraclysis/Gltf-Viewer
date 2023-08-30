#include "pch.h"

#ifndef VIEWER_GLTF_ANIMATION_H
#define VIEWER_GLTF_ANIMATION_H

#include "result.h"

namespace viewer {

    enum gltf_animation_channel_target_path {
        gltf_animation_channel_target_path_translation,
        gltf_animation_channel_target_path_rotation,
        gltf_animation_channel_target_path_scale,
        gltf_animation_channel_target_path_weights,
    };

    struct gltf_animation_channel_target {
        size_t m_node;
        gltf_animation_channel_target_path m_path;
    };

    struct gltf_animation_channel {
        size_t m_sampler;
        gltf_animation_channel_target m_target;
    };

    enum gltf_animation_sampler_interpolation {
        gltf_animation_sampler_interpolation_linear,
        gltf_animation_sampler_interpolation_step,
        gltf_animation_sampler_interpolation_cubicspline,
    };

    struct gltf_animation_sampler {
        size_t m_input;
        size_t m_output;
        gltf_animation_sampler_interpolation m_interpolation;
    };

    struct gltf_animation {
        std::vector<gltf_animation_channel> m_channels;
        std::vector<gltf_animation_sampler> m_samplers;
        std::string m_name;

        gltf_result load(nlohmann::json& t_animation_json, size_t t_index);
    };

} // namespace viewer

#endif // !VIEWER_GLTF_ANIMATION_H
