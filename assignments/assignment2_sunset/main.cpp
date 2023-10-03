#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <lm/shader.h> 

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

struct Vertex {
	float x, y, z;
	float u, v;
};

Vertex vertices[4] = {
	//x     y    z     u     v
   {-0.5, -0.5, 0.0, 0.0, 0.0}, //Bottom left
   { 0.5, -0.5, 0.0, 1.0, 0.0}, //Bottom right
   { 0.5,  0.5, 0.0, 1.0, 1.0}, //Top right
   {-0.5,  0.5, 0.0, 0.0, 1.0}  //Top left
};

unsigned int indices[6] = {
	 0, 1, 2, //Triangle 1
	 0, 2, 3  //Triangle 2
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
float speed = 1.0f;
bool showImGUIDemoWindow = true;
int windowWidth;
int windowHeight;

// Sky
float skyTopColor[3] = { 1.0, 1.0, 0.0 };
float skyBottomColor[3] = { 0.5, 1.0, 1.0 };

// Sun
float sunRadius = 0.1;
float sunGradientFactor = 0.05;
float sunPosition[2] = { 0.5, 0.75 };
float sunColor[3] = { 1.0, 1.0, 0.0 };

// Hills
float hillHeight = 0.3;
float hillPosition = 0.0;
float hillFrequency = 6.0;
float hillColor[3] = { 0.1, 0.1, 0.1 };

int main() {


	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

	unsigned int vao = createVAO(vertices, 4, indices, 6);

	lm::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		shader.setFloat("_Brightness", triangleBrightness);

		float time = (float)glfwGetTime();
		shader.setFloat("_Time", time);
		shader.setFloat("_Speed", speed);
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		shader.setVec2("_Resolution", (float)windowWidth, windowHeight);

		shader.setVec3("_SkyTopColor", skyTopColor[0], skyTopColor[1], skyTopColor[2]);
		shader.setVec3("_SkyBottomColor", skyBottomColor[0], skyBottomColor[1], skyBottomColor[2]);

		shader.setFloat("_SunRadius", sunRadius);
		shader.setFloat("_SunGradientFactor", sunGradientFactor);
		shader.setVec2("_SunPosition", sunPosition[0], sunPosition[1]);
		shader.setVec3("_SunColor", sunColor[0], sunColor[1], sunColor[2]);

		shader.setFloat("_HillHeight", hillHeight);
		shader.setFloat("_HillPosition", hillPosition);
		shader.setFloat("_HillFrequency", hillFrequency);
		shader.setVec3("_HillColor", hillColor[0], hillColor[1], hillColor[2]);

		//Wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//Shaded
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");

			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);

			ImGui::ColorEdit3("Color", triangleColor);
			ImGui::SliderFloat("Brightness", &triangleBrightness, 0.0f, 1.0f);

			ImGui::SliderFloat("Animation Speed", &sunRadius, 0.0f, 10.0f);

			ImGui::ColorEdit3("Sky Top Color", skyBottomColor);
			ImGui::ColorEdit3("Sky Bottom Color", skyTopColor);

			ImGui::SliderFloat("Sun Radius", &sunRadius, 0.0f, 1.0f);
			ImGui::SliderFloat("Sun Gradient", &sunGradientFactor, 0.0f, 1.0f);
			ImGui::SliderFloat2("Sun Positon", sunPosition, 0.0f, 1.0f);
			ImGui::ColorEdit3("Sun Color", sunColor);

			ImGui::SliderFloat("Hill Height", &hillHeight, 0.0f, 1.0f);
			ImGui::SliderFloat("Hill Position", &hillPosition, 0.0f, 1.0f);
			ImGui::SliderFloat("Hill Frequency", &hillFrequency, 0.0f, 100.0f);
			ImGui::ColorEdit3("Hill Color", hillColor);

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 5, vertexData, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

	//UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

