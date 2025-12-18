#include "Character.h"
#include <algorithm>
#include <cstdlib>

using namespace GameConstants;

Character::Character(Vector2D pos, int level, float speed, int size)
    : position_(pos)
    , level_(level)
    , speed_(speed)
    , size_(size)
    , isAlive_(true)
    , facing_(Direction::Right)
{
    maxHp_ = BASE_HP + (level - 1) * HP_PER_LEVEL;
    currentHp_ = maxHp_;
    attack_ = BASE_ATTACK + (level - 1) * ATTACK_PER_LEVEL;
}

void Character::Move(Direction dir) {
    if (!isAlive_) return;
    
    facing_ = dir;
    
    Vector2D newPos = position_;
    switch (dir) {
        case Direction::Up:
            newPos.y -= speed_;
            break;
        case Direction::Down:
            newPos.y += speed_;
            break;
        case Direction::Left:
            newPos.x -= speed_;
            break;
        case Direction::Right:
            newPos.x += speed_;
            break;
        default:
            break;
    }
    
    newPos.x = std::max(0.0f, std::min(newPos.x, (float)(MAP_WIDTH - size_)));
    newPos.y = std::max(0.0f, std::min(newPos.y, (float)(MAP_HEIGHT - size_)));
    
    position_ = newPos;
}

void Character::TakeDamage(int damage) {
    if (!isAlive_) return;
    
    currentHp_ -= damage;
    if (currentHp_ <= 0) {
        currentHp_ = 0;
        isAlive_ = false;
    }
}

void Character::Update(float deltaTime) {
}

float Character::DistanceTo(const Character& other) const {
    return position_.DistanceTo(other.position_);
}

bool Character::IsCollidingWith(const Character& other) const {
    float distance = DistanceTo(other);
    float combinedSize = (size_ + other.size_) / 2.0f;
    return distance < combinedSize;
}

Hero::Hero(Vector2D pos)
    : Character(pos, 1, HERO_SPEED, HERO_SIZE)
    , experience_(0)
    , lastAttackTime_(0)
    , isAttacking_(false)
    , kills_(0)
{
    weapon_.type = WeaponType::None;
}

void Hero::SetWeapon(WeaponType type) {
    switch (type) {
        case WeaponType::Sword:
            weapon_ = WeaponStats::GetSword();
            break;
        case WeaponType::Axe:
            weapon_ = WeaponStats::GetAxe();
            break;
        default:
            break;
    }
}

bool Hero::CanAttack() const {
    if (weapon_.type == WeaponType::None) return false;
    
    DWORD currentTime = GetTickCount();
    return (currentTime - lastAttackTime_) >= (DWORD)weapon_.attackSpeed;
}

int Hero::PerformAttack() {
    if (!CanAttack()) return 0;
    
    lastAttackTime_ = GetTickCount();
    isAttacking_ = true;
    
    return attack_ + weapon_.damage;
}

void Hero::GainExperience(int exp) {
    experience_ += exp;
    
    int expNeeded = level_ * 100;
    while (experience_ >= expNeeded) {
        experience_ -= expNeeded;
        LevelUp();
        expNeeded = level_ * 100;
    }
}

void Hero::LevelUp() {
    level_++;
    maxHp_ = BASE_HP + (level_ - 1) * HP_PER_LEVEL;
    currentHp_ = maxHp_;
    attack_ = BASE_ATTACK + (level_ - 1) * ATTACK_PER_LEVEL;
}

void Hero::Draw(HDC hdc, Vector2D cameraOffset) {
    if (!isAlive_) return;
    
    int screenX = (int)(position_.x - cameraOffset.x);
    int screenY = (int)(position_.y - cameraOffset.y);
    
    HBRUSH bodyBrush = CreateSolidBrush(RGB(0, 100, 200));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bodyBrush);
    Ellipse(hdc, screenX - size_/2, screenY - size_/2, 
            screenX + size_/2, screenY + size_/2);
    
    HBRUSH headBrush = CreateSolidBrush(RGB(255, 220, 180));
    SelectObject(hdc, headBrush);
    int headSize = size_ / 3;
    Ellipse(hdc, screenX - headSize, screenY - size_/2 - headSize*2,
            screenX + headSize, screenY - size_/2);
    
    HBRUSH eyeBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, eyeBrush);
    int eyeSize = 3;
    int eyeOffset = headSize / 2;
    int eyeX = screenX;
    if (facing_ == Direction::Left) eyeX -= eyeOffset/2;
    else if (facing_ == Direction::Right) eyeX += eyeOffset/2;
    
    Ellipse(hdc, eyeX - eyeOffset - eyeSize, screenY - size_/2 - headSize - eyeSize,
            eyeX - eyeOffset + eyeSize, screenY - size_/2 - headSize + eyeSize);
    Ellipse(hdc, eyeX + eyeOffset - eyeSize, screenY - size_/2 - headSize - eyeSize,
            eyeX + eyeOffset + eyeSize, screenY - size_/2 - headSize + eyeSize);
    
    DrawWeapon(hdc, Vector2D((float)screenX, (float)screenY));
    
    SelectObject(hdc, oldBrush);
    DeleteObject(bodyBrush);
    DeleteObject(headBrush);
    DeleteObject(eyeBrush);
}

