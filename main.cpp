#include <iostream>
#include <QCoreApplication>
#include <QDebug>
using namespace std;

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    cout << "Hello, Qt Core!" << endl;

    return app.exec();  // Start the event loop
}

