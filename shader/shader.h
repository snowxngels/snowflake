#pragma once

#include "../glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct shader_list {

  unsigned int i_num_loaded_shaders;
  
};

void init_shader_list() {

  std::string s_vertex_code_src;
  std::string s_fragment_code_src;
  std::ifstream s_vertex_shader_stream;
  std::ifstream s_fragment_shader_stream;
  
  s_vertex_shader_stream.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  s_fragment_shader_stream.exceptions (std::ifstream::failbit | std::ifstream::badbit); 
  
}
