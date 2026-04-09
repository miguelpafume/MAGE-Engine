#pragma once

#define GLM_FORCE_RADIAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cassert>
#include <limits>

namespace MAGE {

class Camera {
public:

void setOrtohraphicProjection(float left, float right, float top, float bottom, float near, float far);
void setPerspectiveProjection(float fov, float aspect, float near, float far);

const glm::mat4x4& getProjection() const { return m_projectionMatrix; }

private:

    glm::mat4x4 m_projectionMatrix {1.0f};

};

} // namespace MAGE