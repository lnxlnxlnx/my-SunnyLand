/***
 * @Date: 2026-05-06 21:57:20
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-08 14:34:27
 * @FilePath: /SunnyLand/src/engine/core/game_app.cpp
 * @Description:
 */
#include "game_app.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include "timer.h"
#include "config.h"
#include "context.h"
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../render/sprite.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"

namespace engine::core
{
    engine::object::GameObject game_object("TestObject", "TestTag");
    GameApp::GameApp() = default;

    GameApp::~GameApp()
    {
        if (is_running_)
        {
            spdlog::warn("GameApp 被销毁时没有显式关闭。现在关闭。 ...");
            close();
        }
    }

    void GameApp::run()
    {
        if (!init())
        {
            spdlog::error("初始化失败，无法运行游戏。");
            return;
        }

        while (is_running_)
        {
            time_manager_->update();
            float delta_time = time_manager_->getDeltaTime();
            input_manager_->update(); // 处理输入事件并更新状态
            handleEvents();
            update(delta_time);
            render();
        }

        close();
    }

    bool GameApp::init()
    {
        spdlog::trace("初始化 GameApp ...");
        if (!initConfig()) // 初始化配置文件, 失败则无法继续，因为后续的模块初始化都依赖于配置设置。
            return false;
        if (!initSDL())
            return false;
        if (!initTime())
            return false;
        if (!initResourceManager())
            return false;
        if (!initRenderer())
            return false;
        if (!initCamera())
            return false;
        if (!initInputManager())
            return false;
        if (!initContext())
            return false;
        // 测试资源管理器
        // testResourceManager();

        is_running_ = true;
        testGameObject(); // 测试 GameObject 和 Component 系统
        return true;
    }

    void GameApp::handleEvents()
    {
        if (input_manager_->shouldQuit())
        {
            is_running_ = false;
        }
    }

    void GameApp::update(float /* delta_time */)
    {
        testCamera();
        testInputManager();
    }

    void GameApp::render()
    {
        renderer_->clearScreen();
        testRenderer();
        game_object.render(*context_);      // 测试 GameObject 的渲染，注意这里也要调用
        renderer_->present();
    }

    void GameApp::close()
    {
        spdlog::trace("关闭 GameApp ...");
        if (sdl_renderer_ != nullptr)
        {
            SDL_DestroyRenderer(sdl_renderer_);
            sdl_renderer_ = nullptr;
        }
        if (window_ != nullptr)
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
        SDL_Quit();
        is_running_ = false;
    }

