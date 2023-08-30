#include "pch.h"

#ifndef VIEWER_GLTF_HELPER_H
#define VIEWER_GLTF_HELPER_H

namespace viewer {
    glm::mat4 json_to_matrix4(nlohmann::json& t_matrix_json);
}

#endif // !VIEWER_GLTF_HELPER_H
