#include "Game.h"
#include <cstdlib>
#include <ctime>

using namespace GameConstants;

Game::Game()
    : gameState_(GameState::WeaponSelect)
    , lastUpdateTime_(0)
    , memDC_(nullptr)
    , memBitmap_(nullptr)
    , oldBitmap_(nullptr)
    , bufferWidth_(0)
    , bufferHeight_(0)
    , fps_(0)
    , frameCount_(0)
    , fpsTimer_(0)
{
    srand((unsigned int)time(nullptr));
    
    ZeroMemory(keyStates_, sizeof(keyStates_));
    
    cameraOffset_ = Vector2D(0, 0);
}

Game::~Game() {
    DeleteBackBuffer();
}

bool Game::Initialize(HWND hWnd) {
    CreateBackBuffer(hWnd);
    
    Vector2D heroPos((float)MAP_WIDTH / 2, (float)MAP_HEIGHT / 2);
    hero_ = std::make_unique<Hero>(heroPos);
    
    InitializeMonsters();
    
    lastUpdateTime_ = GetTickCount();
    fpsTimer_ = lastUpdateTime_;
    
    return true;
}

void Game::InitializeMonsters() {
    monsters_.clear();
    
    for (int i = 0; i < INITIAL_MONSTER_COUNT; i++) {
        Vector2D pos;
        do {
            pos.x = (float)(rand() % (MAP_WIDTH - 100) + 50);
            pos.y = (float)(rand() % (MAP_HEIGHT - 100) + 50);
        } while (hero_ && pos.DistanceTo(hero_->GetPosition()) < 200);
        
        int level;
        int roll = rand() % 100;
        if (roll < 40) level = 1;
        else if (roll < 65) level = 2;
        else if (roll < 80) level = 3;
        else if (roll < 90) level = 4;
        else if (roll < 95) level = 5;
        else level = 6 + rand() % 4;
        
        monsters_.push_back(std::make_unique<Monster>(pos, level));
    }
}

void Game::CreateBackBuffer(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    
    bufferWidth_ = WINDOW_WIDTH;
    bufferHeight_ = WINDOW_HEIGHT;
    
    memDC_ = CreateCompatibleDC(hdc);
    memBitmap_ = CreateCompatibleBitmap(hdc, bufferWidth_, bufferHeight_);
    oldBitmap_ = (HBITMAP)SelectObject(memDC_, memBitmap_);
    
    ReleaseDC(hWnd, hdc);
}

void Game::DeleteBackBuffer() {
    if (memDC_) {
        SelectObject(memDC_, oldBitmap_);
        DeleteObject(memBitmap_);
        DeleteDC(memDC_);
        memDC_ = nullptr;
    }
}

void Game::Update() {
    DWORD currentTime = GetTickCount();
    float deltaTime = (currentTime - lastUpdateTime_) / 1000.0f;
    lastUpdateTime_ = currentTime;
    
    frameCount_++;
    if (currentTime - fpsTimer_ >= 1000) {
        fps_ = frameCount_;
        frameCount_ = 0;
        fpsTimer_ = currentTime;
    }
    
    switch (gameState_) {
        case GameState::WeaponSelect:
            if (IsKeyPressed('1')) {
                hero_->SetWeapon(WeaponType::Sword);
                gameState_ = GameState::Playing;
            } else if (IsKeyPressed('2')) {
                hero_->SetWeapon(WeaponType::Axe);
                gameState_ = GameState::Playing;
            }
            break;
            
        case GameState::Playing:
            UpdatePlaying(deltaTime);
            break;
            
        case GameState::GameOver:
        case GameState::Victory:
            if (IsKeyPressed(VK_SPACE) || IsKeyPressed(VK_RETURN)) {
                hero_ = std::make_unique<Hero>(
                    Vector2D((float)MAP_WIDTH / 2, (float)MAP_HEIGHT / 2)
                );
                InitializeMonsters();
                gameState_ = GameState::WeaponSelect;
            }
            break;
    }
}

