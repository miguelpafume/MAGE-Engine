#pragma once

#include <limits>

#include "Window.hpp"
#include "GameObject.hpp"

namespace MAGE {

class KeyboardController {
public:

    struct KeyMapping {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_E;
        int moveDown = GLFW_KEY_Q;

        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;

        int exit = GLFW_KEY_ESCAPE;
    };

    void moveInPlaneXZ(GLFWwindow* window, float deltaTime, GameObject& gameObject);
    void otherKeys(GLFWwindow* window, float deltaTime, GameObject& gameObject);

    KeyMapping keys {};
    float moveSpeed {3.0f};
    float rotationSpeed {1.5f};

private:

};

} // namespace MAGE