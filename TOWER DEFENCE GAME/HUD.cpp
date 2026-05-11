#include "HUD.h"
#include <string>

static const int BTN_H = 60;
static const int BTN_W = 170;

static const char* TOWER_NAMES[] = {"Cannon","Sniper","MachineGun","Slow","Laser"};
static const int   TOWER_COSTS[] = {100, 125, 150, 100, 175};
static const sf::Color TOWER_COLS[] = {
    sf::Color( 255,  255,  255),
    sf::Color(220, 200,  50),
    sf::Color(200, 100,  30),
    sf::Color( 60, 100, 220),
    sf::Color(180,  50, 220)
};

HUD::HUD(sf::Font& f, int px) : font(f), panelX(px) {
    panel.setSize(sf::Vector2f(200.f, 720.f));
    panel.setPosition(panelX, 0);
    panel.setFillColor(sf::Color(30, 30, 40));

    auto makeText = [&](sf::Text& t, unsigned sz, float x, float y) {
        t.setFont(font);
        t.setCharacterSize(sz);
        t.setFillColor(sf::Color::White);
        t.setPosition(x, y);
    };
    makeText(goldText,  18, panelX + 10, 10);
    makeText(livesText, 18, panelX + 10, 35);
    makeText(waveText,  18, panelX + 10, 60);
    makeText(instrText, 13, panelX + 10, 90);
    instrText.setFillColor(sf::Color(180, 180, 180));

    buildButtons();

    startWaveBtn.setSize(sf::Vector2f(BTN_W, 40));
    startWaveBtn.setPosition(panelX + 15, 590);
    startWaveBtn.setFillColor(sf::Color(50, 160, 50));
    startWaveLabel.setFont(font);
    startWaveLabel.setCharacterSize(16);
    startWaveLabel.setString("Start Wave");
    startWaveLabel.setPosition(panelX + 35, 598);

    sellBtn.setSize(sf::Vector2f(BTN_W, 35));
    sellBtn.setPosition(panelX + 15, 640);
    sellBtn.setFillColor(sf::Color(160, 50, 50));
    sellLabel.setFont(font);
    sellLabel.setCharacterSize(15);
    sellLabel.setString("Sell Tower");
    sellLabel.setPosition(panelX + 40, 648);
}

void HUD::buildButtons() {
    int startY = 120;
    for (int i = 0; i < NUM_TOWERS; i++) {
        TowerButton& btn = towerButtons[i];
        btn.rect.setSize(sf::Vector2f(BTN_W, BTN_H - 5));
        btn.rect.setPosition(panelX + 15, startY + i * (BTN_H + 5));
        btn.rect.setFillColor(sf::Color(50, 50, 60));
        btn.rect.setOutlineColor(TOWER_COLS[i]);
        btn.rect.setOutlineThickness(2.f);

        btn.label.setFont(font);
        btn.label.setCharacterSize(14);
        btn.label.setString(TOWER_NAMES[i]);
        btn.label.setFillColor(TOWER_COLS[i]);
        btn.label.setPosition(panelX + 22, startY + i * (BTN_H + 5) + 8);

        btn.costText.setFont(font);
        btn.costText.setCharacterSize(12);
        btn.costText.setString(std::string("Cost: ") + std::to_string(TOWER_COSTS[i]) + "g");
        btn.costText.setFillColor(sf::Color(200, 200, 100));
        btn.costText.setPosition(panelX + 22, startY + i * (BTN_H + 5) + 30);

        btn.towerType = i;
        btn.cost      = TOWER_COSTS[i];
    }
}

void HUD::update(int gold, int lives, int wave, int totalWaves,
                 int selectedTower, bool waveActive) {
    goldText.setString(std::string("Gold: ")  + std::to_string(gold));
    livesText.setString(std::string("Lives: ") + std::to_string(lives));
    waveText.setString(std::string("Wave: ")  + std::to_string(wave)
                       + "/" + std::to_string(totalWaves));
    instrText.setString("Click a tower\nthen click on any box on map");
    instrText.setPosition(1070.0f,450.0f);

    for (int i = 0; i < NUM_TOWERS; i++) {
        bool sel = (towerButtons[i].towerType == selectedTower);
        towerButtons[i].rect.setFillColor(
            sel ? sf::Color(70, 70, 100) : sf::Color(50, 50, 60));
    }
    startWaveBtn.setFillColor(
        waveActive ? sf::Color(40, 80, 40) : sf::Color(50, 160, 50));
}

void HUD::render(sf::RenderWindow& window) const {
    window.draw(panel);
    window.draw(goldText);
    window.draw(livesText);
    window.draw(waveText);
    window.draw(instrText);
    for (int i = 0; i < NUM_TOWERS; i++) {
        window.draw(towerButtons[i].rect);
        window.draw(towerButtons[i].label);
        window.draw(towerButtons[i].costText);
    }
    window.draw(startWaveBtn);
    window.draw(startWaveLabel);
    window.draw(sellBtn);
    window.draw(sellLabel);
}

int HUD::handleClick(sf::Vector2i mousePos) {
    sf::Vector2f mp(static_cast<float>(mousePos.x),
                    static_cast<float>(mousePos.y));
    for (int i = 0; i < NUM_TOWERS; i++) {
        if (towerButtons[i].rect.getGlobalBounds().contains(mp))
            return towerButtons[i].towerType;
    }
    return -1;
}

bool HUD::isStartWaveClicked(sf::Vector2i mousePos) const {
    sf::Vector2f mp(static_cast<float>(mousePos.x),
                    static_cast<float>(mousePos.y));
    return startWaveBtn.getGlobalBounds().contains(mp);
}

bool HUD::isSellClicked(sf::Vector2i mousePos) const {
    sf::Vector2f mp(static_cast<float>(mousePos.x),
                    static_cast<float>(mousePos.y));
    return sellBtn.getGlobalBounds().contains(mp);
}
