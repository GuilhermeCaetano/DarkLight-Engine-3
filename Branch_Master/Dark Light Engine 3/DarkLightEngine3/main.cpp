// main.cpp
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#pragma warning(disable:4996)

#include <OpenGL\OpenGLSettings.h>
#include <VisualLeakDetector\vld.h>
#include <Engine\cECSEngine.h>


int main(int argc, char **argv)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GLFWwindow* window = SetupInitOpenGL();

	// Create engine
	cECSEngine* pECSEngine = new cECSEngine(window);

	/* Start engine
	 -Create ECS
	 -Loads external configurations
	 -Initialize shaders
	 -Pre allocate memory
	 -Loads assets
	*/
	pECSEngine->StartEngine();

	// Load last engine state
	pECSEngine->LoadLastEngineState("save.json");

	while (!glfwWindowShouldClose(window))
	{
		// Update engine
		pECSEngine->UpdateEngine(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Save current engine state
	//pECSEngine->SaveCurrentEngineState("save.json");

	// Terminate engine
	pECSEngine->StopEngine();

	// Garbage collection
	delete pECSEngine;
	pECSEngine = nullptr;

	// Check engine state
	system("pause");

	// Terminate the program and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	//_CrtDumpMemoryLeaks(); // Check for memory leaks
	exit(EXIT_SUCCESS);

}
