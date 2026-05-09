/***
 * @Date: 2026-05-09 10:03:39
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-09 10:07:57
 * @FilePath: /SunnyLand/src/engine/core/config.cpp
 * @Description:
 */
#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

namespace engine::core
{
    bool Config::loadFromFile(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            spdlog::error("无法打开配置文件: {}, 创建默认配置文件。", filepath);
            if (!saveToFile(filepath))
            {
                spdlog::error("创建默认配置文件失败: {}", filepath);
            }
            return false;
        }
        spdlog::trace("文件{}已打开", filepath);

        nlohmann::json json;
        try
        {
            file >> json;
            file.close();
            fromJson(json);
            spdlog::trace("配置文件加载成功: {}", filepath);
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("加载配置文件时出错: {}", e.what());
            return false;
        }
        return true;
    }
    /*** 
     * @description: 提供将当前配置保存到 JSON 文件的功能，使用 nlohmann::json 库进行序列化。成功保存返回 true，否则返回 false。
     * @param {string} &filepath
     * @return {*}
     */
    bool Config::saveToFile(const std::string &filepath)
    {
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            spdlog::error("无法打开配置文件: {}", filepath);
            return false;
        }
        nlohmann::ordered_json json = toJson();
        try
        {
            file << json.dump(4); // 使用4个空格进行缩进以提高可读性
            file.close();
            spdlog::trace("配置文件保存成功: {}", filepath);
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("保存配置文件时出错: {}", e.what());
        }
        return false;
    }
    /***
     * @description: 反序列化配置文件，支持部分字段缺失，使用默认值。
     * @param {json} &j
     * @return {*}
     * TODO: 可以添加更多的错误检查和日志记录以及文件参数的验证等，以帮助调试配置文件问题。
     */
    void Config::fromJson(const nlohmann::json &j)
    {
        if (j.contains("window"))
        {
            const auto &window = j["window"];
            if (window.contains("title"))
                window_title_ = window["title"];
            if (window.contains("width"))
                window_width_ = window["width"];
            if (window.contains("height"))
                window_height_ = window["height"];
            if (window.contains("resizable"))
                window_resizable_ = window["resizable"];
        }
        if (j.contains("graphics") && j["graphics"].contains("vsync"))
            vsync_enabled_ = j["graphics"]["vsync"];
        if (j.contains("performance") && j["performance"].contains("target_fps"))
            target_fps_ = j["performance"]["target_fps"];
        if (j.contains("audio"))
        {
            const auto &audio = j["audio"];
            if (audio.contains("music_volume"))
                music_volume_ = audio["music_volume"];
            if (audio.contains("sound_volume"))
                sound_volume_ = audio["sound_volume"];
        }
        if (j.contains("input_mappings") && j["input_mappings"].is_object())
            input_mappings_ = j["input_mappings"].get<std::unordered_map<std::string, std::vector<std::string>>>();
    }
    /***
     * @description: 将配置转换为 JSON 对象，序列化时保持字段顺序以提高可读性。
     * @return {*}
     */
    nlohmann::ordered_json Config::toJson() const
    {
        return nlohmann::ordered_json{
            {"window", {{"title", window_title_}, {"width", window_width_}, {"height", window_height_}, {"resizable", window_resizable_}}},
            {"graphics", {{"vsync", vsync_enabled_}}},
            {"performance", {{"target_fps", target_fps_}}},
            {"audio", {{"music_volume", music_volume_}, {"sound_volume", sound_volume_}}},
            {"input_mappings", input_mappings_}};
    }
}
