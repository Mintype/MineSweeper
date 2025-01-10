#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

enum TileState {
    FLAG = -6,
    BOMB_WITH_FLAG = -5,
    ACTIVE_BOMB = -4,
    BOMB = -3,
    EMPTY = 0,
    START = -1
};

int countBombs(std::vector<std::vector<int>>& tileState, int gridWidth, int gridHeight, int gridX, int gridY) {
    int bombCount = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            int neighborX = gridX + dx;
            int neighborY = gridY + dy;
            if (neighborX >= 0 && neighborX < gridWidth && neighborY >= 0 && neighborY < gridHeight) {
                if (tileState[neighborX][neighborY] == BOMB || tileState[neighborX][neighborY] == BOMB_WITH_FLAG) {
                    ++bombCount;
                }
            }
        }
    }
    return bombCount;
}

// Function to randomly place bombs
void placeBombs(std::vector<std::vector<int>>& tileState, int gridWidth, int gridHeight, int numBombs, int gridX, int gridY) {
    int placedBombs = 0;
    while (placedBombs < numBombs) {
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;

        // Ensure the bomb is not placed on the start tile, the tile at gridX, gridY, or its surrounding 8 tiles
        bool isValidPlacement = true;
        if (x >= gridX - 1 && x <= gridX + 1 && y >= gridY - 1 && y <= gridY + 1) {
            isValidPlacement = false;
        }

        if (isValidPlacement) {
            tileState[x][y] = BOMB;  // -3 will represent a bomb
            std::cout << x << ", " << y << "\n";
            ++placedBombs;
        }
    }

    tileState[gridX][gridY] = countBombs(tileState, gridWidth, gridHeight, gridX, gridY);
}
// -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
// Function to start the opening algorithm

void runOpeningAlgorithm(std::vector<std::vector<int>>& tileState,
    std::vector<std::vector<bool>>& revealed,
    int gridX, int gridY,
    int gridWidth, int gridHeight,
    int orgX, int orgY) {
    // Debug: Initial position and state
    std::cout << "Running at (" << gridX << ", " << gridY << ")\n";

    // Check for out-of-bounds or already revealed tiles or flagged tiles
    if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
        std::cout << "Out of bounds\n";
        return;
    }

    if (revealed[gridX][gridY]) {
        if (gridX != orgX && gridY != orgY) {
            std::cout << "Tile already revealed\n";
            return;
        }
    }

    if (tileState[gridX][gridY] == FLAG) {
        std::cout << "Tile is flagged\n";
        return;
    }

    // Count the neighboring bombs
    int bombCount = countBombs(tileState, gridWidth, gridHeight, gridX, gridY);

    std::cout << "Bomb count at (" << gridX << ", " << gridY << "): " << bombCount << "\n";

    // Set the tile's state to the bomb count
    tileState[gridX][gridY] = bombCount;

    // Mark tile as revealed
    revealed[gridX][gridY] = true;
    std::cout << "Revealed tile (" << gridX << ", " << gridY << ")\n";

    // If no bombs around, recursively open neighbors
    if (bombCount == 0) {
        std::cout << "No bombs around, opening neighbors...\n";
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;  // Skip the current tile

                int neighborX = gridX + dx;
                int neighborY = gridY + dy;

                // Check if the neighbor is out of bounds or already revealed
                if (neighborX < 0 || neighborX >= gridWidth || neighborY < 0 || neighborY >= gridHeight || revealed[neighborX][neighborY])
                    continue;

                std::cout << "Recursively opening neighbor (" << neighborX << ", " << neighborY << ")\n";
                runOpeningAlgorithm(tileState, revealed, neighborX, neighborY, gridWidth, gridHeight, orgX, orgY);
            }
        }
    }
    else {
        std::cout << "Bomb count > 0, no recursion needed.\n";
    }

}






