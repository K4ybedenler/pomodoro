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
    if (sqlite3_open("pomobase.db", &db)) {
            std::cerr << "can't open the db: " << sqlite3_errmsg(db) << std::endl;
    }

    const char *sql_launches = "CREATE TABLE IF NOT EXISTS launches ("
        "launch_id INTEGER PRIMARY KEY, "
        "start_time INTEGER, "
        "finish_time INTEGER);";
    const char *sql_rounds = "CREATE TABLE IF NOT EXISTS rounds ("
        "round_id INTEGER PRIMARY KEY, "
        "launch_id INTEGER, "
        "round_number INTEGER, "
        "round_time INTEGER);";

    db_prepare(sql_launches);
    db_prepare(sql_rounds);
    query_prepare(insert_launch, stmt_launches);
    query_prepare(insert_round, stmt_rounds);
    query_prepare(last_launch_id, stmt_last_id);

    round_number = 0;
};

pomo_timer::~pomo_timer() {
    sqlite3_finalize(stmt_launches);
    sqlite3_finalize(stmt_rounds);
    sqlite3_finalize(stmt_last_id);
    sqlite3_close(db);
}

void pomo_timer::db_prepare(const char *table_name)
{
    char *errMsg = 0;
    if (sqlite3_exec(db, table_name, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
    }
}

void pomo_timer::query_prepare(const char *query_tmpl, sqlite3_stmt *&stmt)
{
    if (sqlite3_prepare_v2(db, query_tmpl, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "failed to prepare SQL statement(command): " << sqlite3_errmsg(db) << std::endl;
    }
}

void pomo_timer::print_duration(std::chrono::duration<double> duration)
{
    std::cout << "\rit took me ";
    std::cout << std::setprecision(5) << std::fixed << duration.count();
    std::cout << " seconds." << std::endl;
}

void pomo_timer::print_duration(long int duration)
{
    std::cout << "\rit took me ";
    std::cout << std::setprecision(5) << std::fixed << duration;
    std::cout << " seconds." << std::endl;
}

void pomo_timer::bind_statement(sqlite3_stmt *stmt,
        int column_number,
        std::chrono::system_clock::time_point time_point)
{
    sqlite3_int64 sqlite_time_point = std::chrono::duration_cast<std::chrono::milliseconds>(
            time_point.time_since_epoch()
            ).count();
    sqlite3_bind_int64(stmt, column_number, sqlite_time_point);
}

void pomo_timer::bind_statement(sqlite3_stmt *stmt,
        int column_number,
        sqlite3_int64 value)
{
    sqlite3_bind_int64(stmt, column_number, value);
}

void pomo_timer::push_statement(sqlite3_stmt *stmt) {
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
}

void pomo_timer::update_timer() {
    while (started) {
        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - start_time).count();
        std::cout << "\rtime passed: " << elapsed << " seconds" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool pomo_timer::check_status() {
    return started;
}

void pomo_timer::start_pomo() {
    started = true;
    round_finish_time = start_time = std::chrono::steady_clock::now();
    round_finish_time_db = start_time_db = std::chrono::system_clock::now();
    std::thread timer_thread(&pomo_timer::update_timer, this);
    timer_thread.detach(); 
    bind_statement(stmt_launches, 2, start_time_db);
    last_id = sqlite3_last_insert_rowid(db);
    std::cout << last_id << "here it is\n";
}

void pomo_timer::finish_round() {
    auto current_time = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - round_finish_time
            ).count();
    round_finish_time = std::chrono::steady_clock::now();
    print_duration(time_span);
    if(sqlite3_step(stmt_last_id) == SQLITE_ROW) {
        last_id = sqlite3_column_int64(stmt_last_id, 0);
    }
    bind_statement(stmt_rounds, 2, last_id);
    bind_statement(stmt_rounds, 3, round_number);
    bind_statement(stmt_rounds, 4, time_span);
    push_statement(stmt_rounds);
    round_number++;
}

void pomo_timer::pause_pomo() {
}

void pomo_timer::stop_pomo() {
    auto current_time = std::chrono::steady_clock::now();
    finish_time = std::chrono::system_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - start_time
            ).count();
    round_finish_time = std::chrono::steady_clock::now();
    print_duration(time_span);
    bind_statement(stmt_launches, 3, finish_time);
    push_statement(stmt_launches);
    started = false;
    sqlite3_close(db);
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

