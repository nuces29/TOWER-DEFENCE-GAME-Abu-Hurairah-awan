#include "HUD.h"
#include <string>

static const int BTN_H = 60;
static const int BTN_W = 170;

static const char* TOWER_NAMES[] = { "Cannon","Sniper","MachineGun","Slow","Laser" };
static const int   TOWER_COSTS[] = { 100, 125, 150, 100, 175 };

// Stats for display: damage, range, fire-rate label
static const int   TOWER_DAMAGE[] = { 60,  45, 12,  5,  20 };
static const int   TOWER_RANGE[] = { 130, 280, 110, 120, 200 };
static const char* TOWER_RATE[] = { "Slow","Med","Fast","Med","Med" };

static const char* TOWER_DESC[] = {
    "Heavy cannonball. High\ndamage, slow fire rate.\nBest for Tanks.",
    "Long range weapon.\nTargets lowest HP enemy.\nGreat for cleanup.",
    "Rapid fire bullets.\nLow damage but never\nstops shooting.",
    "Freezes enemies to\n40% speed for 3 sec.\nSupport tower.",
    "Piercing laser beam.\nHits ALL enemies in\nrange at once."
};

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

    startWaveBtn.setSize(sf::Vector2f(BTN_W, 36));
    startWaveBtn.setPosition(panelX + 15, 628);
    startWaveBtn.setFillColor(sf::Color(50, 160, 50));
    startWaveLabel.setFont(font);
    startWaveLabel.setCharacterSize(15);
    startWaveLabel.setString("Start Wave");
    startWaveLabel.setPosition(panelX + 35, 636);

    sellBtn.setSize(sf::Vector2f(BTN_W, 32));
    sellBtn.setPosition(panelX + 15, 672);
    sellBtn.setFillColor(sf::Color(160, 50, 50));
    sellLabel.setFont(font);
    sellLabel.setCharacterSize(14);
    sellLabel.setString("Sell Tower");
    sellLabel.setPosition(panelX + 40, 679);
}

void HUD::buildButtons() {
    int startY = 120;
    for (int i = 0; i < NUM_TOWERS; i++) {
        TowerButton& btn = towerButtons[i];

        // Make buttons taller to fit stats line
        btn.rect.setSize(sf::Vector2f(BTN_W, 68));
        btn.rect.setPosition(panelX + 15, startY + i * 74);
        btn.rect.setFillColor(sf::Color(50, 50, 60));
        btn.rect.setOutlineColor(TOWER_COLS[i]);
        btn.rect.setOutlineThickness(2.f);

        // Tower name  (top line)
        btn.label.setFont(font);
        btn.label.setCharacterSize(14);
        btn.label.setString(TOWER_NAMES[i]);
        btn.label.setFillColor(TOWER_COLS[i]);
        btn.label.setPosition(panelX + 22, startY + i * 74 + 6);

        // Cost  (second line)
        btn.costText.setFont(font);
        btn.costText.setCharacterSize(12);
        btn.costText.setString(std::string("Cost: ") + std::to_string(TOWER_COSTS[i]) + "g");
        btn.costText.setFillColor(sf::Color(200, 200, 100));
        btn.costText.setPosition(panelX + 22, startY + i * 74 + 26);

        // Stats  (third line: dmg / range / rate)
        btn.statsText.setFont(font);
        btn.statsText.setCharacterSize(11);
        btn.statsText.setString(
            std::string("Dmg:") + std::to_string(TOWER_DAMAGE[i]) +
            std::string(" Rng:") + std::to_string(TOWER_RANGE[i]) +
            std::string(" ") + TOWER_RATE[i]);
        btn.statsText.setFillColor(sf::Color(160, 200, 160));
        btn.statsText.setPosition(panelX + 22, startY + i * 74 + 46);

        btn.towerType = i;
        btn.cost = TOWER_COSTS[i];
    }

    // Description panel — sits below the 5 buttons
    int descY = startY + NUM_TOWERS * 74 + 8;
    descPanel.setSize(sf::Vector2f(BTN_W, 90));
    descPanel.setPosition(panelX + 15, descY);
    descPanel.setFillColor(sf::Color(20, 25, 35));
    descPanel.setOutlineColor(sf::Color(80, 80, 100));
    descPanel.setOutlineThickness(1.f);

    descTitle.setFont(font);
    descTitle.setCharacterSize(13);
    descTitle.setFillColor(sf::Color(220, 220, 100));
    descTitle.setPosition(panelX + 22, descY + 6);
    descTitle.setString("Select a tower");

    descBody.setFont(font);
    descBody.setCharacterSize(11);
    descBody.setFillColor(sf::Color(180, 180, 180));
    descBody.setPosition(panelX + 22, descY + 26);
    descBody.setString("Hover over a tower above\nto see its details.");

    lastHighlighted = -1;
}

void HUD::update(int gold, int lives, int wave, int totalWaves,
    int selectedTower, bool waveActive, int hoveredTower) {
    goldText.setString(std::string("Gold: ") + std::to_string(gold));
    livesText.setString(std::string("Lives: ") + std::to_string(lives));
    waveText.setString(std::string("Wave: ") + std::to_string(wave)
        + "/" + std::to_string(totalWaves));
    instrText.setString("Click a tower, then on map");

    // Highlight selected or hovered button
    int highlighted = (selectedTower >= 0) ? selectedTower : hoveredTower;
    for (int i = 0; i < NUM_TOWERS; i++) {
        bool sel = (towerButtons[i].towerType == selectedTower);
        bool hovered = (towerButtons[i].towerType == hoveredTower);
        if (sel)
            towerButtons[i].rect.setFillColor(sf::Color(70, 70, 120));
        else if (hovered)
            towerButtons[i].rect.setFillColor(sf::Color(60, 60, 80));
        else
            towerButtons[i].rect.setFillColor(sf::Color(50, 50, 60));
    }

    // Update description panel when highlight changes
    if (highlighted != lastHighlighted) {
        lastHighlighted = highlighted;
        if (highlighted >= 0 && highlighted < NUM_TOWERS) {
            descTitle.setString(TOWER_NAMES[highlighted]);
            descTitle.setFillColor(TOWER_COLS[highlighted]);
            descBody.setString(TOWER_DESC[highlighted]);
        }
        else {
            descTitle.setFillColor(sf::Color(220, 220, 100));
            descTitle.setString("Select a tower");
            descBody.setString("Click or hover a tower above\nto see the details.");
        }
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
        window.draw(towerButtons[i].statsText);
    }
    window.draw(descPanel);
    window.draw(descTitle);
    window.draw(descBody);
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
