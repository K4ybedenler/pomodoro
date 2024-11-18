#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <atomic>
#include "sqlite3.h"
#include "termios.h"

extern std::atomic<bool> quit;

class pomo_timer {
    std::chrono::steady_clock::time_point start_time, round_finish_time;
    std::chrono::system_clock::time_point start_time_db, round_finish_time_db, finish_time;
    bool started = false;
    void print_duration(long int duration);
    void print_duration(std::chrono::duration<double> duration);
    void update_timer();
    void db_prepare(const char *table_name);
    void query_prepare(const char *query_tmpl, sqlite3_stmt *&stmt);
    sqlite3 *db;
    sqlite3_int64 last_id;
    sqlite3_stmt *stmt_launches, *stmt_rounds, *stmt_last_id;
    int round_number;
    void bind_statement(sqlite3_stmt *stmt,
            int column_number,
            std::chrono::system_clock::time_point time_point);
    void bind_statement(sqlite3_stmt *stmt,
            int column_number,
            sqlite3_int64 value);
    void push_statement(sqlite3_stmt *stmt);
    const char *insert_launch = "INSERT INTO launches ("
        "launch_id, "
        "start_time, "
        "finish_time) "
        "VALUES (?, ?, ?)";
    const char *insert_round = "INSERT INTO rounds ("
        "round_id, "
        "launch_id, "
        "round_number, "
        "round_time) "
        "VALUES (?, ?, ?, ?)";
    const char *last_launch_id = "SELECT MAX(launch_id) FROM launches;";
    
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

