#include <iostream>
#include <QCoreApplication>
#include <QDebug>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include "timer.h"


int main(int argc, char *argv[]) {
    pomo_timer::output_hello();
    QCoreApplication app(argc, argv);
    struct termios orig;
    tcgetattr(STDIN_FILENO, &orig);
    set_input_mode();

    pomo_timer *timer = nullptr;
    
    while (!quit) {
        char input = std::cin.get();
        switch (input) {
            case 's':
                if (timer == nullptr) {
                    timer = new pomo_timer;
                    pomo_timer::output_hello(input);
                    timer->start_pomo();
                } else {
                    std::cout << "already started\n";
                }
                break;
            case 'r':
                if (timer && timer->check_status()) {
                    pomo_timer::output_hello(input);
                    timer->finish_round();
                } else {
                    std::cout << "start before make a round\n";
                }
                break;
            case 'p':
                if (timer && timer->check_status()) {
                    pomo_timer::output_hello(input);
                    timer->stop_pomo();
                    delete timer;
                    timer = nullptr;
                } else {
                    std::cout << "start before stop\n";
                }
                break;
            case 'q':
                std::cout << "\n\n";
                reset_input_mode(orig);
                delete timer;
                return 0;
            default:
                std::cout << "hah, am fucked\n"; 
        }
    }

    reset_input_mode(orig);
    return app.exec();
}