void Game::UpdatePlaying(float deltaTime) {
    if (IsKeyPressed(VK_UP) || IsKeyPressed('W')) {
        hero_->Move(Direction::Up);
    }
    if (IsKeyPressed(VK_DOWN) || IsKeyPressed('S')) {
        hero_->Move(Direction::Down);
    }
    if (IsKeyPressed(VK_LEFT) || IsKeyPressed('A') == false && IsKeyPressed(VK_LEFT)) {
        hero_->Move(Direction::Left);
    }
    if (IsKeyPressed(VK_RIGHT) || IsKeyPressed('D')) {
        hero_->Move(Direction::Right);
    }
    
    if (IsKeyPressed('A')) {
        CheckAttack();
    } else {
        hero_->EndAttack();
    }
    
    for (auto& monster : monsters_) {
        if (monster->IsAlive()) {
            monster->Update(deltaTime);
        }
    }
    
    UpdateCamera();
    
    CheckGameOver();
}

void Game::CheckAttack() {
    if (!hero_->CanAttack()) return;
    
    int damage = hero_->PerformAttack();
    
    for (auto& monster : monsters_) {
        if (!monster->IsAlive()) continue;
        
        float distance = hero_->GetPosition().DistanceTo(monster->GetPosition());
        if (distance <= ATTACK_RANGE) {
            monster->TakeDamage(damage);
            
            if (!monster->IsAlive()) {
                hero_->GainExperience(monster->GetExperienceReward());
                hero_->AddKill();
            }
            break;
        }
    }
}

void Game::UpdateCamera() {
    Vector2D heroPos = hero_->GetPosition();
    
    cameraOffset_.x = heroPos.x - WINDOW_WIDTH / 2.0f;
    cameraOffset_.y = heroPos.y - WINDOW_HEIGHT / 2.0f;
    
    if (cameraOffset_.x < 0) cameraOffset_.x = 0;
    if (cameraOffset_.y < 0) cameraOffset_.y = 0;
    if (cameraOffset_.x > MAP_WIDTH - WINDOW_WIDTH) 
        cameraOffset_.x = (float)(MAP_WIDTH - WINDOW_WIDTH);
    if (cameraOffset_.y > MAP_HEIGHT - WINDOW_HEIGHT) 
        cameraOffset_.y = (float)(MAP_HEIGHT - WINDOW_HEIGHT);
}

void Game::CheckGameOver() {
    if (!hero_->IsAlive()) {
        gameState_ = GameState::GameOver;
        return;
    }
    
    bool allDead = true;
    for (const auto& monster : monsters_) {
        if (monster->IsAlive()) {
            allDead = false;
            break;
        }
    }
    
    if (allDead) {
        gameState_ = GameState::Victory;
    }
}

void Game::HandleKeyDown(WPARAM key) {
    if (key < 256) {
        keyStates_[key] = true;
    }
}

void Game::HandleKeyUp(WPARAM key) {
    if (key < 256) {
        keyStates_[key] = false;
    }
}

bool Game::IsKeyPressed(int key) const {
    return key < 256 && keyStates_[key];
}

void Game::Render(HDC hdc) {
    if (!memDC_) return;
    
    RECT rect = { 0, 0, bufferWidth_, bufferHeight_ };
    HBRUSH bgBrush = CreateSolidBrush(RGB(40, 40, 50));
    FillRect(memDC_, &rect, bgBrush);
    DeleteObject(bgBrush);
    
    switch (gameState_) {
        case GameState::WeaponSelect:
            DrawWeaponSelect(memDC_);
            break;
        case GameState::Playing:
            DrawGame(memDC_);
            break;
        case GameState::GameOver:
            DrawGame(memDC_);
            DrawGameOver(memDC_);
            break;
        case GameState::Victory:
            DrawGame(memDC_);
            DrawVictory(memDC_);
            break;
    }
    
    BitBlt(hdc, 0, 0, bufferWidth_, bufferHeight_, memDC_, 0, 0, SRCCOPY);
}

