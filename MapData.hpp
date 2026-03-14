// MapData.hpp
#ifndef MAP_DATA_HPP
#define MAP_DATA_HPP

#include <string>
#include <utility>
using namespace std;

const int MAP_WIDTH = 10;
const int MAP_HEIGHT = 10;

enum class TeleportType {
    NEWBIE_VILLAGE = 0,
    FIRE_LAND = 1,
    GOBLIN_KINGDOM = 2,
    FROZEN_NIGHT = 3,
    CELESTIAL_SITE = 4,
    LICH_CAVE = 5
};

inline string getLocationName(int x, int y) {
    if (x >= 3 && x <= 5 && y >= 3 && y <= 5) {
        if (x == 4 && y == 4) return "新手村中心";
        else return "新手村周边";
    }
    if (y >= 7 && x >= 3 && x <= 6) return "极炎之地";
    if (y <= 2 && x >= 3 && x <= 6) return "哥布林王国";
    if (x <= 2 && y >= 3 && y <= 6) return "无尽寒夜";
    if (x >= 7 && y >= 3 && y <= 6) return "天璇圣地";
    if (x == 4 && y == 0) return "巫妖王洞穴入口";
    if (x == 4 && y == 1) return "巫妖王洞穴前厅";
    return "未知区域";
}

inline bool isInSafeZone(int x, int y) {
    return (x == 4 && y == 4);
}

inline pair<int, int> getTeleportLocation(TeleportType teleportType) {
    switch (teleportType) {
        case TeleportType::NEWBIE_VILLAGE: return {4, 4};
        case TeleportType::FIRE_LAND: return {4, 7};
        case TeleportType::GOBLIN_KINGDOM: return {4, 2};
        case TeleportType::FROZEN_NIGHT: return {2, 4};
        case TeleportType::CELESTIAL_SITE: return {7, 4};
        case TeleportType::LICH_CAVE: return {4, 1};
    }
    return {4, 4};
}

#endif // MAP_DATA_HPP
