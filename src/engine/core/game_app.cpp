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
#include "../resource/resource_manager.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../render/sprite.h"

namespace engine::core
{

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
            handleEvents();
            update(delta_time);
            render();
        }

        close();
    }

    bool GameApp::init()
    {
        spdlog::trace("初始化 GameApp ...");
        if (!initConfig())      // 初始化配置文件, 失败则无法继续，因为后续的模块初始化都依赖于配置设置。
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

        // 测试资源管理器
        //testResourceManager();

        is_running_ = true;
        return true;
    }

    void GameApp::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running_ = false;
            }
        }
    }

    void GameApp::update(float /* delta_time */)
    {
        testCamera();
    }

    void GameApp::render()
    {
        renderer_->clearScreen();
        testRenderer();
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
        SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_, config_->window_height_, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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
            auto viewport_size = glm::vec2(config_->window_width_, config_->window_height_);
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

} // namespace engine::core