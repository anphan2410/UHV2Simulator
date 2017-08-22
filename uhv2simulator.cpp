#include "uhv2simulator.h"

UHV2Simulator::UHV2Simulator(QObject *parent) : QObject(parent)
{
    timer.setParent(this);
    timer.setInterval(200);
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, this, [&](){isTimedOut=true;});
}

void UHV2Simulator::initSerialPort()
{
    if (SerialPort)
    {
        SerialPort->close();
        delete SerialPort;
        SerialPort = Q_NULLPTR;
    }
    SerialPort = new QSerialPort();
    SerialPort->setParent(this);
    SerialPort->setReadBufferSize(64);
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setBaudRate(QSerialPort::Baud9600);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    SerialPort->setPortName("COM3");
    if (SerialPort->open(QIODevice::ReadWrite))
    {
        anIf(UHV2SimulatorDbgEn, anAck("Successfully Open A Serial Port !"));
    }
    else
    {
        anIf(UHV2SimulatorDbgEn, anWarn("Failed To Open A Serial Port !"));
    }
    QObject::connect(SerialPort, &QSerialPort::readyRead, this, [&](){
        isValid = isTimedOut;
    });
}

void UHV2Simulator::work()
{
    if (isRunning)
    {
        initSerialPort();
        while (isRunning)
        {
            qApp->processEvents();
            if (SerialPort->waitForReadyRead(500))
            {
                currentRead=SerialPort->readAll();
                while (SerialPort->waitForReadyRead(700))
                {
                    currentRead+=SerialPort->readAll();
                }
                BinaryProtocol & currentBP = BinaryProtocol::FromQByteArray(currentRead);
                anIf(UHV2SimulatorDbgEn,
                     anAck("New Command Received !");
                     anInfo(currentBP.GetMessageTranslation()););
                pendingSend.clear();
                if (currentBP.GetCommand()=="HVSwitch")
                {
                    pendingSend.append(0x06);
                }
                else if ((currentBP.GetCommand()=="ReadP")||(currentBP.GetCommand()=="ReadV")||(currentBP.GetCommand()=="ReadI"))
                {
                    pendingSend=currentBP.HdrRsp().Data(QString(QString::number(quint8(qrand()%999))+"E-"+QString::number(quint8(qrand()%9))).toUpper().toLocal8Bit()).GenMsg();
                }
                isTimedOut=false;
                timer.start();
                while(!isTimedOut)
                {
                    qApp->processEvents();
                }
                if (isValid)
                {
                    SerialPort->write(pendingSend);
                    if (SerialPort->waitForBytesWritten(500))
                    {
                        anIf(UHV2SimulatorDbgEn, anAck("Successfully Reply !"));
                    }
                    else
                    {
                        anIf(UHV2SimulatorDbgEn, anWarn("Failed To Reply !"));
                    }
                    anIf(UHV2SimulatorDbgEn, anInfo(pendingSend.toHex()));
                }
                else
                {
                    anIf(UHV2SimulatorDbgEn, anWarn("Invalid Command Transmission !"));
                    SerialPort->write(QByteArray().append(0x15));
                }
                isValid = true;
            }
        }
    }
}

void UHV2Simulator::start()
{
    isRunning = true;
    work();
}

void UHV2Simulator::stop()
{
    isRunning = false;
}
