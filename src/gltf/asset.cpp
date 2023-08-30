#include "pch.h"

#include "asset.h"

#define IF_EXISTS(j, m_name, out)                                                                                      \
    if (j.contains(m_name)) {                                                                                          \
        out = j[m_name];                                                                                               \
    }

namespace viewer {

    gltf_result gltf_asset::load(nlohmann::json& t_asset_json) {
        m_version = t_asset_json["version"];

        IF_EXISTS(t_asset_json, "copyright", m_copyright);
        IF_EXISTS(t_asset_json, "generator", m_generator);
        IF_EXISTS(t_asset_json, "minVersion", m_min_version);

        return gltf_result();
    }

} // namespace viewer