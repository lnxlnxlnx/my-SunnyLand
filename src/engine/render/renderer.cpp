/***
 * @Date: 2026-05-08 20:26:24
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-08 20:29:37
 * @FilePath: /SunnyLand/src/engine/render/renderer.cpp
 * @Description:
 */
#include "renderer.h"
#include "camera.h"
#include "sprite.h"
#include "../resource/resource_manager.h"
#include <SDL3/SDL.h>
#include <stdexcept> // For std::runtime_error
#include <spdlog/spdlog.h>

namespace engine::render
{
    Renderer::Renderer(SDL_Renderer *sdl_renderer, engine::resource::ResourceManager *resource_manager)
    {
        if (sdl_renderer == nullptr)
        {
            throw std::runtime_error("Renderer initialization failed: SDL_Renderer pointer is null.");
        }
        if (resource_manager == nullptr)
        {
            throw std::runtime_error("Renderer initialization failed: ResourceManager pointer is null.");
        }
        renderer_ = sdl_renderer;
        resource_manager_ = resource_manager;
    }
    void Renderer::drawSprite(const Camera &camera, const Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scale, double angle)
    {

    }
}