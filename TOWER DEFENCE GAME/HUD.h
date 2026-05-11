#pragma once
#include <SFML/Graphics.hpp>

class HUD {
public:
    static const int NUM_TOWERS = 5;

    struct TowerButton {
        sf::RectangleShape rect;
        sf::Text           label;
        sf::Text           costText;
        int                towerType;
        int                cost;
    };

    HUD(sf::Font& font, int panelX);

    void update(int gold, int lives, int wave, int totalWaves,
                int selectedTower, bool waveActive);
    void render(sf::RenderWindow& window) const;

    int  handleClick(sf::Vector2i mousePos);          // tower type or -1
    bool isStartWaveClicked(sf::Vector2i mp) const;
    bool isSellClicked(sf::Vector2i mp) const;

private:
    sf::Font& font;
    int panelX;

    sf::RectangleShape panel;
    sf::Text goldText, livesText, waveText, instrText;

    TowerButton        towerButtons[NUM_TOWERS];     

    sf::RectangleShape startWaveBtn;
    sf::Text           startWaveLabel;
    sf::RectangleShape sellBtn;
    sf::Text           sellLabel;

    void buildButtons();
};
