#include "BlockFall.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


using namespace std;

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    // TODO: Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // TODO: For every block, generate its rotations and properly implement the multilevel linked list structure
    //       that represents the game blocks, as explained in the PA instructions.
    // TODO: Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)
    
    ifstream file(input_file);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<vector<bool>> block_shape;
            while (true) {
                vector<bool> row;
                char character;
                for(char c: line) {
                    character = c;
                    if (c == '1'){
                        row.push_back(true);
                    }else if (c == '0') {
                        row.push_back(false);
                    }
                }
                
                block_shape.push_back(row);
                if (character == ']') break;
                getline(file, line);
            }
            if (block_shape.size() == 0) continue;
            Block *new_block = new Block();            
            new_block->shape = block_shape;
            
            if (initial_block == nullptr) {
                initial_block = new_block;
                active_rotation = new_block;
            } else {
                Block *current = initial_block;
                Block *iterate = initial_block;
                while (current->next_block != nullptr) {
                    current = current->next_block;
                    iterate = iterate->next_block;
                }
                current->next_block = new_block;
                while (current->right_rotation != nullptr && current->right_rotation != iterate) {
                    current->right_rotation->next_block = new_block;
                    current = current->right_rotation;
                }
            }
            
            int ro = new_block->shape.size();
            int co = 0;
            for (const auto& innerVector : new_block->shape) {
                if (innerVector.size() > co) {
                    co = innerVector.size();
                }
            }

            for (int i = 0; i < 3; ++i) {
                
                if (i!=0) {
                    int hodor = ro;
                    ro = co;
                    co = hodor;
                }
                vector<vector<bool>> rotatedShape;
                for (int c = 0; c < co; ++c) {
                    vector<bool> newRow;
                    for (int r = ro - 1; r >= 0; --r) {
                        if (i==0) newRow.push_back(new_block->shape[r][c]);
                        else if (i==1) newRow.push_back(new_block->right_rotation->shape[r][c]);
                        else if (i==2) newRow.push_back(new_block->right_rotation->right_rotation->shape[r][c]);
                    }
                    rotatedShape.push_back(newRow);
                }
                
                if (i==0 && rotatedShape == new_block->shape) {
                    new_block->right_rotation = new_block;
                    new_block->left_rotation = new_block;
                    break;
                } else if (new_block->right_rotation != nullptr && new_block->shape == rotatedShape) {
                    new_block->left_rotation = new_block->right_rotation;
                    new_block->right_rotation->right_rotation = new_block;
                    break;
                } else {
                    Block *rotated = new Block();
                    rotated->shape = rotatedShape;
                    if (i==0) {
                        new_block->right_rotation = rotated;
                        new_block->right_rotation->left_rotation = new_block;
                    }
                    else if (i==1) {
                        new_block->right_rotation->right_rotation = rotated;
                        new_block->right_rotation->right_rotation->left_rotation = new_block->right_rotation;
                    }
                    else if (i == 2) {
                        new_block->right_rotation->right_rotation->right_rotation = rotated;
                        new_block->right_rotation->right_rotation->right_rotation->left_rotation = new_block->right_rotation->right_rotation;
                        new_block->right_rotation->right_rotation->right_rotation->right_rotation = new_block;
                        new_block->left_rotation = new_block->right_rotation->right_rotation->right_rotation;
                    }
                }
                
            }
            
        }
        Block *last = initial_block;
        Block *pre = initial_block;
        while (last->next_block != nullptr) {
            pre = last;
            last = last->next_block;
        }
        power_up = last->shape;
        
        Block *rightN = last->right_rotation;
        while (rightN != nullptr && rightN != last) {
            Block *right_temp = rightN;
            rightN = rightN->right_rotation;
            delete right_temp;
        }
        delete last;
        
        pre->next_block = nullptr;
        
        file.close();
    } else {
        cout << "The file could not be opened!" << endl;
    }

}

void BlockFall::initialize_grid(const string &input_file) {
    // TODO: Initialize "rows" and "cols" member variables
    // TODO: Initialize "grid" member variable using the command-line argument 1 in main
    rows = 0;
    cols = 0;
    ifstream file(input_file);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            vector<int> row;
            for (int j = 0; j < line.size(); ++j) {
                if (isdigit(line[j])) {
                    row.push_back(line[j]-'0');
                }
            }
            if (!row.empty()) {
                grid.push_back(row);
                cols = max(cols, static_cast<int>(row.size()));
                ++rows;
            }
        }
        file.close();
    } else {
        cout << "The file could not be opened!" << endl;
    }
}


BlockFall::~BlockFall() {
    // TODO: Free dynamically allocated memory used for storing game blocks-    
    Block *curr = initial_block;
    while (curr != nullptr) {
        Block *temporary = curr;
        curr = curr->next_block;

        Block *right = temporary->right_rotation;
        while (right != nullptr && right != temporary) {
            Block *right_temp = right;
            right = right->right_rotation;
            delete right_temp;
        }

        delete temporary;
    }
    initial_block = nullptr;
    active_rotation = nullptr;
}