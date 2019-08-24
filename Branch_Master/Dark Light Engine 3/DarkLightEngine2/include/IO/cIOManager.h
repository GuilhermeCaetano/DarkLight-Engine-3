#ifndef _cIOManager_HG_
#define _cIOManager_HG_

#include <OpenGL\OpenGLHeaders.h>


class cIOManager
{
public:
	cIOManager();
	~cIOManager();

	enum EditorStates
	{
		Free,
		Camera,
		Light,
		Object,
		//Sound,
		Shader
	};
	static EditorStates GetEditorState();

	static void MouseCursorEnterCallback(GLFWwindow* window, int entered);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	bool IsKeyDown(int key)
	{
		return glfwGetKey(this->_window, key);
	}

	bool keyPDown;
	bool keyIDown;
	bool keyKDown;
	bool keyJDown;
	bool keyLDown;
	bool keyODown;
	bool keyUDown;

	bool key8Down;
	bool key2Down;
	bool key4Down;
	bool key6Down;
	bool key7Down;
	bool key9Down;

	void SetupIOManager(GLFWwindow* window);
	void ProcessAsyncMouse(GLFWwindow* window);

	// Mouse
	float GetMouseX(void);
	float GetMouseY(void);

	float GetDeltaMouseX(void);
	float GetDeltaMouseY(void);
	float GetMouseWheel(void);

	bool IsMouseLeftButtonPressed(GLFWwindow* window);
	bool IsMouseInsideWindow();

	void UpdateEditorState(GLFWwindow* window);

	float mouseSensitivity;
	float mouseWheelSensitivity;

private:
	static EditorStates _editorState;

	// These set methods will also clear the "last" mouse positions
	void _SetMouseXY(double newX, double newY);
	// The mouse wheel (on windows) only returns the delta, or the amount you scrolled
	void _SetMouseWheelDelta(double deltaWheel);

	static double _scrollY;
	static double _scrollX;

	bool _bIsChanging;

	GLFWwindow* _window;

	// Save last mous values
	float _lastMouseX;
	float _lastMouseY;
	float _lastMouseWheel;

	// Current mouse values
	float _mouseX;
	float _mouseY;
	float _mouseWheel;

	// We don't know where the mouse is at the start, 
	//	so things like getDeltaMouse() methods will return 
	//	crazy things... this flag indicates the values are OK
	bool _bMouseInitialStateIsSet;

	static bool _bMouseIsInsideWindow;
};


#endif // !_cIOManager_HG_

