/////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////

#include "shader/shader_class.h"

#include "glad/glad.h"
#include "./components/loaders.h"
#include "./components/utility.h"
#include "./components/primitives.h"

#include "libs/tiny_gltf.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stdlib
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

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

#define AMBIENT_LIGHTING_BASE 0.4f
#define MAX_ALLOWED_LIGHTS 50

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 direction = {0.0f, 0.0f, 0.0f};

// bungie employees hate this one simple trick
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float dampening_coefficient = 0.01f;

// input shit
bool isMouseGrabbed = true;
bool isMouseOnCooldown = true;
bool firstMouse = true;
bool lastMouseState = false;
double lastX = 0;
double lastY = 0;
double yaw = 0;
double pitch = 0;

float cameraBaseSpeed = 1.0f;

// debug stuff
uint64_t total_vertices = 0;
unsigned int window_width = 1900;
unsigned int window_heigh = 1020;

//////////////////////////////////////////////
// INPUT HANDLERS + CALLBACK FUNCTIONS
//////////////////////////////////////////////
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

  std::cout << "Scroll offset: (" << xoffset << ", " << yoffset << ")\n";

  cameraBaseSpeed = cameraBaseSpeed + static_cast<float>(yoffset) * 0.1f;

  std::cout << cameraBaseSpeed << std::endl;

  if(cameraBaseSpeed < 0.1f) {cameraBaseSpeed = 0.1f;}
  
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

  float cameraSpeed = cameraBaseSpeed * 10.0f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {cameraPos += cameraSpeed * glm::normalize(glm::vec3(cameraLookAt.x,0.0f,cameraLookAt.z)); }
    
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {cameraPos += cameraSpeed * glm::normalize(glm::vec3(-cameraLookAt.x,0.0f,-cameraLookAt.z)); }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -=
        glm::normalize(glm::cross(cameraLookAt, cameraUp)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos +=
        glm::normalize(glm::cross(cameraLookAt, cameraUp)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cameraPos +=
      glm::normalize(glm::vec3(0.0f,-1.0f,0.0f)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cameraPos +=
      glm::normalize(glm::vec3(0.0f,1.0f,0.0f)) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
    if (lastMouseState == false) {
      isMouseGrabbed = !isMouseGrabbed; // Toggle the state
      if (isMouseGrabbed) {
        isMouseOnCooldown = true;
        glfwSetInputMode(window, GLFW_CURSOR,
                         GLFW_CURSOR_DISABLED); // Grab the mouse
      } else {
        isMouseOnCooldown = true;
        glfwSetInputMode(window, GLFW_CURSOR,
                         GLFW_CURSOR_NORMAL); // Release the mouse
      }
      lastMouseState = true;
    }
  }

  if (glfwGetKey(window,GLFW_KEY_G) != GLFW_PRESS) {

    lastMouseState = false;

  }

  return;
  
}

///////////////////////////////////////////////////////
// START POINT
///////////////////////////////////////////////////////

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(window_width, window_heigh, "snowflake - debug build", NULL, NULL);
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

  glViewport(0, 0, window_width, window_heigh);

  // setup
  glEnable(GL_DEPTH_TEST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
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
  first_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/floor/floor2.obj"));
  first_model.location_x = 5.0f;
  first_model.contained_meshes[0].shading_type = SHADING_PBR;
  active_scene.add_model_to_scene(first_model);
    
  model second_model;
  second_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/ball/ball.obj"));
  second_model.location_y = -0.2f;
  second_model.physics_type = RIGID_BODY;
  second_model.contained_meshes[0].shading_type = SHADING_PBR;
  active_scene.add_model_to_scene(second_model);

  /*
  model third_model;
  third_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/levi/levi.obj"));
  third_model.location_y = -1.0f;
  third_model.contained_meshes[0].specular_strength = 0.2f;
  active_scene.add_model_to_scene(third_model);
  */

  
  model skybox_model;
  skybox_model.contained_meshes.push_back(import_obj_mesh_rev2("assets/skybox/skybox.obj"));
  skybox_model.contained_meshes[0].mesh_type = MESH_SKYBOX;
  skybox_model.contained_meshes[0].shading_type = SHADING_NONE;
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

  ///////////////////////////////////
  // generating visualizer meshes for light sources
  ///////////////////////////////////

  for(auto &i : active_scene.loaded_lights) {

    model light_source_visualizer;
    light_source_visualizer.contained_meshes.push_back(import_obj_mesh_rev2("assets/light_source/light_source.obj"));
    light_source_visualizer.contained_meshes[0].shading_type = SHADING_NONE;
    light_source_visualizer.contained_meshes[0].mesh_type = MESH_LIGHT;
    light_source_visualizer.contained_meshes[0].mesh_affected_by_light = 0.0f;
    active_scene.loaded_models.push_back(light_source_visualizer);

  }
  
  ////////////////////////////////////
  // initializing mesh buffers
  ////////////////////////////////////
  
  printf("initializing buffers for meshes\n");
  
  for (auto &i : active_scene.loaded_models) {

    for (auto &sub_mesh : i.contained_meshes) {

      ////////////////////////////
      // GENERATE MISSING GEOMETRY
      ////////////////////////////

      //calculate mesh vertex normals + track vertex count for lolz
      
      sub_mesh.mesh_normals = calculate_vert_normals(sub_mesh.mesh_vertices);
      total_vertices = total_vertices + sub_mesh.mesh_vertices.size();

      //calculate vertex tangents / binormals
      tan_bin_glob retglob = calculate_vert_tan_bin(sub_mesh.mesh_vertices,
						       sub_mesh.mesh_normals,
						       sub_mesh.mesh_tex_coordinates);
      
      sub_mesh.mesh_binormals = retglob.vert_binormals;
      sub_mesh.mesh_tangents = retglob.vert_tangents;
			      
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
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(0);

      // vbo tex
      glGenBuffers(1, &sub_mesh.mesh_tex_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_tex_coordinates.size() * sizeof(float),
                   sub_mesh.mesh_tex_coordinates.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(1);

      // vbo normals
      glGenBuffers(1, &sub_mesh.mesh_norm_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_norm_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_normals.size() * sizeof(float),
                   sub_mesh.mesh_normals.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(2);

      // vbo tangents
      glGenBuffers(1, &sub_mesh.mesh_tan_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_tan_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_tangents.size() * sizeof(float),
                   sub_mesh.mesh_tangents.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(3);
      
      // vbo binormals
      glGenBuffers(1, &sub_mesh.mesh_binorm_VBO);
      glBindBuffer(GL_ARRAY_BUFFER, sub_mesh.mesh_binorm_VBO);
      glBufferData(GL_ARRAY_BUFFER, sub_mesh.mesh_binormals.size() * sizeof(float),
                   sub_mesh.mesh_binormals.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(4);

      //      std::cout << sub_mesh.mesh_VBO << " , " << sub_mesh.mesh_tex_VBO << " , " << sub_mesh.mesh_norm_VBO << " - mesh_vbo , tex_vbo and norm_vbo " << std::endl;

    }
  }

  printf("finished importing %d meshes with %d vertices \n", active_scene.loaded_models.size(), total_vertices);

  Shader phong_shader("shader/shader_src/phong_vs.glsl", "shader/shader_src/phong_fs.glsl");
  //init shader
  phong_shader.use();

  Shader pbr_shader("shader/shader_src/new_pbr_vs.glsl", "shader/shader_src/new_pbr_fs.glsl");
  //init shader
  pbr_shader.use();

  Shader depth_shader("shader/shader_src/depth_vs.glsl", "shader/shader_src/depth_fs.glsl");
  //init shader
  depth_shader.use();
  
  printf("trying to import a texture...\n");

  ////////////////////////////////
  // LOADING TEXTURES
  ///////////////////////////////
  
  unsigned int loaded_textures = 0;

  for (auto &i : active_scene.loaded_models) {
    for (auto &j : i.contained_meshes) {

      if ( !(j.texture_path == "" && j.albedo_texture_path == "") ) {

	switch (j.shading_type) {

        case SHADING_PBR: {

	  pbr_shader.use();
	  
          std::cout << "loading texture : " << j.albedo_texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mesh_albedo_texture_id << std::endl;	  
          j.mesh_albedo_texture_id =
              bind_texture_to_slot(j.albedo_texture_path, loaded_textures);
          loaded_textures++;
	  
          std::cout << "loading texture : " << j.normal_texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mesh_normal_texture_id << std::endl;
          j.mesh_normal_texture_id =
	    bind_texture_to_slot(j.normal_texture_path, loaded_textures);
          loaded_textures++;

	  std::cout << "loading texture : " << j.roughness_texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mesh_roughness_texture_id << std::endl;
          j.mesh_roughness_texture_id =
              bind_texture_to_slot(j.roughness_texture_path, loaded_textures);
          loaded_textures++;

	  std::cout << "loading texture : " << j.metallic_texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mesh_metallic_texture_id << std::endl;
          j.mesh_metallic_texture_id =
              bind_texture_to_slot(j.metallic_texture_path, loaded_textures);
          loaded_textures++;

	  std::cout << "loading texture : " << j.ambient_occlusion_texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mesh_ambient_occlusion_texture_id << std::endl;
          j.mesh_ambient_occlusion_texture_id = bind_texture_to_slot(
              j.ambient_occlusion_texture_path, loaded_textures);
          loaded_textures++;
	  break;
        }

        default: {
	  
	  phong_shader.use();
          j.mes_tex_id = bind_texture_to_slot(j.texture_path, loaded_textures);
          std::cout << "loading texture : " << j.texture_path
                    << "into slot :" << loaded_textures
                    << " with texture_id: " << j.mes_tex_id << std::endl;
        }
        }
      } else {

	phong_shader.use();
        // no texture paths in file, using face shading
        std::cout << "no texture path in mesh! using fallback texture! + face shading" << std::endl;
        //	j.mesh_affected_by_light = 0.0f;
	j.disable_tex_shading = 0.0f;

        construct_fallback_uv_coordinates(&j);

        j.mes_tex_id = bind_texture_to_slot( "assets/fallback/fallback.jpg", loaded_textures);
	std::cout << "loaded fallback tex into slot :" << loaded_textures << " with texture_id: " << j.mes_tex_id << std::endl;
      }

      loaded_textures++;
    }
  }

  //////////////////////////////////////////
  // setting up global values before rendering loop
  //////////////////////////////////////////

  int ambient_light_base_loc = glGetUniformLocation(phong_shader.ID, "ambient_light_base");  
  glUniform3f(ambient_light_base_loc,AMBIENT_LIGHTING_BASE,AMBIENT_LIGHTING_BASE,AMBIENT_LIGHTING_BASE);

  //////////////////////////////
  // depth mapping
  //////////////////////////////

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  
  
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
    
    glm::vec3 light_pos(sin(glfwGetTime()*0.5), 2.0f, cos(glfwGetTime()*0.5));

    //active_scene.loaded_models[1].location_x = sin(glfwGetTime()) * 7.0f;
    //    active_scene.loaded_models[1].location_y = cos(glfwGetTime()) * 7.0f;
    active_scene.loaded_models[0].theta_x = sin(glfwGetTime()*0.2) * 45;
    //active_scene.loaded_models[1].theta_y = cos(glfwGetTime()*0.2) * 360;
    
    // projection matrix
    int height = 0, width = 0;
    glfwGetWindowSize(window, &height, &width);
    glm::mat4 proj = glm::perspective(
        glm::radians(FOV_DEF), (float)window_width / (float)window_heigh, NEAR_CLIP_PLANE, FAR_CLIP_PLANE);

    // view matrix
    glm::mat4 view;
    view = glm::lookAt(cameraPos, cameraLookAt + cameraPos, cameraUp);

    ////////////////////////////////////
    // Handle physics
    ///////////////////////////////////

    for (auto &i : active_scene.loaded_models) {

      if (i.las_phys_calculation < (currentFrame + 0.05f) &&
          i.physics_type == RIGID_BODY) {

        if (i.accell_x >= dampening_coefficient) {
          i.location_x = i.location_x + i.accell_x;
          i.accell_x = i.accell_x - dampening_coefficient;
          std::cout << i.location_x << " location " << i.accell_x
                    << " accell_x " << std::endl;
        }
        if (i.accell_x < dampening_coefficient) {
          i.accell_x = 0;
        }

        break;
      }

      i.las_phys_calculation = currentFrame;
    }

    ////////////////////////////////////
    // per mesh depth redering
    ///////////////////////////////////

    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;

    float near_plane = 1.0f, far_plane = 7.5f;

    lightProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,near_plane,far_plane);

    lightView = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));

    lightSpaceMatrix = lightProjection * lightView;

    depth_shader.use();

    GLuint lightSpaceMatrixUni = glGetUniformLocation(depth_shader.ID, "lightSpaceMatrix");
    glUniformMatrix4fv(lightSpaceMatrixUni, 1, GL_FALSE,  glm::value_ptr(lightSpaceMatrix));

    glViewport(0,0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // render the meshes (depth pass)
    for(auto i : active_scene.loaded_models) {
      float parent_mesh_pos_x = i.location_x;
      float parent_mesh_pos_y = i.location_y;
      float parent_mesh_pos_z = i.location_z;

      float parent_offset_theta_x = i.theta_x;
      float parent_offset_theta_y = i.theta_y;
      float parent_offset_theta_z = i.theta_z;
      for(auto j : i.contained_meshes) {

	if(j.shading_type != SHADING_PBR) {break;}
	
	// model matrix
        glm::mat4 model = glm::mat4(1.0f);
	
	// setting mesh position transform
	glm::vec3 mod_transform(j.offset_pos_x + parent_mesh_pos_x,
				j.offset_pos_y + parent_mesh_pos_y,
				j.offset_pos_z + parent_mesh_pos_z);	
	model = glm::translate(model, mod_transform);
	
	int modelLoc = glGetUniformLocation(phong_shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//bind mesh vao
	glBindVertexArray(j.mesh_VAO);
	if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
	  std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
	}
	
      }
      
    }

    glViewport(0,0,window_width,window_heigh);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    ////////////////////////////////////
    // render all meshes
    ////////////////////////////////////
    
    for (auto &i : active_scene.loaded_models) {

      float parent_mesh_pos_x = i.location_x;
      float parent_mesh_pos_y = i.location_y;
      float parent_mesh_pos_z = i.location_z;

      float parent_offset_theta_x = i.theta_x;
      float parent_offset_theta_y = i.theta_y;
      float parent_offset_theta_z = i.theta_z;
      
      for (auto &j : i.contained_meshes) {

	//////////////////////////////
	// calculate lightning
	//////////////////////////////

	//uniform locations (phong specific)
	int aff_by_light_loc = glGetUniformLocation(phong_shader.ID, "affected_by_light");
	int light_source_loc = glGetUniformLocation(phong_shader.ID, "light_source");
	int light_color_loc = glGetUniformLocation(phong_shader.ID, "light_color");
	int light_strength_loc = glGetUniformLocation(phong_shader.ID, "light_strength");
	int cameraPosLoc = glGetUniformLocation(phong_shader.ID, "cameraPos");
	int specular_strengthLoc = glGetUniformLocation(phong_shader.ID, "specular_strength");
	int disable_tex_shading_loc = glGetUniformLocation(phong_shader.ID, "disable_tex_shading");
	int face_color_loc = glGetUniformLocation(phong_shader.ID, "face_color");

	//uniform locations (pbr specific)
	int light_source_loc_pbr = glGetUniformLocation(pbr_shader.ID, "light_source");
	int cameraPosLoc_pbr = glGetUniformLocation(pbr_shader.ID, "cameraPos");
	int light_color_loc_pbr = glGetUniformLocation(pbr_shader.ID, "light_color");
	
	//write global uniforms
	glUniform3f(cameraPosLoc,cameraPos.x,cameraPos.y,cameraPos.z);
	glUniform3f(light_source_loc, light_pos.x,light_pos.y,light_pos.z);
	glUniform3f(light_color_loc, 25.0f, 25.0f, 25.0f);
	glUniform1f(light_strength_loc, 0.1f);
	glUniform4f(face_color_loc,
		    j.face_color_r/255.0f,
		    j.face_color_g/255.0f,
		    j.face_color_b/255.0f,
		    1.0f);
	//shading specific uniforms
        switch(j.shading_type) {

	case SHADING_PBR: {
	  pbr_shader.use();

	  glActiveTexture(j.mesh_albedo_texture_id);
	  glBindTexture(GL_TEXTURE_2D, j.mesh_albedo_texture_id);	  
          pbr_shader.setInt("albedoTexture", 0);

	  glActiveTexture(j.mesh_normal_texture_id);
	  glBindTexture(GL_TEXTURE_2D, j.mesh_normal_texture_id);	  
          pbr_shader.setInt("normalTexture", 1);

	  glBindTexture(GL_TEXTURE_2D, j.mesh_metallic_texture_id);	  
          pbr_shader.setInt("metallicTexture", 2);

	  glBindTexture(GL_TEXTURE_2D, j.mesh_roughness_texture_id);	  
          pbr_shader.setInt("roughnessTexture", 3);
	  
	  glBindTexture(GL_TEXTURE_2D, j.mesh_ambient_occlusion_texture_id);	 
          pbr_shader.setInt("aoTexture", 4);

          //write global uniforms
	  glUniform3f(cameraPosLoc_pbr,cameraPos.x,cameraPos.y,cameraPos.z);
	  glUniform3f(light_source_loc_pbr, light_pos.x,light_pos.y,light_pos.z);
	  glUniform3f(light_color_loc_pbr, 25.0f, 25.0f, 25.0f);
	  
          //bind mesh vao
          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }
	  
          break;
	}

	case SHADING_PHONG: {
	  phong_shader.use();
	  glUniform1f(disable_tex_shading_loc, 0.0f);
	  glUniform1f(specular_strengthLoc,j.specular_strength);
	  glUniform1i(aff_by_light_loc,1.0f);
	            // Upload textures + bind meshes VAO to state machine
          //	glActiveTexture(j.mes_tex_id);
          glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
          phong_shader.setInt("texture1", loaded_textures - 1);

          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }

          break;
	  
	  }

	case SHADING_FACE: {
	  phong_shader.use();
	  glUniform1f(disable_tex_shading_loc, 1.0f);
	  glUniform1f(specular_strengthLoc,j.specular_strength);
	  glUniform1i(aff_by_light_loc,1.0f);
	            // Upload textures + bind meshes VAO to state machine
          //	glActiveTexture(j.mes_tex_id);
          glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
          phong_shader.setInt("texture1", loaded_textures - 1);

          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }

          break;
	  
          }
	  
	case SHADING_FLAT: {
	  glUniform1f(disable_tex_shading_loc, 0.0f);
	  phong_shader.use();
	  glUniform1i(aff_by_light_loc,1.0f);
	            // Upload textures + bind meshes VAO to state machine
          //	glActiveTexture(j.mes_tex_id);
          glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
          phong_shader.setInt("texture1", loaded_textures - 1);

          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }

          break;
	  
	  }
	  
	case SHADING_NONE: {
	  //not affected by light
	  phong_shader.use();
	  glUniform1f(disable_tex_shading_loc, 0.0f);
          glUniform1i(aff_by_light_loc,0.0f);

          // Upload textures + bind meshes VAO to state machine
          //	glActiveTexture(j.mes_tex_id);
          glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
          phong_shader.setInt("texture1", loaded_textures - 1);

          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }

	  break;
	  
	}

        default: {

          // Upload textures + bind meshes VAO to state machine
          //	glActiveTexture(j.mes_tex_id);
          glBindTexture(GL_TEXTURE_2D, j.mes_tex_id);
          phong_shader.setInt("texture1", loaded_textures - 1);

          glBindVertexArray(j.mesh_VAO);
          if (glIsVertexArray(j.mesh_VAO) == GL_FALSE) {
            std::cout << "ERROR::VAO::INVALID_ID: " << j.mesh_VAO << std::endl;
          }
	  
        }
	  
        }
		
        //////////////////////////////
	// translation and rotation
	//////////////////////////////
	
        // model matrix
        glm::mat4 model = glm::mat4(1.0f);

	// setting mesh position transform
	glm::vec3 mod_transform(j.offset_pos_x + parent_mesh_pos_x,
				j.offset_pos_y + parent_mesh_pos_y,
				j.offset_pos_z + parent_mesh_pos_z);	
	model = glm::translate(model, mod_transform);

	// setting mesh rotation transform
	float rad_theta_x = glm::radians(j.offset_theta_x + parent_offset_theta_x);
	float rad_theta_y = glm::radians(j.offset_theta_y + parent_offset_theta_y);
	float rad_theta_z = glm::radians(j.offset_theta_z + parent_offset_theta_z);

	glm::mat4 mat_rot_x = glm::rotate(glm::mat4(1.0f), rad_theta_x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mat_rot_y = glm::rotate(glm::mat4(1.0f), rad_theta_y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mat_rot_z = glm::rotate(glm::mat4(1.0f), rad_theta_z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 mat_rot = mat_rot_x * mat_rot_y * mat_rot_z;

	model = model * mat_rot;
	
        // upload model matrix to shader
	int modelLoc = glGetUniformLocation(phong_shader.ID, "model");
        int viewLoc = glGetUniformLocation(phong_shader.ID, "view");
        int projectionLoc = glGetUniformLocation(phong_shader.ID, "projection");

	// mesh type specific transforms
        switch (j.mesh_type) {

        case MESH_SKYBOX: {
          glm::mat4 skybox = glm::mat4(1.0f);
          glm::vec3 cpy_cam_pos = cameraPos;
          skybox = glm::translate(skybox, cpy_cam_pos);
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(skybox));
          break;
        }

        case MESH_LIGHT: {
          j.offset_pos_x = light_pos.x;
          j.offset_pos_y = light_pos.y;
          j.offset_pos_z = light_pos.z;
          glm::vec3 direction_mesh_2d =
              cameraPos -
              glm::vec3(j.offset_pos_x, j.offset_pos_y, j.offset_pos_z);
          direction_mesh_2d = glm::normalize(direction_mesh_2d);

          float rad_theta_x_mesh_2d =
              atan2(direction_mesh_2d.x, direction_mesh_2d.z) + 3.14 / 2;
          float rad_theta_y_mesh_2d =
              atan2(direction_mesh_2d.y, sqrt(pow(direction_mesh_2d.x, 2) +
                                              pow(direction_mesh_2d.z, 2))) +
              3.14 / 2;

          rad_theta_y_mesh_2d = rad_theta_y_mesh_2d * -1;

          glm::mat4 mat_rot_x_mesh_2d =
              glm::rotate(glm::mat4(1.0f), rad_theta_x_mesh_2d,
                          glm::vec3(0.0f, 1.0f, 0.0f));
          glm::mat4 mat_rot_y_mesh_2d =
              glm::rotate(glm::mat4(1.0f), rad_theta_y_mesh_2d,
                          glm::vec3(0.0f, 0.0f, 1.0f));

          model = model * mat_rot_x_mesh_2d * mat_rot_y_mesh_2d;

          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

          break;
        }

        case MESH_SPRITE: {
          glm::vec3 direction_mesh_2d =
              cameraPos -
              glm::vec3(j.offset_pos_x, j.offset_pos_y, j.offset_pos_z);
          direction_mesh_2d = glm::normalize(direction_mesh_2d);

          float rad_theta_x_mesh_2d =
              atan2(direction_mesh_2d.x, direction_mesh_2d.z) + 3.14 / 2;
          float rad_theta_y_mesh_2d =
              atan2(direction_mesh_2d.y, sqrt(pow(direction_mesh_2d.x, 2) +
                                              pow(direction_mesh_2d.z, 2))) +
              3.14 / 2;

          rad_theta_y_mesh_2d = rad_theta_y_mesh_2d * -1;

          glm::mat4 mat_rot_x_mesh_2d =
              glm::rotate(glm::mat4(1.0f), rad_theta_x_mesh_2d,
                          glm::vec3(0.0f, 1.0f, 0.0f));
          glm::mat4 mat_rot_y_mesh_2d =
              glm::rotate(glm::mat4(1.0f), rad_theta_y_mesh_2d,
                          glm::vec3(0.0f, 0.0f, 1.0f));

          model = model * mat_rot_x_mesh_2d * mat_rot_y_mesh_2d;

          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

          break;
        }
	  
        default: {
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
          break;
        }
	  
        }

        // upload view matrix and projection matrix to shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

	//  #1 debug statement: std::cout << modelLoc << " model " << viewLoc << " view " << projectionLoc << " projection " << std::endl;	
        if (modelLoc == -1 || viewLoc == -1 || projectionLoc == -1) {
          std::cout << "ERROR::UNIFORM::LOCATION_NOT_FOUND - Loc" << std::endl;
        }

	///////////////////////////
	// RENDER EVERYTHING
	//////////////////////////
	
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
