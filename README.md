# Tower Defense Game

A 2D tower defense game built in **C++** using **SFML 2.6**,as an OOP project. Features 5 unique tower types, 5 enemy types, and 5 progressively harder waves.

---

## Gameplay

Place towers on grass tiles to stop enemies from reaching the exit. Enemies follow a fixed winding path from the green entry tile to the red exit tile. Every enemy that reaches the exit costs you one life. Lose all 20 lives and it is game over. Survive all 5 waves to win.

- **Left-click** a tower button in the right panel, then **left-click** a grass tile to place it
- **Left-click** a placed tower to select it, then click **Sell Tower** to get half your gold back
- **Right-click** anywhere to deselect
- Press **R** to restart at any time
- Press **ESC** to quit

---

## Tower Types

| Tower | Cost | Range | Fire Rate | Special |
|---|---|---|---|---|
| Cannon | 100g | 130px | Slow | High damage per shot |
| Sniper | 125g | 280px | Medium | Targets lowest-HP enemy |
| Machine Gun | 150g | 110px | Very fast | Low damage, high volume |
| Slow | 100g | 120px | Medium | Slows enemies to 40% speed for 3 seconds |
| Laser | 175g | 200px | Medium | Piercing beam — damages ALL enemies in range at once |

---

## Enemy Types

| Enemy | Shape | HP | Speed | Gold | Special |
|---|---|---|---|---|---|
| Basic | Circle (red) | Medium | Medium | 30× wave | Standard enemy |
| Fast | Circle (yellow) | Low | Very fast | 18× wave | Hard to hit |
| Tank | Square (blue) | Very high | Slow | 75× wave | Immune to slow |
| Flying | Diamond (teal) | Medium | Fast | 24× wave | Ignores path, flies straight to exit |
| Stealth | Circle (purple) | Medium | Fast | 36× wave | Nearly invisible until within 150px of a tower |

Gold rewards scale with wave number — a wave 5 Tank gives 375 gold.

---

## Wave Progression

| Wave | Enemies |
|---|---|
| 1 | 8 Basic |
| 2 | 6 Basic + 5 Fast |
| 3 | 8 Basic + 6 Fast + 2 Tanks |
| 4 | 6 Basic + 8 Fast + 3 Tanks + 4 Flying |
| 5 | 8 Basic + 8 Fast + 4 Tanks + 5 Flying + 5 Stealth |

Click **Start Wave** in the right panel to begin each wave. The next wave will not start automatically — you get time to place more towers between waves.

---

## Requirements

- **SFML 2.6.1** — Visual C++ 17 (2022) 64-bit build
- **Visual Studio 2022**
- **C++17** or later

---

## How to build and run in Visual Studio 2022

**1. Download SFML**
Go to [sfml-dev.org](https://sfml-dev.org) → Downloads → SFML 2.6.1 → Visual C++ 17 (2022) 64-bit. Extract to `C:\SFML`.

**2. Create an Empty C++ Project** and add all `.cpp` and `.h` files via Solution Explorer → Add → Existing Item.

**3. Open Project Properties** (right-click project → Properties). Set Configuration to **All Configurations**.

- `C/C++` → `General` → `Additional Include Directories` → add `C:\SFML\include`
- `Linker` → `General` → `Additional Library Directories` → add `C:\SFML\lib`
- `Linker` → `Input` → `Additional Dependencies` → add:
  ```
  sfml-graphics-d.lib
  sfml-window-d.lib
  sfml-system-d.lib
  ```

**4. Copy DLLs** from `C:\SFML\bin` into your project's output folder (same folder as the `.vcxproj` file):
```
sfml-graphics-d-2.dll
sfml-window-d-2.dll
sfml-system-d-2.dll
```

**5. Create the `assets` folder** in the same location as the DLLs. Place your font and all PNG images inside it.

**6. Set platform to x64** and press **F5** to build and run.

---

## Assets

If you wish to play the game using your own characters then use 512x512 pixel png images and copy their names as in the assets folder then replace them in assets folder.

The font can be any `.ttf` file renamed to `font.ttf`.

---

## Controls Summary

| Input | Action |
|---|---|
| Left-click tower button | Select tower type to place |
| Left-click grass tile | Place selected tower |
| Left-click placed tower | Select it (shows range circle) |
| Sell Tower button | Sell selected tower for 50% refund |
| Start Wave button | Begin the next wave |
| Right-click | Deselect |
| R | Restart game |
| ESC | Quit |
