#include <QCoreApplication>
#include <QThread>
#include "uhv2simulator.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QTimer timer;
//    timer.setInterval(5000);
//    timer.setSingleShot(true);

//    QObject::connect(&timer, &QTimer::timeout, [](){

//    });

    QThread * aThread = new QThread();
    UHV2Simulator * aTest = new UHV2Simulator();
    aTest->moveToThread(aThread);
    QObject::connect(aThread, &QThread::started, aTest, &UHV2Simulator::start);
    aThread->start();

    return a.exec();
}