    bool GameApp::initSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            spdlog::error("SDL 初始化失败! SDL错误: {}", SDL_GetError());
            return false;
        }

        window_ = SDL_CreateWindow("SunnyLand", config_->window_width_, config_->window_height_, SDL_WINDOW_RESIZABLE);
        if (window_ == nullptr)
        {
            spdlog::error("无法创建窗口! SDL错误: {}", SDL_GetError());
            return false;
        }

        sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (sdl_renderer_ == nullptr)
        {
            spdlog::error("无法创建渲染器! SDL错误: {}", SDL_GetError());
            return false;
        }
        // 设置 VSync (注意: VSync 开启时，驱动程序会尝试将帧率限制到显示器刷新率，有可能会覆盖我们手动设置的 target_fps)
        int vsync_mode = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
        SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
        spdlog::trace("VSync 设置为: {}", config_->vsync_enabled_ ? "Enabled" : "Disabled");

        // 设置逻辑分辨率为窗口大小的一半（针对像素游戏）
        SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_ / 2, config_->window_height_ / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        spdlog::trace("SDL 初始化成功。");
        return true;
    }

    bool GameApp::initTime()
    {
        // 初始化时间管理器
        time_manager_ = std::make_unique<engine::core::Time>();
        if (!time_manager_)
        {
            spdlog::error("无法创建 Time 管理器!");
            return false;
        }
        time_manager_->setTargetFps(config_->target_fps_); // 设置目标帧率为 config_的 FPS
        return true;
    }

    bool GameApp::initResourceManager()
    {
        try
        {
            resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
        }
        catch (const std::exception &e)
        {
            spdlog::error("ResourceManager 初始化失败: {}", e.what());
            return false;
        }
        // if (!resource_manager_)
        // {
        //     spdlog::error("无法创建 ResourceManager!");
        //     return false;
        // }
        return true;
    }

    bool GameApp::initRenderer()
    {
        try
        {
            renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_, resource_manager_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("Renderer 初始化失败: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::initCamera()
    {
        try
        {
            auto viewport_size = glm::vec2(config_->window_width_ / 2.0f, config_->window_height_ / 2.0f); // 窗口分辨率的一半，与逻辑分辨率一致
            camera_ = std::make_unique<engine::render::Camera>(viewport_size);
        }
        catch (const std::exception &e)
        {
            spdlog::error("Camera 初始化失败: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::initConfig()
    {
        try
        {
            config_ = std::make_unique<engine::core::Config>("assets/config.json");
        }
        catch (const std::exception &e)
        {
            spdlog::error("Config 初始化失败: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::initInputManager()
    {
        try
        {
            input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_, config_.get());
        }
        catch (const std::exception &e)
        {
            spdlog::error("InputManager 初始化失败: {}", e.what());
            return false;
        }
        return true;
    }

    bool GameApp::initContext()
    {
        try
        {
            context_ = std::make_unique<engine::core::Context>(
            *input_manager_,
            *renderer_,
            *camera_,
            *resource_manager_
        );
        }
        catch (const std::exception &e)
        {
            spdlog::error("Context 初始化失败: {}", e.what());
            return false;
        }
        return true;
    }

    void GameApp::testResourceManager()
    {
        resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
        resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
        resource_manager_->getSound("assets/audio/button_click.wav");

        resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
        resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
        resource_manager_->unloadSound("assets/audio/button_click.wav");
    }

    void GameApp::testRenderer()
    {
        engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
        engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
        engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

        static float rotation = 0.0f;
        rotation += 0.1f;

        // 注意渲染顺序
        renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100, 100), glm::vec2(0.5f, 0.5f), glm::bvec2(true, false));
        renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200, 200), glm::vec2(1.0f, 1.0f), rotation);
        renderer_->drawUISprite(sprite_ui, glm::vec2(100, 100));
    }

    void GameApp::testCamera()
    {
        auto keyboard_state = SDL_GetKeyboardState(nullptr);
        if (keyboard_state[SDL_SCANCODE_W])
            camera_->move(glm::vec2(0.0f, -1.0f));
        if (keyboard_state[SDL_SCANCODE_S])
            camera_->move(glm::vec2(0.0f, 1.0f));
        if (keyboard_state[SDL_SCANCODE_A])
            camera_->move(glm::vec2(-1.0f, 0.0f));
        if (keyboard_state[SDL_SCANCODE_D])
            camera_->move(glm::vec2(1.0f, 0.0f));
        if (keyboard_state[SDL_SCANCODE_ESCAPE])
            is_running_ = false;
    }

    void GameApp::testInputManager()
    {
        std::vector<std::string> actions = {
            "move_up",
            "move_down",
            "move_left",
            "move_right",
            "jump",
            "attack",
            "pause",
            "MouseLeftClick",
            "MouseRightClick"};

        for (const auto &action : actions)
        {
            if (input_manager_->isActionPressed(action))
            {
                spdlog::info(" {} 按下 ", action);
            }
            if (input_manager_->isActionReleased(action))
            {
                spdlog::info(" {} 抬起 ", action);
            }
            if (input_manager_->isActionDown(action))
            {
                spdlog::info(" {} 按下中 ", action);
            }
        }
    }

    void GameApp::testGameObject()
    {
        spdlog::info("========== 测试组件系统 ==========");

        // 1. 添加 TransformComponent，让对象出现在 (100, 100)
        game_object.addComponent<engine::component::TransformComponent>(
            glm::vec2(100.0f, 100.0f));

        // 2. 添加 SpriteComponent，显示箱子贴图，设置渲染中心为图片的几何中心
        game_object.addComponent<engine::component::SpriteComponent>(
            "assets/textures/Props/big-crate.png",
            *resource_manager_,
            engine::utils::Alignment::CENTER);

        // 3. 获取 TransformComponent，修改缩放和旋转
        game_object.getComponent<engine::component::TransformComponent>()->setScale(glm::vec2(2.0f, 2.0f)); // 放大 2 倍
        game_object.getComponent<engine::component::TransformComponent>()->setRotation(30.0f);              // 旋转 30 度
        spdlog::info("✓ Transform 组件配置完成");
        spdlog::info("====================================");
    }

} // namespace engine::core