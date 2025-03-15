#pragma once


#include <glm/ext/quaternion_geometric.hpp>
#define STB_IMAGE_IMPLEMENTATION

#include "./loaders.h"
#include "../shader/shader_class.h"
#include "../glad/glad.h"
#include "../libs/image/stb_image.h"
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

struct tan_bin_glob {

  std::vector<float> vert_tangents;
  std::vector<float> vert_binormals;
  
};

void construct_fallback_uv_coordinates(mesh *reconstruct_mesh) {

  unsigned int num_vertices = (reconstruct_mesh->mesh_vertices.size()/3)*2;

  for(int i = 0; i < num_vertices; i++) {
    reconstruct_mesh->mesh_tex_coordinates.push_back(std::rand());
    reconstruct_mesh->mesh_tex_coordinates.push_back(std::rand());
  }

  return;
}

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

tan_bin_glob calculate_vert_tan_bin(std::vector<float> mesh_vertices,
                                           std::vector<float> mesh_normals,
					   std::vector<float> texture_coordinates) {
  
  std::vector<float> vert_tangents;
  std::vector<float> vert_binormals;

  tan_bin_glob return_glob;
  
  vert_tangents.resize(mesh_vertices.size());
  vert_binormals.resize(mesh_vertices.size());
  
  for (size_t i = 0; i < mesh_vertices.size(); i += 9) {

    glm::vec3 v0(mesh_vertices[i], mesh_vertices[i + 1],
		 mesh_vertices[i + 2]); // Vertex 1
    glm::vec3 v1(mesh_vertices[i + 3], mesh_vertices[i + 4],
		 mesh_vertices[i + 5]); // Vertex 2
    glm::vec3 v2(mesh_vertices[i + 6], mesh_vertices[i + 7],
		 mesh_vertices[i + 8]); // Vertex 3

    glm::vec2 t0(texture_coordinates[i], texture_coordinates[i+1]); // Vertex 1
    glm::vec2 t1(texture_coordinates[i+2], texture_coordinates[i+3]); // Vertex 2
    glm::vec2 t2(texture_coordinates[i+4], texture_coordinates[i+5]); // Vertex 3
	    
    glm::vec3 e1(v1-v0);
    glm::vec3 e2(v2-v0);

    glm::vec2 delta_uv_1(t1-t0);
    glm::vec2 delta_uv_2(t2-t0);

    //shoutout wikipedia
    float f = 1/((delta_uv_1.x * delta_uv_2.y) - (delta_uv_1.y * delta_uv_2.x));

    glm::vec3 tangent = f * ((delta_uv_2.y * e1) - (delta_uv_1.y * e2));
    glm::vec3 bitangent = f * ((-delta_uv_1.x * e1) + (delta_uv_1.x * e2));

    for(int vert_id = 0; vert_id < 3; vert_id++) {

      vert_tangents[i + (vert_id * 3)] += tangent.x;
      vert_tangents[i + (vert_id * 3) + 1] += tangent.y;
      vert_tangents[i + (vert_id * 3) + 2] += tangent.z;

      vert_binormals[i + (vert_id * 3)] += bitangent.x;
      vert_binormals[i + (vert_id * 3) + 1] += bitangent.y;
      vert_binormals[i + (vert_id * 3) + 2] += bitangent.z;
      
    }
    
  }

  //normalize tangents
  for(int i = 0; i < vert_tangents.size(); i = i+3) {

    glm::vec3 to_norm(vert_tangents[i],vert_tangents[i+1],vert_tangents[i+2]);
    to_norm = glm::normalize(to_norm);
    vert_tangents[i] = to_norm.x;
    vert_tangents[i+1] = to_norm.y;
    vert_tangents[i+2] = to_norm.z;
    
  }

    //normalize binormals
  for(int i = 0; i < vert_binormals.size(); i = i+3) {

    glm::vec3 to_norm(vert_binormals[i],vert_binormals[i+1],vert_binormals[i+2]);
    to_norm = glm::normalize(to_norm);
    vert_binormals[i] = to_norm.x;
    vert_binormals[i+1] = to_norm.y;
    vert_binormals[i+2] = to_norm.z;
    
  }

  for (size_t i = 0; i < vert_tangents.size(); i += 3) {
    glm::vec3 normal(mesh_normals[i], mesh_normals[i + 1], mesh_normals[i + 2]);
    glm::vec3 tangent(vert_tangents[i], vert_tangents[i + 1], vert_tangents[i + 2]);

    // Orthogonalize tangent
    tangent -= normal * glm::dot(normal, tangent);
    tangent = glm::normalize(tangent);

    // Recalculate bitangent
    glm::vec3 bitangent = glm::cross(normal, tangent);

    vert_tangents[i] = tangent.x;
    vert_tangents[i + 1] = tangent.y;
    vert_tangents[i + 2] = tangent.z;

    vert_binormals[i] = bitangent.x;
    vert_binormals[i + 1] = bitangent.y;
    vert_binormals[i + 2] = bitangent.z;
}

  return_glob.vert_binormals = vert_binormals;
  return_glob.vert_tangents = vert_tangents;

  return return_glob;
}

