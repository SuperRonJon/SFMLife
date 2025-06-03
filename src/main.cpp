#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
#include <vector>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int GRID_SIZE = 5; 
const int GRID_WIDTH = WINDOW_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / GRID_SIZE;
const int PLAYING_FRAMERATE = 24;
const int DRAWING_FRAMERATE = 100;

int currentFramerate = PLAYING_FRAMERATE;

const sf::Color ALIVE_COLOR = sf::Color::White;
const sf::Color DEAD_COLOR = sf::Color::Black;
const bool ALIVE = true;
const bool DEAD = false;

std::vector<std::vector<bool>> initCells(bool randomize);
void updateCells(std::vector<std::vector<bool>> &cells, std::vector<std::vector<bool>> &nextCells);
void drawGrid(sf::RenderWindow& window);
void drawGridFromCells(sf::RenderWindow& window, std::vector<std::vector<bool>> const &cells);
std::vector<std::vector<bool>> randomizeCells(std::vector<std::vector<bool>> const &cells);
int modulo(int a, int b);
sf::Color getCellColor(bool cellValue);

int main() {

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Conway's Game of Life");
    window.setFramerateLimit(currentFramerate);
    std::vector<std::vector<bool>> cells = initCells(true);
    std::vector<std::vector<bool>> previousCells = cells;
    std::vector<std::vector<bool>> savedCells = cells;
    bool isPlaying = true;
    bool saveOnStart = false;

    bool leftMouseButtonHeld = false;
    bool rightMouseButtonHeld = false;

    while (window.isOpen()) {
        bool stepOne = false;
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch(keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Right:
                        stepOne = true;
                        if(saveOnStart) {
                            savedCells = cells;
                            saveOnStart = false;
                        }
                        break;
                    case sf::Keyboard::Scancode::Left:
                        cells = savedCells;
                        if(!isPlaying) {
                            saveOnStart = true;
                        }
                        break;
                }
            }
            else if(const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                switch(keyReleased->scancode) {
                    case sf::Keyboard::Scancode::Space:
                        isPlaying = !isPlaying;
                        currentFramerate = isPlaying ? PLAYING_FRAMERATE : DRAWING_FRAMERATE;
                        window.setFramerateLimit(currentFramerate);
                        if(isPlaying && saveOnStart) {
                            savedCells = cells;
                            saveOnStart = false;
                        }
                        break;
                    case sf::Keyboard::Scancode::Backspace:
                        cells = initCells(false);
                        saveOnStart = true;
                        break;
                    case sf::Keyboard::Scancode::Enter:
                        cells = initCells(true);
                        break;
                    case sf::Keyboard::Scancode::Escape:
                        window.close();
                        break;
                }
            }
            else if(const auto* mouseClicked = event->getIf<sf::Event::MouseButtonPressed>()) {
                switch(mouseClicked->button) {
                    case sf::Mouse::Button::Left:
                        if(!isPlaying) {
                            int colIndex = (mouseClicked->position.x - (mouseClicked->position.x % GRID_SIZE)) / GRID_SIZE;
                            int rowIndex = (mouseClicked->position.y - (mouseClicked->position.y % GRID_SIZE)) / GRID_SIZE;
                            cells[rowIndex][colIndex] = ALIVE;
                            leftMouseButtonHeld = true;
                        }
                        break;
                    case sf::Mouse::Button::Right:
                        if(!isPlaying) {
                            int colIndex = (mouseClicked->position.x - (mouseClicked->position.x % GRID_SIZE)) / GRID_SIZE;
                            int rowIndex = (mouseClicked->position.y - (mouseClicked->position.y % GRID_SIZE)) / GRID_SIZE;
                            cells[rowIndex][colIndex] = DEAD;
                            rightMouseButtonHeld = true;
                        }
                        break;
                }
            }
            else if(const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                switch(mouseReleased->button) {
                    case sf::Mouse::Button::Left:
                        leftMouseButtonHeld = false;
                        break;
                    case sf::Mouse::Button::Right:
                        rightMouseButtonHeld = false;
                        break;
                }
            }
            else if(const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                if(!isPlaying && leftMouseButtonHeld) {
                    int colIndex = (mouseMoved->position.x - (mouseMoved->position.x % GRID_SIZE)) / GRID_SIZE;
                    int rowIndex = (mouseMoved->position.y - (mouseMoved->position.y % GRID_SIZE)) / GRID_SIZE;
                    if(rowIndex < GRID_HEIGHT && colIndex < GRID_WIDTH)
                        cells[rowIndex][colIndex] = ALIVE;
                }
                else if(!isPlaying && rightMouseButtonHeld) {
                    int colIndex = (mouseMoved->position.x - (mouseMoved->position.x % GRID_SIZE)) / GRID_SIZE;
                    int rowIndex = (mouseMoved->position.y - (mouseMoved->position.y % GRID_SIZE)) / GRID_SIZE;
                    if(rowIndex < GRID_HEIGHT && colIndex < GRID_WIDTH)
                        cells[rowIndex][colIndex] = DEAD;
                }
            }
        }
        window.clear();    
        drawGridFromCells(window, cells);   
         
        window.display();
        if(isPlaying || stepOne) {
            updateCells(cells, previousCells);
        }
    }

    return 0;
}

