#pragma once
#include <memory> // 用于 std::unique_ptr

// 前向声明, 减少头文件的依赖，增加编译速度
struct SDL_Window;
struct SDL_Renderer;
namespace engine::resource {
    class ResourceManager;
}
namespace engine::core
{
    class Time;
    /**
     * @brief 主游戏应用程序类，初始化SDL，管理游戏循环。
     */
    class GameApp final
    {
    private:
    
        // SDL 相关成员
        SDL_Window *window_ = nullptr;
        SDL_Renderer *sdl_renderer_ = nullptr;
        bool is_running_ = false;

        // 资源管理器       ///@brief 资源管理器，负责加载和管理游戏资源，如纹理、音频和字体。
        std::unique_ptr<engine::resource::ResourceManager> resource_manager_;

        std::unique_ptr<engine::core::Time> time_manager_; // 用于管理游戏时间和帧率

    public:
        GameApp();
        ~GameApp();

        /**
         * @brief 运行游戏应用程序，其中会调用init()，然后进入主循环，离开循环后自动调用close()。
         */
        void run();

        // 禁止拷贝和移动
        GameApp(const GameApp &) = delete;
        GameApp &operator=(const GameApp &) = delete;
        GameApp(GameApp &&) = delete;
        GameApp &operator=(GameApp &&) = delete;

    private:
        [[nodiscard]] bool init();
        void handleEvents();
        void update(float delta_time);
        void render();
        void close();

    private:
        // 其他私有成员函数和变量

        // 各模块的初始化/创建函数，在init()中调用
        bool initSDL();
        bool initTime();
        bool initResourceManager();

        // 测试用函数
        void testResourceManager();
    };

} // namespace engine::core
