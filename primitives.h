#pragma once

#include "shader/shader_class.h"
#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stdlib
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <string>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

enum e_mesh_types {

  MESH_SKYBOX,
  MESH_ENTITY,
  MESH_LIGHT,
  MESH_TRIANGLE,
  MESH_SPRITE
  
};

enum e_physics_type {

  RIGID_BODY,
  SOFT_BODY,
  NON_PHYSICS
  
};

enum e_light_type {

  LIGHT_DIFFUSE,
  LIGHT_SPECULAR,
  LIGHT_AMBIENT
  
};

class light {
  public:
  e_light_type light_type;

  unsigned int strength;
  
  float location_x = 0.0f;
  float location_y = 0.0f;
  float location_z = 0.0f;

  
};

class mesh {

 public:
  std::vector<float> mesh_vertices;
  std::vector<float> mesh_tex_coordinates;
  std::vector<float> mesh_normals;
  std::string texture_path;
  
  e_mesh_types mesh_type = MESH_ENTITY;

  float disable_tex_shading = 0.0f;
  float face_color_r = 20.0f;
  float face_color_g = 50.0f;
  float face_color_b = 80.0f;

  int mesh_affected_by_light = 1;
  
  GLuint mesh_VAO;
  GLuint mesh_VBO;
  GLuint mesh_tex_VBO;
  GLuint mesh_norm_VBO;
  GLuint mes_tex_id;
  GLuint mesh_EBO;

  float offset_pos_x = 0.0f;
  float offset_pos_y = 0.0f;
  float offset_pos_z = 0.0f;

  float offset_theta_x = 0.0f;
  float offset_theta_y = 0.0f;
  float offset_theta_z = 0.0f;
  
};

class model {
 public:
  
  e_physics_type physics_type = NON_PHYSICS;

  std::vector<mesh> contained_meshes;
  
  float location_x = 0.0f;
  float location_y = 0.0f;
  float location_z = 0.0f;
  
  float theta_x = 0.0f;
  float theta_y = 0.0f;
  float theta_z = 0.0f;  

  float accell_x = 0.0f;
  float accell_y = 0.0f;
  float accell_z = 0.0f;

  float las_phys_calculation = 0.0f;
};

class scene {
 public:
  std::vector<model> loaded_models;
  std::vector<light> loaded_lights;

  void add_model_to_scene(model add_model) {

    loaded_models.push_back(add_model);

    return;

  }
  
  void add_light_to_scene(light add_light) {

    loaded_lights.push_back(add_light);

    return;

  }
  
};

