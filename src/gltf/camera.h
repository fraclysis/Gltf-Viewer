#include "pch.h"

#ifndef VIEWER_GLTF_CAMERA_H
#define VIEWER_GLTF_CAMERA_H

#include "result.h"

namespace viewer {
    enum gltf_camera_type {
        gltf_camera_type_perspective,
        gltf_camera_type_orthographic,
    };

    struct gltf_camera_perspective {
        float_t m_aspect_ratio;
        float_t m_yfov;
        float_t m_zfar;
        float_t m_znear;
    };

    struct gltf_camera_orthographic {
        float_t m_aspect_ratio;
        float_t m_xmag;
        float_t m_ymag;
        float_t m_zfar;
        float_t m_znear;
    };

    // TODO camera is broken
    struct gltf_camera {
        glm::mat4 m_projection;

        std::string m_name;
        gltf_camera_type m_type;

        union {
            float_t u_aspect_ratio;
            gltf_camera_perspective u_perspective;
            gltf_camera_orthographic u_orthographic;
        } m_camera;

        float_t update_projection(float_t width, float_t height);

        gltf_result load(nlohmann::json& t_camera_json, size_t t_index);
    };

} // namespace viewer
#endif // !VIEWER_GLTF_CAMERA_H
