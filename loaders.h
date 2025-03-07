#pragma once
/////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////

#include "shader/shader_class.h"
#include "glad/glad.h"
#include "./primitives.h"

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

// assimp
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <strstream>

mesh import_obj_mesh(std::string file_path) {

    mesh output;
    
    std::ifstream file(file_path);
    if(!file.is_open())
      std::cout << "imort mesh not found!!!" << std::endl;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> tex_cords;
    std::vector<float> int_tex_cords;
    
    while(!file.eof()) {

      char line[1024];
      file.getline(line,1024);

      std::strstream s;

      s << line;

      std::string junk;

      if (line[0] == 'v') {

        if (line[1] == ' ') {

	  std::cout << "deserializing a vertex" << std::endl;
	  
          float vecx;
          float vecy;
          float vecz;

          s >> junk >> vecx >> vecy >> vecz;

          vertices.push_back(vecx);
          vertices.push_back(vecy);
          vertices.push_back(vecz);

          printf("found vertex:%f,%f,%f", vecx, vecy, vecz);
	  
	}

	if (line[1] == 't') {

	  std::cout << "deserializing a tex cord" << std::endl;
	  
	  float texx;
	  float texy;

	  s >> junk >> texx >> texy;

	  int_tex_cords.push_back(texx);
	  int_tex_cords.push_back(texy);
	  
	}
	
      }
      
      if(line[0] == 'f') {

	std::cout << "deserializing a face" << std::endl;
	
	std::string chunk1;
	std::string chunk2;
	std::string chunk3;

        s >> junk >> chunk1 >> chunk2 >> chunk3;

	float vert_1_buf;
	float vert_2_buf;
	float vert_3_buf;
	float tex_buf;

	indices.push_back(std::stoi(chunk1.substr(0, chunk1.find('/'))));
	indices.push_back(std::stoi(chunk2.substr(0, chunk2.find('/'))));
	indices.push_back(std::stoi(chunk3.substr(0, chunk3.find('/'))));

	int c1as = std::stoi(chunk1.substr(chunk1.find('/') + 1));
	int c2as = std::stoi(chunk2.substr(chunk2.find('/') + 1));
	int c3as = std::stoi(chunk3.substr(chunk3.find('/') + 1));

	int c1bs = std::stoi(chunk1.substr(0, chunk1.find('/')));
	int c2bs = std::stoi(chunk2.substr(0, chunk2.find('/')));
	int c3bs = std::stoi(chunk3.substr(0, chunk3.find('/')));

	std::cout << "after 1: " << c1as << std::endl;
	std::cout << "after 2: " << c2as << std::endl;
	std::cout << "after 3: " << c3as << std::endl;
	
	std::cout << "before 1: " << c1bs << std::endl;
	std::cout << "before 2: " << c2bs << std::endl;
	std::cout << "before 3: " << c3bs << std::endl;

	std::cout << "size tex_buffer: " << int_tex_cords.size() << std::endl;
	std::cout << "size tex_cords: " << tex_cords.size() << std::endl;

	if(tex_cords.size() < c1as + c2as + c3as) { tex_cords.resize(c1as + c2as + c3as);}
	if(tex_cords.size() < c1bs + c2bs + c3bs) { tex_cords.resize(c1bs + c2bs + c3bs);}
	
	//hm
	tex_cords[c1as] = int_tex_cords[c1bs];
	tex_cords[c2as] = int_tex_cords[c2bs];
	tex_cords[c3as] = int_tex_cords[c2bs];
	
      }

    }

    output.mesh_indices = indices;
    output.mesh_vertices = vertices;
    output.mesh_tex_coordinates = tex_cords;
    
    return output;

  }