// Draws alternating grid across entire field, not used in implementation only to show grid size in development
void drawGrid(sf::RenderWindow& window){
    // Draw the grid
    for (int row = 0; row < GRID_HEIGHT; ++row) {
        for (int col = 0; col < GRID_WIDTH; ++col) {
            sf::RectangleShape square(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            square.setPosition(sf::Vector2f(col * GRID_SIZE, row * GRID_SIZE));

            // Alternate colors
            if ((row + col) % 2 == 0)
                square.setFillColor(sf::Color::White);
            else
                square.setFillColor(sf::Color::Black);

            window.draw(square);
        }
    }
}

// Draws the grid according to the cell values
void drawGridFromCells(sf::RenderWindow& window, std::vector<std::vector<bool>> const &cells){
    for(int row = 0; row < GRID_HEIGHT; ++row){
        for(int col = 0; col < GRID_WIDTH; ++col){
            sf::RectangleShape square(sf::Vector2f(GRID_SIZE, GRID_SIZE));
            square.setPosition(sf::Vector2f(col*GRID_SIZE, row*GRID_SIZE));
            square.setFillColor(getCellColor(cells[row][col]));
            window.draw(square);
        }
    }
}

// Returns an entirely dead grid if randomize=false, otherwise a randomly generated grid
std::vector<std::vector<bool>> initCells(bool randomize){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 3);
    std::vector<std::vector<bool>> cells;

    for(int i = 0; i < GRID_HEIGHT; i++){
        std::vector<bool> row(GRID_WIDTH, DEAD);
        cells.push_back(row);
        for(int j = 0; j < GRID_WIDTH; j++){
            if(distr(gen) == 0 && randomize){
                cells[i][j] = ALIVE;
            }
        }
    }

    return cells;
}

// Randomly swaps cell values
std::vector<std::vector<bool>> randomizeCells(std::vector<std::vector<bool>> const &cells) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 3);
    std::vector<std::vector<bool>> nextCells;

    for(int i = 0; i < GRID_HEIGHT; i++) {
        std::vector<bool> row(GRID_WIDTH, DEAD);
        nextCells.push_back(row);
        for(int j = 0; j < GRID_WIDTH; j++) {
            if(distr(gen) == 0) {
                nextCells[i][j] = ALIVE;
            }
        }
    }

    return nextCells;
}

// Updates cell values according to Conway's Game of Life rules
void updateCells(std::vector<std::vector<bool>> &currentCells, std::vector<std::vector<bool>> &nextCells){
    
    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int j = 0; j < GRID_WIDTH; j++) {
            const int LEFT = modulo((j - 1), GRID_WIDTH);
            const int RIGHT = modulo((j + 1), GRID_WIDTH);
            const int UP = modulo((i - 1), GRID_HEIGHT);
            const int DOWN = modulo((i + 1), GRID_HEIGHT);

            int numNeighbors = 0;
            if(currentCells[UP][LEFT] == ALIVE)
                numNeighbors++;
            if(currentCells[UP][j] == ALIVE)
                numNeighbors++;
            if(currentCells[UP][RIGHT] == ALIVE)
                numNeighbors++;
            if(currentCells[i][LEFT] == ALIVE)
                numNeighbors++;
            if(currentCells[i][RIGHT] == ALIVE)
                numNeighbors++;
            if(currentCells[DOWN][LEFT] == ALIVE)
                numNeighbors++;
            if(currentCells[DOWN][j] == ALIVE)
                numNeighbors++;
            if(currentCells[DOWN][RIGHT] == ALIVE)
                numNeighbors++;
            
            if(currentCells[i][j] == ALIVE && (numNeighbors == 2 || numNeighbors == 3)) {
                nextCells[i][j] = ALIVE;
            }
            else if(currentCells[i][j] == DEAD && numNeighbors == 3) {
                nextCells[i][j] = ALIVE;
            }
            else {
                nextCells[i][j] = DEAD;
            }
        }
    }

    std::swap(currentCells, nextCells);
}

int modulo(int a, int b) {
    return (b + (a % b)) % b;
}

sf::Color getCellColor(bool cellValue) {
    return cellValue ? ALIVE_COLOR : DEAD_COLOR;
}
