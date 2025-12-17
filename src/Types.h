#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <cmath>
#include <memory>

// ============================================================================
// 遊戲常數定義
// ============================================================================
namespace GameConstants {
    // 視窗設定
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 768;
    
    // 遊戲設定
    constexpr int MAP_WIDTH = 2000;
    constexpr int MAP_HEIGHT = 1500;
    constexpr int TILE_SIZE = 50;
    
    // 角色設定
    constexpr int HERO_SIZE = 40;
    constexpr int MONSTER_SIZE = 35;
    constexpr float HERO_SPEED = 5.0f;
    constexpr float MONSTER_SPEED = 1.5f;
    
    // 戰鬥設定
    constexpr int BASE_HP = 100;
    constexpr int HP_PER_LEVEL = 20;
    constexpr int BASE_ATTACK = 10;
    constexpr int ATTACK_PER_LEVEL = 5;
    constexpr int ATTACK_RANGE = 60;
    
    // 怪獸數量
    constexpr int INITIAL_MONSTER_COUNT = 15;
}

// ============================================================================
// 列舉型別
// ============================================================================

// 武器類型
enum class WeaponType {
    None,
    Sword,  // 劍：攻速快，傷害中等
    Axe     // 斧頭：攻速慢，傷害高
};

// 遊戲狀態
enum class GameState {
    WeaponSelect,   // 選擇武器
    Playing,        // 遊戲進行中
    GameOver,       // 遊戲結束
    Victory         // 勝利
};

// 方向
enum class Direction {
    None,
    Up,
    Down,
    Left,
    Right
};

// ============================================================================
// 基礎結構
// ============================================================================

// 2D 向量
struct Vector2D {
    float x;
    float y;
    
    Vector2D() : x(0), y(0) {}
    Vector2D(float _x, float _y) : x(_x), y(_y) {}
    
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    float Length() const {
        return std::sqrt(x * x + y * y);
    }
    
    float DistanceTo(const Vector2D& other) const {
        return (*this - other).Length();
    }
    
    Vector2D Normalize() const {
        float len = Length();
        if (len > 0) {
            return Vector2D(x / len, y / len);
        }
        return Vector2D(0, 0);
    }
};

// 武器屬性
struct WeaponStats {
    WeaponType type;
    std::wstring name;
    int damage;           // 傷害加成
    int attackSpeed;      // 攻擊間隔（毫秒）
    COLORREF color;       // 武器顏色
    
    WeaponStats() : type(WeaponType::None), damage(0), attackSpeed(0), color(RGB(128, 128, 128)) {}
    
    static WeaponStats GetSword() {
        WeaponStats stats;
        stats.type = WeaponType::Sword;
        stats.name = L"長劍";
        stats.damage = 15;
        stats.attackSpeed = 500;  // 0.5秒攻擊一次
        stats.color = RGB(192, 192, 192);  // 銀色
        return stats;
    }
    
    static WeaponStats GetAxe() {
        WeaponStats stats;
        stats.type = WeaponType::Axe;
        stats.name = L"戰斧";
        stats.damage = 30;
        stats.attackSpeed = 1000;  // 1秒攻擊一次
        stats.color = RGB(139, 69, 19);  // 棕色
        return stats;
    }
};
