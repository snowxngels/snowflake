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
#include "newshader.h"
#include <vector>
#include <strstream>

#include "entity.h"

class scene {
 public:

  std::vector<entity> loaded_entities;

  void add_entity(entity to_append) {
    this->loaded_entities.push_back(to_append);
    return;
  }
  
};
