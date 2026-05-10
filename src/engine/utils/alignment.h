#ifndef D237DF79_3020_4A8A_A510_4A160E0B0E30
#define D237DF79_3020_4A8A_A510_4A160E0B0E30
// engine/utils/alignment.h
namespace engine::utils {

/**
 * @enum class Alignment
 * @brief 定义对象或组件相对于其位置参考点的对齐方式。
 */
enum class Alignment {
    NONE,           // 不指定对齐方式，偏移量通常为 (0,0) 或手动设置
    TOP_LEFT,       // 左上角
    TOP_CENTER,     // 顶部中心
    TOP_RIGHT,      // 右上角
    CENTER_LEFT,    // 中心左侧
    CENTER,         // 正中心 (几何中心)
    CENTER_RIGHT,   // 中心右侧
    BOTTOM_LEFT,    // 左下角
    BOTTOM_CENTER,  // 底部中心
    BOTTOM_RIGHT    // 右下角
};

} // namespace engine::utils

#endif /* D237DF79_3020_4A8A_A510_4A160E0B0E30 */
