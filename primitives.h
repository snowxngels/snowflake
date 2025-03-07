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

class mesh {

 public:
  std::vector<float> mesh_vertices;
  std::vector<unsigned int> mesh_indices;
  std::vector<float> mesh_tex_coordinates;
  std::string texture_path;
  
  GLuint mesh_VAO;
  GLuint mesh_VBO;
  GLuint mesh_tex_VBO;
  GLuint mes_tex_id;
  GLuint mesh_EBO;

};

class model {
 public:
  std::vector<mesh> contained_meshes;
  std::vector<std::string>texture_path;
  
};

class scene {
 public:
  std::vector<model> loaded_models;
  
};

