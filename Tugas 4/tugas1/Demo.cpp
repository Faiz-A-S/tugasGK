#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	BuildShaders();
	BuildDepthMap();
	BuildColoredCube();
	BuildColoredPlane();
	BuildDasar();
	InitCamera();
	InitModel();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);

	glDeleteVertexArrays(1, &VAO3);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &EBO3);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		MoveCamera2(CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (besar < 90) {
			besar += 0.001f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (besar > 0) {
			besar -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (samping < 90) {
			samping += 0.001f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (samping > -90) {
			samping -= 0.001f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (atasbawah < 90) {
			atasbawah += 0.001f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (atasbawah > 0) {
			atasbawah -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		if (depanbelakang < 90) {
			depanbelakang += 0.001f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (depanbelakang > -90) {
			depanbelakang -= 0.001f;
		}
	}
}

void Demo::Update(double deltaTime) {
	//angle += (float)((deltaTime * 1.5f) / 1000);
}

void Demo::Render() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Step 1 Render depth of scene to texture
	// ----------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 15.5f;
	lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
	lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	UseShader(this->depthmapShader);
	glUniformMatrix4fv(glGetUniformLocation(this->depthmapShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glViewport(0, 0, this->SHADOW_WIDTH, this->SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawColoredCube(this->depthmapShader);
	DrawColoredPlane(this->depthmapShader);
	DrawDasar(this->depthmapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Step 2 Render scene normally using generated depth map
	// ------------------------------------------------------
	

	glViewport(0, 0, this->screenWidth, this->screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Pass perspective projection matrix
	UseShader(this->shadowmapShader);
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shadowmapShader, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	//glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	//GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::vec3 cameraPos = glm::vec3(posCamX, posCamY, posCamZ);
	glm::vec3 cameraFront = glm::vec3(viewCamX, viewCamY, viewCamZ);
	glm::mat4 view = glm::lookAt(cameraPos, cameraFront, glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shadowmapShader, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// set lighting attributes
	glm::vec3 lightpos = glm::vec3(0, 15, -4);
	glm::vec3 lightdir = glm::vec3(-50, 100, -100);
	glUniformMatrix4fv(glGetUniformLocation(this->shadowmapShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform3f(glGetUniformLocation(this->shadowmapShader, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(this->shadowmapShader, "lightPos"), -2.0f, 4.0f, 5.0f);
	
	// Configure Shaders
	glUniform1i(glGetUniformLocation(this->shadowmapShader, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(this->shadowmapShader, "shadowMap"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawColoredCube(this->shadowmapShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawColoredPlane(this->shadowmapShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	DrawDasar(this->shadowmapShader);

	UseShader(this->shaderProgram);

	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram, "viewPos");
	//glUniform3f(viewPosLoc, glm::vec3(10, 10, 10).x, glm::vec3(10, 10, 10).y, glm::vec3(10, 10, 10).z);
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.direction"), 0.0f, 4.0f, -1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.ambient"), 0.1f, 0.1f, 0.25f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.diffuse"), 0.5f, 0.5f, 0.7f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.7f);
	// point light 1
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].position"), 0.0f, 1.0f, -2.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].ambient"), 0.0f, 0.0f, 0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].diffuse"), 0.0f, 0.0f, 0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].quadratic"), 0.032f);
	// point light 2
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].position"), 3.0f, 0.5f, -0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].ambient"), 0.8f, 0.8f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].diffuse"), 0.5f, 0.5f, 0.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].specular"), 0.5f, 0.5f, 0.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].quadratic"), 0.032f);
	// point light 3
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].position"), -3.0f, 0.5f, -0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].ambient"), 0.0f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].diffuse"), 0.0f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].specular"), 0.0f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].quadratic"), 0.032f);
	// point light 4
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].position"), 0.0f, 2.0f, 2.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].ambient"), 0.0f, 0.0f, 0.1f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].diffuse"), 0.1f, 0.1f, 0.1f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].linear"), 0.07f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].quadratic"), 0.017f);
	// spotLight
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLight.position"), 1, &lightpos[0]);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLight.direction"), 1, &lightdir[0]);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.ambient"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.linear"), 0.07f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(20.0f)));
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(25.0f)));

	DrawColoredCube(this->shaderProgram);
	DrawColoredPlane(this->shaderProgram);
	DrawDasar(this->shaderProgram);

	//glDisable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
}

