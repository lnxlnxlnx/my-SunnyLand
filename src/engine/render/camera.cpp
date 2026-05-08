#include "camera.h"
#include <spdlog/spdlog.h>
namespace engine::render
{
    Camera::Camera(const glm::vec2 &viewport_size, const glm::vec2 &position, const std::optional<engine::utils::Rect> limit_bounds)
        : viewport_size_(viewport_size),
          position_(position),
          limit_bounds_(limit_bounds)
    {
        spdlog::debug("Camera created with limit bounds");
    }

    void Camera::update(float delta_time)
    {
        // 目前没有自动更新逻辑，留空以备将来使用
    }

    /*** 
     * @description: 每次调用这个函数时，都会根据传入的偏移量移动相机位置，并调用clampPosition()确保相机位置在限制范围内。
     * @param {vec2} &offset
     * @return {*}
     */
    void Camera::move(const glm::vec2 &offset)
    {
        position_ += offset;
        clampPosition();
    }

    glm::vec2 Camera::worldToScreen(const glm::vec2 &world_pos) const
    {
        return world_pos - position_;
    }

    glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2 &world_pos, const glm::vec2 &scroll_factor) const
    {
        return world_pos - position_ * scroll_factor;
    }

    glm::vec2 Camera::screenToWorld(const glm::vec2 &screen_pos) const
    {
        return screen_pos + position_;
    }

    void Camera::setPosition(const glm::vec2 &position)
    {
        position_ = position;
        clampPosition();
    }

    void Camera::setLimitBounds(const engine::utils::Rect &bounds)
    {
        limit_bounds_ = bounds;
        clampPosition();
    }

    const glm::vec2 &Camera::getPosition() const
    {
        return position_;
    }

    std::optional<engine::utils::Rect> Camera::getLimitBounds() const
    {
        return limit_bounds_;
    }

    glm::vec2 Camera::getViewportSize() const
    {
        return viewport_size_;
    }

    void Camera::clampPosition()
    {
        if (limit_bounds_.has_value() && limit_bounds_.value().size.x > 0 && limit_bounds_.value().size.y > 0)
        {
            // 计算允许的相机位置范围
            glm::vec2 min_cam_pos = limit_bounds_->position;
            glm::vec2 max_cam_pos = limit_bounds_->position + limit_bounds_->size - viewport_size_;

            // 确保 max_cam_pos 不小于 min_cam_pos (视口可能比世界还大)
            max_cam_pos.x = std::max(min_cam_pos.x, max_cam_pos.x);
            max_cam_pos.y = std::max(min_cam_pos.y, max_cam_pos.y);

            position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
        }
    }

}