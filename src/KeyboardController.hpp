#pragma once

#include <limits>

#include "Window.hpp"
#include "GameObject.hpp"

#include "Chip8.hpp"

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

    struct Chip8KeyMapping {
        int key1 = GLFW_KEY_1;
        int key2 = GLFW_KEY_2;
        int key3 = GLFW_KEY_3;
        int keyC = GLFW_KEY_4;
        int key4 = GLFW_KEY_Q;
        int key5 = GLFW_KEY_W;
        int key6 = GLFW_KEY_E;
        int keyD = GLFW_KEY_R;
        int key7 = GLFW_KEY_A;
        int key8 = GLFW_KEY_S;
        int key9 = GLFW_KEY_D;
        int keyE = GLFW_KEY_F;
        int keyA = GLFW_KEY_Z;
        int key0 = GLFW_KEY_X;
        int keyB = GLFW_KEY_C;
        int keyF = GLFW_KEY_V;

        int exit = GLFW_KEY_ESCAPE;
    };

    void moveInPlaneXZ(GLFWwindow* window, float deltaTime, GameObject& gameObject);
    void otherKeys(GLFWwindow* window, float deltaTime, GameObject& gameObject);
    void updateChip8Input(GLFWwindow* window, Chip8& chip8);

    KeyMapping keys {};
    Chip8KeyMapping chip8Keys {};
    float moveSpeed {3.0f};
    float rotationSpeed {1.5f};

private:

};

} // namespace MAGE