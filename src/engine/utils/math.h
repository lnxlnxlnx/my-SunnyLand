/*** 
 * @Date: 2026-05-08 20:06:24
 * @LastEditors: lnx a16725798566@163.com
 * @LastEditTime: 2026-05-08 20:07:56
 * @FilePath: /SunnyLand/src/engine/utils/math.h
 * @Description: 
 */
#ifndef D242D39F_D791_4356_883B_EDA8EC5DD40F
#define D242D39F_D791_4356_883B_EDA8EC5DD40F
#include <glm/glm.hpp> // 用于 glm::vec2 和其他数学类型

namespace engine::utils
{
    struct Rect final
    {
        glm::vec2 position;
        glm::vec2 size;
    };
}

#endif /* D242D39F_D791_4356_883B_EDA8EC5DD40F */
