#include "pch.h"

#ifndef VIEWER_GLTF_H
#define VIEWER_GLTF_H

#include <filesystem>

#include "accessor.h"
#include "animation.h"
#include "asset.h"
#include "buffer.h"
#include "buffer_view.h"
#include "camera.h"
#include "image.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "result.h"
#include "sampler.h"
#include "scene.h"
#include "skin.h"
#include "texture.h"

#include "../orbit_camera.h"
#include "../queue.h"

namespace viewer {

    enum texture_color {
        texture_color_black = 0,
        texture_color_white = 1,
    };

    struct gl_texture {
        GLuint m_gl_texture = 0;
        GLuint m_tex_coord = 0;
        GLuint m_gl_sampler = 0;
    };

    struct gl_normal_texture : gl_texture {
        float_t m_scale = 1.0f;
    };

    struct gl_occlusion_texture : gl_texture {
        float_t m_strength = 1.0f;
    };

    struct gl_material {
        glm::vec4 m_base_color_factor = glm::vec4(1.0f);
        gl_texture m_base_metallic_roughness_texture;
        gl_texture m_base_color_texture;
        gl_texture m_emissive_texture;
        gl_normal_texture m_normal_texture;
        gl_occlusion_texture m_occlusion_texture;
        glm::vec3 m_emissive_factor = glm::vec3(0.0f);
        float_t m_roughness_factor = 0.5f;
        float_t m_alpha_cutoff = 0.5f;
        float_t m_metallic_factor = 0.5f;
        gltf_alpha_mode m_alpha_mode = gltf_alpha_mode_opaque;
        bool m_double_sided = false;

        void bind(program& t_program);
    };

    struct gl_primitive_target {
        GLuint m_position = 0; // Wp
        GLuint m_normal = 0;   // Wn
        GLuint m_tangent = 0;  // Wt
    };

    struct gl_primitive {
        GLuint m_vao;
        GLuint mo_index_buffer = 0;
        GLuint m_index_count;
        GLuint m_vertex_count;
        GLenum m_draw_mode;
        GLenum m_index_type;
        size_t m_material_index;

        gltf_mesh* m_mesh;
        std::unique_ptr<gl_primitive_target[]> m_target;
        size_t m_target_size = 0;
        gl_primitive_target m_swap_buffers_0;
        gl_primitive_target m_swap_buffers_1;

        void update_morphs();
    };

    struct gl_mesh {
        std::unique_ptr<gl_primitive[]> m_primitives;
        size_t m_primitives_size;
    };

    struct draw_primitive {
        gl_primitive* m_primitive;
        gltf_node* m_node;
    };

    struct draw_group {
        gl_material m_material;
        std::vector<draw_primitive> m_draw_primitives;
    };

    struct animation_track {
        float* m_times;
        float* m_values;   // Depending on target path might be float glm::vec3 or glm::vec4
        gltf_node* m_node; // If vector reallocates will fail

        size_t m_time_0_index;
        glm::vec4 m_value_0;
        glm::vec4 m_value_1;

        float m_time_0;
        float m_time_1;

        size_t m_times_size;  // Size in element (float)
        size_t m_values_size; // Size in element (float, vec3, vec4)
        float m_last_time;

        gltf_animation_sampler_interpolation m_interpolation;
        gltf_animation_channel_target_path m_path;

        bool m_times_owned = false;
        bool m_values_owned = false;

        void update_values();
    };

    struct gl_animation {
        std::unique_ptr<animation_track[]> m_tracks;
        size_t m_tracks_size = 0;
        float m_time = 0.0f;
        float m_animation_length = 0.0f;

        void update(float t_time, gltf& t_gltf);
        void reset_animation();
    };

    struct gl_skin {
        gltf_node* m_root_node;
        glm::mat4* m_inverse_bind_matrices;
        size_t m_inverse_bind_matrices_size;
        std::unique_ptr<gltf_node*[]> m_joints;
        std::unique_ptr<glm::mat4[]> m_joints_matrices;
        size_t m_joints_size;
        bool m_inverse_bind_matrices_owned;
    };

