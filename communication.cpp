#include "communication.h"


Communication::Communication(QWidget *parent) : QWidget(parent),
    baudRate(QSerialPort::Baud19200),
    dataBits(QSerialPort::Data8),
    parity(QSerialPort::NoParity),
    stopBits(QSerialPort::OneStop),
    flowControl(QSerialPort::NoFlowControl),
    serial(nullptr),
    udpSocket(nullptr)

{
    udp_param.ip = nullptr;

    protocol = new MyProtocol();

    protocol->connect<Communication, &Communication::decode_data>(this);



}


void Communication::startUDP_signal(udp_param_t param){

    udp_param = param;
    qDebug() <<"open: "<< param.ip.toString() << " , " << param.out_port << param.in_port;


    if(udpSocket == nullptr ){
        qDebug() << "start";
        udpSocket = new QUdpSocket(this);

        if(udpSocket->bind(param.in_port, QAbstractSocket::ShareAddress)){
            connect(udpSocket, SIGNAL(readyRead()),  this, SLOT(readPendingDatagrams()));
            if(!param.ip.isNull()){
                emit udpStatus("Started");
            }
        }else {
            emit udpStatus("Stoped");
        }
    }
}
void Communication::stopUDP_signal(udp_param_t param){
    if(udpSocket != nullptr){
        disconnect(udpSocket, SIGNAL(readyRead()),  this, SLOT(readPendingDatagrams()));
        delete  udpSocket;
        udpSocket = nullptr;
    }
    qDebug() <<"close: "<< param.ip.toString() << " , " << param.out_port << param.in_port;
    emit udpStatus("Stoped");


}
void Communication::writeDatagram(const char *data, int size)
{
    udpSocket->writeDatagram(data, size, udp_param.ip, udp_param.out_port);
}

void Communication::readPendingDatagrams()
{
    //    qDebug() <<"readPendingDatagrams()";
    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

    if(sender.toIPv4Address() == udp_param.ip.toIPv4Address()){
        qDebug() <<"Datagram";
        for(int i = 0; i < datagram.size(); i++){

            if(senderPort == udp_param.out_port){
                qDebug() << datagram.at(i);
            }
        }
    }
}

void Communication::openSerial(QString serialname){
    if(serial == nullptr){
        serial = new QSerialPort(this);
        serial->setPortName(serialname);
        serial->setBaudRate(baudRate);
        serial->setDataBits(dataBits);
        serial->setParity(parity);
        serial->setStopBits(stopBits);
        serial->setFlowControl(flowControl);
        if ( serial->open(QIODevice::ReadWrite)) {
            QString serial_open_msg = QString(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                              .arg(serialname).arg("19200").arg("8 - data bits")
                                              .arg("no parity").arg("one stop").arg("no flow control"));
            connect(serial, SIGNAL(readyRead()), this, SLOT(on_received_data()));
            emit serialStatus(SERIAL_OPEN_OK, serial_open_msg);
        }else{
            emit serialStatus(SERIAL_OPEN_FAIL, serial->errorString());
        }
    }else {
        emit serialStatus(SERIAL_OPEN_FAIL, serial->errorString());
    }
}
void Communication::closeSerial(){
    if(serial != nullptr){
        if(serial->isOpen()){
            serial->close();
            serial = nullptr;
            disconnect(serial, SIGNAL(readyRead()), this, SLOT(on_received_data()));
        }
        emit serialStatus(SERIAL_CLOSE, tr("Disconnected"));
    }else{
        emit serialStatus(SERIAL_CLOSE, tr("Disconnected"));
    }
}

void Communication::on_received_data(){
//    qDebug() << "on_received_data()";
    std::vector <char>temp_vector_in;
    if (serial->bytesAvailable() > 0) {
        _dane_in = serial->readAll();
        for(const auto &it : _dane_in){
                //qDebug() << it;
                std::cout << it;
            temp_vector_in.insert(temp_vector_in.end(), it);
            temp_vector_in.data();
            if(it == '\n'){
              std::cout <<"koniec <-------->"<< std::endl;
            }
        }




//        protocol->addReceivedData(temp_vector_in);
        //        parse_data(temp_vector_in);


        _dane_in.clear();
        temp_vector_in.clear();
    }
}


void Communication::parse_data(std::vector<char> &data){
    qDebug() << "parse_data";


}
void Communication::send_data(std::vector<unsigned char> &data){
    if(serial != nullptr){
        for(unsigned char byte : data){
            serial->putChar(byte);
        }
    }
}

void Communication::decode_data(std::vector<char> frame)
{
    qDebug() << "decoded ";
    for (int i = 0; i < frame.size(); i++)
    {
        qDebug() << (int)frame[i];
    }
}

void Communication::onReceivedDecodedFrame(std::vector<char> frame)
{

}

void Communication::NadawcaDanych_dataSend(std::vector<unsigned char> &data_to_send){
    if(serial != nullptr){
        // qDebug() << "Data to send size = " << QString::number(data_to_send.size());
        for(unsigned char byte : data_to_send){
            //qDebug() << "byte = " <<QString::number(byte, 16);
            serial->putChar(byte);
        }

    }
}

void Communication::on_incomig_data_to_send(gamepad_t data, bool x){
    if(x == SERIAL_TYPE){
        if(serial != nullptr){
            if(serial->isOpen()){
                std::vector<char> temp_data(data.table_frame, data.table_frame+OUT_FRAME_SIZE);
                //                for(int i = 0; i < OUT_FRAME_SIZE; i++){
                protocol->codeFrame(temp_data);
                serial->write(temp_data.data(), temp_data.size());
                //                }
            }
        }

    }else if(x == UDP_TYPE){
        if(udpSocket != nullptr){
           // qDebug() <<"udp datagram write";
            std::vector<char> temp_data(data.table_frame, data.table_frame+OUT_FRAME_SIZE);
            protocol->codeFrame(temp_data);
            writeDatagram( temp_data.data(), temp_data.size());
        }
    }
}


