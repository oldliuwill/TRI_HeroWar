# GitHub Copilot 練習指南 - Hero War 專案

本文件提供使用 Hero War 專案練習 GitHub Copilot 的具體任務和操作步驟。

---

## 🎯 練習任務列表

### 任務 1：理解程式碼（/explain）

**目標**：使用 Copilot 快速理解現有程式碼

**步驟**：
1. 開啟 `Character.cpp`
2. 選取 `Monster::Draw()` 函數（約第 180-230 行）
3. 按 `Ctrl + /` 開啟 Copilot Chat
4. 輸入 `/explain` 或詢問「這段程式碼在做什麼？」

**預期學習**：了解 GDI 繪圖 API 和向量圖形繪製方式

---

### 任務 2：生成文件註解（/doc）

**目標**：為函數生成 Doxygen 風格註解

**步驟**：
1. 開啟 `Game.h`
2. 選取 `CheckAttack()` 函數宣告
3. 在 Chat 中輸入 `/doc`
4. 將生成的註解貼到程式碼中

**練習檔案**：`Game.h` 中的所有 public 方法

---

### 任務 3：新增功能 - 魔法攻擊

**目標**：透過註解引導 Copilot 生成新功能

**步驟**：
1. 開啟 `Hero` 類別（`Character.h`）
2. 在 `PerformAttack()` 下方加入註解：
```cpp
// Perform a magic attack that deals damage to all monsters within range
// Returns the total damage dealt to all affected monsters
```
3. 按 `Tab` 接受 Copilot 的建議
4. 在 `Character.cpp` 中實作該函數

---

### 任務 4：新增道具系統

**目標**：讓 Copilot 協助設計新的類別

**步驟**：
1. 建立新檔案 `Item.h`
2. 輸入以下註解：
```cpp
// Item class for the hero to collect
// Types: HealthPotion (restore HP), AttackBoost (increase damage), SpeedBoost
// Items should have position, type, and effect value
```
3. 觀察 Copilot 如何生成完整的類別定義

---

### 任務 5：修復問題（/fix）

**目標**：讓 Copilot 幫助發現和修復潛在問題

**步驟**：
1. 開啟 `Game.cpp`
2. 找到 `UpdatePlaying()` 函數
3. 選取整個函數
4. 在 Chat 中輸入 `/fix`
5. 檢視 Copilot 發現的問題（例如：左移按鍵邏輯有誤）

---

### 任務 6：效能優化（/optimize）

**目標**：使用 Copilot 優化程式碼效能

**步驟**：
1. 開啟 `Game.cpp`
2. 選取 `CheckAttack()` 函數
3. 在 Chat 中輸入 `/optimize`
4. 討論：如何改用空間分割（如四叉樹）來優化碰撞檢測？

---

### 任務 7：生成單元測試（/tests）

**目標**：為遊戲邏輯生成測試案例

**步驟**：
1. 開啟 `Character.cpp`
2. 選取 `Character::TakeDamage()` 函數
3. 在 Chat 中輸入 `/tests`
4. 將生成的測試程式碼保存為新檔案

---

### 任務 8：新增怪獸 AI

**目標**：實作追蹤玩家的 AI 行為

**步驟**：
1. 在 `Monster` 類別中新增：
```cpp
// Make the monster chase the hero if within detection range
// Parameters: hero position, detection range (default 200 pixels)
// Movement should be smooth and avoid jittering
void ChaseHero(const Vector2D& heroPos, float detectionRange = 200.0f);
```
2. 讓 Copilot 完成實作
3. 在 `Game::UpdatePlaying()` 中呼叫新功能

---

### 任務 9：重構程式碼

**目標**：使用現代 C++ 特性重構

**步驟**：
1. 選取 `Game::InitializeMonsters()` 函數
2. 詢問 Copilot：「如何使用 C++17 的 structured bindings 和 algorithms 重構？」
3. 或者輸入 `/optimize` 查看重構建議

---

### 任務 10：新增音效系統

**目標**：設計並實作新的遊戲系統

**步驟**：
1. 詢問 Copilot Chat：
   「在 Win32 C++ 中如何使用 PlaySound API 播放 WAV 音效？」
2. 建立 `Audio.h` 並讓 Copilot 生成音效管理類別
3. 在攻擊和升級時播放音效

---

## 📊 進度追蹤

| 任務 | 難度 | 預估時間 | 完成 |
|-----|-----|---------|------|
| 1. 理解程式碼 | ⭐ | 10 分鐘 | [ ] |
| 2. 生成文件 | ⭐ | 15 分鐘 | [ ] |
| 3. 魔法攻擊 | ⭐⭐ | 20 分鐘 | [ ] |
| 4. 道具系統 | ⭐⭐⭐ | 30 分鐘 | [ ] |
| 5. 修復問題 | ⭐⭐ | 15 分鐘 | [ ] |
| 6. 效能優化 | ⭐⭐⭐ | 25 分鐘 | [ ] |
| 7. 單元測試 | ⭐⭐ | 20 分鐘 | [ ] |
| 8. 怪獸 AI | ⭐⭐⭐ | 30 分鐘 | [ ] |
| 9. 重構 | ⭐⭐ | 20 分鐘 | [ ] |
| 10. 音效 | ⭐⭐⭐ | 40 分鐘 | [ ] |

---

## 💡 小技巧

1. **清晰的註解**：註解越詳細，Copilot 的建議越準確
2. **有意義的命名**：好的函數和變數名稱幫助 Copilot 理解意圖
3. **迭代改進**：先接受基礎建議，再透過 Chat 優化
4. **保持懷疑**：永遠審查 AI 生成的程式碼

---

## 🔧 快捷鍵速查

| 操作 | 快捷鍵 |
|-----|-------|
| 接受建議 | `Tab` |
| 下一個建議 | `Alt + ]` |
| 開啟 Chat | `Ctrl + /` |
| Inline Chat | `Alt + /` |

---

祝練習順利！ 🎮
