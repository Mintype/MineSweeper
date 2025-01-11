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
        if (tileState[x][y] == BOMB)
            isValidPlacement = false;

        if (isValidPlacement) {
            tileState[x][y] = BOMB;  // -3 will represent a bomb
            //std::cout << x << ", " << y << "\n";
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
    int orgX, int orgY, int& tilesLeft) {
    // Debug: Initial position and state
    //std::cout << "Running at (" << gridX << ", " << gridY << ")\n";

    // Check for out-of-bounds or already revealed tiles or flagged tiles
    if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
        //std::cout << "Out of bounds\n";
        return;
    }

    if (revealed[gridX][gridY]) {
        if (gridX != orgX && gridY != orgY) {
            //std::cout << "Tile already revealed\n";
            return;
        }
    }

    if (tileState[gridX][gridY] == FLAG) {
        //std::cout << "Tile is flagged\n";
        return;
    }

    // Count the neighboring bombs
    int bombCount = countBombs(tileState, gridWidth, gridHeight, gridX, gridY);

    //std::cout << "Bomb count at (" << gridX << ", " << gridY << "): " << bombCount << "\n";

    // Set the tile's state to the bomb count
    tileState[gridX][gridY] = bombCount;

    // Mark tile as revealed
    revealed[gridX][gridY] = true;
    --tilesLeft;
    //std::cout << "Revealed tile (" << gridX << ", " << gridY << ")\n";

    // If no bombs around, recursively open neighbors
    if (bombCount == 0) {
        //std::cout << "No bombs around, opening neighbors...\n";
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;  // Skip the current tile

                int neighborX = gridX + dx;
                int neighborY = gridY + dy;

                // Check if the neighbor is out of bounds or already revealed
                if (neighborX < 0 || neighborX >= gridWidth || neighborY < 0 || neighborY >= gridHeight || revealed[neighborX][neighborY])
                    continue;

                //std::cout << "Recursively opening neighbor (" << neighborX << ", " << neighborY << ")\n";
                runOpeningAlgorithm(tileState, revealed, neighborX, neighborY, gridWidth, gridHeight, orgX, orgY, tilesLeft);
            }
        }
    }
    else {
        //std::cout << "Bomb count > 0, no recursion needed.\n";
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

    //***

    const sf::Texture resetButtonTexture("resources/images/reset_button.png"); // 48px x 48px
    const sf::Texture resetButtonClickedTexture("resources/images/reset_button_clicked.png"); // 48px x 48px
    sf::Sprite resetButtonSprite(resetButtonTexture);
    // Center the reset button in the window
    resetButtonSprite.setPosition({ 176, 25 });


    //***

    // Set up the grid size (8x8)
    const int gridWidth = 8;
    const int gridHeight = 8;
    const float tileSize = 32.0f;  // Size of each tile in pixels

    bool gameOver = false;
    int numberOfBombs = 10;
    int tilesLeft = gridWidth * gridHeight;
    const float horizontalOffset = (window.getSize().x - gridWidth * tileSize) / 2.0f;  // Center horizontally
    const float verticalOffset = 100.0f; // Space from the top for buttons

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
            //std::cout << tilesLeft << "\n";

            if (tilesLeft == numberOfBombs) {
                // Check the tileState vector and count for BOMB_WITH_FLAG (which is -5)
                int flaggedBombs = 0;
                for (int i = 0; i < gridWidth; ++i)
                    for (int j = 0; j < gridHeight; ++j)
                        if (tileState[i][j] == BOMB_WITH_FLAG)
                            ++flaggedBombs;

                // If the number of flagged bombs equals the total number of bombs, the game is won
                if (flaggedBombs == numberOfBombs)
                    gameOver = true;  // End the game
            }


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

                    // Adjust the mouse position by subtracting the offsets
                    int adjustedX = mousePos.x - horizontalOffset;
                    int adjustedY = mousePos.y - verticalOffset;

                    // Convert adjusted mouse position to grid coordinates
                    int gridX = adjustedX / tileSize;
                    int gridY = adjustedY / tileSize;

                    // Get the button's global bounds
                    sf::FloatRect buttonBounds = resetButtonSprite.getGlobalBounds();

                    // Check if the mouse click is within the button bounds
                    sf::Vector2 mouseCords((static_cast<float> (mousePos.x)), (static_cast<float> (mousePos.y)));
                    if (buttonBounds.contains(mouseCords))
                    {
                        // Mouse clicked on the reset button, change the texture to the clicked version
                        //resetButtonSprite.setTexture(resetButtonClickedTexture);

                        tileState = std::vector<std::vector<int>>(gridWidth, std::vector<int>(gridHeight, -1));
                        tileRevealed = std::vector<std::vector<bool>>(gridWidth, std::vector<bool>(gridHeight, false));

                        bombsPlaced = false;
                        tilesLeft = gridX * gridY;
                        gameOver = false;

                    }

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
                                //--tilesLeft;
                                placeBombs(tileState, gridWidth, gridHeight, numberOfBombs, gridX, gridY); // 10 bombs
                                bombsPlaced = true;

                                // Run opening algorithm
                                runOpeningAlgorithm(tileState, tileRevealed, gridX, gridY, gridWidth, gridHeight, gridX, gridY, tilesLeft);

                            }
                            else {
                                //tileState[gridX][gridY] = 0; // Reveal an empty tile
                                int bombCount = countBombs(tileState, gridWidth, gridHeight, gridX, gridY);
                                tileState[gridX][gridY] = bombCount;
                                tileRevealed[gridX][gridY] = true;
                                --tilesLeft;
                            }
                        }
                    }
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                    // Get the mouse position relative to the window
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Adjust the mouse position by subtracting the offsets
                    int adjustedX = mousePos.x - horizontalOffset;
                    int adjustedY = mousePos.y - verticalOffset;

                    // Convert adjusted mouse position to grid coordinates
                    int gridX = adjustedX / tileSize;
                    int gridY = adjustedY / tileSize;
                    
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
            //else if (event->is<sf::Event::MouseButtonPressed>()) {
            //    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            //        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            //        // Get the button's global bounds
            //        sf::FloatRect buttonBounds = resetButtonSprite.getGlobalBounds();

            //        std::cout << "ewfwe";

            //        // Check if the mouse click is within the button bounds
            //        sf::Vector2 mouseCords((static_cast<float> (mousePos.x)), (static_cast<float> (mousePos.y)));
            //        if (buttonBounds.contains(mouseCords))
            //        {
            //            std::cout << "jhhhh";
            //            // Mouse clicked on the reset button, change the texture to the clicked version
            //            resetButtonSprite.setTexture(resetButtonClickedTexture);

            //            // Optionally, you can handle any reset game logic here, like resetting the grid
            //        }
            //    }
            //}
        }

        //    FLAG = -6,
        //    BOMB_WITH_FLAG = -5,
        //    ACTIVE_BOMB = -4,
        //    BOMB = -3,
        //    EMPTY = 0,
        //    START = -1

        window.clear(sf::Color(143, 143, 143)); // a light gray.

        // Draw the grid of sprites
        for (int i = 0; i < gridWidth; ++i)
        {
            for (int j = 0; j < gridHeight; ++j)
            {
                // Apply the offsets when positioning the tiles
                sf::Vector2f position(i * tileSize + horizontalOffset, j * tileSize + verticalOffset);

                // Position each sprite in the grid
                if (tileState[i][j] == START)  // start tile
                    startTileSprite.setPosition(position);
                else if (tileState[i][j] == EMPTY)  // empty tile
                    emptyTileSprite.setPosition(position);
                else if (tileState[i][j] == FLAG)  // flag tile
                    flagTileSprite.setPosition(position);
                else if (tileState[i][j] == BOMB_WITH_FLAG)  // flagged bomb tile
                    flagTileSprite.setPosition(position);
                else if (tileState[i][j] == BOMB) {  // bomb tile
                    if (gameOver)
                        bombTileSprite.setPosition(position);
                    else
                        startTileSprite.setPosition(position);
                }
                else if (tileState[i][j] == ACTIVE_BOMB) // active bomb tile
                    activeBombTileSprite.setPosition(position);
                else if (tileState[i][j] == 1) // 1 bomb near
                    oneTileSprite.setPosition(position);
                else if (tileState[i][j] == 2) // 2 bomb near
                    twoTileSprite.setPosition(position);
                else if (tileState[i][j] == 3) // 3 bomb near
                    threeTileSprite.setPosition(position);
                else if (tileState[i][j] == 4) // 4 bomb near
                    fourTileSprite.setPosition(position);

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

        window.draw(resetButtonSprite);

        // Update the window
        window.display();
    }
}
