#pragma once
/////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////

#include "./primitives.h"
#include "glad/glad.h"
#include "shader/shader_class.h"
#include <unordered_map>
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

mesh import_obj_mesh_rev2(std::string file_path) {
    mesh output;

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "import mesh not found!!!" << std::endl;
        return output; // Return an empty mesh
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<float> tex_coords;

    std::vector<float> buffer_tex_cords_x;
    std::vector<float> buffer_tex_cords_y;

    std::vector<float> buffer_vertex_x;
    std::vector<float> buffer_vertex_y;
    std::vector<float> buffer_vertex_z;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string junk;

	if(line[0] == 't') {

	  if(line[1] == 'a') {std::string file_path; s >> junk >> file_path; output.albedo_texture_path = file_path;}
	  if(line[1] == 'n') {std::string file_path; s >> junk >> file_path; output.normal_texture_path = file_path;}
	  if(line[1] == 'o') {std::string file_path; s >> junk >> file_path; output.ambient_occlusion_texture_path = file_path;}
	  if(line[1] == 'r') {std::string file_path; s >> junk >> file_path; output.roughness_texture_path = file_path;}
	  if(line[1] == 'm') {std::string file_path; s >> junk >> file_path; output.metallic_texture_path = file_path;}

	  if(line[1] == ' ') {std::string file_path; s >> junk >> file_path; output.texture_path = file_path;}
	  
	}
	
        if (line[0] == 'v') {
            if (line[1] == ' ') {
                float vecx, vecy, vecz;
                s >> junk >> vecx >> vecy >> vecz;
                buffer_vertex_x.push_back(vecx);
                buffer_vertex_y.push_back(vecy);
                buffer_vertex_z.push_back(vecz);
            } else if (line[1] == 't') {
                float texx, texy;
                s >> junk >> texx >> texy;
                buffer_tex_cords_x.push_back(texx);
                buffer_tex_cords_y.push_back(texy);
            }
        } else if (line[0] == 'f') {
            std::string chunk1, chunk2, chunk3;
            s >> junk >> chunk1 >> chunk2 >> chunk3;

            int verti1 = std::stoi(chunk1.substr(0, chunk1.find('/'))) - 1;
            int verti2 = std::stoi(chunk2.substr(0, chunk2.find('/'))) - 1;
            int verti3 = std::stoi(chunk3.substr(0, chunk3.find('/'))) - 1;

            int texi1 = std::stoi(chunk1.substr(chunk1.find('/') + 1)) - 1;
            int texi2 = std::stoi(chunk2.substr(chunk2.find('/') + 1)) - 1;
            int texi3 = std::stoi(chunk3.substr(chunk3.find('/') + 1)) - 1;

            // Add vertices
            vertices.push_back(buffer_vertex_x[verti1]);
            vertices.push_back(buffer_vertex_y[verti1]);
            vertices.push_back(buffer_vertex_z[verti1]);

            vertices.push_back(buffer_vertex_x[verti2]);
            vertices.push_back(buffer_vertex_y[verti2]);
            vertices.push_back(buffer_vertex_z[verti2]);

            vertices.push_back(buffer_vertex_x[verti3]);
            vertices.push_back(buffer_vertex_y[verti3]);
            vertices.push_back(buffer_vertex_z[verti3]);
	    
            // Add texture coordinates
	    if(buffer_tex_cords_x.size() != 0 && buffer_tex_cords_y.size() != 0){
	      tex_coords.push_back(buffer_tex_cords_x[texi1]);
	      tex_coords.push_back(1-buffer_tex_cords_y[texi1]);
	      
	      tex_coords.push_back(buffer_tex_cords_x[texi2]);
	      tex_coords.push_back(1-buffer_tex_cords_y[texi2]);
	      
	      tex_coords.push_back(buffer_tex_cords_x[texi3]);
	      tex_coords.push_back(1-buffer_tex_cords_y[texi3]);
	    }
            // Add indices
	    //            indices.push_back(indices.size());
            //indices.push_back(indices.size());
            //indices.push_back(indices.size());
        }
    }

    //    output.mesh_indices = indices;
    output.mesh_vertices = vertices;
    output.mesh_tex_coordinates = tex_coords;

    return output;
}
