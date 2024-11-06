#include <iostream>
#include <QCoreApplication>
#include <QDebug>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include "timer.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    struct termios orig;
    tcgetattr(STDIN_FILENO, &orig);
    set_input_mode();
    output_hello();
    
    std::chrono::steady_clock::time_point t3;

    while (!quit) {
        char input = std::cin.get();
        switch (input) {
            case 's':
                if (!timer_started) {
                    std::cout << "\rstarted\n";
                    timer_started = true;
                    t1 = std::chrono::steady_clock::now();
                    t3 = t1;
                    std::thread timer_thread(update_timer);
                    timer_thread.detach();
                } else {
                    std::cout << "already started\n";
                }
                break;
            case 'r':
                if (timer_started) {
                    std::cout << "\033[2J\033[H";
                    output_hello();
                    std::cout << "\r\nround finished\n";
                    timer_calling(t3);
                    t3 = std::chrono::steady_clock::now();
                } else {
                    std::cout << "start before make a round\n";
                }
                break;
            case 'p':
                if (timer_started) {
                    std::cout << "paused\n";
                    timer_calling(t1);
                    timer_started = false;
                } else {
                    std::cout << "start before stop\n";
                }
                break;
            case 'q':
                std::cout << "\n\n";
                reset_input_mode(orig);
                return 0;
            default:
                std::cout << "hah, am fucked\n"; 
        }
    }

    reset_input_mode(orig);
    return app.exec();
}

