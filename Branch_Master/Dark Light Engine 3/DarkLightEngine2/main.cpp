// main.cpp
#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#pragma warning(disable:4996)

#include <OpenGL\OpenGLSettings.h>
#include <vld.h>
#include <Engine\cECSEngine.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <iostream>



template<typename TimeT = std::chrono::milliseconds>
struct measure
{
	template<typename F, typename ...Args>
	static typename TimeT::rep execution(F func, Args&&... args)
	{
		auto start = std::chrono::high_resolution_clock::now();
		func(std::forward<Args>(args)...);
		auto duration = std::chrono::duration_cast<TimeT>
			(std::chrono::high_resolution_clock::now() - start);
		return duration.count();
	}
};

int total = 0;

//void test()
//{
//	int foo = 0;
//	for (int i = 0; i < 1000; ++i) ++foo;
//	total += foo;
//}

using namespace std::chrono;

glm::vec3 Orthogonal(glm::vec3 vec)
{
	glm::vec2 vec2 = vec;
	vec2 = glm::normalize(vec);

	return 
}

int main(int argc, char **argv)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GLFWwindow* window = SetupInitOpenGL();

	// Create engine
	//cECSEngine* pECSEngine = new cECSEngine();

	/* Start engine
	 -Create ECS
	 -Loads external configurations
	 -Initialize shaders
	 -Pre allocate memory
	 -Loads assets
	*/
	//pECSEngine->StartEngine();

	//// Load last engine state
	//pECSEngine->LoadLastEngineState("save.json");

	

	for (int i = 0; i < 30; ++i)
	{
		total = 0;
		auto t = measure<std::chrono::nanoseconds>::execution(test);
		std::cout << "Calculated total = " << total << " in " << t << " ns." << std::endl;
	}

	

	while (!glfwWindowShouldClose(window))
	{
		// Update engine
		//pECSEngine->UpdateEngine();
		//printf("waiting...");

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Save current engine state
	//pECSEngine->SaveCurrentEngineState("save.json");

	// Terminate engine
	//pECSEngine->StopEngine();

	//// Garbage collection
	//delete pECSEngine;
	//pECSEngine = nullptr;

	// Terminate the program and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	//_CrtDumpMemoryLeaks(); // Check for memory leaks
	exit(EXIT_SUCCESS);

}
