#include "serialcomm.h"

SerialComm::SerialComm()
{
    serial = new QSerialPort();
}
SerialComm::~SerialComm()
{
    serial->close();
}

bool SerialComm::connect(QString Port, QString Baud){
    bool stateConnect = false;
    name = Port;
    baud = Baud.toInt();
    serial->setPortName(name);
    serial->setBaudRate(baud);
    serial->setDataBits(bit);
    serial->setParity(parity);
    serial->setStopBits(stop);
    serial->setFlowControl(flow);
    stateConnect = serial->open(QIODevice::ReadWrite);
    if (stateConnect) {
        qDebug() << "Connected to " + name + " : " + QString::number(baud) + ", " +
                    QString::number(bit) + ", " + QString::number(parity) + ", " +
                    QString::number(stop) + ", " + QString::number(flow);
    }
    else {
        qDebug() << "Error " + serial->errorString();
    }
    return stateConnect;
}

void SerialComm::disconnect()
{
    serial->clear();
    serial->close();
    qDebug() << "Disconnected to " + name;
}

void SerialComm::writeread()
{
    QString received;
    received = serial->readLine();
}

