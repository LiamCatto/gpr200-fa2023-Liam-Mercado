#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>

#include <lm/camera.h>
#include <iostream>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, lm::Camera* camera, lm::CameraControls* controls, float deltaTime);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	lm::Camera camera;
	lm::CameraControls cameraControls;

	// Camera properties

	camera.orbit = false;
	camera.orbitSpeed = 1.0;
	camera.position = ew::Vec3(0.0, 0.0, 5.0);
	camera.target = ew::Vec3(0.0, 0.0, 0.0);
	camera.fov = 60.0;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.nearPlane = 0.1;
	camera.farPlane = 100.0;
	camera.orthographic = true;
	camera.orthoSize = 6.0;

	float prevTime = 0; //Timestamp of previous frame
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Calculate deltaTime
		float time = (float)glfwGetTime(); //Timestamp of current frame
		float deltaTime = time - prevTime;
		prevTime = time;

		moveCamera(window, &camera, &cameraControls, deltaTime);

		//Set uniforms
		shader.use();

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{  
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			shader.setMat4("_View", camera.ViewMatrix());
			shader.setMat4("_Projection", camera.ProjectionMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				if (ImGui::Button("Reset"))
				{
					switch (i)
					{
					case 0:
						cubeTransforms[0].position = ew::Vec3(-0.5f, -0.5f, 0.0f);
						cubeTransforms[0].rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
						cubeTransforms[0].scale = ew::Vec3(1.0f, 1.0f, 1.0f);
						break;
					case 1:
						cubeTransforms[1].position = ew::Vec3(0.5f, -0.5f, 0.0f);
						cubeTransforms[1].rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
						cubeTransforms[1].scale = ew::Vec3(1.0f, 1.0f, 1.0f);
						break;
					case 2:
						cubeTransforms[2].position = ew::Vec3(-0.5f, 0.5f, 0.0f);
						cubeTransforms[2].rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
						cubeTransforms[2].scale = ew::Vec3(1.0f, 1.0f, 1.0f);
						break;
					case 3:
						cubeTransforms[3].position = ew::Vec3(0.5f, 0.5f, 0.0f);
						cubeTransforms[3].rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
						cubeTransforms[3].scale = ew::Vec3(1.0f, 1.0f, 1.0f);
						break;
					}
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");
			ImGui::Checkbox("Orbit", &camera.orbit);
			ImGui::DragFloat("Orbit Speed", &camera.orbitSpeed, 0.5f);
			ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
			ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
			ImGui::DragFloat("FOV", &camera.fov, 0.05f);
			ImGui::Checkbox("Orthographic", &camera.orthographic);
			ImGui::DragFloat("Orthographic Height", &camera.orthoSize, 0.05f);
			ImGui::DragFloat("Near", &camera.nearPlane, 0.05f);
			ImGui::DragFloat("Far", &camera.farPlane, 0.05f);
			if (ImGui::Button("Reset"))
			{
				camera.orbit = false;
				camera.orbitSpeed = 1.0;
				camera.position = ew::Vec3(0.0f, 0.0f, 5.0f);
				camera.target = ew::Vec3(0.0f, 0.0f, 0.0f);
				camera.fov = 60.0f;
				camera.orthographic = true;
				camera.orthoSize = 6.0f;
				camera.nearPlane = 0.1f;
				camera.farPlane = 100.0f;
			}
			ImGui::Text("Debug"); // debug
			ImGui::DragFloat3("Right", &cameraControls.right.x, 0.05f); // debug
			ImGui::DragFloat3("Up", &cameraControls.up.x, 0.05f); // debug
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
}

void moveCamera(GLFWwindow* window, lm::Camera* camera, lm::CameraControls* controls, float deltaTime)
{
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	//TODO: Get mouse position delta for this frame
	double mouseDeltaX = controls->prevMouseX - mouseX;
	double mouseDeltaY = controls->prevMouseY - mouseY;

	//TODO: Add to yaw and pitch
	controls->yaw -= mouseDeltaX * controls->mouseSensitivity;
	controls->pitch += mouseDeltaY * controls->mouseSensitivity;
	
	//TODO: Clamp pitch between -89 and 89 degrees
	if (controls->pitch > 89)
	{
		controls->pitch = 89;
	}

	if (controls->pitch < -89)
	{
		controls->pitch = -89;
	}

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	//Construct forward vector using yaw and pitch. Don't forget to convert to radians!
	float yawRad = controls->yaw * ew::DEG2RAD;
	float pitchRad = controls->pitch * ew::DEG2RAD;
	ew::Vec3 forward = ew::Vec3(		// At yawRad = 0 and pitchRad = 0, (0, 0, -1)
		sin(yawRad) * cos(pitchRad),
		sin(pitchRad),
		-cos(yawRad) * cos(pitchRad)
	);
	//TODO: Using camera forward and world up (0,1,0), construct camera right and up vectors. Graham-schmidt process!
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0.0, 1.0, 0.0)));
	//right = ew::Vec3(1, 0, 0); // debug
	controls->right = right; // debug
	ew::Vec3 up = ew::Normalize(ew::Cross(right, forward));
	//up = ew::Vec3(0, 1, 0); // debug
	controls->up = up; // debug

	//TODO: Keyboard controls for moving along forward, back, right, left, up, and down. See Requirements for key mappings.
	//EXAMPLE: Moving along forward axis if W is held.
	//Note that this is framerate dependent, and will be very fast until you scale by deltaTime. See the next section.
	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->position += forward * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->position -= forward * controls->moveSpeed * deltaTime;
	}
	
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->position -= right * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->position += right * controls->moveSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->position -= up * controls->moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->position += up * controls->moveSpeed * deltaTime;
	}

	//By setting target to a point in front of the camera along its forward direction, our LookAt will be updated accordingly when rendering.
	//Setting camera.target should be done after changing position. Otherwise, it will use camera.position from the previous frame and lag behind
	camera->target = camera->position + forward;
}