    struct gltf {
        gltf_asset m_asset;
        std::filesystem::path m_base_path;
        size_t m_default_scene = 0;

        glm::vec3 m_max_size;
        glm::vec3 m_min_size;

        std::vector<gltf_accessor> m_accessors;
        std::vector<gltf_animation> m_animations;
        std::vector<gltf_buffer> m_buffers;
        std::vector<gltf_buffer_view> m_buffer_views;
        std::vector<gltf_camera> m_cameras;
        std::vector<gltf_image> m_images;
        std::vector<gltf_material> m_materials;
        std::vector<gltf_mesh> m_meshes;
        std::vector<gltf_node> m_nodes;
        std::vector<gltf_sampler> m_samplers;
        std::vector<gltf_skin> m_skins;
        std::vector<gltf_scene> m_scenes;
        std::vector<gltf_texture> m_textures;

        std::string m_name;

        void find_min_max();

        gltf_result load_from_path(const std::string&& t_path);
        gltf_result load_json(nlohmann::json& t_gltf_json);

        // ~

        int m_current_scene = 0;
        std::unique_ptr<const char*[]> m_scenes_names;
        std::unique_ptr<std::vector<draw_group>[]> m_draw_groups_per_scene;

        std::unique_ptr<GLuint[]> m_gl_images;
        size_t m_gl_images_size = 0;
        std::unique_ptr<GLuint[]> m_gl_samplers;
        size_t m_gl_samplers_size = 0;

        std::unique_ptr<gl_material[]> m_gl_materials;
        size_t m_gl_materials_size = 0;

        std::unique_ptr<std::string[]> m_image_paths;
        size_t m_image_paths_size = 0;

        std::unique_ptr<gl_mesh[]> m_gl_meshes;
        size_t m_gl_meshes_size = 0;

        std::vector<std::vector<unsigned char>> m_gl_buffers;
        std::vector<GLuint> m_cleanup_buffers;

        std::unique_ptr<gl_animation[]> m_gl_animations;
        size_t m_gl_animations_size = 0;
        std::unique_ptr<const char*[]> m_animation_names;

        std::unique_ptr<gl_skin[]> m_gl_skins;
        size_t m_gl_skins_size = 0;

        void fit_camera(orbit_camera& t_camera, float t_step_percent);

        void update_node(gltf_node& t_node, glm::mat4& t_parent_world_matrix);

        void update_scene(size_t t_scene_index);

        void render(program& t_program, glm::mat4& t_view_projection, glm::vec3& t_camera_position, int t_is_override,
                    GLenum t_override_to);

        void load_accessor(gltf_accessor& t_accessor, size_t& r_size, size_t& r_stride, unsigned char*& r_data,
                           bool& r_owned);

        void load_accessor_without_sparse(gltf_accessor& t_accessor, size_t& r_stride, unsigned char*& r_data);

        void get_accessor_data(gltf_accessor& t_accessor, unsigned char*& r_ptr, size_t& r_size, bool& r_owned);
        void load_attribute(GLuint t_vao, GLuint t_location, size_t t_attribute, GLuint& r_buffer);
        void gltf_primitive_to_gl_primitive(gltf_primitive& t_primitive_gltf, gl_primitive& t_primitive_gl,
                                            gltf_mesh& t_mesh);
        void add_node_to_draw_list(std::vector<draw_group>& t_draw_groups, gltf_node& t_node);
        void free(FileTextureQueue& t_file_queue, GpuTextureQueue& t_gpu_queue);
        void load_gl(FileTextureQueue& t_file_queue);
        void update_draw_group(std::vector<draw_group>& t_draw_groups, size_t t_scene_id);
        void gltf_texture_to_gl_texture(gltf_texture_info& t_texture, gl_texture& r_texture_gl,
                                        texture_color t_texture_color);
        void update();
    };

} // namespace viewer

#endif // !VIEWER_GLTF_H
