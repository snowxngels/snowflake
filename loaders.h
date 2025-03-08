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

mesh import_obj_mesh(std::string file_path) {

  mesh output;

  std::ifstream file(file_path);
  if (!file.is_open())
    std::cout << "imort mesh not found!!!" << std::endl;

  std::vector<float> vertices;
  std::vector<unsigned int> indices;
  std::vector<float> tex_cords;

  std::vector<float> buffer_tex_cords_x;
  std::vector<float> buffer_tex_cords_y;

  std::vector<float> buffer_vertex_x;
  std::vector<float> buffer_vertex_y;
  std::vector<float> buffer_vertex_z;

  while (!file.eof()) {

    char line[1024];
    file.getline(line, 1024);

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

        buffer_vertex_x.push_back(vecx);
        buffer_vertex_y.push_back(vecy);
        buffer_vertex_z.push_back(vecz);

        printf("found vertex:%f,%f,%f", vecx, vecy, vecz);
      }

      if (line[1] == 't') {

        std::cout << "deserializing a tex cord" << std::endl;

        float texx;
        float texy;

        s >> junk >> texx >> texy;

        buffer_tex_cords_x.push_back(texx);
        buffer_tex_cords_y.push_back(texy);
      }
    }

    if (line[0] == 'f') {

      std::cout << "deserializing a face" << std::endl;

      // find vertices for face
      std::string chunk1;
      std::string chunk2;
      std::string chunk3;

      s >> junk >> chunk1 >> chunk2 >> chunk3;

      float tex_buf;

      int texi1 = std::stoi(chunk1.substr(chunk1.find('/') + 1));
      int texi2 = std::stoi(chunk2.substr(chunk2.find('/') + 1));
      int texi3 = std::stoi(chunk3.substr(chunk3.find('/') + 1));

      int verti1 = std::stoi(chunk1.substr(0, chunk1.find('/')));
      int verti2 = std::stoi(chunk2.substr(0, chunk2.find('/')));
      int verti3 = std::stoi(chunk3.substr(0, chunk3.find('/')));

      // resolve vertex/texture combination, add to vertices / indices
      // starting with vertices

      while (vertices.size() < verti1) {
        vertices.resize(verti1);
      }
      while (vertices.size() < verti2) {
        vertices.resize(verti2);
      }
      while (vertices.size() < verti3) {
        vertices.resize(verti3);
      }

      vertices.push_back(buffer_vertex_x[verti1 - 1]);
      vertices.push_back(buffer_vertex_y[verti1 - 1]);
      vertices.push_back(buffer_vertex_z[verti1 - 1]);

      vertices.push_back(buffer_vertex_x[verti2 - 1]);
      vertices.push_back(buffer_vertex_y[verti2 - 1]);
      vertices.push_back(buffer_vertex_z[verti2 - 1]);

      vertices.push_back(buffer_vertex_x[verti3 - 1]);
      vertices.push_back(buffer_vertex_y[verti3 - 1]);
      vertices.push_back(buffer_vertex_z[verti3 - 1]);

      // writing indices of face we deserialized

      tex_cords.push_back(buffer_tex_cords_x[texi1-1]);
      tex_cords.push_back(buffer_tex_cords_y[texi1-1]);

      tex_cords.push_back(buffer_tex_cords_x[texi2-1]);
      tex_cords.push_back(buffer_tex_cords_y[texi2-1]);

      tex_cords.push_back(buffer_tex_cords_x[texi3-1]);
      tex_cords.push_back(buffer_tex_cords_y[texi3-1]);

      // now for the 3 vertex triplets, store tex coordinates in tex

      //	if(tex_cords.size() < texi1 + c2 + c3) { tex_cords.resize(c1 +
      //c2 + c3);} if(tex_cords.size() < c1bs + c2bs + c3bs) {
      // tex_cords.resize(c1bs + c2bs + c3bs);}

      //	tex_cords[c1*2+1] = int_tex_cords[c1bs/2-1]
    }
  }

  output.mesh_indices = indices;
  output.mesh_vertices = vertices;
  output.mesh_tex_coordinates = tex_cords;

  return output;
}

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
            tex_coords.push_back(buffer_tex_cords_x[texi1]);
            tex_coords.push_back(buffer_tex_cords_y[texi1]);

            tex_coords.push_back(buffer_tex_cords_x[texi2]);
            tex_coords.push_back(buffer_tex_cords_y[texi2]);

            tex_coords.push_back(buffer_tex_cords_x[texi3]);
            tex_coords.push_back(buffer_tex_cords_y[texi3]);

            // Add indices
            indices.push_back(indices.size());
            indices.push_back(indices.size());
            indices.push_back(indices.size());
        }
    }

    output.mesh_indices = indices;
    output.mesh_vertices = vertices;
    output.mesh_tex_coordinates = tex_coords;

    return output;
}
