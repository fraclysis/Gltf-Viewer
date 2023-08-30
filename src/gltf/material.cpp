#include "pch.h"

#include "gltf.h"
#include "material.h"

namespace viewer {
    using json = nlohmann::json;

    bool gltf_texture_info::is_some() { return mo_index != -1; }

    gltf_result gltf_texture_info::load(nlohmann::json& t_texture_info) {
        if (t_texture_info.contains("index")) {
            mo_index = t_texture_info["index"];
        } else {
            return gltf_result("Texture must contain index.");
        }

        if (t_texture_info.contains("texCoord")) {
            m_tex_coord = t_texture_info["texCoord"];
        }

        return gltf_result();
    }

    gltf_result gltf_normal_texture_info::load(nlohmann::json& t_texture_info) {
        gltf_result res = gltf_texture_info::load(t_texture_info);
        if (res) {
            return res;
        }

        if (t_texture_info.contains("scale")) {
            m_scale = t_texture_info["scale"];
        }

        return gltf_result();
    }

    gltf_result gltf_occlusion_texture_info::load(nlohmann::json& t_texture_info) {
        gltf_result res = gltf_texture_info::load(t_texture_info);
        if (res) {
            return res;
        }

        if (t_texture_info.contains("strength")) {
            m_strength = t_texture_info["strength"];
        }

        return gltf_result();
    }

    gltf_result gltf_material::load(nlohmann::json& t_material_json, size_t t_index) {
        if (t_material_json.contains("name")) {
            m_name = t_material_json["name"];
        } else {
            m_name = "Material " + std::to_string(t_index);
        }

        if (t_material_json.contains("pbrMetallicRoughness")) {
            json& pbrMetallicRoughness = t_material_json["pbrMetallicRoughness"];
            m_pbr_metallic_roughness.load(pbrMetallicRoughness);
        }

        if (t_material_json.contains("normalTexture")) {
            m_normal_texture.load(t_material_json["normalTexture"]);
        }

        if (t_material_json.contains("occlusionTexture")) {
            m_occlusion_texture.load(t_material_json["occlusionTexture"]);
        }

        if (t_material_json.contains("emissiveTexture")) {
            m_emissive_texture.load(t_material_json["emissiveTexture"]);
        }

        if (t_material_json.contains("emissiveFactor")) {
            json& emissive_factor = t_material_json["emissiveFactor"];
            m_emissive_factor.x = emissive_factor[0];
            m_emissive_factor.y = emissive_factor[1];
            m_emissive_factor.z = emissive_factor[2];
        }

        if (t_material_json.contains("alphaMode")) {
            std::string alpha_mode = t_material_json["alphaMode"];
            m_alpha_mode = gltf_alpha_mode_from(alpha_mode);
        }

        if (t_material_json.contains("alphaCutoff")) {
            m_alpha_cutoff = t_material_json["alphaCutoff"];
        }

        if (t_material_json.contains("doubleSided")) {
            m_double_sided = t_material_json["doubleSided"];
        }

        return gltf_result();
    }

    void gltf_pbr_metallic_roughness::load(nlohmann::json& t_pbr_json) {
        if (t_pbr_json.contains("baseColorFactor")) {
            json& emissive_factor = t_pbr_json["baseColorFactor"];
            m_base_color_factor.r = emissive_factor[0];
            m_base_color_factor.g = emissive_factor[1];
            m_base_color_factor.b = emissive_factor[2];
            m_base_color_factor.a = emissive_factor[3];
        }

        if (t_pbr_json.contains("baseColorTexture")) {
            json& texture = t_pbr_json["baseColorTexture"];
            m_base_color_texture.load(texture);
        }

        if (t_pbr_json.contains("metallicRoughnessTexture")) {
            json& texture = t_pbr_json["metallicRoughnessTexture"];
            m_base_metallic_roughness_texture.load(texture);
        }

        if (t_pbr_json.contains("metallicFactor")) {
            m_metallic_factor = t_pbr_json["metallicFactor"];
        }

        if (t_pbr_json.contains("roughnessFactor")) {
            m_roughness_factor = t_pbr_json["roughnessFactor"];
        }
    }

    gltf_alpha_mode gltf_alpha_mode_from(std::string& t_alpha_mode) {
        if (t_alpha_mode == "OPAQUE") {
            return gltf_alpha_mode_opaque;
        }

        if (t_alpha_mode == "MASK") {
            return gltf_alpha_mode_mask;
        }

        if (t_alpha_mode == "BLEND") {
            return gltf_alpha_mode_blend;
        }

        log_warn("Unkown alpha mode %s. Using default alpha mode: "
                 "gltf_alpha_mode_opaque.",
                 t_alpha_mode.c_str());
        log_flush();

        return gltf_alpha_mode_opaque;
    }

} // namespace viewer