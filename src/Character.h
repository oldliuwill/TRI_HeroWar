#pragma once
#include "Types.h"

// ============================================================================
// 角色基底類別
// ============================================================================
class Character {
protected:
    Vector2D position_;      // 位置
    int level_;              // 等級
    int maxHp_;              // 最大生命值
    int currentHp_;          // 當前生命值
    int attack_;             // 攻擊力
    float speed_;            // 移動速度
    int size_;               // 角色大小
    bool isAlive_;           // 是否存活
    Direction facing_;       // 面向方向
    
public:
    Character(Vector2D pos, int level, float speed, int size);
    virtual ~Character() = default;
    
    // 基本屬性存取
    Vector2D GetPosition() const { return position_; }
    void SetPosition(Vector2D pos) { position_ = pos; }
    int GetLevel() const { return level_; }
    int GetMaxHp() const { return maxHp_; }
    int GetCurrentHp() const { return currentHp_; }
    int GetAttack() const { return attack_; }
    int GetSize() const { return size_; }
    bool IsAlive() const { return isAlive_; }
    Direction GetFacing() const { return facing_; }
    
    // 行為方法
    virtual void Move(Direction dir);
    virtual void TakeDamage(int damage);
    virtual void Update(float deltaTime);
    
    // 繪製方法（純虛函數，由子類別實作）
    virtual void Draw(HDC hdc, Vector2D cameraOffset) = 0;
    virtual void DrawStatus(HDC hdc, Vector2D cameraOffset) = 0;
    
    // 計算與其他角色的距離
    float DistanceTo(const Character& other) const;
    
    // 碰撞檢測
    bool IsCollidingWith(const Character& other) const;
};

// ============================================================================
// 英雄類別
// ============================================================================
class Hero : public Character {
private:
    WeaponStats weapon_;         // 武器
    int experience_;             // 經驗值
    DWORD lastAttackTime_;       // 上次攻擊時間
    bool isAttacking_;           // 是否正在攻擊
    int kills_;                  // 擊殺數
    
public:
    Hero(Vector2D pos);
    
    // 武器相關
    void SetWeapon(WeaponType type);
    WeaponStats GetWeapon() const { return weapon_; }
    
    // 攻擊相關
    bool CanAttack() const;
    int PerformAttack();
    void StartAttack() { isAttacking_ = true; }
    void EndAttack() { isAttacking_ = false; }
    bool IsAttacking() const { return isAttacking_; }
    
    // 升級相關
    void GainExperience(int exp);
    void LevelUp();
    int GetKills() const { return kills_; }
    void AddKill() { kills_++; }
    
    // 繪製
    void Draw(HDC hdc, Vector2D cameraOffset) override;
    void DrawStatus(HDC hdc, Vector2D cameraOffset) override;
    void DrawWeapon(HDC hdc, Vector2D screenPos);
};

// ============================================================================
// 怪獸類別
// ============================================================================
class Monster : public Character {
private:
    COLORREF bodyColor_;         // 身體顏色
    int experienceReward_;       // 擊殺獎勵經驗
    float wanderTimer_;          // 漫遊計時器
    Direction wanderDirection_;  // 漫遊方向
    
public:
    Monster(Vector2D pos, int level);
    
    // 怪獸特有行為
    void Wander(float deltaTime);
    int GetExperienceReward() const { return experienceReward_; }
    
    // 根據等級生成顏色
    static COLORREF GetColorByLevel(int level);
    
    // 繪製
    void Draw(HDC hdc, Vector2D cameraOffset) override;
    void DrawStatus(HDC hdc, Vector2D cameraOffset) override;
    
    // 更新
    void Update(float deltaTime) override;
};
