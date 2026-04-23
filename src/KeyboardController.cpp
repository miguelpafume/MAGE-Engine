#include "KeyboardController.hpp"
namespace MAGE {

void KeyboardController::moveInPlaneXZ(GLFWwindow* window, float deltaTime, GameObject& gameObject) {
    glm::vec3& objRotation = gameObject.m_transform.rotation;
    glm::vec3& objTranslation = gameObject.m_transform.translation;
    
    glm::vec3 rotate {0.0f};
    
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;
    
    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        objRotation += rotationSpeed * deltaTime * glm::normalize(rotate);
    }
    
    objRotation.x = glm::clamp(objRotation.x, -glm::radians<float>(85), glm::radians<float>(85));
    objRotation.y = glm::mod(objRotation.y, glm::two_pi<float>());
    
    float yaw = objRotation.y;
    const glm::vec3 forwardDir {sin(yaw), 0.0f, cos(yaw)};
    const glm::vec3 rightDir {forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir {0.0f, -1.0f, 0.0f};

    glm::vec3 moveDir {0.0f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        objTranslation += moveSpeed * deltaTime * glm::normalize(moveDir);
    }    
}

void KeyboardController::otherKeys(GLFWwindow *window, float deltaTime, GameObject &gameObject) {
    if (glfwGetKey(window, keys.exit) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void KeyboardController::updateChip8Input(GLFWwindow* window, Chip8& chip8) {
    static const std::pair<int, uint8_t> keyMap[] = {
        {chip8Keys.key0, 0x0}, {chip8Keys.key1, 0x1}, {chip8Keys.key2, 0x2}, {chip8Keys.key3, 0x3},
        {chip8Keys.key4, 0x4}, {chip8Keys.key5, 0x5}, {chip8Keys.key6, 0x6}, {chip8Keys.key7, 0x7},
        {chip8Keys.key8, 0x8}, {chip8Keys.key9, 0x9}, {chip8Keys.keyA, 0xA}, {chip8Keys.keyB, 0xB},
        {chip8Keys.keyC, 0xC}, {chip8Keys.keyD, 0xD}, {chip8Keys.keyE, 0xE}, {chip8Keys.keyF, 0xF}
    };

    for (auto [glfwKey, chip8Key] : keyMap) {
        chip8.m_keypad[chip8Key] = glfwGetKey(window, glfwKey) == GLFW_PRESS ? 1 : 0;
    }
}

} // namespace MAGE