void Game::DrawWeaponSelect(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);
    
    HFONT titleFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
    
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOut(hdc, WINDOW_WIDTH / 2, 100, L"⚔ HERO WAR ⚔", 14);
    
    SetTextColor(hdc, RGB(200, 200, 200));
    HFONT subtitleFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                     CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, subtitleFont);
    TextOut(hdc, WINDOW_WIDTH / 2, 160, L"英 雄 戰 爭", 5);
    
    HFONT menuFont = CreateFont(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                 CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, menuFont);
    
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, WINDOW_WIDTH / 2, 280, L"選擇你的武器", 6);
    
    int optionY = 350;
    SetTextColor(hdc, RGB(192, 192, 192));
    TextOut(hdc, WINDOW_WIDTH / 2, optionY, L"[1] 長劍", 6);
    
    HFONT descFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                 CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, descFont);
    SetTextColor(hdc, RGB(150, 150, 150));
    TextOut(hdc, WINDOW_WIDTH / 2, optionY + 35, L"攻擊快速 | 傷害: +15 | 攻速: 0.5秒", 20);
    
    HPEN swordPen = CreatePen(PS_SOLID, 3, RGB(192, 192, 192));
    HPEN oldPen = (HPEN)SelectObject(hdc, swordPen);
    MoveToEx(hdc, WINDOW_WIDTH / 2 - 100, optionY + 15, NULL);
    LineTo(hdc, WINDOW_WIDTH / 2 - 60, optionY + 15);
    HPEN hiltPen = CreatePen(PS_SOLID, 2, RGB(139, 69, 19));
    SelectObject(hdc, hiltPen);
    MoveToEx(hdc, WINDOW_WIDTH / 2 - 100, optionY + 10, NULL);
    LineTo(hdc, WINDOW_WIDTH / 2 - 100, optionY + 20);
    
    optionY = 450;
    SelectObject(hdc, menuFont);
    SetTextColor(hdc, RGB(139, 69, 19));
    TextOut(hdc, WINDOW_WIDTH / 2, optionY, L"[2] 戰斧", 6);
    
    SelectObject(hdc, descFont);
    SetTextColor(hdc, RGB(150, 150, 150));
    TextOut(hdc, WINDOW_WIDTH / 2, optionY + 35, L"傷害強大 | 傷害: +30 | 攻速: 1.0秒", 20);
    
    HPEN axePen = CreatePen(PS_SOLID, 3, RGB(139, 69, 19));
    SelectObject(hdc, axePen);
    MoveToEx(hdc, WINDOW_WIDTH / 2 - 100, optionY + 15, NULL);
    LineTo(hdc, WINDOW_WIDTH / 2 - 65, optionY + 15);
    
    HBRUSH axeBrush = CreateSolidBrush(RGB(100, 100, 100));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, axeBrush);
    POINT axeHead[4] = {
        { WINDOW_WIDTH / 2 - 65, optionY + 5 },
        { WINDOW_WIDTH / 2 - 55, optionY + 15 },
        { WINDOW_WIDTH / 2 - 65, optionY + 25 },
        { WINDOW_WIDTH / 2 - 65, optionY + 5 }
    };
    Polygon(hdc, axeHead, 4);
    
    SelectObject(hdc, descFont);
    SetTextColor(hdc, RGB(100, 100, 100));
    TextOut(hdc, WINDOW_WIDTH / 2, 580, L"操作說明：方向鍵移動 | A鍵攻擊 | ESC退出", 23);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldFont);
    SelectObject(hdc, oldBrush);
    DeleteObject(titleFont);
    DeleteObject(subtitleFont);
    DeleteObject(menuFont);
    DeleteObject(descFont);
    DeleteObject(swordPen);
    DeleteObject(hiltPen);
    DeleteObject(axePen);
    DeleteObject(axeBrush);
}

