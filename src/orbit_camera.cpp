#include "pch.h"

#include "orbit_camera.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace viewer {

    const glm::vec3 UP(0.0f, 1.0f, 0.0f);
    const glm::vec3 ZUP(0.0f, 0.0f, 1.0f);

    void orbit_camera::zoom(float t_delta) {
        m_radius -= t_delta * m_step;
        if (m_radius < m_min_radius) {
            m_radius = m_min_radius;
        }
    }

    void orbit_camera::control(float t_delta_x, float t_delta_y) {

        m_yaw += t_delta_x;

        constexpr float fullCircle = 2.0f * glm::pi<float>();
        m_yaw = fmodf(m_yaw, fullCircle);
        if (m_yaw < 0.0f) {
            m_yaw = fullCircle + m_yaw;
        }

        m_pitch += t_delta_y;

        constexpr float polarCap = glm::pi<float>() / 2.0f - 0.001f;
        if (m_pitch > polarCap) {
            m_pitch = polarCap;
        }

        if (m_pitch < -polarCap) {
            m_pitch = -polarCap;
        }
    }

    void orbit_camera::perspective(double aspect, double fovy) {
        m_projection = glm::perspective(fovy, aspect, 0.01, 1000.0);
    }

    void orbit_camera::update() {
        float yaw = m_yaw;
        float pitch = m_pitch;
        float r = m_radius;

        float p = r * sin(pitch);

        // float x = p * cos(yaw);
        // float y = r * cos(pitch);
        // float z = p * sin(yaw);

        float x = m_center.x + m_radius * cos(pitch) * cos(yaw);
        float y = m_center.y + m_radius * sin(pitch);
        float z = m_center.z + m_radius * cos(pitch) * sin(yaw);

        m_eye = glm::vec3(x, y, z);
        m_view = glm::lookAt(m_eye, m_center, UP);
    }
} // namespace viewer