int main()
{
    // Initialize random seed
    srand(time(0));

    // Create the main window
    sf::RenderWindow window(sf::VideoMode({ 400, 600 }), "Mine Sweeper");

    // Set FPS limit to 30 (because we don't need a lot of frames)
    window.setFramerateLimit(30);

    // Load sprites
    const sf::Texture startTileTexture("resources/images/start_tile.png");
    sf::Sprite startTileSprite(startTileTexture);

    const sf::Texture emptyTileTexture("resources/images/empty_tile.png");
    sf::Sprite emptyTileSprite(emptyTileTexture);

    const sf::Texture flagTileTexture("resources/images/flag_tile.png");
    sf::Sprite flagTileSprite(flagTileTexture);

    const sf::Texture bombTileTexture("resources/images/bomb_tile.png");
    sf::Sprite bombTileSprite(bombTileTexture);

    const sf::Texture activeBombTileTexture("resources/images/active_bomb_tile.png");
    sf::Sprite activeBombTileSprite(activeBombTileTexture);

    const sf::Texture oneTileTexture("resources/images/one_tile.png");
    sf::Sprite oneTileSprite(oneTileTexture);

    const sf::Texture twoTileTexture("resources/images/two_tile.png");
    sf::Sprite twoTileSprite(twoTileTexture);

    const sf::Texture threeTileTexture("resources/images/three_tile.png");
    sf::Sprite threeTileSprite(threeTileTexture);

    const sf::Texture fourTileTexture("resources/images/four_tile.png");
    sf::Sprite fourTileSprite(fourTileTexture);

    // Set up the grid size (8x8)
    const int gridWidth = 8;
    const int gridHeight = 8;
    const float tileSize = 32.0f;  // Size of each tile in pixels

    bool gameOver = false;

    //2D vector to store the current state of each tile
    // -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
    std::vector<std::vector<int>> tileState(gridWidth, std::vector<int>(gridHeight, -1));
    std::vector<std::vector<bool>> tileRevealed(gridWidth, std::vector<bool>(gridHeight, false));

    // Flag to indicate if the bombs have been placed
    bool bombsPlaced = false;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();

            // Handle mouse click
            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    // Get the mouse position relative to the window
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Convert mouse position to grid coordinates
                    int gridX = mousePos.x / tileSize;
                    int gridY = mousePos.y / tileSize;

                    // Check if coordinates are within bounds
                    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight && !gameOver)
                    {


                        // If the tile is a bomb, show the bomb
                        if (tileState[gridX][gridY] == BOMB) {
                            tileState[gridX][gridY] = ACTIVE_BOMB;
                            tileRevealed[gridX][gridY] = true;
                            gameOver = true; // end game
                        }
                        else if (tileState[gridX][gridY] == START) { // if its a starting tile
                            if (!bombsPlaced) {
                                tileState[gridX][gridY] = EMPTY;
                                tileRevealed[gridX][gridY] = true;
                                placeBombs(tileState, gridWidth, gridHeight, 10, gridX, gridY); // 10 bombs
                                bombsPlaced = true;

                                // Run opening algorithm
                                runOpeningAlgorithm(tileState, tileRevealed, gridX, gridY, gridWidth, gridHeight, gridX, gridY);

                            }
                            else {
                                //tileState[gridX][gridY] = 0; // Reveal an empty tile
                                int bombCount = countBombs(tileState, gridWidth, gridHeight, gridX, gridY);
                                std::cout << "bro waht " << bombCount << "\n";
                                tileState[gridX][gridY] = bombCount;
                                tileRevealed[gridX][gridY] = true;
                            }
                        }
                    }
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    // Get the mouse position relative to the window
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Convert mouse position to grid coordinates
                    int gridX = mousePos.x / tileSize;
                    int gridY = mousePos.y / tileSize;
                    // -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
                    // Check if coordinates are within bounds
                    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight && !gameOver)
                    {
                        if (tileState[gridX][gridY] == START)
                            tileState[gridX][gridY] = FLAG;
                        else if (tileState[gridX][gridY] == FLAG)
                            tileState[gridX][gridY] = START;
                        else if (tileState[gridX][gridY] == BOMB)
                            tileState[gridX][gridY] = BOMB_WITH_FLAG;
                        else if (tileState[gridX][gridY] == BOMB_WITH_FLAG)
                            tileState[gridX][gridY] = BOMB;
                    }
                }
            }
        }

        //    FLAG = -6,
        //    BOMB_WITH_FLAG = -5,
        //    ACTIVE_BOMB = -4,
        //    BOMB = -3,
        //    EMPTY = 0,
        //    START = -1

        window.clear();

        // Draw the grid of sprites
        for (int i = 0; i < gridWidth; ++i)
        {
            for (int j = 0; j < gridHeight; ++j)
            {
                // Position each sprite in the grid
                if (tileState[i][j] == START)  // start tile
                    startTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == EMPTY)  // empty tile
                    emptyTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == FLAG)  // flag tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == BOMB_WITH_FLAG)  // flagged bomb tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == BOMB) {  // bomb tile
                    if (gameOver)
                        bombTileSprite.setPosition({ i * tileSize, j * tileSize });
                    else
                        startTileSprite.setPosition({ i * tileSize, j * tileSize });
                }
                else if (tileState[i][j] == ACTIVE_BOMB) // active bomb tile
                    activeBombTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 1) // 1 bomb near
                    oneTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 2) // 2 bomb near
                    twoTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 3) // 3 bomb near
                    threeTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 4) // 4 bomb near
                    fourTileSprite.setPosition({ i * tileSize, j * tileSize });

                //    FLAG = -6,
                //    BOMB_WITH_FLAG = -5,
                //    ACTIVE_BOMB = -4,
                //    BOMB = -3,
                //    EMPTY = 0,
                //    START = -1

                // Check if coordinates are within bounds
                // Draw the appropriate tile
                if (tileState[i][j] == START)  // start tile
                    window.draw(startTileSprite);
                else if (tileState[i][j] == EMPTY)  // empty tile
                    window.draw(emptyTileSprite);
                else if (tileState[i][j] == FLAG)  // flag tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == BOMB_WITH_FLAG)  // flagged bomb tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == BOMB) {  // bomb tile
                    if(gameOver)
                        window.draw(bombTileSprite);
                    else
                        window.draw(startTileSprite);
                }
                else if (tileState[i][j] == ACTIVE_BOMB) // active bomb tile
                    window.draw(activeBombTileSprite);
                else if (tileState[i][j] == 1) // 1 bomb near tile
                    window.draw(oneTileSprite);
                else if (tileState[i][j] == 2) // 2 bomb near tile
                    window.draw(twoTileSprite);
                else if (tileState[i][j] == 3) // 3 bomb near tile
                    window.draw(threeTileSprite);
                else if (tileState[i][j] == 4) // 4 bomb near tile
                    window.draw(fourTileSprite);
            }
        }

        // Update the window
        window.display();
    }
}
