#pragma once

#include<fstream> 
#include <glm/ext/vector_float3.hpp>
#include<iostream>
#include "./glad/glad.h"
#include<GLFW/glfw3.h>
#include<string>
#include<sstream>
#include<cstring>
#include <cmath>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader/shader_class.h"
#include <vector>
#include <strstream>

enum e_available_entity_types {

  e_mesh,
  e_light,
  e_vertex

};

struct entity_data {

  unsigned int entity_VAO;
  unsigned int entity_VBO;
  unsigned int entity_EBO;

  float entity_pos_x = 0;
  float entity_pos_y = 0;
  float entity_pos_z = 0;
  
  e_available_entity_types entity_type;
  std::vector<float> entity_vertices;
  std::vector<int> entity_indices;
  std::vector<std::string>texture_paths;
  
};

class entity {
 public:

  entity_data data;
  
  entity() {
    data.entity_vertices = {};
    data.entity_indices = {};
  }

  void updatepos() {


  }
  
};
