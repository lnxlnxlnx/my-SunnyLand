#include "audio_manager.h"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::resource {

// 构造函数：初始化SDL_mixer
AudioManager::AudioManager() {
    // SDL_mixer 3.2.0 不再需要传入格式标志
    if (!MIX_Init()) {
        throw std::runtime_error("AudioManager 错误: MIX_Init 失败: " + std::string(SDL_GetError()));
    }

    // 创建连接到默认音频输出设备的混音器
    mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer_) {
        MIX_Quit();
        throw std::runtime_error("AudioManager 错误: MIX_CreateMixerDevice 失败: " + std::string(SDL_GetError()));
    }
    spdlog::trace("AudioManager 构造成功。");
}

AudioManager::~AudioManager()
{
    // 立即停止所有音频播放
    MIX_StopAllTracks(mixer_, 0);

    // 清理资源映射（unique_ptrs会调用删除器）
    clearSounds();
    clearMusic();

    // 销毁混音器（同时关闭音频设备）
    MIX_DestroyMixer(mixer_);

    // 退出SDL_mixer子系统
    MIX_Quit();
    spdlog::trace("AudioManager 析构成功。");
}

// --- 音效管理 ---
MIX_Audio* AudioManager::loadSound(std::string_view file_path) {
    // 首先检查缓存
    auto it = sounds_.find(std::string(file_path));
    if (it != sounds_.end()) {
        return it->second.get();
    }

    // 加载音效块（predecode=true，预先解码为PCM，适合短音效）
    spdlog::debug("加载音效: {}", file_path);
    MIX_Audio* audio = MIX_LoadAudio(mixer_, file_path.data(), true);
    if (!audio) {
        spdlog::error("加载音效失败: '{}': {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    sounds_.emplace(file_path, std::unique_ptr<MIX_Audio, MIXAudioDeleter>(audio));
    spdlog::debug("成功加载并缓存音效: {}", file_path);
    return audio;
}

MIX_Audio* AudioManager::getSound(std::string_view file_path) {
    auto it = sounds_.find(std::string(file_path));
    if (it != sounds_.end()) {
        return it->second.get();
    }
    spdlog::warn("音效 '{}' 未找到缓存，尝试加载。", file_path);
    return loadSound(file_path);
}

void AudioManager::unloadSound(std::string_view file_path) {
    auto it = sounds_.find(std::string(file_path));
    if (it != sounds_.end()) {
        spdlog::debug("卸载音效: {}", file_path);
        sounds_.erase(it);      // unique_ptr处理MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音效: {}", file_path);
    }
}

void AudioManager::clearSounds() {
    if (!sounds_.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音效。", sounds_.size());
        sounds_.clear(); // unique_ptr处理删除
    }
}

// --- 音乐管理 ---
MIX_Audio* AudioManager::loadMusic(std::string_view file_path) {
    // 首先检查缓存
    auto it = music_.find(std::string(file_path));
    if (it != music_.end()) {
        return it->second.get();
    }

    // 加载音乐（predecode=false，流式解码，适合长音乐）
    spdlog::debug("加载音乐: {}", file_path);
    MIX_Audio* audio = MIX_LoadAudio(mixer_, file_path.data(), false);
    if (!audio) {
        spdlog::error("加载音乐失败: '{}': {}", file_path, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    music_.emplace(file_path, std::unique_ptr<MIX_Audio, MIXAudioDeleter>(audio));
    spdlog::debug("成功加载并缓存音乐: {}", file_path);
    return audio;
}

MIX_Audio* AudioManager::getMusic(std::string_view file_path) {
    auto it = music_.find(std::string(file_path));
    if (it != music_.end()) {
        return it->second.get();
    }
    spdlog::warn("音乐 '{}' 未找到缓存，尝试加载。", file_path);
    return loadMusic(file_path);
}

void AudioManager::unloadMusic(std::string_view file_path) {
    auto it = music_.find(std::string(file_path));
    if (it != music_.end()) {
        spdlog::debug("卸载音乐: {}", file_path);
        music_.erase(it); // unique_ptr处理MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音乐: {}", file_path);
    }
}

void AudioManager::clearMusic() {
    if (!music_.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音乐曲目。", music_.size());
        music_.clear(); // unique_ptr处理删除
    }
}

void AudioManager::clearAudio()
{
    clearSounds();
    clearMusic();
}

} // namespace engine::resource