struct MeshData {
    std::vector<glm::vec3> vertices;  // Mesh vertices
    std::vector<glm::vec3> normals;   // Mesh normals
    std::vector<glm::vec3> tangents;  // Mesh tangents
    std::vector<glm::vec3> binormals; // Mesh binormals
};

MeshData createMeshData(const std::vector<float>& mesh_vertices, const std::vector<float>& mesh_texture_coordinates) {
    MeshData meshData;

    if (mesh_vertices.size() % 9 != 0) {
        throw std::invalid_argument("mesh_vertices must contain a multiple of 9 floats.");
    }

    if (mesh_texture_coordinates.size() % 2 != 0 || mesh_texture_coordinates.size() / 2 != mesh_vertices.size() / 3) {
        throw std::invalid_argument("mesh_texture_coordinates must contain valid UV pairs for each vertex.");
    }

    size_t numFaces = mesh_vertices.size() / 9;

    for (size_t i = 0; i < numFaces; ++i) {
        glm::vec3 v1(mesh_vertices[i * 9], mesh_vertices[i * 9 + 1], mesh_vertices[i * 9 + 2]);
        glm::vec3 v2(mesh_vertices[i * 9 + 3], mesh_vertices[i * 9 + 4], mesh_vertices[i * 9 + 5]);
        glm::vec3 v3(mesh_vertices[i * 9 + 6], mesh_vertices[i * 9 + 7], mesh_vertices[i * 9 + 8]);

        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        meshData.vertices.push_back(v1);
        meshData.vertices.push_back(v2);
        meshData.vertices.push_back(v3);
        meshData.normals.push_back(normal);
        meshData.normals.push_back(normal);
        meshData.normals.push_back(normal);

        glm::vec2 uv1(mesh_texture_coordinates[i * 6], mesh_texture_coordinates[i * 6 + 1]);
        glm::vec2 uv2(mesh_texture_coordinates[i * 6 + 2], mesh_texture_coordinates[i * 6 + 3]);
        glm::vec2 uv3(mesh_texture_coordinates[i * 6 + 4], mesh_texture_coordinates[i * 6 + 5]);

        glm::vec3 tangent, binormal;
        glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        tangent.x = r * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = r * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = r * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	//        binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
	binormal.x = r* (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	binormal.y = r* (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	binormal.z = r* (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        tangent = glm::normalize(tangent);
        binormal = glm::normalize(binormal);

        meshData.tangents.push_back(tangent);
        meshData.tangents.push_back(tangent);
        meshData.tangents.push_back(tangent);
        meshData.binormals.push_back(binormal);
        meshData.binormals.push_back(binormal);
        meshData.binormals.push_back(binormal);
    }

    return meshData;
}

std::vector<float> convert_varr_to_farr(std::vector<glm::vec3> inarr) {

  std::vector<float> out_arr;

  out_arr.resize((inarr.size()*3)+1);
  
  for(int i = 0; i< inarr.size(); i++) {

    out_arr.push_back(inarr[i].x);
    out_arr.push_back(inarr[i].y);
    out_arr.push_back(inarr[i].z);

  }

  return out_arr;
  
}
