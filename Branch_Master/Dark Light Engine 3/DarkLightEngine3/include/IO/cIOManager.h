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

	void SetupIOManager(GLFWwindow* window);

	inline bool IsKeyDown(int key)
	{
		if (!this->_CheckWindow())
		{
			return false;
		}
		return glfwGetKey(this->_window, key);
	}

	inline bool IsKeyUp(int key)
	{
		if (!this->_CheckWindow())
		{
			return false;
		}
		return !glfwGetKey(this->_window, key);
	}

	
	void ProcessAsyncMouse();

	// Mouse
	float GetMouseX(void);
	float GetMouseY(void);

	float GetDeltaMouseX(void);
	float GetDeltaMouseY(void);
	float GetMouseWheel(void);

	bool IsMouseLeftButtonPressed();
	bool IsMouseInsideWindow();

	void UpdateEditorState();

	float mouseSensitivity;
	float mouseWheelSensitivity;

private:
	static EditorStates _editorState;

	bool _CheckWindow();

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

