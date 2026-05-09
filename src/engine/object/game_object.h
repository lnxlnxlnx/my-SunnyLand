#ifndef A36827E6_75E7_46AF_AC2D_9F5FD8E80BF2
#define A36827E6_75E7_46AF_AC2D_9F5FD8E80BF2
#pragma once
#include "../component/component.h"
#include <memory>
#include <unordered_map>
#include <typeindex> // 用于类型索引
#include <utility>   // 用于完美转发
#include <spdlog/spdlog.h>

namespace engine::object
{

    /**
     * @brief 游戏对象类，负责管理游戏对象的组件。
     *
     * 该类管理游戏对象的组件，并提供添加、获取、检查和移除组件的功能。
     * 它还提供更新和渲染游戏对象的方法。
     */
    class GameObject final
    {
    private:
        std::string name_;                                                                              ///< @brief 名称
        std::string tag_;                                                                               ///< @brief 标签
        std::unordered_map<std::type_index, std::unique_ptr<engine::component::Component>> components_; ///< @brief 组件列表
        bool need_remove_ = false;                                                                      ///< @brief 延迟删除的标识，将来由场景类负责删除

    public:
        GameObject(const std::string &name = "", const std::string &tag = ""); ///< @brief 构造函数。默认名称为空，标签为空

        // 禁止拷贝和移动，确保唯一性 (通常游戏对象不应随意拷贝)
        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject &) = delete;
        GameObject(GameObject &&) = delete;
        GameObject &operator=(GameObject &&) = delete;

        // setters and getters
        void setName(const std::string &name) { name_ = name; }              ///< @brief 设置名称
        const std::string &getName() const { return name_; }                 ///< @brief 获取名称
        void setTag(const std::string &tag) { tag_ = tag; }                  ///< @brief 设置标签
        const std::string &getTag() const { return tag_; }                   ///< @brief 获取标签
        void setNeedRemove(bool need_remove) { need_remove_ = need_remove; } ///< @brief 设置是否需要删除
        bool isNeedRemove() const { return need_remove_; }                   ///< @brief 获取是否需要删除

        /***
         * @description: 添加组件，传递构造参数，返回组件指针(如果组件已存在则返回现有组件指针)。使用模板实现，确保类型安全。
         * @return {*}
         */
        template <typename T, typename... Args> ///< @brief 添加组件，传递构造参数，返回组件指针
        T *addComponent(Args &&...args)
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T must be a subclass of Component");
            if (hasComponent<T>())
            {
                spdlog::warn("GameObject '{}' already has component of type '{}'", name_, typeid(T).name());
                return getComponent<T>(); // 已经有了就返回现有的
            }
            auto type_id = std::type_index(typeid(T));
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            ptr->setOwner(this);                   // 设置组件的拥有者为当前 GameObject
            T *raw_ptr = ptr.get();                // 获取原始指针以返回
            components_[type_id] = std::move(ptr); // 存储组件
            /**
    T 是具体组件子类，比如 TransformComponent
//std::unique_ptr<T> comp = std::make_unique<T>();
// 存入 map：value 是 unique_ptr<Component>
//components_[std::type_index(typeid(T))] = std::move(comp);这里发生了隐式向上转型
             *
             */
            raw_ptr->init(); // 初始化组件
            spdlog::info("Added component '{}' to GameObject '{}'", typeid(T).name(), name_);
            return raw_ptr;
        }

        /***
         * @description: 获取组件指针，返回组件指针(如果组件不存在则返回nullptr)。使用模板实现，确保类型安全。
         * @return {*}
         */
        template <typename T>
        T *getComponent() const
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T must be a subclass of Component");
            auto type_id = std::type_index(typeid(T));
            auto it = components_.find(type_id);
            if (it != components_.end())
            {
                return static_cast<T *>(it->second.get()); // NOTE: 这里的static_cast是安全的，因为我们在addComponent时确保了类型正确性，不过C++ 不会自动帮你转回子类，必须手动强转
            }
            return nullptr; // 没有找到
        }

        /***
         * @description: 检查组件是否存在。使用模板实现，确保类型安全。
         * @return {*}
         */
        template <typename T>
        bool hasComponent() const
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T must be a subclass of Component");
            auto type_id = std::type_index(typeid(T));
            return components_.contains(type_id);
        }

        /***
         * @description: 移除组件。使用模板实现，确保类型安全。
         * @return {*}
         */
        template <typename T>
        void removeComponent()
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T must be a subclass of Component");
            auto type_id = std::type_index(typeid(T));
            auto it = components_.find(type_id);
            if (it != components_.end())
            {
                it->second->clean(); // 先调用组件的清理方法    //NOTE: 这里调用clean方法是为了让组件有机会在被删除前清理资源，虽然unique_ptr会自动释放内存，但如果组件内部有其它资源（比如SDL_Texture等）需要手动释放，就可以在clean方法里处理
                components_.erase(it);  // 智能指针只是不需要手动delete了，erase后unique_ptr会自动释放内存，但是如果组件内部有其它资源需要手动释放，就可以在clean方法里处理
                spdlog::info("Removed component '{}' from GameObject '{}'", typeid(T).name(), name_);
            }else{
                spdlog::warn("GameObject '{}' does not have component of type '{}'", name_, typeid(T).name());
            }
        }

        // 关键循环函数
        void update(float delta_time); ///< @brief 更新所有组件
        void render();                 ///< @brief 渲染所有组件
        void clean();                  ///< @brief 清理所有组件
        void handleInput();            ///< @brief 处理输入
    };

} // namespace engine::object

#endif /* A36827E6_75E7_46AF_AC2D_9F5FD8E80BF2 */
