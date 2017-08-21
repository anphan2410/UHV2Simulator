#ifndef UHV2SIMULATOR_H
#define UHV2SIMULATOR_H

#define UHV2SimulatorDbgEn 1

#include <QCoreApplication>
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>
#include "binaryprotocol.h"
#include "anlogger.h"

class UHV2Simulator : public QObject
{
    Q_OBJECT
public:
    explicit UHV2Simulator(QObject *parent = nullptr);
    void initSerialPort();

    void work();
    QSerialPort * SerialPort = Q_NULLPTR;
    bool isRunning = false;
    bool isTimedOut = true;
    bool isValid = true;
    QByteArray currentRead;
    QByteArray pendingSend;
    QTimer timer;
signals:

public slots:
    void start();
    void stop();
};

#endif // UHV2SIMULATOR_H
