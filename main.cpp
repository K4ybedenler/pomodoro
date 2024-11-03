#include <iostream>
#include <QCoreApplication>
#include <QDebug>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <termios.h>
#include <unistd.h>
using namespace std::chrono;

void timer_calling(steady_clock::time_point t1);
void set_input_mode();
void reset_input_mode(struct termios &orig);

int main(int argc, char *argv[]) {

  QCoreApplication app(argc, argv); 
  struct termios orig;
  tcgetattr(STDIN_FILENO, &orig);
  set_input_mode();
  
  std::cout << "here are available buttons:" << std::endl;
  std::cout << "[s] start timer\n[p] pause timer\n[r] finish the round" << std::endl;
  steady_clock::time_point t1, t2, t3;
  bool timer_started = false;

  while (true) {
      char input = std::cin.get();
      switch (input) {
          case 's':
              if (!timer_started) { 
                  std::cout << "started\n";
                  timer_started = true;
                  t1 = steady_clock::now();
                  t3 = t1;
              }
              else std::cout << "already started\n";
              break;
          case 'r':
              if (timer_started) {
                  std::cout << "round finished\n";
                  timer_calling(t3);
                  t3 = steady_clock::now();
              }
              else std::cout << "start before make a round\n";
              break;
          case 'p':
              if (timer_started) {
                  std::cout << "paused\n";
                  timer_calling(t1);
                  timer_started = false;
              }
              else std::cout << "start before stop\n";
              break;
          default:
              std::cout << "hah, am fucked\n"; 
      }
  }

  reset_input_mode(orig);
  return app.exec();
}

void timer_calling(steady_clock::time_point t1){
  steady_clock::time_point t2 = steady_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
  std::cout << "It took me " << "seconds.";
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
