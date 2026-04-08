#pragma once

#include "Model.hpp"
#include "Util.hpp"

#include <memory>

namespace MAGE {

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
    Transform3dComponent m_transform3d;
    bool m_is2d = false;

private:
    GameObject(id_t objId) : m_id(objId) {} 

    id_t m_id;
};


} // namespace MAGE