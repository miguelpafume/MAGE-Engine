#pragma once

#include "Engine.hpp"
#include "GameObject.hpp"
#include "Texture.hpp"

#include <memory>
#include <vector>

class CubeApp {
public:
    void run ();

private:
    void loadGameObjects ();

    MAGE::Engine m_engine {"M.A.G.E."};
    std::vector<MAGE::GameObject> m_gameObjects;
    std::vector<std::unique_ptr<MAGE::Texture>> m_textures;
    std::unique_ptr<MAGE::Texture> m_defaultTexture;
};