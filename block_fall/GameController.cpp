#include "GameController.h"
#include <fstream>
#include <iostream>

bool GameController::play(BlockFall& game, const string& commands_file){

    // TODO: Implement the gameplay here while reading the commands from the input file given as the 3rd command-line
    //       argument. The return value represents if the gameplay was successful or not: false if game over,
    //       true otherwise.
    
    ifstream file(commands_file);
    if (file.is_open()) {
        string line;
        bool inDrop = false;
        add_block(game.grid, game.active_rotation->shape, 0, 0);

        while (getline(file, line)) {
            if (line.empty()) continue;
            
            else if (line == "PRINT_GRID") {
                std::cout << "Score: " << game.current_score << std::endl;
                std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;
                print_grid(game.grid);
                std::cout << std::endl;
            }
                
            else if (line == "ROTATE_RIGHT") {
                remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                if (!checkCollision(game.grid, game.active_rotation->right_rotation->shape, game.active_rotation->startX, game.active_rotation->startY)) {
                    game.active_rotation->right_rotation->startX = game.active_rotation->startX;
                    game.active_rotation->right_rotation->startY = game.active_rotation->startY;
                    game.active_rotation = game.active_rotation->right_rotation;
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                } else {
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                }
            }
                
            else if (line == "ROTATE_LEFT") {
                remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                if (!checkCollision(game.grid, game.active_rotation->left_rotation->shape, game.active_rotation->startX, game.active_rotation->startY)) {
                    game.active_rotation->left_rotation->startX = game.active_rotation->startX;
                    game.active_rotation->left_rotation->startY = game.active_rotation->startY;
                    game.active_rotation = game.active_rotation->left_rotation;
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                } else {
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                }
            }
                
            else if (line == "MOVE_RIGHT") {
                remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                if (!checkCollision(game.grid, game.active_rotation->shape, game.active_rotation->startX+1, game.active_rotation->startY)) {
                    game.active_rotation->startX = game.active_rotation->startX + 1;
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                } else {
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                }
            }
            
            else if (line == "MOVE_LEFT") {
                remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                if (!checkCollision(game.grid, game.active_rotation->shape, game.active_rotation->startX-1, game.active_rotation->startY)) {
                    game.active_rotation->startX = game.active_rotation->startX - 1;
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                } else {
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                }
            }
                
            else if (line == "DROP") {                                
                remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                while (moveDown(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY)) {
                    game.current_score += occupiedCellsAmount(game.active_rotation->shape);
                    game.active_rotation->startY = game.active_rotation->startY + 1;
                }
                add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                
                if (game.gravity_mode_on) {
                    gravity_on(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                }
                
                if (control_power_up(game)) {
                    std::cout << "Before clearing:" << std::endl;
                    print_grid(game.grid);
                    std::cout << std::endl;
                    game.current_score = 1000 + occupiedCellsGrid(game.grid) + game.current_score;
                    for (int r = 0; r < game.rows; ++r) {
                        for (int c = 0; c < game.cols; ++c) {
                            game.grid[r][c] = 0;
                        }
                    }
                } else {
                    inDrop = true;
                    clear_row(game.grid, inDrop, game.current_score);
                    inDrop = false;
                }
                
    
                if (game.active_rotation->next_block != nullptr) {
                    if (!checkCollision(game.grid, game.active_rotation->next_block->shape, 0, 0)) {
                        game.active_rotation = game.active_rotation->next_block;
                        add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                    } else {
                        std::cout << "GAME OVER!\nNext block that couldn't fit:" << std::endl;
                        for (const auto& row : game.active_rotation->next_block->shape) {
                            for (int cell : row) {
                                if (cell) std::cout << occupiedCellChar;
                                else std::cout << unoccupiedCellChar;
                            }
                            std::cout<<endl;
                        }
                        std::cout<<endl;
                        std::cout << "Final grid and score:" << std::endl;
                        std::cout << std::endl;
                        std::cout << "Score: " << game.current_score << std::endl;
                        std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;
                        print_grid(game.grid);
                        return false;
                    }
                }else {
                    std::cout << "YOU WIN!\nNo more blocks." << std::endl;
                    std::cout << "Final grid and score:" << std::endl;
                    std::cout << std::endl;
                    std::cout << "Score: " << game.current_score << std::endl;
                    std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;
                    print_grid(game.grid);
                    return true;
                }
            }
                
            else if (line == "GRAVITY_SWITCH") {
                game.gravity_mode_on = !game.gravity_mode_on;
                if (game.gravity_mode_on) {
                    remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
                    gravity_switch_on(game.grid);
                    clear_row(game.grid, inDrop, game.current_score);
                    add_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);

                }
            }
                
            else {
                std::cout << "Unknown command: " << line << std::endl;
            }
        }
        if(game.active_rotation!=nullptr) {
            remove_block(game.grid, game.active_rotation->shape, game.active_rotation->startX, game.active_rotation->startY);
        }
        std::cout << "GAME FINISHED!\nNo more commands." << std::endl;
        std::cout << "Final grid and score:" << std::endl;
        std::cout << std::endl;
        std::cout << "Score: " << game.current_score << std::endl;
        std::cout << "High Score: " << game.leaderboard.head_leaderboard_entry->score << std::endl;
        print_grid(game.grid);
        
        file.close();
        return true;
    } else {
        cout << "The file could not be opened!" << endl;
    }
    

    return false;
}



