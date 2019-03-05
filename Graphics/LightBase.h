#ifndef LIGHTMAP_H
#define LIGHTMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include<string>
using namespace std;

class LightMap {
public:
	LightMap() {

	}


	//Directional Light
	void initDirectionalLight(unsigned int ID, Camera camera, const std::string &inputDirectionalLightName, const glm::vec3 & directionalLightDirectionValue, const glm::vec3 & directionalLightAmbientValue, const glm::vec3 & directionalLightDiffuseValue, const  glm::vec3 & directionalLightSpecularValue) {

		glUseProgram(ID);
		string viewPos = "viewPos";
		glUniform3fv(glGetUniformLocation(ID, (inputDirectionalLightName + ".direction").c_str()), 1, &directionalLightDirectionValue[0]);
		glUniform3fv(glGetUniformLocation(ID, viewPos.c_str()), 1, &camera.Position[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputDirectionalLightName + ".ambient").c_str()), 1, &directionalLightAmbientValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputDirectionalLightName + ".diffuse").c_str()), 1, &directionalLightDiffuseValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputDirectionalLightName + ".specular").c_str()), 1, &directionalLightSpecularValue[0]);


	}


	//Point Lights
	void initPointLight(unsigned int ID, Camera camera, const std::string &inputPointLightName, const glm::vec3 & pointLightPositionValue, const glm::vec3 & pointLightAmbientValue,
		const glm::vec3 & pointLightDiffuseValue, const  glm::vec3 & pointLightSpecularValue, float pointLightConstantValue, float pointLightLinearValue,
		float pointLightQuadraticValue) {

		glUseProgram(ID);
		string viewPos = "viewPos";
		glUniform3fv(glGetUniformLocation(ID, (inputPointLightName + ".position").c_str()), 1, &pointLightPositionValue[0]);
		glUniform3fv(glGetUniformLocation(ID, viewPos.c_str()), 1, &camera.Position[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputPointLightName + ".ambient").c_str()), 1, &pointLightAmbientValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputPointLightName + ".diffuse").c_str()), 1, &pointLightDiffuseValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputPointLightName + ".specular").c_str()), 1, &pointLightSpecularValue[0]);
		glUniform1f(glGetUniformLocation(ID, (inputPointLightName + ".constant").c_str()), pointLightConstantValue);
		glUniform1f(glGetUniformLocation(ID, (inputPointLightName + ".linear").c_str()), pointLightLinearValue);
		glUniform1f(glGetUniformLocation(ID, (inputPointLightName + ".quadratic").c_str()), pointLightQuadraticValue);



	}

	//Spot Light ---- Flashlight
	void initSpotLight(unsigned int ID, Camera camera, const std::string &inputSpotLightName, const glm::vec3 & spotLightPositionValue,
		const glm::vec3 & spotLightDirectionValue, float spotLightCutOff, float spotOuterLightCutOff, const glm::vec3 & spotLightAmbientValue,
		const glm::vec3 & spotLightDiffuseValue, const  glm::vec3 & spotLightSpecularValue, float spotLightConstantValue,
		float spotLightLinearValue, float spotLightQuadraticValue) {

		glUseProgram(ID);
		string viewPos = "viewPos";
		glUniform3fv(glGetUniformLocation(ID, (inputSpotLightName + ".position").c_str()), 1, &spotLightPositionValue[0]);
		glUniform3fv(glGetUniformLocation(ID, viewPos.c_str()), 1, &camera.Position[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputSpotLightName + ".direction").c_str()), 1, &spotLightDirectionValue[0]);
		glUniform1f(glGetUniformLocation(ID, (inputSpotLightName + ".cutOff").c_str()), spotLightCutOff);
		glUniform1f(glGetUniformLocation(ID, (inputSpotLightName + ".outerCutOff").c_str()), spotOuterLightCutOff);
		glUniform3fv(glGetUniformLocation(ID, (inputSpotLightName + ".ambient").c_str()), 1, &spotLightAmbientValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputSpotLightName + ".diffuse").c_str()), 1, &spotLightDiffuseValue[0]);
		glUniform3fv(glGetUniformLocation(ID, (inputSpotLightName + ".specular").c_str()), 1, &spotLightSpecularValue[0]);
		glUniform1f(glGetUniformLocation(ID, (inputSpotLightName + ".constant").c_str()), spotLightConstantValue);
		glUniform1f(glGetUniformLocation(ID, (inputSpotLightName + ".linear").c_str()), spotLightLinearValue);
		glUniform1f(glGetUniformLocation(ID, (inputSpotLightName + ".quadratic").c_str()), spotLightQuadraticValue);



	}


private:



};

//使用注意事项

/*
//Directional Light
调用时eg：
LightMap DirLight;
DirLight.initDirectionalLight(lightingShader.ID, camera, "light", glm::vec3(-0.2f, -1.0f, -0.3f),glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));

GLSL中：
1.init 函数中"light"对应"uniform Light light"
2.ID对应glUseProgram
3.调用时 对应设置object的Material ambient/diffuse/specular
//eg：循环之前
	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
	unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	// material properties
	lightingShader.setFloat("material.shininess", 32.0f);

	//循环时
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

	//ps：
	unsigned int loadTexture(char const * path){
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
		}
*/

/*

//Point Lights
调用时eg：
LightMap pointLight;
pointLight.initPointLight(lightingShader.ID, camera, "light", glm::vec3(1.2f, 1.0f, 2.0f),glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
其中：constant 一般为1.0，linear为了覆盖更远的距离通常都很小，quadratic更小，这三个分量用于计算衰减值
	 我们可以将环境光分量保持不变，让环境光照不会随着距离减少 (注释掉GLSL：ambient  *= attenuation;)

GLSL中：
1.init 函数中"light"对应"uniform Light light"
2.ID对应glUseProgram
3.调用时 对应设置object的Material ambient(可以忽略)/diffuse/specular
//eg：循环之前
	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
	unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	// material properties
	lightingShader.setFloat("material.shininess", 32.0f);

	//循环时
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

	//ps：
	unsigned int loadTexture(char const * path){
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
		}




*/




/*

//Spot Lights
调用时eg：
LightMap flashLight;
flashlight.initSpotLight(lightingShader.ID, camera, "light", camera.Position, camera.Front, glm::cos(glm::radians(12.5f)),glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);

其中：constant 一般为1.0，linear为了覆盖更远的距离通常都很小，quadratic更小，这三个分量用于计算衰减值
	 cutoff光照面积
	 设置边缘平滑渐变在GLSL中 float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

GLSL中：
1.init 函数中"light"对应"uniform Light light"
2.ID对应glUseProgram
3.调用时 对应设置object的Material ambient(可以忽略)/diffuse/specular
//eg：循环之前
	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
	unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	// material properties
	lightingShader.setFloat("material.shininess", 32.0f);

	//循环时
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

	//ps：
	unsigned int loadTexture(char const * path){
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
		}




*/






#endif