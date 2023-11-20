#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Vec3 position = ew::Vec3(0.0f, 2.0f, 0.0f); //World space
	ew::Vec3 color = ew::Vec3(1.0f, 0.0f, 0.0f); //RGB
	int enable = 1;
};

void resetLight(Light lights[], int light);

struct Material {
	float ambientK = 0.1; //Ambient coefficient (0-1)
	float diffuseK = 0.5; //Diffuse coefficient (0-1)
	float specularK = 0.5; //Specular coefficient (0-1)
	float shininess = 10.0; //Shininess
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader light_Shader("assets/unlit.vert", "assets/unlit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
	ew::Mesh lightMesh(ew::createSphere(0.1f, 64));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	ew::Transform lightTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);
	
	Light lights[4];
	bool enableLight_1 = true;
	bool enableLight_2 = true;
	bool enableLight_3 = true;
	bool enableLight_4 = true;
	ew::Vec3 aColor = ew::Vec3(0.5, 0.5, 0.5);	// ambient light color
	int numLights = 4;
	bool slider = true;

	Material mat;

	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
			case 0:
				lights[i].position = ew::Vec3(-2.0f, 1.0f, -2.0f);
				lights[i].color = ew::Vec3(1.0f, 0.0f, 0.0f);
				lights[i].enable = 1;
				break;
			case 1:
				lights[i].position = ew::Vec3(2.0f, 1.0f, -2.0f);
				lights[i].color = ew::Vec3(0.0f, 1.0f, 0.0f);
				lights[i].enable = 1;
				break;
			case 2:
				lights[i].position = ew::Vec3(2.0f, 1.0f, 2.0f);
				lights[i].color = ew::Vec3(0.0f, 0.0f, 1.0f);
				lights[i].enable = 1;
				break;
			case 3:
				lights[i].position = ew::Vec3(-2.0f, 1.0f, 2.0f);
				lights[i].color = ew::Vec3(0.5f, 0.5f, 0.0f);
				lights[i].enable = 1;
				break;
		}
	}

	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		if (slider)
		{
			for (int i = 1; i < 5; i++)
			{
				if (i <= numLights)
				{
					lights[i - 1].enable = 1;
				}
				else
				{
					lights[i - 1].enable = 0;
				}
			}
		}
		else
		{
			lights[0].enable = enableLight_1;
			lights[1].enable = enableLight_2;
			lights[2].enable = enableLight_3;
			lights[3].enable = enableLight_4;
		}


		for(int i = 0; i < 4; i++)
		{
			shader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
			shader.setInt("_Lights[" + std::to_string(i) + "].enable", lights[i].enable);
		}
		shader.setVec3("_ViewPos", camera.position);
		shader.setVec3("_ambientColor", aColor);
		shader.setFloat("_ambientK", mat.ambientK);
		shader.setFloat("_diffuseK", mat.diffuseK);
		shader.setFloat("_specularK", mat.specularK);
		shader.setFloat("_shininess", mat.shininess);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//TODO: Render point lights

		light_Shader.use();
		light_Shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < 4; i++)
		{
			if (lights[i].enable)
			{
				lightTransform.position = lights[i].position;
				light_Shader.setMat4("_Model", lightTransform.getModelMatrix());
				light_Shader.setVec3("_Color", lights[i].color);
				lightMesh.draw();
			}
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}
			if (ImGui::CollapsingHeader("Lights"))
			{
				if (ImGui::CollapsingHeader("Light 1"))
				{
					if (!slider) ImGui::Checkbox("Enable Light##1", &enableLight_1);
					ImGui::DragFloat3("Position##1", &lights[0].position.x, 0.1f);
					ImGui::DragFloat3("Color##1", &lights[0].color.x, 0.01f, 0.0, 1.0);
					if (ImGui::Button("Reset##1")) {
						resetLight(lights, 0);
						enableLight_1 = true;
					}
				}
				if (ImGui::CollapsingHeader("Light 2"))
				{
					if (!slider) ImGui::Checkbox("Enable Light##2", &enableLight_2);
					ImGui::DragFloat3("Position##2", &lights[1].position.x, 0.1f);
					ImGui::DragFloat3("Color##2", &lights[1].color.x, 0.01f, 0.0, 1.0);
					if (ImGui::Button("Reset##2")) {
						resetLight(lights, 1);
						enableLight_2 = true;
					}
				}
				if (ImGui::CollapsingHeader("Light 3"))
				{
					if (!slider) ImGui::Checkbox("Enable Light##3", &enableLight_3);
					ImGui::DragFloat3("Position##3", &lights[2].position.x, 0.1f);
					ImGui::DragFloat3("Color##3", &lights[2].color.x, 0.01f, 0.0, 1.0);
					if (ImGui::Button("Reset##3")) {
						resetLight(lights, 2);
						enableLight_3 = true;
					}
				}
				if (ImGui::CollapsingHeader("Light 4"))
				{
					if (!slider) ImGui::Checkbox("Enable Light##4", &enableLight_4);
					ImGui::DragFloat3("Position##4", &lights[3].position.x, 0.1f);
					ImGui::DragFloat3("Color##4", &lights[3].color.x, 0.01f, 0.0, 1.0);
					if (ImGui::Button("Reset##4")) {
						resetLight(lights, 3);
						enableLight_4 = true;
					}
				}

				ImGui::DragFloat3("Ambient Color", &aColor.x, 0.05, 0, 1);
				ImGui::DragFloat("AmbientK", &mat.ambientK, 0.01, 0.0, 10);
				ImGui::DragFloat("DiffuseK", &mat.diffuseK, 0.01, 0, 10);
				ImGui::DragFloat("SpecularK", &mat.specularK, 0.01, 0, 10);
				ImGui::DragFloat("Shininess", &mat.shininess, 0.05, 0, 100);
				ImGui::Text("Disable slider to toggle specific lights");
				ImGui::Checkbox("Enable Slider", &slider);
				if (slider) ImGui::SliderInt("Number of Lights", &numLights, 0, 4);
				if (ImGui::Button("Reset Lights")) {
					for (int i = 0; i < 4; i++)
					{
						resetLight(lights, i);
						enableLight_1, enableLight_2, enableLight_3, enableLight_4 = true;
					}

					aColor = ew::Vec3(0.5, 0.5, 0.5);
					mat.ambientK = 0.1;
					mat.diffuseK = 0.5;
					mat.specularK = 0.5;
					mat.shininess = 10.0;
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}

void resetLight(Light lights[], int light)
{
	switch (light)
	{
	case 0:
		lights[0].position = ew::Vec3(-2.0f, 1.0f, -2.0f);
		lights[0].color = ew::Vec3(1.0f, 0.0f, 0.0f);
		break;
	case 1:
		lights[1].position = ew::Vec3(2.0f, 1.0f, -2.0f);
		lights[1].color = ew::Vec3(0.0f, 1.0f, 0.0f);
		break;
	case 2:
		lights[2].position = ew::Vec3(2.0f, 1.0f, 2.0f);
		lights[2].color = ew::Vec3(0.0f, 0.0f, 1.0f);
		break;
	case 3:
		lights[3].position = ew::Vec3(-2.0f, 1.0f, 2.0f);
		lights[3].color = ew::Vec3(0.5f, 0.5f, 0.0f);
		break;
	}
}