void Demo::BuildColoredCube() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("body.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stexture);
	glBindTexture(GL_TEXTURE_2D, stexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("metallic_specular.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// front utama foto
		-1, -1.75,  0.45, 0, 0, 0.0f,  0.0f,  -1.0f,  // 0
		 1, -1.75,  0.45, 1, 0, 0.0f,  0.0f,  -1.0f,  // 1
		 1,  1.75, -0.05, 1, 1, 0.0f,  0.0f,  -1.0f,  // 2
		-1,  1.75, -0.05, 0, 1, 0.0f,  0.0f,  -1.0f,  // 3

		// //////////////////////////////////
		// left bingkai bingkai kanan
		1,  1.75,  0.055, 0, 0,  -1.0f,  0.0f,  0.0f,// 4
		1,  1.75, -0.05,  1, 0,  -1.0f,  0.0f,  0.0f,// 5
		1, -1.75,  0.45,  1, 1,  -1.0f,  0.0f,  0.0f,// 6
		1, -1.75,  0.55,  0, 1,  -1.0f,  0.0f,  0.0f,// 7

		// back bingkai bingkai kanan
		1.2,-1.75,  0.45, 0, 0,  0.0f,  0.0f,  -1.0f,// 8 
		1,  -1.75,  0.45, 1, 0,  0.0f,  0.0f,  -1.0f,// 9
		1,   1.75, -0.05, 1, 1,  0.0f,  0.0f,  -1.0f,// 10
		1.2, 1.75, -0.05, 0, 1,  0.0f,  0.0f,  -1.0f,// 11

		// right bingkai bingkai kanan
		1.2,  1.75, -0.05,  0, 0, 1.0f,  0.0f,  0.0f,// 12
		1.2,  1.75,  0.055, 1, 0, 1.0f,  0.0f,  0.0f,// 13
		1.2, -1.75,  0.55,  1, 1, 1.0f,  0.0f,  0.0f,// 14
		1.2, -1.75,  0.45,  0, 1, 1.0f,  0.0f,  0.0f,// 15

		// front bingkai bingkai kanan
		1.2, -1.75,  0.55,  0, 0, 0.0f,  0.0f,  1.0f, // 16
		1,   -1.75,  0.55,  1, 0, 0.0f,  0.0f,  1.0f, // 17
		1,    1.75,  0.055, 1, 1, 0.0f,  0.0f,  1.0f, // 18
		1.2,  1.75,  0.055, 0, 1, 0.0f,  0.0f,  1.0f, // 19

		//// //////////////////////////////////////
		// left bingkai bingkai kiri
		-1,  1.75,  0.055, 0, 0,  -1.0f,  0.0f,  0.0f,// 20
		-1,  1.75, -0.05,  1, 0,  -1.0f,  0.0f,  0.0f,// 21
		-1, -1.75,  0.45,  1, 1,  -1.0f,  0.0f,  0.0f,// 22
		-1, -1.75,  0.55,  0, 1,  -1.0f,  0.0f,  0.0f,// 23

		// back bingkai bingkai kiri
		-1.2,   -1.75, 0.45, 0, 0, 0.0f,  0.0f,  -1.0f,// 24
		-1,  -1.75, 0.45, 1, 0, 0.0f,  0.0f,  -1.0f,// 25
		-1,   1.75, -0.05, 1, 1, 0.0f,  0.0f,  -1.0f,// 26
		-1.2,  1.75, -0.05, 0, 1, 0.0f,  0.0f,  -1.0f,// 27

		// right bingkai bingkai kiri
		-1.2, 1.75, -0.05, 0, 0, 1.0f,  0.0f,  0.0f,// 28
		-1.2, 1.75,  0.055, 1, 0, 1.0f,  0.0f,  0.0f,// 29
		-1.2, -1.75,  0.55, 1, 1, 1.0f,  0.0f,  0.0f,// 30
		-1.2, -1.75, 0.45, 0, 1, 1.0f,  0.0f,  0.0f,// 31

		// front bingkai bingkai kiri
		-1.2, -1.75,  0.55, 0, 0, 0.0f,  0.0f,  1.0f,  // 32
		-1, -1.75,  0.55, 1, 0,   0.0f,  0.0f,  1.0f,// 33
		-1, 1.75, 0.055, 1, 1,    0.0f,  0.0f,  1.0f,// 34
		-1.2,  1.75, 0.055, 0, 1, 0.0f,  0.0f,  1.0f,   // 35

		// //////////////////////////////////////
		// top bingkai bingkai atas
		1.2,  2,  -0.05, 0, 0, 0.0f,  1.0f,  0.0f, // 36
		1.2,  2,  0.055, 1, 0, 0.0f,  1.0f,  0.0f, // 37
		-1.2, 2,  0.055, 1, 1, 0.0f,  1.0f,  0.0f, // 38
		-1.2, 2,  -0.05, 0, 1, 0.0f,  1.0f,  0.0f, // 39

		// bot bingkai bingkai atas
		1.2,   1.75, -0.05, 0, 0, 0.0f,  -1.0f,  0.0f,// 40
		1.2,  1.75,  0.055, 1, 0, 0.0f,  -1.0f,  0.0f,// 41
		-1.2,   1.75,  0.055, 1, 1, 0.0f,  -1.0f,  0.0f,// 42
		-1.2,  1.75,  -0.05, 0, 1, 0.0f,  -1.0f,  0.0f,// 43

		// front bingkai bingkai atas
		1.2, 1.75, 0.05, 0, 0, 0.0f,  0.0f,  1.0f, // 44
		1.2, 2,  0.055, 1, 0, 0.0f,  0.0f,  1.0f,// 45
		-1.2, 2,  0.05, 1, 1, 0.0f,  0.0f,  1.0f,// 46
		-1.2, 1.75, 0.055, 0, 1, 0.0f,  0.0f,  1.0f,// 47

		// back bingkai bingkai atas
		1.2, 1.75,  -0.05, 0, 0,   0.0f,  0.0f,  -1.0f,// 48
		1.2, 2,  -0.055, 1, 0,  0.0f,  0.0f,  -1.0f,// 49
		-1.2, 2, -0.05, 1, 1,  0.0f,  0.0f,  -1.0f,// 50
		-1.2,  1.75, -0.055, 0, 1,   0.0f,  0.0f,  -1.0f,// 51

		// l bingkai bingkai atas
		1.2, 2, -0.05, 0, 0, -1.0f,  0.0f,  0.0f,// 52
		1.2, 2,  0.055, 1, 0, -1.0f,  0.0f,  0.0f,// 53
		1.2, 1.75,  0.055, 1, 1, -1.0f,  0.0f,  0.0f,// 54
		1.2, 1.75, -0.05, 0, 1, -1.0f,  0.0f,  0.0f,// 55

		// r bingkai bingkai atas
		-1.2, 2, -0.05, 0, 0, 1.0f,  0.0f,  0.0f,// 56
		-1.2, 2,  0.055, 1, 0, 1.0f,  0.0f,  0.0f,// 57
		-1.2, 1.75,  0.055, 1, 1, 1.0f,  0.0f,  0.0f,// 58
		-1.2, 1.75, -0.05, 0, 1, 1.0f,  0.0f,  0.0f,// 59

		// //////////////////////////////////////
		// bot bingkai bingkai atas
		1.2,  -2,  0.45, 0, 0,  0.0f,  -1.0f,  0.0f,// 60
		1.2,  -2,  0.55, 1, 0,  0.0f,  -1.0f,  0.0f,// 61
		-1.2, -2,  0.55, 1, 1,  0.0f,  -1.0f,  0.0f,// 62
		-1.2, -2,  0.45, 0, 1,  0.0f,  -1.0f,  0.0f,// 63

		// top bingkai bingkai atas
		1.2,   -1.75, 0.5, 0, 0, 0.0f, 1.0f, 0.0f,// 64
		1.2,  -1.75,  0.55, 1, 0, 0.0f, 1.0f, 0.0f,// 65
		-1.2,   -1.75,  0.55, 1, 1, 0.0f, 1.0f, 0.0f,// 66
		-1.2,  -1.75,  0.5, 0, 1, 0.0f, 1.0f, 0.0f,// 67

		// front bingkai bingkai atas
		1.2, -1.75, 0.55, 0, 0, 0.0f, 0.0f, 1.0f,// 68
		1.2, -2,  0.55, 1, 0, 0.0f, 0.0f, 1.0f,// 69
		-1.2, -2,  0.55, 1, 1, 0.0f, 0.0f, 1.0f,// 70
		-1.2, -1.75, 0.55, 0, 1, 0.0f, 0.0f, 1.0f,// 71

		// back bingkai bingkai atas
		1.2, -1.75,  0.45, 0, 0, 0.0f, 0.0f, -1.0f,// 72
		1.2, -2,  0.45, 1, 0, 0.0f, 0.0f, -1.0f,// 73
		-1.2, -2, 0.45, 1, 1, 0.0f, 0.0f, -1.0f,// 74
		-1.2,  -1.75, 0.45, 0, 1, 0.0f, 0.0f, -1.0f, // 75

		// l bingkai bingkai atas
		1.2, -2, 0.45, 0, 0, -1.0f, 0.0f, 0.0f,// 76
		1.2, -2,  0.55, 1, 0, -1.0f, 0.0f, 0.0f,// 77
		1.2, -1.75,  0.55, 1, 1, -1.0f, 0.0f, 0.0f,// 78
		1.2, -1.75, 0.45, 0, 1, -1.0f, 0.0f, 0.0f,// 79

		// r bingkai bingkai atas
		-1.2, -2, 0.45, 0, 0, 1.0f, 0.0f, 0.0f,// 80
		-1.2, -2,  0.55, 1, 0, 1.0f, 0.0f, 0.0f,// 81
		-1.2, -1.75,  0.55, 1, 1, 1.0f, 0.0f, 0.0f,// 82
		-1.2, -1.75, 0.45, 0, 1, 1.0f, 0.0f, 0.0f,// 83

		////////////////////////////////////////
		//kaki?
		-0.6, -1.80, -1, 0, 0, 0.0f, 0.0f, -1.0f,// 84
		0.6, -1.80, -1, 1, 0, 0.0f, 0.0f, -1.0f,// 85
		0.5, 0.80, 0.1, 1, 1, 0.0f, 0.0f, -1.0f,// 86
		-0.5, 0.80, 0.1, 0, 1, 0.0f, 0.0f, -1.0f,// 87
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		//bing kanan
		4,  5,  6,  4,  6,  7,   // l
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // r
		16, 18, 17, 16, 19, 18,  // f
		//bing kiri
		20, 22, 21, 20, 23, 22,   // l
		24, 26, 25, 24, 27, 26,   // back
		28, 30, 29, 28, 31, 30,   // r
		32, 34, 33, 32, 35, 34,    // f
		//bing atas
		36, 38, 37, 36, 39, 38,   //t
		40, 42, 41, 40, 43, 42,   //b
		44, 46, 45, 44, 47, 46,
		48, 50, 49, 48, 51, 50,
		52, 54, 53, 52, 55, 54,
		56, 58, 57, 56, 59, 58,
		//bing bawah
		60, 62, 61, 60, 63, 62,
		64, 66, 65, 64, 67, 66,
		68, 70, 69, 68, 71, 70,
		72, 74, 73, 72, 75, 74,
		76, 78, 77, 76, 79, 78,
		80, 82, 81, 80, 83, 82,
		84, 86, 85, 84, 87, 86		//22
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawColoredCube(GLuint shader)
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stexture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 100.0f);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	UseShader(shader);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(samping, atasbawah, depanbelakang));
	model = glm::rotate(model, angle, glm::vec3(0, -1, 0)); //0,-1,0
	model = glm::scale(model, glm::vec3(besar, besar, besar));
	
	GLint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 132, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildColoredPlane()
{
	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("snow.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stexture2);
	glBindTexture(GL_TEXTURE_2D, stexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("snow_specular.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// foto utama
		-1, -1.75, 0.5, 0, 0,  0.0f,  0.0f,  1.0f,// 0
		 1, -1.75, 0.5, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		 1,  1.75, 0,   1, 1,  0.0f,  0.0f,  1.0f, // 2
		-1,  1.75, 0,   0, 1,  0.0f,  0.0f,  1.0f,// 3


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}


void Demo::DrawColoredPlane(GLuint shader)
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, stexture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 3);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 100.0f);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	
	UseShader(shader);
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(samping, atasbawah, depanbelakang));
	model = glm::rotate(model, angle, glm::vec3(0, -1, 0));//0,-1,0
	model = glm::scale(model, glm::vec3(besar, besar, besar));

	GLint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildDasar()
{
	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("floor.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stexture3);
	glBindTexture(GL_TEXTURE_2D, stexture3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("floor2_specular.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -2.1, -50.0,  0,  0, 0.0f,  1.0f,  0.0f,
		 50.0, -2.1, -50.0, 50,  0, 0.0f,  1.0f,  0.0f,
		 50.0, -2.1,  50.0, 50, 50, 0.0f,  1.0f,  0.0f,
		-50.0, -2.1,  50.0,  0, 50, 0.0f,  1.0f,  0.0f,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}



void Demo::DrawDasar(GLuint shader)
{
	UseShader(this->shaderProgram);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, stexture3);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 5);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 100.0f);

	glBindVertexArray(VAO3); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	UseShader(shader);

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	radius = 10;
	fovy = 45.0f;

	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void Demo::InitModel() {
	besar = 1;
	samping = 0;
	atasbawah = 0;
	depanbelakang = 0;
}

void Demo::MoveCamera(float speed)
{
	float x = sin(glfwGetTime()) * radius;
	//float posCamY = cos(glfwGetTime()) * radius;
	float z = cos(glfwGetTime()) * radius;
	// 
	posCamX = x;
	posCamZ = z;

}

void Demo::MoveCamera2(float speed)
{
	float x = cos(glfwGetTime()) * radius;
	//float posCamY = cos(glfwGetTime()) * radius;
	float z = sin(glfwGetTime()) * radius;
	// 
	posCamX = x;
	posCamZ = z;
}
//
//void Demo::RotateCamera(float speed)
//{
//	float x = viewCamX - posCamX;
//	float z = viewCamZ - posCamZ;
//	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
//	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
//}

void Demo::BuildDepthMap() {
	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->SHADOW_WIDTH, this->SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Demo::BuildShaders()
{
	// build and compile our shader program
	// ------------------------------------
	shadowmapShader = BuildShader("shadowMapping.vert", "shadowMapping.frag", nullptr);
	depthmapShader = BuildShader("depthMap.vert", "depthMap.frag", nullptr);
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

}

int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Bingkai Foto", 800, 600, false, false);
}