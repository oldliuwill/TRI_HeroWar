#pragma once
#include "Character.h"
#include <vector>
#include <memory>

// ============================================================================
// 遊戲主類別
// ============================================================================
class Game {
private:
    // 遊戲物件
    std::unique_ptr<Hero> hero_;
    std::vector<std::unique_ptr<Monster>> monsters_;
    
    // 遊戲狀態
    GameState gameState_;
    Vector2D cameraOffset_;
    DWORD lastUpdateTime_;
    
    // 輸入狀態
    bool keyStates_[256];
    
    // 雙緩衝繪圖
    HDC memDC_;
    HBITMAP memBitmap_;
    HBITMAP oldBitmap_;
    int bufferWidth_;
    int bufferHeight_;
    
    // 統計資訊
    int fps_;
    int frameCount_;
    DWORD fpsTimer_;
    
public:
    Game();
    ~Game();
    
    // 初始化
    bool Initialize(HWND hWnd);
    void InitializeMonsters();
    
    // 遊戲迴圈
    void Update();
    void Render(HDC hdc);
    
    // 輸入處理
    void HandleKeyDown(WPARAM key);
    void HandleKeyUp(WPARAM key);
    bool IsKeyPressed(int key) const;
    
    // 遊戲邏輯
    void UpdatePlaying(float deltaTime);
    void CheckAttack();
    void UpdateCamera();
    void CheckGameOver();
    
    // 繪製方法
    void DrawWeaponSelect(HDC hdc);
    void DrawGame(HDC hdc);
    void DrawBackground(HDC hdc);
    void DrawMinimap(HDC hdc);
    void DrawHUD(HDC hdc);
    void DrawGameOver(HDC hdc);
    void DrawVictory(HDC hdc);
    
    // 工具方法
    void CreateBackBuffer(HWND hWnd);
    void DeleteBackBuffer();
    
    // 存取方法
    GameState GetState() const { return gameState_; }
};
