#include "pch.h"

#include "gltf.h"

#include "glm/gtx/associated_min_max.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "../log.h"

#include "../morph.h"
#include "../renderer.h"

namespace viewer {

    extern morph_program g_tfp;

    using json = nlohmann::json;

    template <typename T> gltf_result load_type(nlohmann::json& t_type_json, std::vector<T>& t_output) {
        size_t size = t_type_json.size();
        t_output.reserve(size);

        size_t i = 0;
        for (json& type : t_type_json) {
            t_output.emplace_back();
            auto& val = t_output.back();

            gltf_result res = val.load(type, i);
            if (res)
                return res;

            i++;
        }

        return gltf_result();
    }

#define RETURN_LOAD(x, y)                                                                                              \
    if (t_gltf_json.contains(#x)) {                                                                                    \
        gltf_result res = load_type(t_gltf_json[#x], y);                                                               \
        if (res)                                                                                                       \
            return res;                                                                                                \
    }

    gltf_result gltf::load_from_path(const std::string&& t_path) {
        std::filesystem::path path(t_path);
        m_base_path = path.parent_path();

        log_info("Model path: %s\n", t_path.c_str());
        log_info("Model base path: %s\n", m_base_path.string().c_str());

        std::ifstream gltf_file(path);
        json data = json::parse(gltf_file);

        gltf_result res = load_json(data);
        if (res) {
            __debugbreak();
            return res;
        }

        return gltf_result();
    }

    gltf_result gltf::load_json(nlohmann::json& t_gltf_json) {
        if (t_gltf_json.contains("asset")) {
            gltf_result res = m_asset.load(t_gltf_json["asset"]);
            if (res)
                return res;
        } else {
            return gltf_result("gltf do not contains an asset.");
        }

        RETURN_LOAD(accessors, m_accessors);
        RETURN_LOAD(animations, m_animations);
        RETURN_LOAD(buffers, m_buffers);
        RETURN_LOAD(bufferViews, m_buffer_views);
        RETURN_LOAD(cameras, m_cameras);
        RETURN_LOAD(materials, m_materials);
        RETURN_LOAD(nodes, m_nodes);
        RETURN_LOAD(samplers, m_samplers);
        RETURN_LOAD(scenes, m_scenes);
        RETURN_LOAD(meshes, m_meshes);
        RETURN_LOAD(skins, m_skins);
        RETURN_LOAD(images, m_images);
        RETURN_LOAD(textures, m_textures);

        find_min_max();

        return gltf_result();
    }

    void gltf::find_min_max() {
        for (gltf_mesh& mesh : m_meshes) {
            for (gltf_primitive& primitive : mesh.m_primitives) {
                if (primitive.m_attributes.mo_POSITION != ~0) {
                    gltf_accessor& accessor = m_accessors[primitive.m_attributes.mo_POSITION];
                    if (accessor.m_type == gltf_accessor_type_vec3) {
                        m_min_size = glm::min(accessor.mo_min.value().u_vec3, m_min_size);
                        m_max_size = glm::max(accessor.mo_max.value().u_vec3, m_max_size);
                    } else {
                        __debugbreak();
                    }
                }
            }
        }
    }

    void gltf::load_gl(FileTextureQueue& t_file_queue) {
        // Section buffers
        m_gl_buffers.resize(m_buffers.size());
        for (size_t ii = 0; ii < m_buffers.size(); ii++) {
            std::vector<unsigned char>& buffer_gl = m_gl_buffers[ii];
            gltf_buffer& buffer_gltf = m_buffers[ii];
            std::filesystem::path buffer_path = m_base_path;
            buffer_path.append(buffer_gltf.m_uri);

            std::ifstream file(buffer_path, std::ios::binary | std::ios::ate);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<unsigned char>& bufferData = m_gl_buffers[ii];
            bufferData.resize(size);

            if (size != buffer_gltf.m_byte_length) {
                // TODO error
                log_error("%s buffer size is not correct.", buffer_gltf.m_name.c_str());
                log_flush();
            }

            if (!file.read((char*)bufferData.data(), size)) {
                __debugbreak();
            }
        }
        log_debug("Loaded %zd buffers.", m_gl_buffers.size());

        // Section images
        const int image_offset = 2;
        m_gl_images_size = m_images.size() + image_offset;
        m_gl_images.reset(new GLuint[m_gl_images_size]);
        m_gl_images[0] = (GLuint)0;
        GL(glGenTextures((GLuint)m_gl_images_size - 1, m_gl_images.get() + 1));
        log_debug("Generated %zd(including texture 0) textures.", m_gl_images_size);

        {
            assert(m_gl_images[0] == 0);
            unsigned char pixels[] = {255, 255, 255};
            GL(glBindTexture(GL_TEXTURE_2D, m_gl_images[1]));
            GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels));
            GL(glGenerateMipmap(GL_TEXTURE_2D));
        }

        m_image_paths.reset(new std::string[m_images.size()]);
        for (size_t ii = 0; ii < m_images.size(); ii++) {
            gltf_image& image = m_images[ii];
            std::filesystem::path path = m_base_path;
            path.append(image.m_uri);
            m_image_paths[ii] = path.string();

            FileQueueWork work = {&m_image_paths[ii], m_gl_images[ii + image_offset]};
            t_file_queue.add_work(work);
            log_trace("New work: load image id %zd with path %s.", ii + image_offset, m_image_paths[ii].c_str());
        }

        // Section samplers
        int samplers_offset = 1;
        m_gl_samplers_size = m_samplers.size() + 1;
        m_gl_samplers.reset(new GLuint[m_gl_samplers_size]);
        GL(glGenSamplers((GLuint)m_gl_samplers_size, m_gl_samplers.get()));

        log_debug("Generated %zd samplers.", m_gl_samplers_size);

        {
            GLuint sampler = m_gl_samplers[0];
            GL(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
            GL(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
        }

        for (size_t ii = 0; ii < m_samplers.size(); ii++) {
            gltf_sampler& sampler_gltf = m_samplers[ii];
            GLuint sampler = m_gl_samplers[ii + 1];

            GL(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, (GLint)sampler_gltf.m_wrap_s));
            GL(glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, (GLint)sampler_gltf.m_wrap_t));

            if (sampler_gltf.m_mag_filter != -1) {
                GL(glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, (GLint)sampler_gltf.m_mag_filter));
            }

            if (sampler_gltf.m_min_filter != -1) {
                GL(glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (GLint)sampler_gltf.m_min_filter));
            }
        }

