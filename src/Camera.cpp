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

void Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w {glm::normalize(direction)};
    const glm::vec3 u {glm::normalize(glm::cross(w, up))};
    const glm::vec3 v {glm::cross(w, u)};

    m_viewMatrix = glm::mat4{1.f};
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;

    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;

    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;

    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
}

void Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    assert(glm::dot(target, position) != 0 && "DIRECTION FOR VIEW TARGET CANNOT BE ZERO");
    setViewDirection(position, target - position, up);
}

void Camera::setViewXYZ(glm::vec3 position, glm::vec3 rotation) {
    const float c1 = glm::cos(rotation.x);
    const float c2 = glm::cos(rotation.y);
    const float c3 = glm::cos(rotation.z);

    const float s1 = glm::sin(rotation.x);
    const float s2 = glm::sin(rotation.y);
    const float s3 = glm::sin(rotation.z);

    const glm::vec3 u{(c2 * c3), (c1 * s3 + c3 * s1 * s2), (s1 * s3 - c1 * c3 * s2)};
    const glm::vec3 v{(-c2 * s3), (c1 * c3 - s1 * s2 * s3), (c3 * s1 + c1 * s2 * s3)};
    const glm::vec3 w{(s2), (-c2 * s1), (c1 * c2)};

    m_viewMatrix = glm::mat4{1.f};
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;

    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;

    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;
    
    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
}


} // namespace MAGE