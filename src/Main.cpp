// ============================================================================
// Hero War - 英雄戰爭
// 
// 一個使用 Win32 API 和 GDI 繪圖的 C++ 遊戲範例
// 設計用於練習 GitHub Copilot 功能
//
// 功能特色：
// - 向量圖形構成的英雄與怪獸
// - 等級與血量顯示系統
// - 武器選擇（劍 vs 斧頭）
// - 升級與戰鬥系統
// ============================================================================

#include <windows.h>
#include "Game.h"

// 全域變數
Game* g_pGame = nullptr;
const wchar_t* WINDOW_CLASS = L"HeroWarClass";
const wchar_t* WINDOW_TITLE = L"Hero War 英雄戰爭";

// ============================================================================
// 視窗程序
// ============================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            // 建立遊戲物件
            g_pGame = new Game();
            if (!g_pGame->Initialize(hWnd)) {
                MessageBox(hWnd, L"遊戲初始化失敗！", L"錯誤", MB_ICONERROR);
                PostQuitMessage(1);
                return -1;
            }
            // 設定計時器（約60FPS）
            SetTimer(hWnd, 1, 16, NULL);
            return 0;
            
        case WM_TIMER:
            // 遊戲迴圈
            if (g_pGame) {
                g_pGame->Update();
                InvalidateRect(hWnd, NULL, FALSE);
            }
            return 0;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (g_pGame) {
                g_pGame->Render(hdc);
            }
            EndPaint(hWnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN:
            if (g_pGame) {
                g_pGame->HandleKeyDown(wParam);
            }
            // ESC 鍵退出
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            return 0;
            
        case WM_KEYUP:
            if (g_pGame) {
                g_pGame->HandleKeyUp(wParam);
            }
            return 0;
            
        case WM_DESTROY:
            KillTimer(hWnd, 1);
            if (g_pGame) {
                delete g_pGame;
                g_pGame = nullptr;
            }
            PostQuitMessage(0);
            return 0;
            
        case WM_ERASEBKGND:
            // 防止閃爍
            return 1;
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// ============================================================================
// 程式進入點
// ============================================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    LPWSTR lpCmdLine, int nCmdShow) {
    // 註冊視窗類別
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"視窗類別註冊失敗！", L"錯誤", MB_ICONERROR);
        return 1;
    }
    
    // 計算視窗大小（含邊框）
    RECT windowRect = { 0, 0, GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
    // 置中顯示
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;
    
    // 建立視窗
    HWND hWnd = CreateWindowEx(
        0,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,  // 固定大小視窗
        posX, posY,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (!hWnd) {
        MessageBox(NULL, L"視窗建立失敗！", L"錯誤", MB_ICONERROR);
        return 1;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    // 訊息迴圈
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

// ============================================================================
// GitHub Copilot 練習提示
// 
// 以下是一些可以用 Copilot 練習的功能擴展：
//
// 1. 新增魔法攻擊：
//    // Add a magic attack that deals area damage to all nearby monsters
//
// 2. 新增道具系統：
//    // Create a potion item that heals the hero
//
// 3. 新增技能冷卻顯示：
//    // Draw a cooldown indicator for the attack skill
//
// 4. 新增怪獸 AI：
//    // Make monsters chase the hero when within a certain range
//
// 5. 新增音效：
//    // Play a sound effect when attacking
//
// 試著在上面的註解後面按 Tab，看看 Copilot 會建議什麼！
// ============================================================================
