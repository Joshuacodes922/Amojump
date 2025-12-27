#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>
enum class Actions {
	JUMP,
	MOVE_LEFT,
	MOVE_RIGHT,
	IDLE
};

static std::unordered_map<Actions, int> actionToKey = {
	{Actions::JUMP,GLFW_KEY_SPACE},
	{Actions::MOVE_LEFT,GLFW_KEY_A },
	{Actions::MOVE_RIGHT,GLFW_KEY_D, }
};

class InputManager {

	GLFWwindow* window;
	

public:
	InputManager(GLFWwindow* window) : window(window) {}
	bool checkAction(Actions action);

};