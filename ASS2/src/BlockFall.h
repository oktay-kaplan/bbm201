#ifndef BLOCKFALL_H
#define BLOCKFALL_H

#include <vector>
#include <string>
#include "Block.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"


using namespace std;

class BlockFall {
public:

    BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name,
              const string &player_name);
    virtual ~BlockFall();

    int rows;  // Number of rows in the grid
    int cols;  // Number of columns in the grid
    vector<vector<int> > grid;  // 2D game grid
    vector<vector<bool>> power_up; // 2D matrix of the power-up shape
    Block * initial_block = nullptr; // Head of the list of game blocks. Must be filled up and initialized after a call to read_blocks()
    Block * active_rotation = nullptr; // Currently active rotation of the active block. Must start with the initial_block
    bool gravity_mode_on = false; // Gravity mode of the game
    unsigned long current_score = 0; // Current score of the game
    string leaderboard_file_name; // Leaderboard file name, taken from the command-line argument 5 in main
    string player_name; // Player name, taken from the command-line argument 6 in main
    Leaderboard leaderboard;


    void initialize_grid(const string & input_file); // Initializes the grid using the command-line argument 1 in main
    void read_blocks(const string & input_file); // Reads the input file and calls the read_block() function for each block;


    void print_grid() ;
    void rotate_active_block(bool clockwise) ;
    void move_active_block_right() ;
    void move_active_block_left() ;
    void clear_rows_and_update_score2();
    void drop_active_block() ;
    void toggle_gravity();
    void update_grid();
    bool can_move_left();
    bool can_move_right();
    void move_active_block_down();
    void clear_rows_and_update_score();
    bool findMatrix(const std::vector<std::vector<int>>& source, const std::vector<std::vector<bool>>& target) ;

    int count_occupied_cells(const vector<vector<bool>> block_shape) const;
    int count_occupied_cells(const vector<vector<int>> block_shape) const ;

    void apply_gravity_to_bottom() ;
    bool check_collision(const Block* block, int & position,int & position2) const ;
    bool check_collision() ;
    void printBlockShape(const vector<vector<bool>>& blockShape) ;
    void printGridShape(const vector<vector<int>>& blockShape) ;

private:
    Block* rotate_clockwise(const Block* block) ;
    vector<bool> parseRow(string basicString) ;
};

#endif // BLOCKFALL_H
