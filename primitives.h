#pragma once

/////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////

#include "entity.h"
#include "shader/shader_class.h"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/image/stb_image.h"
#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float3.hpp>

// stdlib
#include <vector>
#include <strstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <random>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>



class Texture {

  GLuint handle;
  GLuint width;
  GLuint height;

  void load_texture() {

    
    
  }

};

class Model {

  std::vector<int> mesh_indices;

  void load_model() {

  }
};

class Mesh {

  std::vector<int> mesh_vao_handle;
  int index_count;
  
  void load_model() {

  }
  
};