void Game::DrawGame(HDC hdc) {
    DrawBackground(hdc);
    
    for (const auto& monster : monsters_) {
        if (monster->IsAlive()) {
            monster->Draw(hdc, cameraOffset_);
            monster->DrawStatus(hdc, cameraOffset_);
        }
    }
    
    if (hero_->IsAlive()) {
        hero_->Draw(hdc, cameraOffset_);
        hero_->DrawStatus(hdc, cameraOffset_);
    }
    
    if (hero_->IsAttacking()) {
        HPEN rangePen = CreatePen(PS_DOT, 1, RGB(255, 100, 100));
        HPEN oldPen = (HPEN)SelectObject(hdc, rangePen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        
        int screenX = (int)(hero_->GetPosition().x - cameraOffset_.x);
        int screenY = (int)(hero_->GetPosition().y - cameraOffset_.y);
        Ellipse(hdc, screenX - ATTACK_RANGE, screenY - ATTACK_RANGE,
                screenX + ATTACK_RANGE, screenY + ATTACK_RANGE);
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
        DeleteObject(rangePen);
    }
    
    DrawMinimap(hdc);
    
    DrawHUD(hdc);
}

void Game::DrawBackground(HDC hdc) {
    HBRUSH grass1 = CreateSolidBrush(RGB(50, 120, 50));
    HBRUSH grass2 = CreateSolidBrush(RGB(45, 110, 45));
    
    int startTileX = (int)(cameraOffset_.x / TILE_SIZE);
    int startTileY = (int)(cameraOffset_.y / TILE_SIZE);
    int endTileX = startTileX + (WINDOW_WIDTH / TILE_SIZE) + 2;
    int endTileY = startTileY + (WINDOW_HEIGHT / TILE_SIZE) + 2;
    
    for (int ty = startTileY; ty < endTileY; ty++) {
        for (int tx = startTileX; tx < endTileX; tx++) {
            int screenX = tx * TILE_SIZE - (int)cameraOffset_.x;
            int screenY = ty * TILE_SIZE - (int)cameraOffset_.y;
            
            RECT tileRect = { screenX, screenY, screenX + TILE_SIZE, screenY + TILE_SIZE };
            FillRect(hdc, &tileRect, ((tx + ty) % 2 == 0) ? grass1 : grass2);
        }
    }
    
    DeleteObject(grass1);
    DeleteObject(grass2);
    
    HPEN borderPen = CreatePen(PS_SOLID, 3, RGB(100, 50, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    
    RECT mapRect = {
        -(int)cameraOffset_.x,
        -(int)cameraOffset_.y,
        MAP_WIDTH - (int)cameraOffset_.x,
        MAP_HEIGHT - (int)cameraOffset_.y
    };
    Rectangle(hdc, mapRect.left, mapRect.top, mapRect.right, mapRect.bottom);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(borderPen);
}

void Game::DrawMinimap(HDC hdc) {
    int mapWidth = 150;
    int mapHeight = 112;
    int mapX = WINDOW_WIDTH - mapWidth - 10;
    int mapY = 10;
    float scaleX = (float)mapWidth / MAP_WIDTH;
    float scaleY = (float)mapHeight / MAP_HEIGHT;
    
    HBRUSH bgBrush = CreateSolidBrush(RGB(30, 30, 30));
    RECT bgRect = { mapX - 2, mapY - 2, mapX + mapWidth + 2, mapY + mapHeight + 2 };
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    HBRUSH mapBrush = CreateSolidBrush(RGB(50, 80, 50));
    RECT mapRect = { mapX, mapY, mapX + mapWidth, mapY + mapHeight };
    FillRect(hdc, &mapRect, mapBrush);
    DeleteObject(mapBrush);
    
    for (const auto& monster : monsters_) {
        if (!monster->IsAlive()) continue;
        
        int dotX = mapX + (int)(monster->GetPosition().x * scaleX);
        int dotY = mapY + (int)(monster->GetPosition().y * scaleY);
        
        HBRUSH dotBrush = CreateSolidBrush(RGB(255, 0, 0));
        RECT dotRect = { dotX - 2, dotY - 2, dotX + 2, dotY + 2 };
        FillRect(hdc, &dotRect, dotBrush);
        DeleteObject(dotBrush);
    }
    
    int heroX = mapX + (int)(hero_->GetPosition().x * scaleX);
    int heroY = mapY + (int)(hero_->GetPosition().y * scaleY);
    
    HBRUSH heroBrush = CreateSolidBrush(RGB(0, 150, 255));
    Ellipse(hdc, heroX - 4, heroY - 4, heroX + 4, heroY + 4);
    DeleteObject(heroBrush);
    
    HPEN viewPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(hdc, viewPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    
    int viewX = mapX + (int)(cameraOffset_.x * scaleX);
    int viewY = mapY + (int)(cameraOffset_.y * scaleY);
    int viewW = (int)(WINDOW_WIDTH * scaleX);
    int viewH = (int)(WINDOW_HEIGHT * scaleY);
    Rectangle(hdc, viewX, viewY, viewX + viewW, viewY + viewH);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(viewPen);
}

void Game::DrawHUD(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_LEFT);
    
    HFONT hudFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, hudFont);
    
    int y = 10;
    const int lineHeight = 22;
    
    SetTextColor(hdc, RGB(255, 215, 0));
    wchar_t text[64];
    swprintf_s(text, L"Lv. %d", hero_->GetLevel());
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    SetTextColor(hdc, RGB(100, 255, 100));
    swprintf_s(text, L"HP: %d / %d", hero_->GetCurrentHp(), hero_->GetMaxHp());
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    SetTextColor(hdc, RGB(255, 150, 100));
    swprintf_s(text, L"ATK: %d + %d", hero_->GetAttack(), hero_->GetWeapon().damage);
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    SetTextColor(hdc, RGB(200, 200, 200));
    swprintf_s(text, L"武器: %s", hero_->GetWeapon().name.c_str());
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    SetTextColor(hdc, RGB(255, 100, 100));
    swprintf_s(text, L"擊殺: %d", hero_->GetKills());
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    int aliveMonsters = 0;
    for (const auto& m : monsters_) {
        if (m->IsAlive()) aliveMonsters++;
    }
    SetTextColor(hdc, RGB(255, 200, 100));
    swprintf_s(text, L"剩餘怪獸: %d", aliveMonsters);
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    y += lineHeight;
    
    SetTextColor(hdc, RGB(100, 100, 100));
    swprintf_s(text, L"FPS: %d", fps_);
    TextOut(hdc, 10, y, text, (int)wcslen(text));
    
    SetTextAlign(hdc, TA_CENTER);
    SetTextColor(hdc, RGB(150, 150, 150));
    HFONT tipFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, tipFont);
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT - 25, 
            L"方向鍵：移動 | A：攻擊 | ESC：退出", 19);
    
    SelectObject(hdc, oldFont);
    DeleteObject(hudFont);
    DeleteObject(tipFont);
}

