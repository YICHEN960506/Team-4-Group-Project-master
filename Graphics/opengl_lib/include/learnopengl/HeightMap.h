

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>
#include"mesh.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define RAW_WIDTH 257
#define RAW_HEIGHT 257
#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 0.5f
#define HEIGHTMAP_TEX_X 1.0f/ 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh {
public:
	HeightMap(std::string name);
	// void WaterMap(std::string name);
	~HeightMap(void) {};
};
