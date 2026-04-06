#pragma once

#include "Model.hpp"

#include <memory>

namespace MAGE {

struct 
Transform2dComponent {
    glm::vec2 translation {}; // Position offset
    glm::vec2 scale {1.f, 1.f}; // Position offset
    float rotation;

    // GLM READS FROM COLUMNS NOT ROWS
    glm::mat2x2 mat2x2() { 
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);
        glm::mat2x2 rotationMatrix{{c, s}, {-s, c}};

        glm::mat2x2 scaleMatrix{{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotationMatrix * scaleMatrix;
    }
};

class GameObject {
public:
    using id_t = unsigned int;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) = default;
    GameObject& operator=(GameObject&&) = default;
    
    static GameObject createGameObject() {
        static id_t currentId = 0;
        return GameObject{currentId++};
    }

    const id_t getId() { return m_id; }

    std::shared_ptr<Model> m_model {};
    glm::vec3 m_color {};
    Transform2dComponent m_transform2d;

private:
    GameObject(id_t objId) : m_id(objId) {}

    id_t m_id;
};


} // namespace MAGE