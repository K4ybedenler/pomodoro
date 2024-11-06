#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <atomic>

extern std::atomic<bool> timer_started;
extern std::atomic<bool> quit;
extern std::chrono::steady_clock::time_point t1;

void timer_calling(std::chrono::steady_clock::time_point t1);
void set_input_mode();
void reset_input_mode(struct termios &orig);
void update_timer();
void output_hello();

#endif

