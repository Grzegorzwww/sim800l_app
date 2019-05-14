#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QWidget>
#include "ui_mainwindow.h"
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextCursor>
#include <QTimer>
#include "defines.h"
#include <QHostInfo>
#include <QNetworkInterface>



class Graphics : public QWidget
{
    Q_OBJECT
public:
    explicit Graphics(Ui::MainWindow *_ui, QWidget *parent = 0);



private:

      Ui::MainWindow *ui;

      void fillPortsInfo();
      void showPortInfo(int idx);
      void clearGamepadState();


      QTextCursor textbrowser_cursor;

       udp_param_t udp_param;
       gamepad_t gamepad_data;

       bool interface_type;





signals:

      void open_serialport_signal(QString );
      void close_serialport_signal( );


      void on_udp_open_signal(udp_param_t );
      void on_udp_close_signal(udp_param_t );

      void on_data_send_signal( gamepad_t, bool);


private slots:


      void on_set_serial_interface();
      void on_set_ethernet_interface();







public slots:

      void on_open_serial_clicked(bool x);
      void on_close_serial_clicked(bool x);
      void on_open_udp_clicked(bool x);
      void on_close_udp_clicked(bool x);
      void on_change_portname_box(int index);
      void on_handle_serial_satus(SERIAL_OPEN_STATUS status, QString msg);
      void on_handle_udp_satus(QString msg);






private slots:

      void on_inser_udp_data();


};

#endif // GRAPHICS_H
