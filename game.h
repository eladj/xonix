// TODO:
// 1. Add enemy within the filled area
// 2. Handle some edge case of out_of_range when closing very small area (probably something with checking the tile of the player itself)
// 3. Add some UI to end game and restart level
// 4. Add sound
#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>


constexpr int tileSize = 10;  // Tile size in pixels
constexpr int numRows = 120;
constexpr int numCols = 160;
constexpr int numTiles = numRows * numCols;
constexpr int textLineHeight = 40;
constexpr float moveDelay = 0.01;  // Time in seconds between movement steps

enum class TileStatus : uint8_t {
    Empty = 1,
    Filled = 2,
    NewFilled = 4,
    Player = 8,
    Enemy = 16,
    MaybeToFill = 32,  // Mark a tile we consider to fill
};
constexpr enum TileStatus operator|(const enum TileStatus a, const enum TileStatus b){
    return (enum TileStatus)(std::underlying_type<TileStatus>::type(a) | std::underlying_type<TileStatus>::type(b));
}
constexpr enum TileStatus operator&(const enum TileStatus a, const enum TileStatus b){
    return (enum TileStatus)(std::underlying_type<TileStatus>::type(a) & std::underlying_type<TileStatus>::type(b));
}
constexpr enum TileStatus operator~(const enum TileStatus a){
    return (enum TileStatus)~(std::underlying_type<TileStatus>::type(a));
}


class Grid {
private:
    int numFilledTiles() const;  // Compute the number of filled tiles
    int initialFilledArea;  // Initial number of tiles that were filled
    std::vector<std::vector<TileStatus>> grid;  // Holds the status of each tile
public:
    Grid();
    void init();  // Fill the initial borders
    float areaFilled() const;  // Fraction of the area which is filled (between 0 to 1)
    void set(int row, int col, TileStatus TileStatus);  // Fill in specific coordinate
    void add(int row, int col, TileStatus TileStatus);  // Fill in specific coordinate
    void remove(int row, int col, TileStatus TileStatus);  // Remove a status from a tile
    void changeUniqueStatus(TileStatus currentStatus, TileStatus newStatus);  // Change the status of all tiles, only if tile is exactly the currentStatus
    void changeStatus(TileStatus currentStatus, TileStatus newStatus);  // Change the status of all tiles that has currentStatus
    TileStatus get(int row, int col) const;
    bool has(int row, int col, TileStatus tileStatus) const;  // Check if this tile has a specific flag
};


enum EnemyType {Empty, Filled};


class Enemy {
public:
    int x, y;   // Position in grid
    int dx, dy;  // Forward direction
    EnemyType type;

    Enemy() : x(5), y(5), dx(1), dy(1), type(EnemyType::Empty) {}
};


class Player{
public:
    int x, y;   // Position in grid

    Player() : x(0), y(0){}
};


class Game{
private:
    std::unique_ptr<sf::RenderWindow> window;
    sf::VideoMode videoMode;
    sf::Event event;
    sf::Clock clock;
    float deltaTime;  // The interval in seconds from the last frame to the current one
    float timer;  // The interval from the last movement step

    // Resources
    sf::Font font;

	// Text
	sf::Text uiText;

    // Keyboard Inputs
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;

	// Game logic
	bool endGame;
    int life;     // Player's health
    int numEnemiesInside;  // Number of enemies inside the filled area
    int numEnemiesOutside;  // Number of enemies in the unfilled area
    float areaToWin;  // Area which is required to win

	// Game objects
	std::vector<Enemy> enemies;
	Player player;
    Grid grid;

    void initVariables();
    void initWindow();
    void initArea();
    void initText();
    void initEnemies();

    // Random Generator
    std::mt19937 randomGenerator;

public:
    Game();

    const bool running() const;
    const bool getEndGame() const;

    void pollEvents();

    // Fill all empty area when we complete a loop
    void tryFill(int row, int col);
    void fillArea(int row, int col, bool& foundEnemy);

    // Reduce one life and reset
    void lose();

    // Place enemies in random position
    void spawnEnemies();

    void updatePlayer();
    void updateEnemies();
    void updateText();
    void update();

    void renderText();
    void renderGrid();
    void render();
};
