#include "inputManager.hpp"


bool InputManager::checkAction(Actions action)
{
	int key = actionToKey.at(action);
	return glfwGetKey(window, key) == GLFW_PRESS;
}
