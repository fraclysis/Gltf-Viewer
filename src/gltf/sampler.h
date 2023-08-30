#include "pch.h"

#ifndef VIEWER_GLTF_SAMPLER_H
#define VIEWER_GLTF_SAMPLER_H

#include "result.h"

namespace viewer {

    struct gltf_sampler {
        std::string m_name;
        size_t m_mag_filter = -1;
        size_t m_min_filter = -1;
        size_t m_wrap_s = 10497;
        size_t m_wrap_t = 10497;

        gltf_result load(nlohmann::json& t_sampler_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_SAMPLER_H
