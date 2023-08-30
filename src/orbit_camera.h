#include "pch.h"

#ifndef VIEWER_ORBIT_CAMERA_H_
#define VIEWER_ORBIT_CAMERA_H_

#include <glm/glm.hpp>

namespace viewer {

    struct orbit_camera {
        glm::mat4 m_projection;
        glm::mat4 m_view;

        glm::vec3 m_eye;
        glm::vec3 m_center;

        float m_yaw;
        float m_pitch;
        float m_radius = 10.f;
        float m_min_radius = 0.001f;
        float m_step = 1.0f;
        float m_object_radius;

        void zoom(float t_delta);
        void control(float t_delta_x, float t_delta_y);
        void perspective(double aspect, double fovy = 3.14 / 2.0);
        void update();
    };

} // namespace viewer

#endif // !VIEWER_ORBIT_CAMERA_H_
