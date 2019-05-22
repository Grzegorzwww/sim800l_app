#include "graphics.h"

Graphics::Graphics(Ui::MainWindow *_ui, QWidget *parent) :ui(_ui),
    QWidget(parent)
{

    fillPortsInfo();
    showPortInfo(ui->comboBox->currentIndex());

    QTextCharFormat format;
    format.setFontWeight( QFont::DemiBold );
    format.setForeground( QBrush( QColor( "white" ) ) );
    textbrowser_cursor.setCharFormat( format );
    textbrowser_cursor.insertText("...\n");





    QString ipRange = "(([ 0]+)|([ 0]*[0-9] *)|([0-9][0-9] )|([ 0][0-9][0-9])|(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->lineEdit->setValidator(ipValidator);
    ui->lineEdit->setInputMask("000.000.000.000");
    ui->lineEdit->setCursorPosition(0);

    ui->lineEdit_4->setValidator(ipValidator);
    ui->lineEdit_4->setInputMask("000.000.000.000");
    ui->lineEdit_4->setCursorPosition(0);


    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int nIter=0; nIter<list.count(); nIter++)
    {
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
                ui->lineEdit_4->setText(list[nIter].toString());

    }

    ui->lineEdit->setText("127.  0.  0.  1");
    ui->lineEdit_2->setText("2006");
    ui->lineEdit_3->setText("2007");


    ui->lineEdit_4->setReadOnly(true);
    ui->radioButton_serial->setChecked(true);
    on_set_serial_interface();

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_change_portname_box(int)));
    connect(ui->pushButton_openserial, SIGNAL(clicked(bool)), this, SLOT(on_open_serial_clicked(bool)));
    connect(ui->pushButton_closeserial, SIGNAL(clicked(bool)), this, SLOT(on_close_serial_clicked(bool)));

    connect(ui->pushButton_open_udp, SIGNAL(clicked(bool)), this, SLOT(on_open_udp_clicked(bool)));

    connect(ui->pushButton_close_upd, SIGNAL(clicked(bool)), this, SLOT(on_close_udp_clicked(bool)));

    connect(ui->radioButton_serial, SIGNAL(clicked()), this, SLOT(on_set_serial_interface()));

    connect(ui->radioButton_udp, SIGNAL(clicked()), this, SLOT(on_set_ethernet_interface()));



}

//void Graphics::clearGamepadState(){
//    for(int i = 0; i < OUT_FRAME_SIZE; i++){
////        gamepad_data.table_frame[i] = 0;
//    }
//}


void Graphics::parsedDataSlot(meteo_station_data_t data)
{

    static int command_no = 0;
    ui->label_temperatura_value->setText(QString::number(data.temperature) +" "+ data.units[TEMPERATURE]);


    ui->label_dewpoint_value->setText(QString::number(data.devpoint)+" "+ data.units[DEWPOINT] );


    ui->label_nazwa_stacji_2->setText(data.type);

//    qDebug() <<"type from qui "<< data.type;


    ui->label_command_counter_value->setText(QString::number(command_no++));

    ui->label_status_value->setText(QString::number(data.status));

    ui->label_start_up_value->setText(QString::number(1));

    ui->label_node_value->setText(QString(data.node));

    ui->label_zasilanie_value->setText(QString::number(data.supply_voltage)+" "+ data.units[SUPPLY_VOLTAGE]);

    ui->label_time_value->setText(QString::number(data.meteo_time.hours)+":"+
                                  QString::number(data.meteo_time.min)+":"+
                                  QString::number(data.meteo_time.sec));


    ui->label_date_value->setText(QString::number(data.meteo_date.day)+"-"+
                                  QString::number(data.meteo_date.month)+"-"+
                                  QString::number(data.meteo_date.year));

    ui->label_label_longnitude_value->setText(QString::number(data.gps_longitude));

    ui->label_latitude_value->setText(QString::number(data.gps_latitude));
    ui->label_latitude_value->setText(QString::number(data.gps_latitude));
    ui->label_wysokosc_value->setText(QString::number(data.gps_height));

    ui->label_wind_speed_value->setText(QString::number(data.wind_speed));

    ui->label_wind_dir_value->setText(QString::number(data.wind_dir));

     ui->label_rh_value->setText(QString::number(data.reletive_humidity));

     ui->label_press_value->setText(QString::number(data.pressure));

      ui->label_speed_value->setText(QString::number(data.gps_speed));



}


