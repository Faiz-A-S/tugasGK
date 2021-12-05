#pragma once
#include "RenderEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SOIL/SOIL.h>

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	GLuint depthmapShader, shadowmapShader, shaderProgram, VBO, VAO, EBO, texture, VBO2, VAO2, EBO2, texture2, VBO3, VAO3, EBO3, texture3;
	GLuint stexture, stexture2, stexture3, depthMapFBO, depthMap;
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED, fovy;
	float angle = 0, besar, samping, atasbawah, depanbelakang;
	float radius;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow *window);
	void BuildColoredCube();
	void BuildColoredPlane();
	void BuildDasar();
	void DrawColoredCube(GLuint shader);
	void DrawColoredPlane(GLuint shader);
	void DrawDasar(GLuint shader);
	void MoveCamera(float speed);
	void MoveCamera2(float speed);
	//void StrafeCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();
	void InitModel();
	void BuildDepthMap();
	void BuildShaders();
};

