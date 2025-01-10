#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Function to randomly place bombs
void placeBombs(std::vector<std::vector<int>>& tileState, int gridWidth, int gridHeight, int numBombs) {
    int placedBombs = 0;
    while (placedBombs < numBombs) {
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;

        // Ensure no bomb is placed on a tile that already has one
        if (tileState[x][y] == 0) {  // 0 means it's a start tile
            tileState[x][y] = -1;  // -1 will represent a bomb
            ++placedBombs;
        }
    }
}

// Function to start the opening algorithm
void runOpeningAlgorithm(std::vector<std::vector<int>>& tileState, int gridX, int gridY) {

    // Check if the coordinates are within the bounds of the grid
    if (gridX >= 0 && gridX < tileState.size() && gridY >= 0 && gridY < tileState[0].size()) {
        if (tileState[gridX][gridY] == 0) {
            tileState[gridX][gridY] = 1;
            // Recursively call for neighboring cells
            runOpeningAlgorithm(tileState, gridX - 1, gridY);
            runOpeningAlgorithm(tileState, gridX + 1, gridY);
            runOpeningAlgorithm(tileState, gridX, gridY - 1);
            runOpeningAlgorithm(tileState, gridX, gridY + 1);
        }
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

    // Set up the grid size (8x8)
    const int gridWidth = 8;
    const int gridHeight = 8;
    const float tileSize = 32.0f;  // Size of each tile in pixels

    bool gameOver = false;

    //2D vector to store the current state of each tile
    // 0 = start tile, 1 = empty tile, 2 = flag tile, -1 = bomb tile, -2 = active bomb tile*
    // 3 = bomb tile with a flag
    // *Active bomb tile = the bomb you clicked on!
    std::vector<std::vector<int>> tileState(gridWidth, std::vector<int>(gridHeight, 0));

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
                        if (tileState[gridX][gridY] == -1) {
                            tileState[gridX][gridY] = -2;
                            gameOver = true; // end game
                        }
                        else if (tileState[gridX][gridY] == 0) { // if its a starting tile
                            if (!bombsPlaced) {
                                placeBombs(tileState, gridWidth, gridHeight, 10); // 10 bombs
                                bombsPlaced = true;

                                // Run opening algorithm
                                runOpeningAlgorithm(tileState, gridX, gridY);

                            }
                            else
                                tileState[gridX][gridY] = 1; // Reveal an empty tile
                        }
                    }
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    // Get the mouse position relative to the window
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Convert mouse position to grid coordinates
                    int gridX = mousePos.x / tileSize;
                    int gridY = mousePos.y / tileSize;

                    // Check if coordinates are within bounds
                    if (gridX >= 0 && gridX < gridWidth && gridY >= 0 && gridY < gridHeight && !gameOver)
                    {
                        if (tileState[gridX][gridY] == 0)
                            tileState[gridX][gridY] = 2;
                        else if (tileState[gridX][gridY] == 2)
                            tileState[gridX][gridY] = 0;
                        else if (tileState[gridX][gridY] == -1)
                            tileState[gridX][gridY] = 3;
                        else if (tileState[gridX][gridY] == 3)
                            tileState[gridX][gridY] = -1;
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
                if (tileState[i][j] == 0)  // start tile
                    startTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 1)  // empty tile
                    emptyTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 2)  // flag tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == 3)  // flagged bomb tile
                    flagTileSprite.setPosition({ i * tileSize, j * tileSize });
                else if (tileState[i][j] == -1) {  // bomb tile
                    if (gameOver)
                        bombTileSprite.setPosition({ i * tileSize, j * tileSize });
                    else
                        startTileSprite.setPosition({ i * tileSize, j * tileSize });
                }
                else if (tileState[i][j] == -2) // active bomb tile
                    activeBombTileSprite.setPosition({ i * tileSize, j * tileSize });

                // Draw the appropriate tile
                if (tileState[i][j] == 0)  // start tile
                    window.draw(startTileSprite);
                else if (tileState[i][j] == 1)  // empty tile
                    window.draw(emptyTileSprite);
                else if (tileState[i][j] == 2)  // flag tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == 3)  // flagged bomb tile
                    window.draw(flagTileSprite);
                else if (tileState[i][j] == -1) {  // bomb tile
                    if(gameOver)
                        window.draw(bombTileSprite);
                    else
                        window.draw(startTileSprite);
                }
                else if (tileState[i][j] == -2) // active bomb tile
                    window.draw(activeBombTileSprite);
            }
        }

        // Update the window
        window.display();
    }
}
