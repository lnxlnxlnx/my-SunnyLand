/***
 * @Date: 2026-05-07 23:19:53
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-08 14:17:41
 * @FilePath: /SunnyLand/src/engine/resource/font_manager.cpp
 * @Description:
 */
#include "font_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource
{
    FontManager::FontManager()
    {
        if (!(TTF_WasInit()) && !TTF_Init())
        {
            throw std::runtime_error("FontManager 错误: SDL_ttf 初始化失败: " + std::string(SDL_GetError()));
        }
        spdlog::trace("FontManager: SDL_ttf 初始化成功");
    }
    FontManager::~FontManager()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("FontManager: 析构时正在清除 {} 个缓存的字体。", fonts_.size());
            fonts_.clear(); // unique_ptr会调用SDLFontDeleter
        }
        TTF_Quit();
        spdlog::trace("FontManager: SDL_ttf 退出");
    }
    TTF_Font *FontManager::loadFont(const std::string &file_path, int point_size)
    {
        if (point_size < 0){
            spdlog::warn("FontManager: point_size 不能小于 0");
        }
        FontKey key{file_path, point_size};
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }
        TTF_Font *raw_font = TTF_OpenFont(file_path.c_str(), point_size);
        if (!raw_font)
        {
            spdlog::error("FontManager::loadFont: 无法加载字体: '{}', 大小: {}", file_path, point_size);
            return nullptr;
        }
        fonts_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(raw_font));
        return raw_font;
    }

    TTF_Font *FontManager::getFont(const std::string &file_path, int point_size)
    {
        FontKey key{file_path, point_size};
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }
        spdlog::warn("FontManager: Font not found: {}({}pt), 尝试加载", file_path, point_size);
        return loadFont(file_path, point_size); // 尝试加载字体
    }

    void FontManager::unloadFont(const std::string &file_path, int point_size)
    {
        FontKey key{file_path, point_size};
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            spdlog::debug("FontManager: 卸载字体: {}, 大小 {}", file_path, point_size);
            fonts_.erase(it); // unique_ptr会调用SDLFontDeleter
        }
        else
        {
            spdlog::warn("FontManager: 尝试卸载不存在的字体: {}, 大小 {}", file_path, point_size);
        }
    }

    void FontManager::clearFonts()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("FontManager: 正在清除所有 {} 个缓存的字体。", fonts_.size());
            fonts_.clear(); // unique_ptr会调用SDLFontDeleter
        }
    }

}