        // Section materials
        m_gl_materials_size = m_materials.size();
        m_gl_materials.reset(new gl_material[m_gl_materials_size]);

        for (size_t ii = 0; ii < m_materials.size(); ii++) {
            gl_material& material = m_gl_materials[ii];
            gltf_material& material_gltf = m_materials[ii];

            gltf_texture_to_gl_texture(material_gltf.m_pbr_metallic_roughness.m_base_metallic_roughness_texture,
                                       material.m_base_metallic_roughness_texture, texture_color_white);

            gltf_texture_to_gl_texture(material_gltf.m_pbr_metallic_roughness.m_base_color_texture,
                                       material.m_base_color_texture, texture_color_white);

            gltf_texture_to_gl_texture(material_gltf.m_emissive_texture, material.m_emissive_texture,
                                       texture_color_black);

            gltf_texture_to_gl_texture(material_gltf.m_normal_texture, material.m_normal_texture, texture_color_black);

            gltf_texture_to_gl_texture(material_gltf.m_occlusion_texture, material.m_occlusion_texture,
                                       texture_color_black);

            material.m_base_color_factor = material_gltf.m_pbr_metallic_roughness.m_base_color_factor;
            material.m_alpha_cutoff = material_gltf.m_alpha_cutoff;

            material.m_emissive_factor = material_gltf.m_emissive_factor;

            if (material_gltf.m_pbr_metallic_roughness.m_base_metallic_roughness_texture.mo_index != -1) {
                material.m_roughness_factor = 1.0f;
                material.m_metallic_factor = 1.0f;
            }

            material.m_alpha_mode = material_gltf.m_alpha_mode;
            material.m_double_sided = material_gltf.m_double_sided;
        }

        // Section meshes
        m_gl_meshes_size = m_meshes.size();
        m_gl_meshes.reset(new gl_mesh[m_gl_meshes_size]);
        for (size_t ii = 0; ii < m_meshes.size(); ii++) {
            gltf_mesh& mesh_gltf = m_meshes[ii];
            gl_mesh& mesh_gl = m_gl_meshes[ii];
            mesh_gl.m_primitives_size = mesh_gltf.m_primitives.size();
            mesh_gl.m_primitives.reset(new gl_primitive[mesh_gl.m_primitives_size]);
            for (size_t jj = 0; jj < mesh_gltf.m_primitives.size(); jj++) {
                gl_primitive& primitive_gl = mesh_gl.m_primitives[jj];
                gltf_primitive& primitive_gltf = mesh_gltf.m_primitives[jj];
                gltf_primitive_to_gl_primitive(primitive_gltf, primitive_gl, mesh_gltf);
            }
        }

        // Section animations
        m_gl_animations_size = m_animations.size();
        m_gl_animations.reset(new gl_animation[m_gl_animations_size]);
        m_animation_names.reset(new const char*[m_gl_animations_size]);

        for (size_t ii = 0; ii < m_animations.size(); ii++) {
            gl_animation& animation = m_gl_animations[ii];
            gltf_animation& animation_gltf = m_animations[ii];
            m_animation_names[ii] = animation_gltf.m_name.c_str();

            animation.m_tracks_size = animation_gltf.m_channels.size();
            animation.m_tracks.reset(new animation_track[animation.m_tracks_size]);

            for (size_t jj = 0; jj < animation_gltf.m_channels.size(); jj++) {
                animation_track& track = animation.m_tracks[jj];
                gltf_animation_channel channel = animation_gltf.m_channels[jj];
                gltf_animation_sampler sampler = animation_gltf.m_samplers[channel.m_sampler];

                track.m_interpolation = sampler.m_interpolation;
                track.m_path = channel.m_target.m_path;

                track.m_node = &m_nodes[channel.m_target.m_node];

                sampler.m_input;
                sampler.m_output;

                size_t input_size, input_stride;
                bool input_owned = false;
                unsigned char* input_data;

                gltf_accessor& input = m_accessors[sampler.m_input];
                load_accessor(input, input_size, input_stride, input_data, input_owned);

                track.m_times = (float*)input_data;
                track.m_times_owned = input_owned;
                track.m_times_size = input.m_count;

                size_t output_size, output_stride;
                bool output_owned = false;
                unsigned char* output_data;

                gltf_accessor& output = m_accessors[sampler.m_output];
                load_accessor(output, output_size, output_stride, output_data, output_owned);

                if (output.m_component_type == GL_FLOAT) {
                    track.m_values = (float*)output_data;
                    track.m_values_size = output.m_count;
                    track.m_values_owned = output_owned;
                } else {
                    track.m_values = new float[output.m_count];
                    track.m_values_owned = true;

                    size_t component_count_in_buffer = output.component_count * output.m_count;

                    switch (output.m_component_type) {

                    case GL_BYTE: {
                        auto data_b = (char*)output_data;
                        for (size_t kk = 0; kk < component_count_in_buffer; kk++) {
                            track.m_values[kk] = std::max(data_b[kk] / 127.0f, -1.0f);
                        }
                    } break;

                    case GL_UNSIGNED_BYTE: {
                        auto data_ub = (unsigned char*)output_data;
                        for (size_t kk = 0; kk < component_count_in_buffer; kk++) {
                            track.m_values[kk] = data_ub[kk] / 255.0f;
                        }
                    } break;

                    case GL_SHORT: {
                        auto data_s = (short*)output_data;
                        for (size_t kk = 0; kk < component_count_in_buffer; kk++) {
                            track.m_values[kk] = std::max(data_s[kk] / 32767.0f, -1.0f);
                        }
                    } break;
                    case GL_UNSIGNED_SHORT: {
                        auto data_us = (unsigned short*)output_data;
                        for (size_t kk = 0; kk < component_count_in_buffer; kk++) {
                            track.m_values[kk] = data_us[kk] / 65535.0f;
                        }
                    } break;

                    default:
                        log_error("Unknown animation values type.");
                        log_flush();
                        __debugbreak();
                        break;
                    }

                    if (output_owned) {
                        delete[] output_data;
                    }
                }

                // TODO might be out of bounds
                track.m_time_0_index = 0;
                track.update_values();
                track.m_last_time = track.m_times[input.m_count - 1];

                animation.m_animation_length = std::max(animation.m_animation_length, track.m_last_time);
            }
        }

