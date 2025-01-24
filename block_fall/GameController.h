#ifndef PA2_GAMECONTROLLER_H
#define PA2_GAMECONTROLLER_H

#include "BlockFall.h"

using namespace std;

class GameController {
public:
    bool play(BlockFall &game, const string &commands_file); // Function that implements the gameplay
    void print_grid(const std::vector<vector<int>>& grid);
    void add_block(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY);
    void remove_block(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY);
    bool checkCollision(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY);
    bool moveDown(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY);
    void gravity_on(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& block_shape, int startX, int startY);
    void gravity_switch_on(std::vector<std::vector<int>>& grid);
    void clear_row(std::vector<std::vector<int>>& grid, bool inDrop, unsigned long &score);
    bool control_power_up(BlockFall& game);
    int occupiedCellsAmount(const std::vector<std::vector<bool>>& block_shape);
    int occupiedCellsGrid(const std::vector<std::vector<int>>& grid);




};


#endif //PA2_GAMECONTROLLER_H