void Hero::DrawWeapon(HDC hdc, Vector2D screenPos) {
    if (weapon_.type == WeaponType::None) return;
    
    HPEN weaponPen = CreatePen(PS_SOLID, 3, weapon_.color);
    HPEN oldPen = (HPEN)SelectObject(hdc, weaponPen);
    
    int weaponLength = 25;
    int startX = (int)screenPos.x;
    int startY = (int)screenPos.y;
    int endX = startX;
    int endY = startY;
    
    switch (facing_) {
        case Direction::Up:
            endY = startY - weaponLength;
            break;
        case Direction::Down:
            endY = startY + weaponLength;
            break;
        case Direction::Left:
            endX = startX - weaponLength;
            break;
        case Direction::Right:
        default:
            endX = startX + weaponLength;
            break;
    }
    
    MoveToEx(hdc, startX, startY, NULL);
    LineTo(hdc, endX, endY);
    
    if (weapon_.type == WeaponType::Axe) {
        HBRUSH axeBrush = CreateSolidBrush(RGB(100, 100, 100));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, axeBrush);
        
        POINT axeHead[4];
        int axeSize = 10;
        
        if (facing_ == Direction::Right || facing_ == Direction::Left) {
            axeHead[0] = { endX, endY - axeSize };
            axeHead[1] = { endX + (facing_ == Direction::Right ? axeSize : -axeSize), endY };
            axeHead[2] = { endX, endY + axeSize };
            axeHead[3] = { endX, endY - axeSize };
        } else {
            axeHead[0] = { endX - axeSize, endY };
            axeHead[1] = { endX, endY + (facing_ == Direction::Down ? axeSize : -axeSize) };
            axeHead[2] = { endX + axeSize, endY };
            axeHead[3] = { endX - axeSize, endY };
        }
        Polygon(hdc, axeHead, 4);
        
        SelectObject(hdc, oldBrush);
        DeleteObject(axeBrush);
    }
    
    if (weapon_.type == WeaponType::Sword) {
        HPEN hiltPen = CreatePen(PS_SOLID, 2, RGB(139, 69, 19));
        SelectObject(hdc, hiltPen);
        
        int hiltSize = 8;
        if (facing_ == Direction::Right || facing_ == Direction::Left) {
            MoveToEx(hdc, startX, startY - hiltSize, NULL);
            LineTo(hdc, startX, startY + hiltSize);
        } else {
            MoveToEx(hdc, startX - hiltSize, startY, NULL);
            LineTo(hdc, startX + hiltSize, startY);
        }
        
        DeleteObject(hiltPen);
    }
    
    SelectObject(hdc, oldPen);
    DeleteObject(weaponPen);
}

void Hero::DrawStatus(HDC hdc, Vector2D cameraOffset) {
    if (!isAlive_) return;
    
    int screenX = (int)(position_.x - cameraOffset.x);
    int screenY = (int)(position_.y - cameraOffset.y) - size_/2 - 35;
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);
    
    HFONT font = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    SetTextColor(hdc, RGB(255, 215, 0));
    wchar_t levelText[32];
    swprintf_s(levelText, L"Lv.%d ★", level_);
    TextOut(hdc, screenX, screenY, levelText, (int)wcslen(levelText));
    
    int barWidth = 50;
    int barHeight = 6;
    int barY = screenY + 15;
    
    HBRUSH bgBrush = CreateSolidBrush(RGB(60, 60, 60));
    RECT bgRect = { screenX - barWidth/2, barY, screenX + barWidth/2, barY + barHeight };
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    float hpRatio = (float)currentHp_ / maxHp_;
    int hpWidth = (int)(barWidth * hpRatio);
    COLORREF hpColor = hpRatio > 0.5f ? RGB(0, 200, 0) : 
                       hpRatio > 0.25f ? RGB(255, 165, 0) : RGB(200, 0, 0);
    HBRUSH hpBrush = CreateSolidBrush(hpColor);
    RECT hpRect = { screenX - barWidth/2, barY, screenX - barWidth/2 + hpWidth, barY + barHeight };
    FillRect(hdc, &hpRect, hpBrush);
    DeleteObject(hpBrush);
    
    SetTextColor(hdc, RGB(255, 255, 255));
    wchar_t hpText[32];
    swprintf_s(hpText, L"%d/%d", currentHp_, maxHp_);
    TextOut(hdc, screenX, barY + 8, hpText, (int)wcslen(hpText));
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

Monster::Monster(Vector2D pos, int level)
    : Character(pos, level, MONSTER_SPEED + level * 0.2f, MONSTER_SIZE)
    , wanderTimer_(0)
    , wanderDirection_(Direction::None)
{
    bodyColor_ = GetColorByLevel(level);
    experienceReward_ = level * 50;
}

