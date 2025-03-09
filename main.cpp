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
#define NEAR_CLIP_PLANE 0.1f
#define FAR_CLIP_PLANE 100000.0f

#define AMBIENT_LIGHTING_BASE 0.1f
#define MAX_ALLOWED_LIGHTS 50
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

  model first_model;
  first_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/torus/torus.obj"));
  first_model.contained_meshes[0].theta_y = 0.0f;
  first_model.contained_meshes[0].render_type = 1.0f;
  active_scene.add_model_to_scene(first_model);
  
  model second_model;
  second_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/ball/ball.obj"));
  second_model.contained_meshes[0].location_y = 2.0f;
  second_model.contained_meshes[0].render_type = 0.0f;
  active_scene.add_model_to_scene(second_model);
  
  model third_model;
  third_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/floor/floor.obj"));
  third_model.contained_meshes[0].location_y = -1.0f;
  active_scene.add_model_to_scene(third_model);

  model skybox_model;
  skybox_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/skybox/skybox.obj"));
  skybox_model.contained_meshes[0].mesh_type = MESH_SKYBOX;
  skybox_model.contained_meshes[0].mesh_affected_by_light = 0;
  active_scene.add_model_to_scene(skybox_model);

  light first_light;
  first_light.location_x = 5.0f;
  first_light.location_y = 1.0f;
  first_light.strength = 10;
  active_scene.add_light_to_scene(first_light);


  ///////////////////////////////////
  // initialize lights in scene
  ///////////////////////////////////

  GLuint lightUBO;
  glGenBuffers(1, &lightUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) * 10 + sizeof(float), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);

  glBufferSubData(GL_UNIFORM_BUFFER,
		  0,
		  sizeof(float)*active_scene.loaded_lights.size(),
		  active_scene.loaded_lights.data());

  //tmp light src def below


  ////////////////////////////////////
  // initializing mesh buffers
  ////////////////////////////////////
  
  printf("initializing buffers for meshes\n");
  
  for (auto &i : active_scene.loaded_models) {

    for (auto &sub_mesh : i.contained_meshes) {


      //calculate mesh vertex normals
      std::vector<glm::vec3> vertexNormals; // buffer
      size_t numVertices = sub_mesh.mesh_vertices.size() / 3;
      vertexNormals.resize(numVertices, glm::vec3(0.0f));

      for (size_t i = 0; i < sub_mesh.mesh_vertices.size(); i += 9) {

        glm::vec3 v0(sub_mesh.mesh_vertices[i], sub_mesh.mesh_vertices[i + 1], sub_mesh.mesh_vertices[i + 2]); // Vertex 1
        glm::vec3 v1(sub_mesh.mesh_vertices[i + 3], sub_mesh.mesh_vertices[i + 4],
                     sub_mesh.mesh_vertices[i + 5]); // Vertex 2
        glm::vec3 v2(sub_mesh.mesh_vertices[i + 6], sub_mesh.mesh_vertices[i + 7],
                     sub_mesh.mesh_vertices[i + 8]); // Vertex 3

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
        sub_mesh.mesh_normals.push_back(normal.x);
        sub_mesh.mesh_normals.push_back(normal.y);
        sub_mesh.mesh_normals.push_back(normal.z);
      }

      std::cout << "done calculating x normals x = " << sub_mesh.mesh_normals.size() << std::endl;
      
      //////////////////////////
      // BUFFERS
      //////////////////////////
      
      // vao
      glGenVertexArrays(1, &sub_mesh.mesh_VAO);
      glBindVertexArray(sub_mesh.mesh_VAO);

      // vbo mesh (vertices)
      glGenBuffers(1, &sub_mesh.mesh_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_vertices.size() * sizeof(float),
                   sub_mesh.mesh_vertices.data(), GL_STATIC_DRAW);
      // mesh stride
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(0);

      // vbo tex
      glGenBuffers(1, &sub_mesh.mesh_tex_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_coordinates.size() * sizeof(float),
                   sub_mesh.mesh_tex_coordinates.data(), GL_STATIC_DRAW);
      // texture stride
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(1);

      // vbo normals
      glGenBuffers(1, &sub_mesh.mesh_norm_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_norm_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_normals.size() * sizeof(float),
                   sub_mesh.mesh_normals.data(), GL_STATIC_DRAW);
      // normals stride
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(2);

      std::cout << sub_mesh.mesh_VBO << " vbo " << sub_mesh.mesh_tex_VBO << " tex_vbo " << sub_mesh.mesh_norm_VBO << " norm_vbo " << std::endl;
      
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
  
  unsigned int loaded_textures = 0;

  for(auto &i : active_scene.loaded_models) {
    for(auto &j : i.contained_meshes) {

      j.mes_tex_id = bind_texture_to_slot( j.texture_path, loaded_textures);

      std::cout << "loaded tex: " << j.texture_path << "in slot :" << loaded_textures << "with texture_id" << j.mes_tex_id << std::endl;
      
      loaded_textures++;

    }
    
  }
  
  //////////////////////////////////////////
  // setting up global values before rendering loop
  //////////////////////////////////////////

  int ambient_light_base_loc = glGetUniformLocation(mainShader.ID, "ambient_light_base");  
  glUniform3f(ambient_light_base_loc,AMBIENT_LIGHTING_BASE,AMBIENT_LIGHTING_BASE,AMBIENT_LIGHTING_BASE);
  
  ////////////////////////
  // rendering loop
  ///////////////////////

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // clear shit wow comment to look nice
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // bungie (deltatime)
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ///////////////////////////////////
    // DEMO MOVEMENT CODE
    ///////////////////////////////////
    
    float modifier_rotate = glm::radians(sin(currentFrame*4)*360);
    active_scene.loaded_models[0].contained_meshes[0].theta_y = modifier_rotate;
    
    // projection matrix
    int height = 0, width = 0;
    glfwGetWindowSize(window, &height, &width);
    glm::mat4 proj = glm::perspective(
        glm::radians(FOV_DEF), (float)width / (float)height, NEAR_CLIP_PLANE, FAR_CLIP_PLANE);

    // view matrix
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraLookAt + cameraPos, cameraUp);

    //skybox movement
    glm::mat4 skybox = glm::mat4(1.0f);
    skybox = glm::translate(skybox, cameraPos);

    ////////////////////////////////////
    // initialize all lights
    ////////////////////////////////////

    int num_lights_loc = glGetUniformLocation(mainShader.ID, "num_lights");
    glUniform1i(num_lights_loc, MAX_ALLOWED_LIGHTS);

    for (auto &i : active_scene.loaded_lights) {

      

    }    
    
    ////////////////////////////////////
    // render all meshes
    ////////////////////////////////////
    
    for (auto &i : active_scene.loaded_models) {
      
      for (auto &j : i.contained_meshes) {

	mainShader.use();
		
	//////////////////////////////////////
	// change texture rendering or face rendering
	//////////////////////////////////////

	int render_type_loc = glGetUniformLocation(mainShader.ID, "render_type");
	int face_color_loc = glGetUniformLocation(mainShader.ID, "face_color");

	//copy render type (face shade / texture) and the face color to shader
        glUniform1f(render_type_loc, j.render_type);
	glUniform4f(face_color_loc,
		    j.face_color_r/255.0f,
		    j.face_color_g/255.0f,
		    j.face_color_b/255.0f,
		    1.0f);
	
        // TEXTURE (i coded this on accident no clue why it works)
        glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
        mainShader.setInt("texture1", loaded_textures - 1);
        glBindVertexArray(j.mesh_VAO);
        if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
          std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
        }

	//////////////////////////////
	// calculate lightning
	//////////////////////////////

	float face_brightness;

	face_brightness = 1;
	
	int face_brightness_loc = glGetUniformLocation(mainShader.ID, "face_brightness");
	int aff_by_light_loc = glGetUniformLocation(mainShader.ID, "affected_by_light");

	glUniform1i(aff_by_light_loc,j.mesh_affected_by_light);

	//tmp below

	int light_source_loc = glGetUniformLocation(mainShader.ID, "light_source");
	glUniform3f(light_source_loc, sin(currentFrame)*5.0f, 2.0f, cos(currentFrame)*5.0f);
	int light_color_loc = glGetUniformLocation(mainShader.ID, "light_color");
	glUniform3f(light_color_loc, 50.0f, 50.0f, 40.0f);
	int light_strength_loc = glGetUniformLocation(mainShader.ID, "light_strength");
	glUniform1f(light_strength_loc, 0.3f);
	
	//////////////////////////////
	// translation and rotation
	//////////////////////////////
	
        // model matrix
        glm::mat4 model = glm::mat4(1.0f);

	// setting mesh position transform
	glm::vec3 mod_transform(j.location_x,j.location_y,j.location_z);	
	model = glm::translate(model, mod_transform);

	// setting mesh rotation transform
	float rad_theta_x = glm::radians(j.theta_x);
	float rad_theta_y = glm::radians(j.theta_y);
	float rad_theta_z = glm::radians(j.theta_z);

	glm::mat4 mat_rot_x = glm::rotate(glm::mat4(1.0f), rad_theta_x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mat_rot_y = glm::rotate(glm::mat4(1.0f), rad_theta_y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mat_rot_z = glm::rotate(glm::mat4(1.0f), rad_theta_z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 mat_rot = mat_rot_x * mat_rot_y * mat_rot_z;

	model = model * mat_rot;
	
        // upload model matrix to shader
	int modelLoc = glGetUniformLocation(mainShader.ID, "model");
        int viewLoc = glGetUniformLocation(mainShader.ID, "view");
        int projectionLoc = glGetUniformLocation(mainShader.ID, "projection");

	// mesh type transforms
        if(j.mesh_type == MESH_SKYBOX) {
	  
	  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(skybox));
	  
	} else {
	  
	  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));        
	  
	}
	
        // upload view matrix and projection matrix to shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

	//	std::cout << modelLoc << " model " << viewLoc << " view " << projectionLoc << " projection " << std::endl;	
        if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1) {
          std::cout << "ERROR::UNIFORM::LOCATION_NOT_FOUND - Loc" << std::endl;
        }
	 
	// glDrawElements(GL_TRIANGLES, j.mesh_indices.size(), GL_UNSIGNED_INT, 0);
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
