#include "Camera.hpp"

namespace MAGE {

void Camera::setOrtohraphicProjection(float left, float right, float top, float bottom, float near, float far) {
    m_projectionMatrix = glm::mat4x4{1.0f};
    m_projectionMatrix[0][0] = 2.0f / (right - left);
    m_projectionMatrix[1][1] = 2.0f / (bottom - top);
    m_projectionMatrix[2][2] = 1.0f / (far - near);
    m_projectionMatrix[3][0] = -(right + left) / (right - left);
    m_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    m_projectionMatrix[3][2] = -near / (far - near);
}

void Camera::setPerspectiveProjection(float fov, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    
    const float tanHalfFov = tan(fov / 2.0f);
    
    m_projectionMatrix = glm::mat4x4{0.0f};
    m_projectionMatrix[0][0] = 1.0f / (aspect * tanHalfFov);
    m_projectionMatrix[1][1] = 1.0f / tanHalfFov;
    m_projectionMatrix[2][2] = far / (far - near);
    m_projectionMatrix[2][3] = 1.0f;
    m_projectionMatrix[3][2] = -(far * near) / (far - near);
}

} // namespace MAGE