        // Section skins
        m_gl_skins_size = m_skins.size();
        m_gl_skins.reset(new gl_skin[m_gl_skins_size]);
        for (size_t ii = 0; ii < m_skins.size(); ii++) {
            gltf_skin& skin_gltf = m_skins[ii];
            gl_skin& skin_gl = m_gl_skins[ii];

            skin_gltf.m_inverse_bind_matrices;
            skin_gltf.m_joints;
            skin_gltf.m_skeleton;
            skin_gltf.m_name;

            skin_gl.m_joints_size = skin_gltf.m_joints.size();
            skin_gl.m_joints.reset(new gltf_node*[skin_gl.m_joints_size]);
            skin_gl.m_joints_matrices.reset(new glm::mat4[skin_gl.m_joints_size]);
            for (size_t jj = 0; jj < skin_gltf.m_joints.size(); jj++) {
                skin_gl.m_joints[jj] = &m_nodes[skin_gltf.m_joints[jj]];
            }

            if (skin_gltf.m_inverse_bind_matrices != -1) {
                size_t size = 0;
                size_t stride = 0;
                unsigned char* data = nullptr;
                bool owned = false;
                gltf_accessor& accessor = m_accessors[skin_gltf.m_inverse_bind_matrices];
                load_accessor(accessor, size, stride, data, owned);

                skin_gl.m_inverse_bind_matrices_size = accessor.m_count;
                skin_gl.m_inverse_bind_matrices = (glm::mat4*)data;
                skin_gl.m_inverse_bind_matrices_owned = owned;

            } else {
                skin_gl.m_inverse_bind_matrices_size = skin_gl.m_joints_size;
                skin_gl.m_inverse_bind_matrices = new glm::mat4[skin_gl.m_inverse_bind_matrices_size];
                skin_gl.m_inverse_bind_matrices_owned = true;
            }
        }

