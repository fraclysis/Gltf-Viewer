#include "pch.h"

#ifndef VIEWER_GLTF_MATERIAL_H
#define VIEWER_GLTF_MATERIAL_H

#include "../program.h"

#include "result.h"

namespace viewer {

    struct gltf;

    enum gltf_alpha_mode {
        gltf_alpha_mode_opaque,
        gltf_alpha_mode_mask,
        gltf_alpha_mode_blend,
    };

    gltf_alpha_mode gltf_alpha_mode_from(std::string& t_alpha_mode);

    struct gltf_texture_info {
        size_t mo_index = -1;
        size_t m_tex_coord = 0;

        bool is_some();

        gltf_result load(nlohmann::json& t_texture_info);
    };

    struct gltf_normal_texture_info : gltf_texture_info {
        float_t m_scale = 1.0f;

        gltf_result load(nlohmann::json& t_texture_info);
    };

    struct gltf_occlusion_texture_info : gltf_texture_info {
        float_t m_strength = 1.0f;

        gltf_result load(nlohmann::json& t_texture_info);
    };

    struct gltf_pbr_metallic_roughness {
        glm::vec4 m_base_color_factor = glm::vec4(1.0f);
        gltf_texture_info m_base_color_texture;
        float_t m_metallic_factor = 0.5f;
        float_t m_roughness_factor = 0.5f;
        gltf_texture_info m_base_metallic_roughness_texture;

        void load(nlohmann::json& t_pbr_json);
    };

    struct gltf_material {
        std::string m_name;
        gltf_pbr_metallic_roughness m_pbr_metallic_roughness;
        gltf_normal_texture_info m_normal_texture;
        gltf_occlusion_texture_info m_occlusion_texture;
        gltf_texture_info m_emissive_texture;
        glm::vec3 m_emissive_factor = glm::vec3(0.0f);
        gltf_alpha_mode m_alpha_mode = gltf_alpha_mode_opaque;
        float_t m_alpha_cutoff = 0.5f;
        bool m_double_sided = false;

        gltf_result load(nlohmann::json& t_material_json, size_t t_index);
    };

} // namespace viewer

#endif // !VIEWER_GLTF_MATERIAL_H