void Graphics::fillPortsInfo()
{
    // m_ui->serialPortInfoListBox->clear();

    ui->comboBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;

        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();

        list << info.portName()
             << (!description.isEmpty() ? description : " - " )
             << (!manufacturer.isEmpty() ? manufacturer : " - ")
             << (!serialNumber.isEmpty() ? serialNumber : " - ")
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString(" - "))
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString(" - "));
        ui->comboBox->addItem(list.first(), list);
    }


}

void Graphics::showPortInfo(int idx)
{
    if (idx == -1)
        return;
    const QStringList list = ui->comboBox->itemData(idx).toStringList();
    ui->label_serial_description->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr("-")));
    ui->label_serial_manufacturer->setText(tr("Mcanufaturer: %1").arg(list.count() > 2 ? list.at(2) : tr("-")));
    ui->label_serial_serialnumber->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr("-")));
    ui->label_serial_locationnumber->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr("-")));
    ui->label_serial_vidlabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr("-")));
    ui->label_serial_pidlabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr("-")));
}

void Graphics::on_change_portname_box(int index){
    showPortInfo(index);
}

void Graphics::on_open_serial_clicked(bool x){
    const QStringList list = ui->comboBox->itemData( ui->comboBox->currentIndex()).toStringList();
    emit open_serialport_signal(list.at(0));
}

void Graphics::on_close_serial_clicked(bool x){
    emit close_serialport_signal();
}


void Graphics::on_open_udp_clicked(bool x){


    udp_param.ip.setAddress(ui->lineEdit->text());
    udp_param.out_port = ui->lineEdit_2->text().toInt();
    udp_param.in_port = ui->lineEdit_3->text().toInt();

    emit on_udp_open_signal(udp_param);

}
void Graphics::on_close_udp_clicked(bool x){
    emit on_udp_close_signal(udp_param);

}


void Graphics::on_handle_serial_satus(SERIAL_OPEN_STATUS status, QString msg){
    switch(status)
    {
    case SERIAL_OPEN_FAIL:
        ui->graphicsView->setStyleSheet(SET_STYLE_RED);
        break;
    case SERIAL_OPEN_OK:
        ui->graphicsView->setStyleSheet(SET_STYLE_GREEN);
        break;
    case SERIAL_CLOSE:
        ui->graphicsView->setStyleSheet( SET_STYLE_BUTTON_DEFAULT);
        break;
    }
    ui->label_serial_open_status->setText(msg);
}

void Graphics::on_handle_udp_satus(QString msg){
    if(msg == QString("Started")){
        ui->graphicsView_udp->setStyleSheet(SET_STYLE_GREEN);
    }else{
        ui->graphicsView_udp->setStyleSheet(SET_STYLE_RED);
    }
    ui->label_serial_open_status_udp->setText("STATUS "+msg);
}

void Graphics::on_inser_udp_data()
{

//    udp_param.ip = QHostAddress(ui->lineEdit->text());
//    udp_param.in_port = ui->lineEdit_3->text().toInt();
//    udp_param.out_port = ui->lineEdit_2->text().toInt();
//    emit on_udp_open_signal(udp_param);

}


void Graphics::on_set_serial_interface(){
    ui->groupBox_2->setEnabled(true);
    ui->groupBox->setEnabled(false);
    interface_type = SERIAL_TYPE;

}
void Graphics::on_set_ethernet_interface(){
    ui->groupBox_2->setEnabled(false);
    ui->groupBox->setEnabled(true);
    interface_type = UDP_TYPE;

}




