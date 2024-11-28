#include <QCoreApplication>
#include <QDebug>
#include <timer.h>

int main(int argc, char *argv[]) {
    pomo_timer::output_hello();
    QCoreApplication app(argc, argv);

    pomodoro();

    return app.exec();
}

