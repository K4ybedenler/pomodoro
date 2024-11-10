#include "timer.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <termios.h>
#include "sqlite3.h"

std::atomic<bool> quit(false);

pomo_timer::pomo_timer() {
    char *errMsg = 0;

    if (sqlite3_open("pomobase.db", &db)) {
            std::cerr << "can't open the db: " << sqlite3_errmsg(db) << std::endl;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS pomodoro ("
        "launch_id INTEGER PRIMARY KEY, "
        "launch_time TEXT, "
        "round_finish TEXT, "
        "launch_time_finish TEXT);";

    if(sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
};

pomo_timer::~pomo_timer() {
    sqlite3_close(db);
}

void pomo_timer::print_duration(std::chrono::duration<double> duration)
{
    std::cout << "\rIt took me ";
    std::cout << std::setprecision(5) << std::fixed << duration.count();
    std::cout << " seconds." << std::endl;
}

void pomo_timer::update_timer() {
    while (started) {
        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(t2 - start_time).count();
        std::cout << "\rtime passed: " << elapsed << " seconds" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool pomo_timer::check_status() {
    return started;
}

void pomo_timer::start_pomo() {
    started = true;
    start_time = std::chrono::steady_clock::now();
    round_finish_time = start_time;
    std::thread timer_thread(&pomo_timer::update_timer, this);
    timer_thread.detach();
}

void pomo_timer::finish_round() {
    auto current_time = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
            current_time - round_finish_time
            );
    round_finish_time = std::chrono::steady_clock::now();
    print_duration(time_span);
}

void pomo_timer::pause_pomo() {
}

void pomo_timer::stop_pomo() {
    auto current_time = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
            current_time - start_time
            );
    round_finish_time = std::chrono::steady_clock::now();
    print_duration(time_span);
    started = false;
}

void pomo_timer::output_hello(char input) {
    std::cout << "\033[2J\033[H";
    std::cout << "here are available buttons:" << std::endl;
    std::cout << "[s] start timer\n[p] pause timer\n[r] finish the round\n" << std::endl;

    switch(input) {
        case 's':
            std::cout << "started\n";
            break;
        case 'r':
            std::cout << "round\n";
            break;
        case 'p':
            std::cout << "pause\n";
    }
}

void pomo_timer::output_hello() {
    std::cout << "\033[2J\033[H";
    std::cout << "here are available buttons:" << std::endl;
    std::cout << "[s] start timer\n[p] pause timer\n[r] finish the round\n" << std::endl;
}

void set_input_mode() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_input_mode(struct termios &orig) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);
}


