/////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////

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

// defines:
#define FOV_DEF 90.0f
#define POLY_WIREFRAME false
#define MESH_TO_LOAD "assets/Car.fbx"

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 direction = {0.0f, 0.0f, 0.0f};

// bungie employees hate this one simple trick
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// input shit
bool isMouseGrabbed = true;
bool isMouseOnCooldown = true;
bool firstMouse = true;
double lastX = 0;
double lastY = 0;
double yaw = 0;
double pitch = 0;

//////////////////////////////////////////////
// INPUT HANDLERS + CALLBACK FUNCTIONS
//////////////////////////////////////////////

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  float cameraSpeed = 10.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraLookAt;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraLookAt;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraLookAt, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraLookAt, cameraUp)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
    isMouseGrabbed = !isMouseGrabbed; // Toggle the state
    if (isMouseGrabbed) {
      isMouseOnCooldown = true;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Grab the mouse
    } else {
      isMouseOnCooldown = true;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Release the mouse
    }
  }
  
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {

  if(!isMouseGrabbed) {return;}

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  if (firstMouse) {
    xoffset = xpos - lastX;
    yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
    isMouseOnCooldown = false;
  }

  if(isMouseOnCooldown) {
    xoffset = xpos - lastX;
    yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    isMouseOnCooldown = false;
    return;
  }
  
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.08f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraLookAt = glm::normalize(direction);
}

///////////////////////////////////////////////////////
// UTILITY FUNCTIONS
///////////////////////////////////////////////////////

GLuint bind_texture_to_slot(std::string to_load, unsigned int slot) {
  printf("trying to load textures into slot :%d\n", slot);
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

///////////////////////////////////////////////////////
// START POINT
///////////////////////////////////////////////////////

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "wdwion", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);

  // setup
  glEnable(GL_DEPTH_TEST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (POLY_WIREFRAME)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // model loading

  scene active_scene;

  model second_model;

  second_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/torus/torus.obj")); 
  
  active_scene.add_model_to_scene(second_model);
  
  ////////////////////////////////////
  

  printf("initializing buffers\n");

  for (auto &i : active_scene.loaded_models) {

    for (auto &sub_mesh : i.contained_meshes) {

      // vao
      glGenVertexArrays(1, &sub_mesh.mesh_VAO);
      glBindVertexArray(sub_mesh.mesh_VAO);

      // vbo mesh (vertices)
      glGenBuffers(1, &sub_mesh.mesh_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_vertices.size() * sizeof(float),
                   sub_mesh.mesh_vertices.data(), GL_STATIC_DRAW);
      // tell attrib pointers where to read
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(0);

      // vbo tex
      glGenBuffers(1, &sub_mesh.mesh_tex_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_coordinates.size() * sizeof(float),
                   sub_mesh.mesh_tex_coordinates.data(), GL_STATIC_DRAW);
      // texture read
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(1);

      // ebo
      /*
      glGenBuffers(1, &sub_mesh.mesh_EBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub_mesh.mesh_EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sub_mesh.mesh_indices.size() * sizeof(int),
                   sub_mesh.mesh_indices.data(), GL_STATIC_DRAW);
      */
      
    }
  }

  Shader mainShader("default.vert", "default.frag");

  mainShader.use();

  printf("trying to import a texture...\n");

  ////// JANK FIX LATER!!!
  
  active_scene.loaded_models[0].contained_meshes[0].mes_tex_id = bind_texture_to_slot("assets/torus/torus_texture.jpg", 0);

  mainShader.setInt("texture1", 0);

  // random shit for render loop
  //  unsigned int transformLoc = glGetUniformLocation(mainShader.ID,
  //  "transform");

  // pre debugging space

  //====================

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // clear shit wow comment to look nice
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // bungie 
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // projection matrix
    int height = 0, width = 0;
    glfwGetWindowSize(window, &height, &width);
    glm::mat4 proj = glm::perspective(
        glm::radians(FOV_DEF), (float)width / (float)height, 0.1f, 100.0f);

    // view matrix
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraLookAt + cameraPos, cameraUp);

    for (auto &i : active_scene.loaded_models) {
      
      for (auto &j : i.contained_meshes) {

        // render loop for all loaded models ALSO CALL BIND TEXTURE FOR EVERY
        // TEXTURE!!!!!!!
        glBindTexture(
            GL_TEXTURE_2D,
            active_scene.loaded_models[0].contained_meshes[0].mes_tex_id);
        glBindVertexArray(j.mesh_VAO);
        if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
          std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
        }

        mainShader.use();

        // model matrix
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model,
                               glm::vec3(1.0f * sin(currentFrame), 0.0f, 0.0f));

        // get ID of uniform for model pos data
        int modelLoc = glGetUniformLocation(mainShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // get ID of uniform for view matrix data
        int viewLoc = glGetUniformLocation(mainShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // get ID of uniform for proj matrix
        int projectionLoc = glGetUniformLocation(mainShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

        if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1) {
          std::cout << "ERROR::UNIFORM::LOCATION_NOT_FOUND" << std::endl;
        }

        // actually draw elements
	//        glDrawElements(GL_TRIANGLES, j.mesh_indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES,0,j.mesh_vertices.size());
	
      }
    }
    
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
