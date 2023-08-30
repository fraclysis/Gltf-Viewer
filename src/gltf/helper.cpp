#include "pch.h"

#include "helper.h"

namespace viewer {

    glm::mat4 json_to_matrix4(nlohmann::json& t_matrix_json) {
        float m0 = t_matrix_json[0];
        float m1 = t_matrix_json[1];
        float m2 = t_matrix_json[2];
        float m3 = t_matrix_json[3];
        float m4 = t_matrix_json[4];
        float m5 = t_matrix_json[5];
        float m6 = t_matrix_json[6];
        float m7 = t_matrix_json[7];
        float m8 = t_matrix_json[8];
        float m9 = t_matrix_json[9];
        float m10 = t_matrix_json[10];
        float m11 = t_matrix_json[11];
        float m12 = t_matrix_json[12];
        float m13 = t_matrix_json[13];
        float m14 = t_matrix_json[14];
        float m15 = t_matrix_json[15];

        return glm::mat4(m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);
    }

} // namespace viewer