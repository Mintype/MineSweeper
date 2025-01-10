#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Function to randomly place bombs
void placeBombs(std::vector<std::vector<int>>& tileState, int gridWidth, int gridHeight, int numBombs, int gridX, int gridY) {
    int placedBombs = 0;
    while (placedBombs < numBombs) {
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;
        // 0 = start tile, 1 = empty tile, 2 = flag tile, -1 = bomb tile, -2 = active bomb tile*
        // 3 = bomb tile with a flag
        // -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
        // Ensure no bomb is placed on a tile that already has one
        if (tileState[x][y] == -1) {  // -1 means it's a start tile
            tileState[x][y] = -3;  // -3 will represent a bomb
            std::cout << x << ", " << y << "\n";
            ++placedBombs;
        }
    }

    int bombCount = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            int neighborX = gridX + dx;
            int neighborY = gridY + dy;
            if (neighborX >= 0 && neighborX < gridWidth && neighborY >= 0 && neighborY < gridHeight) {
                if (tileState[neighborX][neighborY] == -3 || tileState[neighborX][neighborY] == -5) {
                    ++bombCount;
                }
            }
        }
    }
    tileState[gridX][gridY] = bombCount;
}

// Function to start the opening algorithm
void runOpeningAlgorithm(std::vector<std::vector<int>>& tileState, int gridX, int gridY, int gridWidth, int gridHeight) {
    // Debugging output
    std::cout << "Checking tile at (" << gridX << ", " << gridY << ")\n";

    // Check if the coordinates are within the bounds of the grid
    if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
        std::cout << "Out of bounds: " << gridX << ", " << gridY << "\n";
        return; // Out of bounds, return early
    }

    // If the tile is already revealed (or flagged), return
    if (tileState[gridX][gridY] >= 0) {
        std::cout << "Already revealed or flagged: " << tileState[gridX][gridY] << "\n";
        return; // Already revealed or flagged, return early
    }

    // If it's a bomb tile (-3) or a flagged bomb tile (-5), do not open
    if (tileState[gridX][gridY] == -3 || tileState[gridX][gridY] == -5) {
        std::cout << "Hit a bomb or flagged tile: " << tileState[gridX][gridY] << "\n";
        return; // It's a bomb, return early
    }

    // Count the neighboring bombs
    int bombCount = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            // Skip the center tile (dx, dy == 0, 0)
            if (dx == 0 && dy == 0) continue;

            int neighborX = gridX + dx;
            int neighborY = gridY + dy;
            if (neighborX >= 0 && neighborX < gridWidth && neighborY >= 0 && neighborY < gridHeight) {
                if (tileState[neighborX][neighborY] == -3 || tileState[neighborX][neighborY] == -5) {
                    ++bombCount;
                }
            }
        }
    }

    // Set the tile's state to the bomb count (or 0 if no bombs around)
    tileState[gridX][gridY] = bombCount;
    std::cout << "Set tile (" << gridX << ", " << gridY << ") to " << bombCount << "\n";

    // If there are no bombs around, recursively open neighboring tiles
    if (bombCount == 0) {
        std::cout << "No bombs around, opening neighbors...\n";

        // 8 directions: top-left, top, top-right, left, right, bottom-left, bottom, bottom-right
        runOpeningAlgorithm(tileState, gridX - 1, gridY - 1, gridWidth, gridHeight); // top-left
        runOpeningAlgorithm(tileState, gridX, gridY - 1, gridWidth, gridHeight);     // top
        runOpeningAlgorithm(tileState, gridX + 1, gridY - 1, gridWidth, gridHeight); // top-right
        runOpeningAlgorithm(tileState, gridX - 1, gridY, gridWidth, gridHeight);     // left
        runOpeningAlgorithm(tileState, gridX + 1, gridY, gridWidth, gridHeight);     // right
        runOpeningAlgorithm(tileState, gridX - 1, gridY + 1, gridWidth, gridHeight); // bottom-left
        runOpeningAlgorithm(tileState, gridX, gridY + 1, gridWidth, gridHeight);     // bottom
        runOpeningAlgorithm(tileState, gridX + 1, gridY + 1, gridWidth, gridHeight); // bottom-right
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
    const sf::Texture startTileTexture("start_tile.png");
    sf::Sprite startTileSprite(startTileTexture);

    const sf::Texture emptyTileTexture("empty_tile.png");
    sf::Sprite emptyTileSprite(emptyTileTexture);

    const sf::Texture flagTileTexture("flag_tile.png");
    sf::Sprite flagTileSprite(flagTileTexture);

    const sf::Texture bombTileTexture("bomb_tile.png");
    sf::Sprite bombTileSprite(bombTileTexture);

    const sf::Texture activeBombTileTexture("active_bomb_tile.png");
    sf::Sprite activeBombTileSprite(activeBombTileTexture);

    const sf::Texture oneTileTexture("one_tile.png");
    sf::Sprite oneTileSprite(oneTileTexture);

    const sf::Texture twoTileTexture("two_tile.png");
    sf::Sprite twoTileSprite(twoTileTexture);

    const sf::Texture threeTileTexture("three_tile.png");
    sf::Sprite threeTileSprite(threeTileTexture);

    const sf::Texture fourTileTexture("four_tile.png");
    sf::Sprite fourTileSprite(fourTileTexture);

    // Set up the grid size (8x8)
    const int gridWidth = 8;
    const int gridHeight = 8;
    const float tileSize = 32.0f;  // Size of each tile in pixels

    bool gameOver = false;

    //2D vector to store the current state of each tile
    // 0 = start tile, 1 = empty tile, 2 = flag tile, -1 = bomb tile, -2 = active bomb tile*
    // 3 = bomb tile with a flag
    // *Active bomb tile = the bomb you clicked on!
    // -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
    std::vector<std::vector<int>> tileState(gridWidth, std::vector<int>(gridHeight, -1));

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
                        if (tileState[gridX][gridY] == -3) {
                            tileState[gridX][gridY] = -4;
                            gameOver = true; // end game
                        }
                        else if (tileState[gridX][gridY] == -1) { // if its a starting tile
                            if (!bombsPlaced) {
                                tileState[gridX][gridY] = 0;
                                placeBombs(tileState, gridWidth, gridHeight, 10, gridX, gridY); // 10 bombs
                                bombsPlaced = true;

                                // Run opening algorithm
                                runOpeningAlgorithm(tileState, gridX, gridY, gridWidth, gridHeight);

                            }
                            else {
                                //tileState[gridX][gridY] = 0; // Reveal an empty tile
                                int bombCount = 0;
                                for (int dx = -1; dx <= 1; ++dx) {
                                    for (int dy = -1; dy <= 1; ++dy) {
                                        int neighborX = gridX + dx;
                                        int neighborY = gridY + dy;
                                        if (neighborX >= 0 && neighborX < gridWidth && neighborY >= 0 && neighborY < gridHeight) {
                                            if (tileState[neighborX][neighborY] == -3 || tileState[neighborX][neighborY] == -5) {
                                                ++bombCount;
                                            }
                                        }
                                    }
                                }
                                std::cout << "bro waht " << bombCount << "\n";
                                tileState[gridX][gridY] = bombCount;
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
                    // 0 = start tile, 1 = empty tile, 2 = flag tile, -1 = bomb tile, -2 = active bomb tile*
                    // 3 = bomb tile with a flag
                    // -6 = flag tile, -5 = bomb with flag, -4 = active bomb tile, -3 = bomb tile, -2 = bomb tile, -1 = start tile, 0 = empty tile
                    // Check if coordinates are within bounds
                    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight && !gameOver)
                    {
                        if (tileState[gridX][gridY] == -1)
                            tileState[gridX][gridY] = -6;
                        else if (tileState[gridX][gridY] == -6)
                            tileState[gridX][gridY] = -1;
                        else if (tileState[gridX][gridY] == -3)
                            tileState[gridX][gridY] = -5;
                        else if (tileState[gridX][gridY] == -5)
                            tileState[gridX][gridY] = -3;
                    }
                }
            }
        }

        window.clear();

        // Draw the grid of sprites
        for (int i = 0; i < gridWidth; ++i)
        {
            for (int j = 0; j < gridHeight; ++j)
            {
                // Position each sprite in the grid
                if (tileState[i][j] == -1)  // start tile
                    startTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 0)  // empty tile
                    emptyTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == -6)  // flag tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == -5)  // flagged bomb tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == -3) {  // bomb tile
                    if (gameOver)
                        bombTileSprite.setPosition({ i * tileSize, j * tileSize });
                    else
                        startTileSprite.setPosition({ i * tileSize, j * tileSize });
                }
                else if (tileState[i][j] == -4) // active bomb tile
                    activeBombTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 1) // 1 bomb near
                    oneTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 2) // 2 bomb near
                    twoTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 3) // 3 bomb near
                    threeTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 4) // 4 bomb near
                    fourTileSprite.setPosition({ i * tileSize, j * tileSize });



                // Check if coordinates are within bounds
                // Draw the appropriate tile
                if (tileState[i][j] == -1)  // start tile
                    window.draw(startTileSprite);
                else if (tileState[i][j] == 0)  // empty tile
                    window.draw(emptyTileSprite);
                else if (tileState[i][j] == -6)  // flag tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == -5)  // flagged bomb tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == -3) {  // bomb tile
                    if(gameOver)
                        window.draw(bombTileSprite);
                    else
                        window.draw(startTileSprite);
                }
                else if (tileState[i][j] == -4) // active bomb tile
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
