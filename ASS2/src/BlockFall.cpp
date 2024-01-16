#include "BlockFall.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#define occupiedCellChar  "██"
#define unoccupiedCellChar  "▒▒"


std::vector<bool> parseRow(string basicString);

using namespace std;

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);


}
void BlockFall::read_blocks(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error opening file: " << input_file << endl;
        exit(EXIT_FAILURE);
    }
    string line;
    Block *first_block = nullptr; // Track the first block
    Block *tail = nullptr; // Track the first block

    vector<vector<bool>> blockShape;

    while (getline(file, line)) {


        if (line.find('[') != string::npos) {
            blockShape.clear();
            if (line.find(']') == string::npos){
                vector<bool> row = parseRow(line);
                blockShape.push_back(row);
            }
        }
        if (line.size() == 0){
            continue;
        }

        if (line.find(']') != string::npos) {



            vector<bool> row = parseRow(line);
            blockShape.push_back(row);

            Block *newBlock = new Block;
            newBlock->shape = blockShape;

            newBlock->xCoordinat = 0;
            newBlock->yCoordinat = 0;

            // Set the initial rotation to the right
            Block* currentRotation = newBlock;

            for (int i = 0; i < 3; ++i) {
                // Perform a right rotation and update the connections
                currentRotation->right_rotation = rotate_clockwise(currentRotation);
                currentRotation->right_rotation->left_rotation = currentRotation;

                // Move to the next rotation
                currentRotation = currentRotation->right_rotation;
            }


            // Make it circular by connecting the last rotation to the first one
            currentRotation->right_rotation = newBlock;
            currentRotation->right_rotation->left_rotation = currentRotation;


            if (initial_block == nullptr){
                tail = newBlock;
                initial_block = newBlock;
                active_rotation = newBlock;
                first_block = newBlock;

            }

            if (tail != nullptr){
                tail->next_block = newBlock;
                tail->right_rotation->right_rotation->next_block = newBlock;
                tail->left_rotation->next_block = newBlock;
                tail->right_rotation->next_block = newBlock;
            }

            tail = newBlock;

        }else if (line.find('[') == string::npos && line.find(']') == string::npos){
            vector<bool> row = parseRow(line);
            blockShape.push_back(row);

        }
    }

    power_up = tail->shape;

    while(first_block->next_block->next_block != nullptr){
        first_block = first_block->next_block;

    }


    first_block->next_block = nullptr;
    first_block->right_rotation->right_rotation->next_block = nullptr;
    first_block->left_rotation->next_block = nullptr;
    first_block->right_rotation->next_block = nullptr;


    delete tail;




    file.close();
}

vector<bool> BlockFall::parseRow(string basicString) {
    vector<bool> row;
    for (char c : basicString) {
        if (c == '1') {
            row.push_back(true);
        } else if (c == '0') {
            row.push_back(false);
        }
    }
    return row;
}



void BlockFall::printBlockShape(const vector<vector<bool>>& blockShape) {
    for (const auto& row : blockShape) {
        for (bool value : row) {
            cout << (value ? occupiedCellChar : unoccupiedCellChar) << "";
        }
        cout << endl;
    }
}

void BlockFall::printGridShape(const vector<vector<int>>& blockShape) {
    for (const auto& row : blockShape) {
        for (bool value : row) {
            cout << (value ? occupiedCellChar : unoccupiedCellChar) << "";


        }
        cout << endl;
    }

    cout << endl;
    cout << endl;



}


Block* BlockFall::rotate_clockwise(const Block *block) {
    // Rotate the block clockwise
    vector<vector<bool>> rotated(block->shape[0].size(), vector<bool>(block->shape.size(), false));

    Block *newRotation = new Block;
    for (int i = 0; i < block->shape.size(); ++i) {
        for (int j = 0; j < block->shape[0].size(); ++j) {
            rotated[j][block->shape.size() - 1 - i] = block->shape[i][j];
        }
    }

    newRotation->shape = rotated;
    newRotation->xCoordinat = block->xCoordinat;
    newRotation->yCoordinat = block->yCoordinat;

    return newRotation;
}


BlockFall::~BlockFall() {
    // TODO: Free dynamically allocated memory used for storing game blocks
    Block* current = initial_block;
    while (current != nullptr) {
        Block* next = current->next_block;
        delete current->right_rotation->right_rotation;
        delete current->right_rotation;
        delete current->left_rotation;
        delete current;
        current = next;
    }
}


