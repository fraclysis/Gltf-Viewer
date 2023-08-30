#include "pch.h"

#ifndef VIEWER_GLTF_ASSET_H
#define VIEWER_GLTF_ASSET_H

#include "result.h"

namespace viewer {

    struct gltf_asset {
        std::string m_copyright;
        std::string m_generator;
        std::string m_version;
        std::string m_min_version;

        gltf_result load(nlohmann::json& t_asset_json);
    };

} // namespace viewer

#endif // !VIEWER_GLTF_ASSET_H
