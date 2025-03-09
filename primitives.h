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
  MESH_TRIANGLE
  
};

enum e_light_type {

  LIGHT_DIFFUSE,
  LIGHT_SPECULAR,
  LIGHT_AMBIENT
  
};

class light {

  e_light_type light_type;

  unsigned int strength;
  
  float location_x = 0.0f;
  float location_y = 0.0f;
  float location_z = 0.0f;

  
};

class mesh {

 public:
  std::vector<float> mesh_vertices;
  std::vector<unsigned int> mesh_indices;
  std::vector<float> mesh_tex_coordinates;
  std::string texture_path;

  e_mesh_types mesh_type;

  float render_type = 0.0f;
  float face_color_r = 20.0f;
  float face_color_g = 50.0f;
  float face_color_b = 80.0f;
  
  GLuint mesh_VAO;
  GLuint mesh_VBO;
  GLuint mesh_tex_VBO;
  GLuint mes_tex_id;
  GLuint mesh_EBO;

  float location_x = 0.0f;
  float location_y = 0.0f;
  float location_z = 0.0f;

  float theta_x = 0.0f;
  float theta_y = 0.0f;
  float theta_z = 0.0f;

};

class model {
 public:
  std::vector<mesh> contained_meshes;
  
};

class scene {
 public:
  std::vector<model> loaded_models;
  std::vector<light> loaded_lights;

  void add_model_to_scene(model add_model) {

    loaded_models.push_back(add_model);

    return;

  }
  
};