COLORREF Monster::GetColorByLevel(int level) {
    switch (level) {
        case 1: return RGB(100, 200, 100);
        case 2: return RGB(50, 150, 50);
        case 3: return RGB(200, 200, 50);
        case 4: return RGB(255, 165, 0);
        case 5: return RGB(255, 100, 50);
        case 6: return RGB(200, 50, 50);
        case 7: return RGB(150, 0, 150);
        case 8: return RGB(100, 0, 100);
        case 9: return RGB(50, 50, 50);
        default: return RGB(0, 0, 0);
    }
}

void Monster::Wander(float deltaTime) {
    wanderTimer_ += deltaTime;
    
    if (wanderTimer_ >= 2.0f + (rand() % 20) / 10.0f) {
        wanderTimer_ = 0;
        int randDir = rand() % 5;
        wanderDirection_ = static_cast<Direction>(randDir);
    }
    
    if (wanderDirection_ != Direction::None) {
        Move(wanderDirection_);
    }
}

void Monster::Update(float deltaTime) {
    Character::Update(deltaTime);
    
    if (isAlive_) {
        Wander(deltaTime);
    }
}

void Monster::Draw(HDC hdc, Vector2D cameraOffset) {
    if (!isAlive_) return;
    
    int screenX = (int)(position_.x - cameraOffset.x);
    int screenY = (int)(position_.y - cameraOffset.y);
    
    HBRUSH bodyBrush = CreateSolidBrush(bodyColor_);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, bodyBrush);
    
    POINT body[6];
    int r = size_ / 2;
    for (int i = 0; i < 6; i++) {
        float angle = (float)i * 3.14159f / 3.0f - 3.14159f / 6.0f;
        body[i].x = screenX + (int)(r * cos(angle));
        body[i].y = screenY + (int)(r * sin(angle));
    }
    Polygon(hdc, body, 6);
    
    HBRUSH hornBrush = CreateSolidBrush(RGB(100, 50, 50));
    SelectObject(hdc, hornBrush);
    
    POINT leftHorn[3] = {
        { screenX - r/2, screenY - r/2 },
        { screenX - r/3, screenY - r - 10 },
        { screenX, screenY - r/2 }
    };
    Polygon(hdc, leftHorn, 3);
    
    POINT rightHorn[3] = {
        { screenX, screenY - r/2 },
        { screenX + r/3, screenY - r - 10 },
        { screenX + r/2, screenY - r/2 }
    };
    Polygon(hdc, rightHorn, 3);
    
    HBRUSH eyeBrush = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(hdc, eyeBrush);
    
    int eyeSize = 5;
    Ellipse(hdc, screenX - r/3 - eyeSize, screenY - eyeSize - 3,
            screenX - r/3 + eyeSize, screenY + eyeSize - 3);
    Ellipse(hdc, screenX + r/3 - eyeSize, screenY - eyeSize - 3,
            screenX + r/3 + eyeSize, screenY + eyeSize - 3);
    
    HPEN mouthPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, mouthPen);
    
    MoveToEx(hdc, screenX - r/3, screenY + r/4, NULL);
    LineTo(hdc, screenX - r/6, screenY + r/3);
    LineTo(hdc, screenX, screenY + r/4);
    LineTo(hdc, screenX + r/6, screenY + r/3);
    LineTo(hdc, screenX + r/3, screenY + r/4);
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(bodyBrush);
    DeleteObject(hornBrush);
    DeleteObject(eyeBrush);
    DeleteObject(mouthPen);
}

void Monster::DrawStatus(HDC hdc, Vector2D cameraOffset) {
    if (!isAlive_) return;
    
    int screenX = (int)(position_.x - cameraOffset.x);
    int screenY = (int)(position_.y - cameraOffset.y) - size_/2 - 25;
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextAlign(hdc, TA_CENTER);
    
    HFONT font = CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    SetTextColor(hdc, RGB(255, 50, 50));
    wchar_t levelText[32];
    swprintf_s(levelText, L"Lv.%d", level_);
    TextOut(hdc, screenX, screenY, levelText, (int)wcslen(levelText));
    
    int barWidth = 40;
    int barHeight = 4;
    int barY = screenY + 12;
    
    HBRUSH bgBrush = CreateSolidBrush(RGB(60, 60, 60));
    RECT bgRect = { screenX - barWidth/2, barY, screenX + barWidth/2, barY + barHeight };
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);
    
    float hpRatio = (float)currentHp_ / maxHp_;
    int hpWidth = (int)(barWidth * hpRatio);
    HBRUSH hpBrush = CreateSolidBrush(RGB(200, 0, 0));
    RECT hpRect = { screenX - barWidth/2, barY, screenX - barWidth/2 + hpWidth, barY + barHeight };
    FillRect(hdc, &hpRect, hpBrush);
    DeleteObject(hpBrush);
    
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}