        // Section scene
        m_draw_groups_per_scene.reset(new std::vector<draw_group>[m_scenes.size()]);
        m_scenes_names.reset(new const char*[m_scenes.size()]);
        for (size_t ii = 0; ii < m_scenes.size(); ii++) {
            gltf_scene& scene_gltf = m_scenes[ii];
            m_scenes_names[ii] = scene_gltf.m_name.c_str();

            auto& draw_groups = m_draw_groups_per_scene[ii];

            draw_groups.resize(m_gl_materials_size + 1);

            {
                gl_material& default_material = draw_groups[0].m_material;
                default_material.m_base_color_texture.m_gl_texture = 1;
                default_material.m_double_sided = true;
            }

            for (size_t ii = 0; ii < m_gl_materials_size; ii++) {
                draw_groups[ii + 1].m_material = m_gl_materials[ii];
            }

            for (size_t node_id : scene_gltf.m_nodes) {
                add_node_to_draw_list(draw_groups, m_nodes[node_id]);
            }

            std::sort(draw_groups.begin(), draw_groups.end(), [](const draw_group& lhs, const draw_group& rhs) {
                return lhs.m_material.m_alpha_mode < rhs.m_material.m_alpha_mode;
            });
        }
    }

    void gltf::get_accessor_data(gltf_accessor& t_accessor, unsigned char*& r_ptr, size_t& r_size, bool& r_owned) {
        r_size = t_accessor.component_size() * t_accessor.m_count;

        if (t_accessor.mo_buffer_view == -1) {
            if (t_accessor.mo_sparse.has_value()) {
                // TODO sparse loading
                r_ptr = new unsigned char[r_size]{0};
                r_owned = true;
            } else {
                // TODO invalid accessor
                __debugbreak();
            }
        }

        gltf_buffer_view& buffer_view_gltf = m_buffer_views[t_accessor.mo_buffer_view];

        std::vector<unsigned char>& buffer = m_gl_buffers[buffer_view_gltf.m_buffer];

        size_t offset = buffer_view_gltf.m_byte_offset;

        assert((t_accessor.m_byte_offset != 0) and (buffer_view_gltf.mo_byte_stride != 0));

        r_ptr = &buffer[offset];
        r_owned = false;
    }

    void gltf::load_accessor_without_sparse(gltf_accessor& t_accessor, size_t& r_stride, unsigned char*& r_data) {
        if (t_accessor.mo_buffer_view != -1) {
            gltf_buffer_view buffer_view = m_buffer_views[t_accessor.mo_buffer_view];

            std::vector<unsigned char>& buffer_gl = m_gl_buffers[buffer_view.m_buffer];

            r_stride = buffer_view.mo_byte_stride;
            r_data = &buffer_gl[buffer_view.m_byte_offset + t_accessor.m_byte_offset];
        } else {
            r_data = nullptr;
            r_stride = 0;
            log_error("TODO error Buffer view is not exists.");
            log_flush();
            return;
        }
    }

    template <typename T>
    void sparse_load(size_t t_count, size_t t_element_size, unsigned char* t_data, unsigned char* t_indices,
                     unsigned char* t_values) {
        T* indices = (T*)t_indices;

        for (size_t ii = 0; ii < t_count; ii++) {
            T index = indices[ii];
            memcpy(&t_data[index * t_element_size], &t_values[ii * t_element_size], t_element_size);
        }
    }

    void gltf::load_accessor(gltf_accessor& t_accessor, size_t& r_size, size_t& r_stride, unsigned char*& r_data,
                             bool& r_owned) {
        r_size = t_accessor.component_size() * t_accessor.m_count;
        r_stride = 0;
        r_data = nullptr;
        r_owned = false;

        if (t_accessor.mo_sparse.has_value()) {
            r_owned = true;

            if (t_accessor.mo_buffer_view != -1) {
                r_data = new unsigned char[r_size];
                unsigned char* buf_data = 0;
                load_accessor_without_sparse(t_accessor, r_stride, buf_data);
                if (buf_data) {
                    memcpy(r_data, buf_data, r_size);
                } else {
                    // fix
                    log_fatal("Fix me buf_data might be null.");
                    log_flush();
                }
            } else {
                r_data = new unsigned char[r_size]{0};
            }

            gltf_accessor_sparse sparse = t_accessor.mo_sparse.value();

            gltf_buffer_view& indices = m_buffer_views[sparse.m_indices.mo_buffer_view];

            gltf_buffer_view& values = m_buffer_views[sparse.m_values.mo_buffer_view];

            unsigned char* indices_data = nullptr;
            unsigned char* values_data = nullptr;
            size_t must_zero_stride = 0;

            indices.mo_byte_stride; // Must 0
            values.mo_byte_stride;  // Must 0

            indices_data = &m_gl_buffers[indices.m_buffer][indices.m_byte_offset];
            values_data = &m_gl_buffers[values.m_buffer][values.m_byte_offset];

            t_accessor.m_type;
            // load from template
            switch (sparse.m_indices.m_component_type) {
            case GL_UNSIGNED_BYTE:
                sparse_load<unsigned char>(sparse.m_count, t_accessor.component_size(), r_data, indices_data,
                                           values_data);
                break;
            case GL_UNSIGNED_SHORT:
                sparse_load<unsigned short>(sparse.m_count, t_accessor.component_size(), r_data, indices_data,
                                            values_data);
                break;
            case GL_UNSIGNED_INT:
                sparse_load<unsigned int>(sparse.m_count, t_accessor.component_size(), r_data, indices_data,
                                          values_data);
                break;
            default:
                __debugbreak();
                break;
            }

        } else {
            load_accessor_without_sparse(t_accessor, r_stride, r_data);
        }
    }

    void gltf::update_node(gltf_node& t_node, glm::mat4& t_parent_world_matrix) {
        t_node.update_local_matrix();
        t_node.m_world_matrix = t_parent_world_matrix * t_node.m_matrix;

        if (t_node.m_mesh != -1) {
            gl_mesh& mesh = m_gl_meshes[t_node.m_mesh];
            for (size_t ii = 0; ii < mesh.m_primitives_size; ii++) {
                mesh.m_primitives[ii].update_morphs();
            }
        }

        for (size_t ii = 0; ii < t_node.m_children.size(); ii++) {
            update_node(m_nodes[t_node.m_children[ii]], t_node.m_world_matrix);
        }
    }

    void gltf::update() { update_scene(m_current_scene); }

    void gltf::update_scene(size_t t_scene_index) {
        glm::mat4 origin(1.0f);

        gltf_scene& scene = m_scenes[t_scene_index];

        for (size_t ii = 0; ii < scene.m_nodes.size(); ii++) {
            update_node(m_nodes[scene.m_nodes[ii]], origin);
        }
    }

    void gltf::load_attribute(GLuint t_vao, GLuint t_location, size_t t_attribute, GLuint& r_buffer) {
        if (t_attribute != -1) {
            GLuint buffer;
            gltf_accessor& accessor = m_accessors[t_attribute];

            size_t size = 0;
            size_t stride = 0;
            unsigned char* data = nullptr;
            bool owned = false;
            load_accessor(accessor, size, stride, data, owned);

            if (stride > accessor.component_size()) {
                // TODO interleaved data
                log_error("Interleaved data is not supported.");
                log_flush();
                __debugbreak();
                return;
            }

            GL(glGenBuffers(1, &buffer));
            GL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
            GL(glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW));

            GL(glEnableVertexArrayAttrib(t_vao, t_location));

            GL(glVertexAttribPointer(t_location, (GLint)accessor.component_count, (GLenum)accessor.m_component_type,
                                     (GLboolean)accessor.m_normalized, (GLsizei)stride, 0));

            if (owned) {
                delete[] data;
            }

            r_buffer = buffer;
            m_cleanup_buffers.push_back(buffer);
        }
    }

    void gltf::gltf_primitive_to_gl_primitive(gltf_primitive& t_primitive_gltf, gl_primitive& r_primitive_gl,
                                              gltf_mesh& t_mesh) {
        gltf_attributes& a = t_primitive_gltf.m_attributes;

        r_primitive_gl.m_mesh = &t_mesh;
        r_primitive_gl.m_draw_mode = t_primitive_gltf.m_mode;

        GL(glGenVertexArrays(1, &r_primitive_gl.m_vao));
        GL(glBindVertexArray(r_primitive_gl.m_vao));

        GLuint c;
        GLuint p;
        GLuint n;
        GLuint t;

        load_attribute(r_primitive_gl.m_vao, 0, a.mo_POSITION, p);
        load_attribute(r_primitive_gl.m_vao, 1, a.mo_NORMAL, n);
        load_attribute(r_primitive_gl.m_vao, 2, a.mo_TANGENT, t);
        load_attribute(r_primitive_gl.m_vao, 3, a.mo_TEXCOORD_0, c);
        load_attribute(r_primitive_gl.m_vao, 4, a.mo_TEXCOORD_1, c);
        load_attribute(r_primitive_gl.m_vao, 5, a.mo_COLOR_0, c);
        load_attribute(r_primitive_gl.m_vao, 6, a.mo_JOINTS_0, c);
        load_attribute(r_primitive_gl.m_vao, 7, a.mo_WEIGHTS_0, c);

        if (a.mo_POSITION != -1) {
            gltf_accessor& pos_acc = m_accessors[a.mo_POSITION];
            r_primitive_gl.m_vertex_count = (GLuint)pos_acc.m_count;
            r_primitive_gl.m_swap_buffers_0.m_position = p;
            r_primitive_gl.m_swap_buffers_1.m_position = p;
            GL(glGenBuffers(1, &r_primitive_gl.m_swap_buffers_1.m_position));
            GL(glBindBuffer(GL_ARRAY_BUFFER, r_primitive_gl.m_swap_buffers_1.m_position));
            GL(glBufferData(GL_ARRAY_BUFFER, pos_acc.component_size() * pos_acc.m_count, nullptr, GL_STATIC_DRAW));
        }

        if (a.mo_NORMAL != -1) {
            gltf_accessor& pos_acc = m_accessors[a.mo_NORMAL];
            r_primitive_gl.m_vertex_count = (GLuint)pos_acc.m_count;
            r_primitive_gl.m_swap_buffers_0.m_normal = n;
            r_primitive_gl.m_swap_buffers_1.m_normal = n;
            GL(glGenBuffers(1, &r_primitive_gl.m_swap_buffers_1.m_normal));
            GL(glBindBuffer(GL_ARRAY_BUFFER, r_primitive_gl.m_swap_buffers_1.m_normal));
            GL(glBufferData(GL_ARRAY_BUFFER, pos_acc.component_size() * pos_acc.m_count, nullptr, GL_STATIC_DRAW));
        }

        if (a.mo_TANGENT != -1) {
            gltf_accessor& pos_acc = m_accessors[a.mo_TANGENT];
            r_primitive_gl.m_vertex_count = (GLuint)pos_acc.m_count;
            r_primitive_gl.m_swap_buffers_0.m_tangent = t;
            r_primitive_gl.m_swap_buffers_1.m_tangent = t;
            GL(glGenBuffers(1, &r_primitive_gl.m_swap_buffers_1.m_tangent));
            GL(glBindBuffer(GL_ARRAY_BUFFER, r_primitive_gl.m_swap_buffers_1.m_tangent));
            GL(glBufferData(GL_ARRAY_BUFFER, pos_acc.component_size() * pos_acc.m_count, nullptr, GL_STATIC_DRAW));
        }

        r_primitive_gl.m_target_size = t_primitive_gltf.m_targets.size();
        r_primitive_gl.m_target.reset(new gl_primitive_target[r_primitive_gl.m_target_size]);
        for (size_t ii = 0; ii < t_primitive_gltf.m_targets.size(); ii++) {
            gl_primitive_target& m = r_primitive_gl.m_target[ii];
            m.m_position;
            m.m_tangent;
            m.m_normal;

            size_t accessor_id_position = t_primitive_gltf.m_targets[ii].mo_POSITION;
            if (accessor_id_position != -1) {
                gltf_accessor& accessor_position = m_accessors[accessor_id_position];
                size_t size;
                size_t stride;
                unsigned char* data;
                bool owned;

                load_accessor(accessor_position, size, stride, data, owned);

                GL(glGenBuffers(1, &m.m_position));
                GL(glBindBuffer(GL_ARRAY_BUFFER, m.m_position));
                GL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

                if (owned) {
                    delete[] data;
                }
            }

            accessor_id_position = t_primitive_gltf.m_targets[ii].mo_NORMAL;
            if (accessor_id_position != -1) {
                gltf_accessor& accessor_position = m_accessors[accessor_id_position];
                size_t size;
                size_t stride;
                unsigned char* data;
                bool owned;

                load_accessor(accessor_position, size, stride, data, owned);

                GL(glGenBuffers(1, &m.m_normal));
                GL(glBindBuffer(GL_ARRAY_BUFFER, m.m_normal));
                GL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

                if (owned) {
                    delete[] data;
                }
            }

            accessor_id_position = t_primitive_gltf.m_targets[ii].mo_TANGENT;
            if (accessor_id_position != -1) {
                gltf_accessor& accessor_position = m_accessors[accessor_id_position];
                size_t size;
                size_t stride;
                unsigned char* data;
                bool owned;

                load_accessor(accessor_position, size, stride, data, owned);

                GL(glGenBuffers(1, &m.m_tangent));
                GL(glBindBuffer(GL_ARRAY_BUFFER, m.m_tangent));
                GL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

                if (owned) {
                    delete[] data;
                }
            }
        }

        if (t_primitive_gltf.mo_indices != -1) {
            gltf_accessor& indices = m_accessors[t_primitive_gltf.mo_indices];
            r_primitive_gl.m_index_type = (GLenum)indices.m_component_type;
            r_primitive_gl.m_index_count = (GLuint)indices.m_count;

            size_t size = 0;
            size_t stride = 0;
            unsigned char* data = nullptr;
            bool owned = false;

            load_accessor(indices, size, stride, data, owned);

            GL(glGenBuffers(1, &r_primitive_gl.mo_index_buffer));
            GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_primitive_gl.mo_index_buffer));

            GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

            m_cleanup_buffers.push_back(r_primitive_gl.mo_index_buffer);

            if (owned) {
                delete[] data;
            }
        }

        r_primitive_gl.m_target_size = t_primitive_gltf.m_targets.size();
    }

