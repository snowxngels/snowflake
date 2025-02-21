#include <assimp/material.h>
#include <glm/ext/vector_float3.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stdlib
#include <vector>
#include <strstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <random>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#define FOV_DEF 90.0f
#define POLY_WIREFRAME false
#define MESH_TO_LOAD "assets/Car.fbx"

#include "./shader/shader.h"
#include "stb_image.h"
#include "entity.h"
#include "scene.h"
#include "glad/glad.h"

//camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::vec3 direction = {0.0f,0.0f,0.0f};

// bungie employees hate this one simple trick
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// input shit
bool firstMouse = true;
double lastX = 0;
double lastY = 0;
double yaw = 0;
double pitch = 0;