void GameController::print_grid(const std::vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell == 0) {
                std::cout << unoccupiedCellChar;
            } else {
                std::cout << occupiedCellChar;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameController::add_block(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY) {
    int blockHeight = block_shape.size();
    int blockWidth = block_shape[0].size();

    for (int i = 0; i < blockHeight; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            if (block_shape[i][j]) {
                grid[startY + i][startX + j] = 1;
            }
        }
    }
}

void GameController::remove_block(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY) {
    int blockHeight = block_shape.size();
    int blockWidth = block_shape[0].size();

    for (int i = 0; i < blockHeight; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            if (block_shape[i][j]) {
                grid[startY + i][startX + j] = 0;
            }
        }
    }
}

bool GameController::checkCollision(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY) {
    
    int blockHeight = block_shape.size();
    int blockWidth = block_shape[0].size();

    for (int i = 0; i < blockHeight; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            if (startY + i >= grid.size() || startX + j >= grid[0].size() || (grid[startY + i][startX + j] != 0 && block_shape[i][j] != false)) {
                return true;
            }
        }
    }
    return false;
}

bool GameController::moveDown(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY) {
    int blockHeight = block_shape.size();
    int blockWidth = block_shape[0].size();
    int rows = grid.size();

    for (int i = 0; i < blockHeight; ++i) {
        for (int j = 0; j < blockWidth; ++j) {
            if (block_shape[i][j] != 0) {
                int x = startX + j;
                int y = startY + i + 1;

                if (y >= rows || (y < rows && grid[y][x] != 0)) {
                    return false;
                }
            }
        }
    }
    return true;
}


void GameController::gravity_on(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY) {
    int blockHeight = block_shape.size();
    int blockWidth = block_shape[0].size();
    int rows = grid.size();
    
    for (int i = blockHeight - 1; i >= 0; --i) {
        for (int j = 0; j < blockWidth; ++j) {
            if (block_shape[i][j] != 0) {
                int x = startX + j;
                int y = startY + i;
                                
                grid[y][x] = 0;

                while (y < rows - 1 && grid[y + 1][x] == 0) {
                    y++;
                }
                grid[y][x] = 1;
            }
        }
    }
}

void GameController::gravity_switch_on(std::vector<std::vector<int>>& grid) {
    int rows = grid.size();
    
    for (int i = rows - 1; i >= 0; --i) {
        for (int j = 0; j < grid[0].size(); ++j) {
            if (grid[i][j] != 0) {
                int y = i;
                grid[y][j] = 0;

                while (y < rows - 1 && grid[y + 1][j] == 0) {
                    y++;
                }
                grid[y][j] = 1;
            }
        }
    }
}

void GameController::clear_row(std::vector<std::vector<int>>& grid, bool inDrop, unsigned long& score) {
    int rows = grid.size();
    int cols = grid[0].size();
    bool isThereFullRow = true;
    
    while (isThereFullRow) {
        isThereFullRow = false;
        for (int i = rows - 1; i >= 0; --i) {
            bool fullyOccupied = true;
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] == 0) {
                    fullyOccupied = false;
                    break;
                }
            }
            if (fullyOccupied) {
                if (inDrop) {
                    std::cout << "Before clearing:" << std::endl;
                    print_grid(grid);
                    std::cout << std::endl;
                }
                for (int j = 0; j < cols; ++j) {
                    grid[i][j] = 0;
                }
                score += cols;
                isThereFullRow = true;
                
                for (int k = i - 1; k >= 0; --k) {
                    for (int j = 0; j < cols; ++j) {
                        grid[k + 1][j] = grid[k][j];
                    }
                }
                for (int j = 0; j < cols; ++j) {
                    grid[0][j] = 0;
                }
            }
        }
    }    
}


bool GameController::control_power_up(BlockFall& game) {
    int powerUpRows = game.power_up.size();
    int powerUpCols = game.power_up[0].size();

    for (int i = 0; i <= game.rows - powerUpRows; ++i) {
        for (int j = 0; j <= game.cols - powerUpCols; ++j) {
            bool foundPowerUp = true;
            for (int r = 0; r < powerUpRows; ++r) {
                for (int c = 0; c < powerUpCols; ++c) {
                    if (game.power_up[r][c] != (game.grid[i + r][j + c] != 0)) {
                        foundPowerUp = false;
                        break;
                    }
                }
                if (!foundPowerUp) break;
            }
            if (foundPowerUp) {
                return true;
            }
        }
    }
    return false;
}

int GameController::occupiedCellsAmount(const std::vector<std::vector<bool>>& block_shape) {
    int amount = 0;
    for (const auto& row : block_shape) {
        for (bool cell: row) {
            if (cell) amount++;
        }
    }
    return amount;
}

int GameController::occupiedCellsGrid(const std::vector<std::vector<int>>& grid) {
    int amount = 0;
    for (const auto& row : grid) {
        for (int cell: row) {
            if (cell==1) {
                amount++;
            }
        }
    }
    return amount;
}