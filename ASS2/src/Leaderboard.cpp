#include "Leaderboard.h"
#include "Leaderboard.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
    if (head_leaderboard_entry == nullptr || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry* current_entry = head_leaderboard_entry;
        while (current_entry->next_leaderboard_entry != nullptr &&
               new_entry->score <= current_entry->next_leaderboard_entry->score) {
            current_entry = current_entry->next_leaderboard_entry;
        }
        new_entry->next_leaderboard_entry = current_entry->next_leaderboard_entry;
        current_entry->next_leaderboard_entry = new_entry;
    }

    // Keep only the top 10 entries and set the next pointer of the 10th entry to null.
    int number_of_entries = 0;
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current != nullptr && number_of_entries < 9) {
        current = current->next_leaderboard_entry;
        number_of_entries++;
    }
    if (current != nullptr) {
        current->next_leaderboard_entry = nullptr;
    }


}

void Leaderboard::write_to_file(const string& filename) {
    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions

    ofstream file(filename);
    LeaderboardEntry *current = head_leaderboard_entry;

    while (current) {
        file << current->score << " " << current->last_played << " " << current->player_name << "\n";
        current = current->next_leaderboard_entry;
    }

    file.close();
}

void Leaderboard::read_from_file(const string& filename) {
    // TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    //       variable will point to the highest all-times score, and all other scores will be reachable from it
    //       via the "next_leaderboard_entry" member variable pointer.

    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        unsigned long score;
        time_t last_played;
        string player_name;

        istringstream iss(line);
        iss >> score >> last_played >> player_name;

        LeaderboardEntry *new_entry = new LeaderboardEntry(score, last_played, player_name);
        insert_new_entry(new_entry);
    }

    file.close();
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    cout << "Leaderboard\n";
    cout << "-----------\n";

    LeaderboardEntry *current = head_leaderboard_entry;
    int order = 1;

    while (current) {
        cout << order << ". " << current->player_name << " " << current->score << " ";
        cout << put_time(localtime(&current->last_played), "%H:%M:%S/%d.%m.%Y") << "\n";

        current = current->next_leaderboard_entry;
        order++;
    }
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    LeaderboardEntry *current = head_leaderboard_entry;
    while (current) {
        LeaderboardEntry *next = current->next_leaderboard_entry;
        delete current;
        current = next;
    }
}
int Leaderboard::get_high_score() const {

    if (head_leaderboard_entry != nullptr) {
        return head_leaderboard_entry->score;
    } else {
        return 0;
    }
}
