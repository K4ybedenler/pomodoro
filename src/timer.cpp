#include "timer.h"
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <termios.h>

std::atomic<bool> timer_started(false);
std::atomic<bool> quit(false);
std::chrono::steady_clock::time_point t1;

void timer_calling(std::chrono::steady_clock::time_point t1) {
    auto t2 = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "\rIt took me " << "seconds.";
    std::cout << std::setprecision(5) << std::fixed << time_span.count();
    std::cout << std::endl;
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

void update_timer() {
    while (timer_started) {
        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
        std::cout << "\rtime passed: " << elapsed << " seconds" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void output_hello() {
    std::cout << "here are available buttons:" << std::endl;
    std::cout << "[s] start timer\n[p] pause timer\n[r] finish the round" << std::endl;
}

