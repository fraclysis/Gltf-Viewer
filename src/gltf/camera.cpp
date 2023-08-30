#include "pch.h"

#include "camera.h"

#include "helper.h"
#include <glm/gtc/matrix_transform.hpp>

namespace viewer {

    float_t gltf_camera::update_projection(float_t width, float_t height) {
        if (m_camera.u_aspect_ratio) {
            return m_camera.u_aspect_ratio;
        }

        float_t aspect_ratio = width / height;

        // switch (m_type)
        //{
        // case gltf_camera_type_perspective:
        //	gltf_camera_perspective& p_camera = m_camera.u_perspective;

        //	m_projection = glm::perspective(p_camera.m_yfov, aspect_ratio,
        // p_camera.m_znear, p_camera.m_zfar); 	break; case
        // gltf_camera_type_orthographic: 	gltf_camera_orthographic& o_camera =
        // m_camera.u_orthographic;

        //	float_t r = o_camera.m_xmag;
        //	float_t t = o_camera.m_ymag;
        //	float_t f = o_camera.m_zfar;
        //	float_t n = o_camera.m_znear;

        //	m_projection = glm::mat4(
        //		1.0f / r, 0.0f, 0.0f, 0.0f,
        //		0.0f, 1.0f / t, 0.0f, 0.0f,
        //		0.0f, 0.0f, 2.0f / (n - f), (f + n) / (n - f),
        //		0.0f, 0.0f, 0.0f, 1.0f
        //	);
        //	break;
        // default:
        //	__debugbreak();
        //	break;
        //}

        return aspect_ratio;
    }

    gltf_result gltf_camera::load(nlohmann::json& t_camera_json, size_t t_index) {
        (void)(t_index);
        return gltf_result();

        if (t_camera_json.contains("perspective")) {
            m_type = gltf_camera_type_perspective;
            m_projection = json_to_matrix4(t_camera_json["perspective"]);
        } else if (t_camera_json.contains("orthographic")) {
            m_type = gltf_camera_type_orthographic;
            m_projection = json_to_matrix4(t_camera_json["orthographic"]);
        } else {
            gltf_result("No camera.");
        }

        return gltf_result();
    }

} // namespace viewer