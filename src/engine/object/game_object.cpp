/*** 
 * @Date: 2026-05-09 20:14:06
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-10 23:10:11
 * @FilePath: /SunnyLand/src/engine/object/game_object.cpp
 * @Description: 
 */
#include "game_object.h"
#include "../render/renderer.h"
#include "../input/input_manager.h" 
#include "../render/camera.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::object {
GameObject::GameObject(const std::string &name, const std::string &tag): name_(name), tag_(tag)
{
    spdlog::trace("GameObject created: {} {}", name_, tag_);
}

void GameObject::update(float delta_time, engine::core::Context& context) {
    // 遍历所有组件并调用它们的 update 方法
    for (auto& pair : components_) {
        pair.second->update(delta_time, context);
    }
}

void GameObject::render(engine::core::Context& context) {
    // 遍历所有组件并调用它们的 render 方法
    for (auto& pair : components_) {
        pair.second->render(context);
    }
}

void GameObject::clean() {
    spdlog::trace("Cleaning GameObject...");
    // 遍历所有组件并调用它们的 clean 方法
    for (auto& pair : components_) {
        pair.second->clean();
    }
    components_.clear(); // 清空 map, unique_ptr 会自动释放内存
}

void GameObject::handleInput(engine::core::Context& context) {
    // 遍历所有组件并调用它们的 handleInput 方法
    for (auto& pair : components_) {
        pair.second->handleInput(context);
    }
}


} // namespace engine::object 