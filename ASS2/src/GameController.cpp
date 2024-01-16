#include "GameController.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <ctime>

bool GameController::play(BlockFall &game, const string &commands_file) {
    ifstream file(commands_file);
    string command;
    while (getline(file, command)) {
        stringstream ss(command);
        string command_word;
        ss >> command_word;

        // Process the command dynamically
        if (command_word == "PRINT_GRID") {
            game.print_grid();
        } else if (command_word == "ROTATE_RIGHT") {
            game.rotate_active_block(true);
        } else if (command_word == "ROTATE_LEFT") {
            game.rotate_active_block(false);
        } else if (command_word == "MOVE_RIGHT") {
            game.move_active_block_right();
        } else if (command_word == "MOVE_LEFT") {
            game.move_active_block_left();
        } else if (command_word == "DROP") {
            game.drop_active_block();
            game.active_rotation =game.active_rotation->next_block;
            if (game.active_rotation == nullptr) {
                std::cout << "YOU WIN!\nNo more block." << std::endl;
                std::cout<<"Final grid and score:\n"<<endl;
                std::cout<<"Score: "<<game.current_score<<endl;
                std::cout<<"High Score: "<<game.leaderboard.get_high_score()<<endl;

                game.printGridShape(game.grid);

                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                LeaderboardEntry *entry = new LeaderboardEntry(game.current_score,currentTime,game.player_name);
                game.leaderboard.insert_new_entry(entry);

                game.leaderboard.print_leaderboard();
                std::cout<<endl;
                std::cout<<endl;
                game.leaderboard.write_to_file(game.leaderboard_file_name);
                return true;
            }
            if (game.check_collision()){
                std::cout<<"GAME OVER!"<<endl<<"Next block that couldn't fit:"<<endl;
                game.printBlockShape(game.active_rotation->shape);
                std::cout<<"\nFinal grid and score:\n"<<endl;
                std::cout<<"Score: "<<game.current_score<<endl;
                std::cout<<"High Score: "<<game.leaderboard.get_high_score()<<endl;

                game.printGridShape(game.grid);

                auto now = std::chrono::system_clock::now();
                std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
                LeaderboardEntry *entry = new LeaderboardEntry(game.current_score,currentTime,game.player_name);
                game.leaderboard.insert_new_entry(entry);

                game.leaderboard.print_leaderboard();
                std::cout<<endl;
                std::cout<<endl;
                game.leaderboard.write_to_file(game.leaderboard_file_name);
                return false;
            }

        } else if (command_word == "GRAVITY_SWITCH") {
            game.toggle_gravity();
        } else {
            // Unknown command
            cout << "Unknown command: " << command_word << endl;
        }
    }

    std::cout << "GAME FINISHED!\nNo more commands." << std::endl;
    std::cout<<"Final grid and score:\n"<<endl;
    std::cout<<"Score: "<<game.current_score<<endl;
    std::cout<<"High Score: "<<game.leaderboard.get_high_score()<<endl;

    game.printGridShape(game.grid);

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    LeaderboardEntry *entry = new LeaderboardEntry(game.current_score,currentTime,game.player_name);
    game.leaderboard.insert_new_entry(entry);

    game.leaderboard.print_leaderboard();
    std::cout<<endl;
    std::cout<<endl;
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    file.close();

    return true;


}