void BlockFall::initialize_grid(const string &input_file) {
    ifstream file(input_file);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << input_file << endl;
        exit(EXIT_FAILURE);
    }

    // Initialize rows and cols
    rows = 0;
    cols = 0;

    // Count the number of rows and columns
    string line;
    while (getline(file, line)) {
        vector<int> row;
        for (char c : line) {
            if (c == '1') {
                row.push_back(1);
            } else if (c == '0') {
                row.push_back(0);
            }
        }
        grid.push_back(row);

    }

    rows = grid.size();
    cols = grid[0].size();



    file.close();
}
// Inside the BlockFall class

void BlockFall::print_grid()  {

    // Print the current score and all-time high score
    cout << "Score: " << current_score << endl;
    cout << "High Score: " << leaderboard.get_high_score() << endl;

    int number = 0;
    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;

    // Grid üzerindeki ilgili hücreleri güncelle ve temizle
    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
            // Aktif blokun ilgili hücresinin grid içindeki konumu
            int gridX = blockX + j;
            int gridY = blockY + i;

            // Sadece dolu hücreleri güncelle ve temizle
            if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                grid[gridY][gridX] = 1;  // Temizle

            }
        }
    }



    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {

            if (grid[i][j] == 1) {
                cout << occupiedCellChar<<"";;  // Assuming '██' represents an occupied cell
            }
            else {
                cout << unoccupiedCellChar<<"";;  // Assuming '▒▒' represents an unoccupied cell
            }
        }

        cout << endl;


    }

    cout << endl;
    cout << endl;


    int blockX1 = active_rotation->xCoordinat;
    int blockY1 = active_rotation->yCoordinat;

    for (int i = 0; i < active_rotation->shape.size(); ++i) {
        for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
            int gridX = blockX1 + j;
            int gridY = blockY1 + i;

            if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                grid[gridY][gridX] = 0;  // Temizle

            }
        }
    }
}



void BlockFall::rotate_active_block(bool clockwise) {

    // Check if there is an active block to rotate
    if (active_rotation == nullptr) {
        return;
    }

    // Save the current state of the active block
    Block* temp_block = new Block(*active_rotation);

    // Rotate the temporary block
    if (clockwise){

        temp_block = (active_rotation->right_rotation);
        temp_block->shape = (active_rotation->right_rotation->shape);

        temp_block->next_block = active_rotation->right_rotation->next_block;
        temp_block->xCoordinat = active_rotation->xCoordinat;
        temp_block->yCoordinat = active_rotation->yCoordinat;

    }else{

        temp_block = (active_rotation->left_rotation);
        temp_block->shape = (active_rotation->left_rotation->shape);

        temp_block->next_block = active_rotation->left_rotation->next_block;
        temp_block->xCoordinat = active_rotation->xCoordinat;
        temp_block->yCoordinat = active_rotation->yCoordinat;


    }
    // Check for collisions and update the game state accordingly
    if (!check_collision(temp_block, active_rotation->xCoordinat, active_rotation->yCoordinat)) {
        // No collision, update the active block

        int blockX = active_rotation->xCoordinat;
        int blockY = active_rotation->yCoordinat;




        // Grid üzerindeki ilgili hücreleri güncelle ve temizle
        for (int i = 0; i < active_rotation->shape.size(); ++i) {
            for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
                // Aktif blokun ilgili hücresinin grid içindeki konumu
                int gridX = blockX + j;
                int gridY = blockY + i;

                // Sadece dolu hücreleri güncelle ve temizle
                if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                    grid[i][j] = 0;  // Temizle
                }
            }
        }

        active_rotation = temp_block;




    } else {

        // Collision occurred, free the temporary block
    }
}