#if 0
    void gltf::reset_draw_groups() {
        if (m_draw_groups.size() == 0) {
            m_draw_groups.resize(m_gl_materials_size + 1);

            {
                gl_material& default_material = m_draw_groups[0].m_material;
                default_material.m_base_color_texture.m_gl_texture = 1;
                default_material.m_double_sided = true;
            }

            for (size_t ii = 0; ii < m_gl_materials_size; ii++) {
                m_draw_groups[ii + 1].m_material = m_gl_materials[ii];
            }
        } else {
            for (draw_group& group : m_draw_groups) {
                group.m_draw_primitives.clear();
            }
        }
    }
#endif

    static inline GLenum get_draw_mode(int t_is_override, GLenum t_override_from, GLenum t_override_to) {
        return t_override_from * (1 - t_is_override) + t_override_to * t_is_override;
    }

    void gltf::render(program& t_program, glm::mat4& t_view_projection, glm::vec3& t_camera_position, int t_is_override,
                      GLenum t_override_to) {
        GL(glUseProgram(t_program.m_program));

        // glm::vec4 camera_position = t_camera.m_view * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        GL(glUniform3f(t_program.cameraPosition, t_camera_position.x, t_camera_position.y, t_camera_position.z));

        for (size_t ii = 0; ii < m_draw_groups_per_scene[m_current_scene].size(); ii++) {
            draw_group& group = m_draw_groups_per_scene[m_current_scene][ii];
            group.m_material.bind(t_program);

            for (size_t jj = 0; jj < group.m_draw_primitives.size(); jj++) {
                draw_primitive& primitive_dr = group.m_draw_primitives[jj];
                gl_primitive& primitive = *primitive_dr.m_primitive;

                size_t i_skin = primitive_dr.m_node->m_skin;
                if (i_skin != -1) {
                    gl_skin& skin = m_gl_skins[i_skin];
                    glm::mat4 inverse_transform = glm::inverse(primitive_dr.m_node->m_world_matrix);

                    for (size_t kk = 0; kk < skin.m_joints_size; kk++) {
                        gltf_node* joint = skin.m_joints[kk];
                        skin.m_joints_matrices[kk] =
                            inverse_transform * joint->m_world_matrix * skin.m_inverse_bind_matrices[kk];
                    }
                    GL(glUniform1i(t_program.jointMatSize, (GLint)skin.m_joints_size));
                    GL(glUniformMatrix4fv(t_program.jointMat, (GLint)skin.m_joints_size, GL_FALSE,
                                          glm::value_ptr(skin.m_joints_matrices[0])));
                } else {
                    GL(glUniform1i(t_program.jointMatSize, 0));
                }

                GL(glUniformMatrix4fv(t_program.viewProjection, 1, GL_FALSE, glm::value_ptr(t_view_projection)));
                GL(glUniformMatrix4fv(t_program.model, 1, GL_FALSE,
                                      glm::value_ptr(primitive_dr.m_node->m_world_matrix)));

                GL(glBindVertexArray(primitive.m_vao));
                if (primitive.mo_index_buffer != 0) {
                    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.mo_index_buffer));
                    GL(glDrawElements(get_draw_mode(t_is_override, primitive.m_draw_mode, t_override_to),
                                      primitive.m_index_count, primitive.m_index_type, 0));
                } else {
                    GL(glDrawArrays(get_draw_mode(t_is_override, primitive.m_draw_mode, t_override_to), 0,
                                    primitive.m_vertex_count));
                }
            }
        }
    }

    void gltf::add_node_to_draw_list(std::vector<draw_group>& t_draw_groups, gltf_node& t_node) {

        if (t_node.m_mesh != -1) {
            gltf_mesh& mesh = m_meshes[t_node.m_mesh];

            for (size_t ii = 0; ii < mesh.m_primitives.size(); ii++) {
                gltf_primitive& primitive = mesh.m_primitives[ii];
                draw_group& group = t_draw_groups[primitive.mo_material + 1];
                gl_mesh& mesh_gl = m_gl_meshes[t_node.m_mesh];
                group.m_draw_primitives.emplace_back();
                draw_primitive& p = group.m_draw_primitives.back();
                p.m_primitive = &mesh_gl.m_primitives[ii];
                p.m_node = &t_node;
            }
        }

        for (size_t child_node_id : t_node.m_children) {
            add_node_to_draw_list(t_draw_groups, m_nodes[child_node_id]);
        }
    }

    void gltf::gltf_texture_to_gl_texture(gltf_texture_info& t_texture, gl_texture& r_texture_gl,
                                          texture_color t_texture_color) {
        if (t_texture.mo_index != -1) {
            gltf_texture& texture = m_textures[t_texture.mo_index];

            if (texture.m_source >= 0) {
                size_t gl_texture_index = texture.m_source + 2;
                r_texture_gl.m_gl_texture = m_gl_images[gl_texture_index];
            } else {
                r_texture_gl.m_gl_texture = m_gl_images[t_texture_color];
            }
            r_texture_gl.m_tex_coord = (GLuint)t_texture.m_tex_coord;
            r_texture_gl.m_gl_sampler = m_gl_samplers[texture.m_sampler + 1];
        } else {
            r_texture_gl.m_gl_texture = m_gl_images[t_texture_color];
            r_texture_gl.m_tex_coord = (GLuint)t_texture.m_tex_coord;
            r_texture_gl.m_gl_sampler = m_gl_samplers[0];
        }
    }

    void gltf::free(FileTextureQueue& t_file_queue, GpuTextureQueue& t_gpu_queue) {
        t_file_queue.finish();
        t_gpu_queue.finish();

        m_default_scene = 0;

        m_min_size = glm::vec3(0.0f);
        m_max_size = glm::vec3(0.0f);

        m_accessors.clear();
        m_animations.clear();
        m_buffers.clear();
        m_buffer_views.clear();
        m_cameras.clear();
        m_images.clear();
        m_materials.clear();
        m_meshes.clear();
        m_nodes.clear();
        m_samplers.clear();
        m_skins.clear();
        m_scenes.clear();
        m_textures.clear();

        // ~

        m_current_scene = 0;
        m_draw_groups_per_scene.reset();
        m_scenes_names.reset();

        if (m_gl_images) {
            GL(glDeleteTextures((GLuint)m_gl_images_size - 1,
                                m_gl_images.get() + 1)); // 1 for texture GLuint = 0
        }
        m_gl_images.reset();
        m_gl_images_size = 0;

        if (m_gl_samplers) {
            GL(glDeleteSamplers((GLuint)m_gl_samplers_size, m_gl_samplers.get()));
        }
        m_gl_samplers.reset();
        m_gl_samplers_size = 0;

        m_gl_materials.reset();
        m_gl_materials_size = 0;

        m_image_paths.reset();
        m_image_paths_size = 0;

        for (size_t ii = 0; ii < m_gl_meshes_size; ii++) {
            for (size_t jj = 0; jj < m_gl_meshes[ii].m_primitives_size; jj++) {
                gl_primitive& primitive = m_gl_meshes[ii].m_primitives[jj];
                GL(glDeleteVertexArrays(1, &primitive.m_vao));
            }
        }
        m_gl_meshes_size = 0;
        m_gl_meshes.reset();

        m_gl_buffers.clear();

        if (m_cleanup_buffers.size()) {
            GL(glDeleteBuffers((GLuint)m_cleanup_buffers.size(), m_cleanup_buffers.data()));
            m_cleanup_buffers.clear();
        }
        m_gl_buffers.clear();

        for (size_t ii = 0; ii < m_gl_animations_size; ii++) {
            gl_animation& animation = m_gl_animations[ii];
            for (size_t jj = 0; jj < animation.m_tracks_size; jj++) {
                animation_track& track = animation.m_tracks[ii];
                if (track.m_times_owned) {
                    delete[] track.m_times;
                }

                if (track.m_values_owned) {
                    delete[] track.m_values;
                }
            }
        }
        m_gl_animations.reset();
        m_gl_animations_size = 0;

        m_animation_names.reset();

        for (size_t ii = 0; ii < m_gl_skins_size; ii++) {
            if (m_gl_skins[ii].m_inverse_bind_matrices_owned)
                delete[] m_gl_skins[ii].m_inverse_bind_matrices;
        }
        m_gl_skins.reset();
        m_gl_skins_size = 0;
    }

    void gl_material::bind(program& t_program) {
        if (m_double_sided) {
            GL(glDisable(GL_CULL_FACE));
        } else {
            GL(glEnable(GL_CULL_FACE));
        }

        switch (m_alpha_mode) {
        case gltf_alpha_mode_blend:
            GL(glEnable(GL_BLEND));
            GL(glUniform1i(t_program.alphaCutoffEnabled, false));
            break;
        case gltf_alpha_mode_opaque:
            GL(glUniform1i(t_program.alphaCutoffEnabled, false));
            glDisable(GL_BLEND);
            break;
        case gltf_alpha_mode_mask:
            glDisable(GL_BLEND);
            GL(glUniform1i(t_program.alphaCutoffEnabled, true));
            break;
        default:
            break;
        }

        GL(glUniform1f(t_program.alphaCutoff, m_alpha_cutoff));

        GL(glUniform4f(t_program.baseColorFactor, m_base_color_factor.r, m_base_color_factor.g, m_base_color_factor.b,
                       m_base_color_factor.a));

        bind_texture_sampler(0, m_base_color_texture.m_gl_texture, t_program.baseColorTexture,
                             m_base_color_texture.m_gl_sampler);

        bind_texture_sampler(1, m_base_metallic_roughness_texture.m_gl_texture, t_program.metallicRoughnessTexture,
                             m_base_metallic_roughness_texture.m_gl_sampler);

        bind_texture_sampler(2, m_normal_texture.m_gl_texture, t_program.normalTexture, m_normal_texture.m_gl_sampler);

        bind_texture_sampler(3, m_occlusion_texture.m_gl_texture, t_program.occlusionTexture,
                             m_occlusion_texture.m_gl_sampler);

        bind_texture_sampler(4, m_emissive_texture.m_gl_texture, t_program.emissiveTexture,
                             m_emissive_texture.m_gl_sampler);

        GL(glUniform3f(t_program.emissiveFactor, m_emissive_factor.x, m_emissive_factor.y, m_emissive_factor.z));
        GL(glUniform1f(t_program.occlusionTextureStrength, m_occlusion_texture.m_strength));
        GL(glUniform1f(t_program.normalTextureScale, m_normal_texture.m_scale));

        GL(glUniform1f(t_program.metallicFactor, m_metallic_factor));
        GL(glUniform1f(t_program.roughnessFactor, m_roughness_factor));
    }

    GLuint g_transform_callback = 0;
    GLuint g_weight = 0;

    void gl_primitive::update_morphs() {

        for (size_t ii = 0; ii < m_target_size; ii++) {

            float delta = m_mesh->m_weights[ii].first - m_mesh->m_weights[ii].second;
            m_mesh->m_weights[ii].second = m_mesh->m_weights[ii].first;

            if (delta != 0.0f) {
                GL(glUseProgram(g_tfp.m_program));
                GL(glUniform1f(glGetUniformLocation(g_tfp.m_program, "u_weight"), delta));

                gl_primitive_target target = m_target[ii];
                GL(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_swap_buffers_1.m_position));
                GL(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_swap_buffers_1.m_normal));
                // GL(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_swap_buffers_1.m_tangent));

                // ------------------

                GL(glBindVertexArray(g_tfp.m_vao));

                // ------------------

                if (target.m_position && m_swap_buffers_0.m_position) {
                    GL(glBindBuffer(GL_ARRAY_BUFFER, m_swap_buffers_0.m_position));
                    GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));

                    GL(glBindBuffer(GL_ARRAY_BUFFER, target.m_position));
                    GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0));
                }

                // ------------------

                if (target.m_normal && m_swap_buffers_0.m_normal) {
                    GL(glBindBuffer(GL_ARRAY_BUFFER, m_swap_buffers_0.m_normal));
                    GL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0));

                    GL(glBindBuffer(GL_ARRAY_BUFFER, target.m_normal));
                    GL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0));
                }

                // ------------------

                if (target.m_tangent && m_swap_buffers_0.m_tangent) {
                    GL(glBindBuffer(GL_ARRAY_BUFFER, m_swap_buffers_0.m_tangent));
                    GL(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0));

                    GL(glBindBuffer(GL_ARRAY_BUFFER, target.m_tangent));
                    GL(glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0));
                }

                // ------------------

                GL(glBeginTransformFeedback(GL_POINTS));
                GL(glDrawArrays(GL_POINTS, 0, m_vertex_count));
                GL(glEndTransformFeedback());

                std::swap(m_swap_buffers_0, m_swap_buffers_1);
            }
        }

        // Bind to vao for drawing
    }

    void animation_track::update_values() {
        m_time_0 = m_times[m_time_0_index];
        m_time_1 = m_times[m_time_0_index + 1];

        switch (m_path) {
        case gltf_animation_channel_target_path_scale:
        case gltf_animation_channel_target_path_translation:
            m_value_0 = glm::vec4(((glm::vec3*)m_values)[m_time_0_index], 0.0f);
            m_value_1 = glm::vec4(((glm::vec3*)m_values)[m_time_0_index + 1], 0.0f);
            break;
        case gltf_animation_channel_target_path_rotation:
            m_value_0 = ((glm::vec4*)m_values)[m_time_0_index];
            m_value_1 = ((glm::vec4*)m_values)[m_time_0_index + 1];
            break;
        case gltf_animation_channel_target_path_weights:
            m_value_0.x = m_values[m_time_0_index];
            m_value_1.x = m_values[m_time_0_index + 1];
            break;
        }
    }

    void gl_animation::reset_animation() {
        m_time = 0.0f;
        for (size_t ii = 0; ii < m_tracks_size; ii++) {
            animation_track& track = m_tracks[ii];
            track.m_time_0_index = 0;
            track.update_values();
        }
    }

    void gl_animation::update(float t_time_delta, gltf& t_gltf) {
        m_time += t_time_delta;

        if (m_time > m_animation_length) {
            reset_animation();
        }

        for (size_t ii = 0; ii < m_tracks_size; ii++) {
            animation_track& track = m_tracks[ii];

            if (track.m_last_time <= m_time) {
                continue;
            }

            // Bug is here
            // prepare m_time_i and m_value_i
            if ((track.m_time_0 <= m_time) && (m_time <= track.m_time_1)) {
            } else {
                if (track.m_time_0 > m_time) {
                } else {
                    track.m_time_0_index++;
                    assert(track.m_time_0_index + 1 < track.m_times_size);
                    track.update_values();
                }
            }

            float u = std::max(0.0f, m_time - track.m_time_0) / (track.m_time_1 - track.m_time_0);

            switch (track.m_path) {
            case gltf_animation_channel_target_path_translation:
                track.m_node->m_translation = glm::mix(track.m_value_0, track.m_value_1, u);
                break;
            case gltf_animation_channel_target_path_rotation:
                // m_value_i XYZW rotation quaternion
                glm::quat q0 = glm::quat(track.m_value_0.w, track.m_value_0.x, track.m_value_0.y, track.m_value_0.z);
                glm::quat q1 = glm::quat(track.m_value_1.w, track.m_value_1.x, track.m_value_1.y, track.m_value_1.z);
                track.m_node->m_rotation = glm::normalize(glm::slerp(q0, q1, u));
                break;
            case gltf_animation_channel_target_path_scale:
                track.m_node->m_scale = glm::mix(track.m_value_0, track.m_value_1, u);
                break;
            case gltf_animation_channel_target_path_weights:
                gltf_mesh& mesh = t_gltf.m_meshes[track.m_node->m_mesh];
                size_t value0_start_index = track.m_time_0_index * mesh.m_weights.size();
                size_t value1_start_index = (track.m_time_0_index + 1) * mesh.m_weights.size();

                track.m_values[0];
                for (size_t jj = 0; jj < mesh.m_weights.size(); jj++) {
                    mesh.m_weights[jj].first =
                        glm::mix(track.m_values[value0_start_index + jj], track.m_values[value1_start_index + jj], u);
                }
                break;
            }
        }
    }

    void gltf::fit_camera(orbit_camera& t_camera, float t_step_percent) {
        find_min_max();
        m_min_size;
        m_max_size;

        float fov_y = 3.14f / 2.0f;
        glm::vec3 center = (m_min_size + m_max_size) / 2.0f;

        float min_cent_dist = glm::distance(m_min_size, center);
        float max_cent_dist = glm::distance(m_max_size, center);

        float radius = glm::max(min_cent_dist, max_cent_dist);

        t_camera.m_radius = radius;
        t_camera.m_object_radius = radius;
        t_camera.m_step = t_camera.m_object_radius / 100.0f * t_step_percent;
        t_camera.m_center = center;
    }
} // namespace viewer