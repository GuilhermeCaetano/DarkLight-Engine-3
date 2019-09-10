// OpenGLSettings.cpp
#include <stdio.h>
#include <OpenGL\OpenGLSettings.h>

void Error_Callback(int error, const char* description)
{
	// Tell us the error that happened
	printf("Error: %s\n", description);
}

void ExitKey_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Set exiting the window to ESC or F4
	if ((key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) || (key == GLFW_KEY_F4) && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void SetError_Callback()
{
	glfwSetErrorCallback(Error_Callback);
}

void SetKeyCallbacks()
{

}

GLFWwindow* SetupInitOpenGL()
{
	// Initialize glfw
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	// Create window
	GLFWwindow* window;
	window = glfwCreateWindow(1280, 720, "DarkLight Engine V3", NULL, NULL);
	if (!window)
	{
		printf("ERROR: Could not open window with glf3w");
		glfwTerminate();
	}
	
	SetError_Callback();
	glfwSetKeyCallback(window, ExitKey_Callback); // Set how window closes
	glfwMakeContextCurrent(window); // Set current window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // Set API version length
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // Load Glad
	glfwSwapInterval(1); // Number of screen updates

	return window;
}