bool BlockFall::check_collision(const Block* block, int & position,int & position2) const {
    // Get the dimensions of the block
    int block_rows = block->shape.size();
    int block_cols = block->shape[0].size();

    for (int i = 0; i < block_rows; ++i) {
        for (int j = 0; j < block_cols; ++j) {
            int gridX = position + j;
            int gridY = position2 + i;

            if (block->shape[i][j] == 1) {
                if (gridX < 0 || gridX >= cols || gridY >= rows || (gridY >= 0 && gridY < rows && grid[gridY][gridX] != 0)) {
                    // Çakışma var
                    return true;
                }

                if (gravity_mode_on) {
                    for (int k = i + 1; k < block_rows; ++k) {
                        int belowGridY = position2 + k;
                        if (belowGridY >= rows || (belowGridY >= 0 && belowGridY < rows && grid[belowGridY][gridX] != 0)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    // No collision
    return false;
}


void BlockFall::move_active_block_right() {
    // TODO: Implement MOVE_RIGHT command processing

    if (can_move_right()) {


        int blockX = active_rotation->xCoordinat;
        int blockY = active_rotation->yCoordinat;

        for (int i = 0; i < active_rotation->shape.size(); ++i) {
            for (int j = 0; j < active_rotation->shape[i].size(); ++j) {

                int gridX = blockX + j;
                int gridY = blockY + i;
                if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                    grid[gridY][gridX] = 0;  // Temizle
                }
            }
        }
        active_rotation->yCoordinat =0;
        active_rotation->xCoordinat ++;
    }

}
bool BlockFall::findMatrix(const std::vector<std::vector<int>>& source, const std::vector<std::vector<bool>>& target) {
    for (size_t i = 0; i <= source.size() - target.size(); ++i) {
        for (size_t j = 0; j <= source[i].size() - target[0].size(); ++j) {
            bool found = true;
            for (size_t k = 0; k < target.size(); ++k) {
                for (size_t l = 0; l < target[k].size(); ++l) {
                    if (source[i + k][j + l] != target[k][l]) {
                        found = false;
                        break;
                    }
                }
                if (!found) {
                    break;
                }
            }
            if (found) {
                return true;
            }
        }
    }
    return false;
}

void BlockFall::move_active_block_left() {


    // TODO: Implement MOVE_LEFT command processing

    if (can_move_left()) {


        int blockX = active_rotation->xCoordinat;
        int blockY = active_rotation->yCoordinat;
        for (int i = 0; i < active_rotation->shape.size(); ++i) {
            for (int j = 0; j < active_rotation->shape[i].size(); ++j) {

                int gridX = blockX + j;
                int gridY = blockY + i;

                if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                    grid[gridY][gridX] = 0;  // Temizle
                }
            }
        }

        active_rotation->yCoordinat =0;
        active_rotation->xCoordinat --;
    }
}

bool BlockFall::can_move_right() {
    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;

    if (blockX < (cols - active_rotation->shape[0].size())) {
        if (grid[blockY][blockX + active_rotation->shape[0].size()] == 0) {
            return true;
        }
    }



    return false;
}


bool BlockFall::can_move_left() {
    // Aktif bloğun konumunu al
    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;

    if (blockX > 0) {
        if (grid[blockY][blockX - 1] == 0) {
            return true;
        }
    }

    return false;
}

// Helper function to count the number of occupied cells in the given block
int BlockFall::count_occupied_cells(const vector<vector<bool>> block_shape) const {
    int occupied_cells = 0;

    for (const auto& row : block_shape) {
        for (int cell : row) {
            if (cell == 1) {
                occupied_cells++;
            }
        }
    }

    return occupied_cells;
}


int BlockFall::count_occupied_cells(const vector<vector<int>> block_shape) const {
    int occupied_cells = 0;

    for (const auto& row : block_shape) {
        for (int cell : row) {
            if (cell == 1) {
                occupied_cells++;
            }
        }
    }

    return occupied_cells;
}




void BlockFall::drop_active_block() {
    int initial_row = active_rotation->xCoordinat;
    int initial_col = active_rotation->yCoordinat;

    if (!gravity_mode_on) {
        if (!check_collision()) {
            move_active_block_down();
            clear_rows_and_update_score();


        }
    }

    if (gravity_mode_on) {

        if (!check_collision()) {
            move_active_block_down();


        }


        apply_gravity_to_bottom();

        clear_rows_and_update_score();

    }


    if (findMatrix(grid,power_up)){
        current_score += 1000;
        current_score += count_occupied_cells(grid);
        std::cout<<"Before clearing:"<<std::endl;
        printGridShape(grid);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; j++) {
                if (grid[i][j] == 1) {
                    grid[i][j] = 0;
                }
            }
        }

    }

    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;
}

void BlockFall::toggle_gravity() {
    gravity_mode_on = !gravity_mode_on;
    if (gravity_mode_on) {
        for (int i = 0; i < active_rotation->shape.size(); ++i) {
            for (int j = 0; j < active_rotation->shape[i].size(); ++j) {
                int gridX = active_rotation->xCoordinat + j;
                int gridY = 0 + i;

                if (active_rotation->shape[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                    grid[gridY][gridX] = 0;  // Temizle
                }
            }
        }

        apply_gravity_to_bottom();

        clear_rows_and_update_score2();

    }
}
void BlockFall::apply_gravity_to_bottom() {


    int number = 0;
    for (int j = 0; j < cols; ++j) {
        for (int i = rows - 2; i >= 0; --i) {
            if (grid[i][j] == 1) {
                int row = i;
                while (row < rows - 1 && (grid[row + 1][j] == 0 )) {

                    grid[row + 1][j] = 1;
                    grid[row][j] = 0;

                    ++row;

                }

            }
        }

    }
}


bool BlockFall::check_collision() {
    vector<vector<bool>> currentBlockState = active_rotation->shape;

    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;

    for (int i = 0; i < currentBlockState.size(); ++i) {
        for (int j = 0; j < currentBlockState[i].size(); ++j) {
            int gridX = blockX + j;
            int gridY = blockY + i;

            if (currentBlockState[i][j] == 1) {

                if (gridX < 0 || gridX >= cols || gridY >= rows || (gridY >= 0 && gridY < rows && grid[gridY][gridX] != 0)) {
                    // Çakışma var
                    return true;
                }
            }
        }
    }

    return false;
}


void BlockFall::update_grid() {
    vector<vector<bool>> currentBlockState = active_rotation->shape;
    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;
    for (int i = 0; i < currentBlockState.size(); ++i) {
        for (int j = 0; j < currentBlockState[i].size(); ++j) {
            int gridX = blockX + j;
            int gridY = blockY + i;

            if (currentBlockState[i][j] == 1 && gridY >= 0 && gridY < rows && gridX >= 0 && gridX < cols) {
                grid[gridY][gridX] = currentBlockState[i][j];
            }

        }
    }
}
void BlockFall::move_active_block_down() {

    int blockX = active_rotation->xCoordinat;
    int blockY = active_rotation->yCoordinat;

    int initial_row = active_rotation->yCoordinat;

    active_rotation->yCoordinat++;

    while (!check_collision()) {

        active_rotation->yCoordinat++;
    }
    active_rotation->yCoordinat--;





    int final_row = active_rotation->yCoordinat;

    int distance_fallen = final_row - initial_row;
    int occupied_cells = count_occupied_cells(active_rotation->shape);

    int score_increase = distance_fallen * occupied_cells;

    current_score += score_increase;

    update_grid();
}
void BlockFall::clear_rows_and_update_score2() {
    vector<int> completed_rows;
    for (int i = 0; i < rows; ++i) {
        bool row_completed = true;
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 0) {
                row_completed = false;
                break;
            }
        }
        if (row_completed) {
            completed_rows.push_back(i);


        }
    }
    int total_cleared_cells = 0;
    for (int i : completed_rows) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = 0;
            total_cleared_cells++;
        }

        for (int k = i; k > 0; --k) {
            for (int j = 0; j < cols; ++j) {
                grid[k][j] = grid[k - 1][j];
            }
        }
    }
    current_score += total_cleared_cells ;

}

void BlockFall::clear_rows_and_update_score() {
    vector<int> completed_rows;
    int a = 0;

    // Tamamlanmış satırları bul
    for (int i = 0; i < rows; ++i) {
        bool row_completed = true;
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 0) {
                row_completed = false;
                break;
            }
        }
        if (row_completed) {
            completed_rows.push_back(i);
            a = 1;
        }
    }
    if (a){
        std::cout<<"Before clearing:"<<std::endl;
        printGridShape(grid);
    }

    int total_cleared_cells = 0;
    for (int i : completed_rows) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j] = 0;
            total_cleared_cells++;
        }
        for (int k = i; k > 0; --k) {
            for (int j = 0; j < cols; ++j) {
                grid[k][j] = grid[k - 1][j];
            }
        }
    }
    current_score += total_cleared_cells ;
}