void Game::DrawGameOver(HDC hdc) {
    HBRUSH overlayBrush = CreateSolidBrush(RGB(0, 0, 0));
    RECT overlayRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);
    
    HFONT titleFont = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
    
    SetTextColor(hdc, RGB(200, 0, 0));
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50, L"GAME OVER", 9);
    
    HFONT subFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, subFont);
    
    SetTextColor(hdc, RGB(255, 255, 255));
    wchar_t text[64];
    swprintf_s(text, L"最終等級: %d | 擊殺數: %d", hero_->GetLevel(), hero_->GetKills());
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 30, text, (int)wcslen(text));
    
    SetTextColor(hdc, RGB(150, 150, 150));
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 70, L"按 SPACE 或 ENTER 重新開始", 16);
    
    SelectObject(hdc, oldFont);
    DeleteObject(titleFont);
    DeleteObject(subFont);
    DeleteObject(overlayBrush);
}

void Game::DrawVictory(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);
    
    HFONT titleFont = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, titleFont);
    
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50, L"VICTORY!", 8);
    
    HFONT subFont = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    SelectObject(hdc, subFont);
    
    SetTextColor(hdc, RGB(255, 255, 255));
    wchar_t text[64];
    swprintf_s(text, L"最終等級: %d | 消滅所有怪獸!", hero_->GetLevel());
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 30, text, (int)wcslen(text));
    
    SetTextColor(hdc, RGB(150, 150, 150));
    TextOut(hdc, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 70, L"按 SPACE 或 ENTER 再次挑戰", 15);
    
    SelectObject(hdc, oldFont);
    DeleteObject(titleFont);
    DeleteObject(subFont);
}
