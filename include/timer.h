#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <atomic>
#include "sqlite3.h"

extern std::atomic<bool> quit;

class pomo_timer {
    std::chrono::steady_clock::time_point start_time, round_finish_time;
    bool started = false;
    void print_duration(std::chrono::duration<double> duration);
    void update_timer();
    sqlite3 *db;

    public:
    pomo_timer();
    ~pomo_timer();
    bool check_status();
    void start_pomo();
    void finish_round();
    void pause_pomo();
    void stop_pomo();
    static void output_hello();
    static void output_hello(char input);
};

void set_input_mode();
void reset_input_mode(struct termios &orig);

#endif

