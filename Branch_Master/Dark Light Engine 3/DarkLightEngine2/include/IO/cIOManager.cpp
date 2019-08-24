// cIOManager

#include <IO\cIOManager.h>
#include <stdio.h>
#include <SimplePhysics\Physics\cSimplePhysicsSystem.h>


double cIOManager::_scrollX = 0.0;
double cIOManager::_scrollY = 0.0;
bool cIOManager::_bMouseIsInsideWindow = false;
cIOManager::EditorStates cIOManager::_editorState = cIOManager::Free;

cIOManager::cIOManager()
{
	// Set mouse
	printf("EditorState: Free\n");
	this->_mouseWheel = 2.0f;
	this->mouseSensitivity = 0.1f;
	this->mouseWheelSensitivity = 0.1f;
	this->_bMouseInitialStateIsSet = false;
	this->_SetMouseXY(0.0, 0.0);

	this->keyPDown = false;
	this->keyIDown = false;
	this->keyKDown = false;
	this->keyJDown = false;
	this->keyLDown = false;
	this->keyODown = false;
	this->keyUDown = false;

	this->key8Down = false;
	this->key2Down = false;
	this->key4Down = false;
	this->key6Down = false;
	this->key7Down = false;
	this->key9Down = false;

	this->_bIsChanging = false;

	return;
}

cIOManager::~cIOManager()
{
	return;
}

void cIOManager::SetupIOManager(GLFWwindow* window)
{
	this->_window = window;
	glfwSetCursorEnterCallback(window, MouseCursorEnterCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
}

void cIOManager::MouseCursorEnterCallback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		_bMouseIsInsideWindow = true;
		//printf("Mouse moved indide window\n");
	}
	else
	{
		_bMouseIsInsideWindow = false;
		//printf("Mouse moved outside window\n");
	}
	return;
}

void cIOManager::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// A regular mouse wheel returns the y value
	_scrollX = xoffset;
	_scrollY = yoffset;
}

void cIOManager::_SetMouseXY(double newX, double newY)
{
	// Update last mouse positions and add to new ones
	this->_lastMouseX = this->_mouseX;
	this->_mouseX = (float)newX;
	this->_lastMouseY = this->_mouseY;
	this->_mouseY = (float)newY;

	// Mouse set
	this->_bMouseInitialStateIsSet = true;
}

void cIOManager::_SetMouseWheelDelta(double deltaWheel)
{
	// Update last mouse wheel and add to new one
	this->_lastMouseWheel = this->_mouseWheel;
	this->_mouseWheel += (float)deltaWheel;
	if (this->_mouseWheel < 0.0f) this->_mouseWheel = 0.0f;
	if (this->_mouseWheel > 200.0f) this->_mouseWheel = 200.0f;
}

float cIOManager::GetMouseX()
{
	return this->_mouseX;
}

float cIOManager::GetMouseY()
{
	return this->_mouseY;
}

float cIOManager::GetDeltaMouseX()
{
	if (!this->_bMouseInitialStateIsSet)
	{
		return 0.0f;
	}
	return this->_mouseX - this->_lastMouseX;
}

float cIOManager::GetDeltaMouseY()
{
	if (!this->_bMouseInitialStateIsSet)
	{
		return 0.0f;
	}
	return this->_mouseY - this->_lastMouseY;
}

float cIOManager::GetMouseWheel()
{
	return this->_mouseWheel;
}

bool cIOManager::IsMouseLeftButtonPressed(GLFWwindow* window)
{
	// Mouse left button while inside screen
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && _bMouseIsInsideWindow)
	{
		// Mouse button is down
		return true;
	}

	return false;
}

bool cIOManager::IsMouseInsideWindow()
{
	return _bMouseIsInsideWindow;
}

void cIOManager::ProcessAsyncMouse(GLFWwindow* window)
{
	// Get current mouse cursor position
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	
	this->_SetMouseXY(x, y);
	this->_SetMouseWheelDelta(_scrollY);
}

cIOManager::EditorStates cIOManager::GetEditorState()
{
	return _editorState;
}

void cIOManager::UpdateEditorState(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
	{
		if (glfwGetKey(window, GLFW_KEY_TAB) && !this->_bIsChanging)
		{
			this->_bIsChanging = true;
			switch (this->_editorState)
			{
			case EditorStates::Free:

				printf("EditorState: Camera\n");
				this->_editorState = EditorStates::Light;

			break;
			/*case EditorStates::Camera:

				printf("EditorState: Light\n");
				this->_editorState = EditorStates::Light;

				break;*/
			case EditorStates::Light:

				printf("EditorState: Object\n");
				this->_editorState = EditorStates::Object;

				break;
			case EditorStates::Object:

				printf("EditorState: Shader\n");
				this->_editorState = EditorStates::Shader;

				break;
			case EditorStates::Shader:

				printf("EditorState: Free\n");
				this->_editorState = EditorStates::Free;

				break;
			}
		}
		else if(!glfwGetKey(window, GLFW_KEY_TAB))
		{
			this->_bIsChanging = false;
		}
	}

	//HACK: dlls hate functions from outside of it, there's no good way of getting glm key presses
	if (glfwGetKey(window, GLFW_KEY_P)) { this->keyPDown = true; }
	else { this->keyPDown = false; }
	if (glfwGetKey(window, GLFW_KEY_I)) { this->keyIDown = true; }
	else { this->keyIDown = false; }
	if (glfwGetKey(window, GLFW_KEY_K)) { this->keyKDown = true; }
	else { this->keyKDown = false; }
	if (glfwGetKey(window, GLFW_KEY_J)) { this->keyJDown = true; }
	else { this->keyJDown = false; }
	if (glfwGetKey(window, GLFW_KEY_L)) { this->keyLDown = true; }
	else { this->keyLDown = false; }
	if (glfwGetKey(window, GLFW_KEY_O)) { this->keyODown = true; }
	else { this->keyODown = false; }
	if (glfwGetKey(window, GLFW_KEY_U)) { this->keyUDown = true; }
	else { this->keyUDown = false; }

	if (glfwGetKey(window, GLFW_KEY_KP_8)) { this->key8Down = true; }
	else { this->key8Down = false; }
	if (glfwGetKey(window, GLFW_KEY_KP_2)) { this->key2Down = true; }
	else { this->key2Down = false; }
	if (glfwGetKey(window, GLFW_KEY_KP_4)) { this->key4Down = true; }
	else { this->key4Down = false; }
	if (glfwGetKey(window, GLFW_KEY_KP_6)) { this->key6Down = true; }
	else { this->key6Down = false; }
	if (glfwGetKey(window, GLFW_KEY_KP_7)) { this->key7Down = true; }
	else { this->key7Down = false; }
	if (glfwGetKey(window, GLFW_KEY_KP_9)) { this->key9Down = true; }
	else { this->key9Down = false; }
}
