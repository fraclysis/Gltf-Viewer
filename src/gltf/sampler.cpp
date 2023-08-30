#include "pch.h"

#include "sampler.h"

namespace viewer {

    gltf_result gltf_sampler::load(nlohmann::json& t_sampler_json, size_t t_index) {
        if (t_sampler_json.contains("magFilter")) {
            size_t magFilter = t_sampler_json["magFilter"];
            m_mag_filter = magFilter;
        }

        if (t_sampler_json.contains("minFilter")) {
            size_t minFilter = t_sampler_json["minFilter"];
            m_min_filter = minFilter;
        }

        if (t_sampler_json.contains("wrapS")) {
            size_t wrapS = t_sampler_json["wrapS"];
            m_wrap_s = wrapS;
        }

        if (t_sampler_json.contains("wrapT")) {
            size_t wrapT = t_sampler_json["wrapT"];
            m_wrap_t = wrapT;
        }

        if (t_sampler_json.contains("name")) {
            m_name = t_sampler_json["name"];
        } else {
            m_name = "Sampler " + std::to_string(t_index);
        }

        return gltf_result();
    }

#if 0
void gltf_sampler::create_gl_sampler()
{
	glGenSamplers(1, &m_gl_sampler);

	glSamplerParameteri(m_gl_sampler, GL_TEXTURE_WRAP_S, (GLint)m_wrap_s);
	glSamplerParameteri(m_gl_sampler, GL_TEXTURE_WRAP_T, (GLint)m_wrap_t);

	if (m_mag_filter != ~0) {
		glSamplerParameteri(m_gl_sampler, GL_TEXTURE_MAG_FILTER, (GLint)m_mag_filter);
	}

	if (m_min_filter != ~0) {
		glSamplerParameteri(m_gl_sampler, GL_TEXTURE_MIN_FILTER, (GLint)m_min_filter);
	}

	//glSamplerParameteri(m_gl_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 //	glSamplerParameteri(m_gl_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

}

void gltf_sampler::free()
{
	if (m_gl_sampler) glDeleteSamplers(1, &m_gl_sampler); else __debugbreak();
}
#endif

} // namespace viewer