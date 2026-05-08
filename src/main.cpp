/*** 
 * @Date: 2026-05-06 21:57:20
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-08 15:03:46
 * @FilePath: /SunnyLand/src/main.cpp
 * @Description: 
 */
#include "engine/core/game_app.h"
#include <spdlog/spdlog.h>

int main(int /* argc */, char* /* argv */[]) {
    spdlog::set_level(spdlog::level::trace); // 设置全局日志级别为 trace，记录所有日志
    engine::core::GameApp app;
    app.run();
    return 0;
}