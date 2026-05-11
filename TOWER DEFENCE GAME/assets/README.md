# Assets Folder

Place your texture and font files here.

## Font
- `font.ttf`  ← Any TrueType font (e.g. DejaVuSans.ttf, Arial.ttf)
  The game falls back to the system DejaVu font if this is missing.

## Textures (optional – game draws coloured shapes by default)
To use textures, load them in the relevant .cpp files.
Suggested filenames:

### Tower textures
- `tower_cannon.png`
- `tower_sniper.png`
- `tower_machinegun.png`
- `tower_slow.png`
- `tower_laser.png`

### Enemy textures
- `enemy_basic.png`
- `enemy_fast.png`
- `enemy_tank.png`
- `enemy_flying.png`
- `enemy_stealth.png`

### Map/UI textures
- `tile_grass.png`
- `tile_path.png`
- `background.png`

## How to use textures in the code

In `Enemy.cpp`, load and apply a texture like this:

```cpp
// In Enemy constructor or a loadTextures() function:
static sf::Texture tex;
tex.loadFromFile("assets/enemy_basic.png");

sf::Sprite sprite(tex);
sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);
sprite.setPosition(posX, posY);
window.draw(sprite);
```

Replace the `sf::CircleShape` / `sf::RectangleShape` drawing code
in each class's `render()` method with a sprite draw call.
