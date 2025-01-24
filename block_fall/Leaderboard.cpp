#include "Leaderboard.h"
#include <fstream>
#include <iostream>
#include <iomanip>

void Leaderboard::insert_new_entry(LeaderboardEntry* new_entry) {
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry* current = head_leaderboard_entry;
        while (current->next_leaderboard_entry && new_entry->score <= current->next_leaderboard_entry->score) {
            current = current->next_leaderboard_entry;
        }
        new_entry->next_leaderboard_entry = current->next_leaderboard_entry;
        current->next_leaderboard_entry = new_entry;
    }

    LeaderboardEntry* prev = nullptr;
    LeaderboardEntry* current = head_leaderboard_entry;
    int count = 0;
    while (current && count < MAX_LEADERBOARD_SIZE) {
        count++;
        prev = current;
        current = current->next_leaderboard_entry;
    }

    if (prev) {
        prev->next_leaderboard_entry = nullptr;
    }
}


void Leaderboard::write_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        LeaderboardEntry* current = head_leaderboard_entry;
        while (current) {
            file << current->score << " " << current->last_played << " " << current->player_name << "\n";
            current = current->next_leaderboard_entry;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void Leaderboard::read_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        head_leaderboard_entry = nullptr;

        unsigned long score;
        time_t lastPlayed;
        std::string playerName;

        while (file >> score >> lastPlayed >> playerName) {
            LeaderboardEntry* new_entry = new LeaderboardEntry(score, lastPlayed, playerName);
            insert_new_entry(new_entry);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void Leaderboard::print_leaderboard() {
    std::cout << "Leaderboard\n-----------\n";
    int order = 1;
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current && order <= MAX_LEADERBOARD_SIZE) {
        std::cout << order << ". " << current->player_name << " " << current->score << " "
                  << std::put_time(std::localtime(&current->last_played), "%H:%M:%S/%d.%m.%Y") << std::endl;
        current = current->next_leaderboard_entry;
        order++;
    }
}

Leaderboard::~Leaderboard() {
    LeaderboardEntry* current = head_leaderboard_entry;
    while (current) {
        LeaderboardEntry* temp = current;
        current = current->next_leaderboard_entry;
        delete temp;
    }
}
