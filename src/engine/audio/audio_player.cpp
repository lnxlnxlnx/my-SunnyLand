#include "audio_player.h"
#include "../resource/resource_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>

namespace engine::audio {
AudioPlayer::~AudioPlayer() {
    if (music_track_) {
        MIX_DestroyTrack(music_track_);
        music_track_ = nullptr;
    }
}

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* resource_manager)
    : resource_manager_(resource_manager) {
    if (!resource_manager_) {
        throw std::runtime_error("AudioPlayer 构造失败: 提供的 ResourceManager 指针为空。");
    }
    mixer_ = resource_manager_->getMixer();
    music_track_ = MIX_CreateTrack(mixer_);
    if (!music_track_) {
        throw std::runtime_error("AudioPlayer 构造失败: 无法创建音乐轨道: " + std::string(SDL_GetError()));
    }
}

int AudioPlayer::playSound(std::string_view sound_path) {

    MIX_Audio* audio = resource_manager_->getSound(sound_path); // 通过 ResourceManager 获取资源
    if (!audio) {
        spdlog::error("AudioPlayer: 无法获取音效 '{}' 播放。", sound_path);
        return -1;
    }

    if (!MIX_PlayAudio(mixer_, audio)) {    // 即发即忘方式播放音效
        spdlog::error("AudioPlayer: 无法播放音效 '{}': {}", sound_path, SDL_GetError());
        return -1;
    }
    spdlog::trace("AudioPlayer: 播放音效 '{}'。", sound_path);
    return 0;
}

bool AudioPlayer::playMusic(std::string_view music_path, int loops, int fade_in_ms) {
    if (music_path == current_music_) return true;      // 如果当前音乐已经在播放，则不重复播放
    current_music_ = music_path;
    MIX_Audio* music = resource_manager_->getMusic(music_path); // 通过 ResourceManager 获取资源
    if (!music) {
        spdlog::error("AudioPlayer: 无法获取音乐 '{}' 播放。", music_path);
        return false;
    }

    MIX_StopTrack(music_track_, 0);         // 立即停止之前的音乐
    MIX_SetTrackAudio(music_track_, music); // 设置音乐轨道的音频源

    // 配置播放参数（循环次数、淡入时长）
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (fade_in_ms > 0) {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fade_in_ms);
    }
    bool result = MIX_PlayTrack(music_track_, props);
    SDL_DestroyProperties(props);

    if (!result) {
        spdlog::error("AudioPlayer: 无法播放音乐 '{}': {}", music_path, SDL_GetError());
    } else {
        spdlog::trace("AudioPlayer: 播放音乐 '{}'。", music_path);
    }
    return result;
}

void AudioPlayer::stopMusic(int fade_out_ms) {
    Sint64 fade_frames = (fade_out_ms > 0) ? MIX_TrackMSToFrames(music_track_, fade_out_ms) : 0;
    MIX_StopTrack(music_track_, fade_frames);
    current_music_.clear();
    spdlog::trace("AudioPlayer: 停止音乐。");
}

void AudioPlayer::pauseMusic() {
    MIX_PauseTrack(music_track_);
    spdlog::trace("AudioPlayer: 暂停音乐。");
}

void AudioPlayer::resumeMusic() {
    MIX_ResumeTrack(music_track_);
    spdlog::trace("AudioPlayer: 恢复音乐。");
}

void AudioPlayer::setSoundVolume(float volume) {
    // 通过混音器整体增益控制音效音量（0.0-1.0）
    MIX_SetMixerGain(mixer_, volume);
    spdlog::trace("AudioPlayer: 设置音效音量为 {:.2f}。", volume);
}

void AudioPlayer::setMusicVolume(float volume) {
    MIX_SetTrackGain(music_track_, volume);
    spdlog::trace("AudioPlayer: 设置音乐音量为 {:.2f}。", volume);
}

float AudioPlayer::getMusicVolume() {
    return MIX_GetTrackGain(music_track_);
}

float AudioPlayer::getSoundVolume() {
    return MIX_GetMixerGain(mixer_);
}

} // namespace engine::audio
