#pragma once


#include "loaders.h"
#include "shader/shader_class.h"
#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include "libs/image/stb_image.h"
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


// Tex to slot
GLuint bind_texture_to_slot(std::string to_load, unsigned int slot) {
  printf("trying to load texture into slot :%d\n", slot);
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load(to_load.c_str(), &width, &height, &nrChannels, 0);

  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, texture);

  if (data) {
    printf("deserialized image successfully.\n");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, texture);

  return texture;
}

// surface normals
std::vector<float> calculate_vert_normals(std::vector<float> mesh_vertices) {
  std::vector<glm::vec3> vertexNormals; // buffer
  std::vector<float> mesh_normals;
  size_t numVertices = mesh_vertices.size() / 3;
  vertexNormals.resize(numVertices, glm::vec3(0.0f));

  for (size_t i = 0; i < mesh_vertices.size(); i += 9) {

    glm::vec3 v0(mesh_vertices[i], mesh_vertices[i + 1],
                 mesh_vertices[i + 2]); // Vertex 1
    glm::vec3 v1(mesh_vertices[i + 3], mesh_vertices[i + 4],
                 mesh_vertices[i + 5]); // Vertex 2
    glm::vec3 v2(mesh_vertices[i + 6], mesh_vertices[i + 7],
                 mesh_vertices[i + 8]); // Vertex 3

    // Calculate the normal for the face
    glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

    // Accumulate the face normal to each vertex's normal
    vertexNormals[i / 3] += faceNormal;     // Vertex 1
    vertexNormals[i / 3 + 1] += faceNormal; // Vertex 2
    vertexNormals[i / 3 + 2] += faceNormal; // Vertex 3
  }

  for (size_t i = 0; i < vertexNormals.size(); ++i) {
    vertexNormals[i] = glm::normalize(vertexNormals[i]);
  }

  for (const auto &normal : vertexNormals) {
    mesh_normals.push_back(normal.x);
    mesh_normals.push_back(normal.y);
    mesh_normals.push_back(normal.z);
  }

  std::cout << "done calculating " << mesh_normals.size() << " normals"
            << std::endl;
  return mesh_normals;
}
