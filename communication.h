#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QUdpSocket>
#include <QDebug>
#include <defines.h>
#include <vector>
#include <my_protocol.hpp>

//#include "cdelegate.h"



class OdbiorcaKomunikacji{
public:
    virtual void OdbiorcaKomunikacji_dataArrive(char * data, int size) = 0;
};

class NadawcaDanych{
public:
    virtual  void NadawcaDanych_dataSend(std::vector<unsigned char> &data_to_send) = 0;
};


class Communication : public QWidget, public NadawcaDanych
{

    Q_OBJECT
public:
    explicit Communication(QWidget *parent = 0);
    void NadawcaDanych_dataSend(std::vector<unsigned char> &data_to_send);


    void send_data(std::vector<unsigned char> &data);

    void setEventListener(OdbiorcaKomunikacji * odb)
    {
        listener = odb;
    }

private:
     QSerialPort *serial;
     QSerialPort::BaudRate baudRate;
     QSerialPort::DataBits dataBits;
     QSerialPort::Parity parity;
     QSerialPort::StopBits stopBits;
     QSerialPort::FlowControl flowControl;
     QByteArray _dane_in;

     QUdpSocket *udpSocket = nullptr;

    OdbiorcaKomunikacji * listener = nullptr;

    udp_param_t udp_param;

    MyProtocol *protocol;

    void decode_data(std::vector<char> frame);

    void onReceivedDecodedFrame(std::vector<char> frame);


signals:
     void serialStatus(SERIAL_OPEN_STATUS, QString);
     void udpStatus(QString str);


public slots:

     void openSerial(QString serialname);
     void closeSerial();
     void on_incomig_data_to_send(gamepad_t data, bool x);

     void startUDP_signal(udp_param_t param);
     void stopUDP_signal(udp_param_t param);
     void readPendingDatagrams();
     void writeDatagram(const char *data, int size);

     void on_received_data();
     void parse_data(std::vector<char> &data);

};

#endif // COMMUNICATION_H
