#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communication.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    communication = new Communication(this);
    graphics = new Graphics(ui, this);

    connect(graphics, SIGNAL(open_serialport_signal(QString)), communication, SLOT(openSerial(QString)));
    connect(graphics, SIGNAL(close_serialport_signal()), communication, SLOT(closeSerial()));


    connect(graphics, SIGNAL(on_udp_open_signal(udp_param_t)), communication, SLOT(startUDP_signal(udp_param_t)));
    connect(graphics, SIGNAL(on_udp_close_signal(udp_param_t)), communication, SLOT(stopUDP_signal(udp_param_t)));
    connect(communication, SIGNAL(udpStatus(QString)), graphics, SLOT(on_handle_udp_satus(QString)));


    connect(communication, SIGNAL(serialStatus(SERIAL_OPEN_STATUS, QString)), graphics, SLOT(on_handle_serial_satus(SERIAL_OPEN_STATUS, QString)));
    //connect(graphics, SIGNAL(on_data_send_signal(gamepad_t,bool)), communication, SLOT(on_incomig_data_to_send(gamepad_t,bool)));


    connect(communication, SIGNAL(on_data_parsed_signal(meteo_station_data_t)), graphics, SLOT(parsedDataSlot(meteo_station_data_t)));





}

MainWindow::~MainWindow()
{
    delete ui;
}
