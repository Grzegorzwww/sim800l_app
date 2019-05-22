#ifndef DEFINES_H
#define DEFINES_H
#include <iostream>
#include <QString>
#include <QHostAddress>


typedef enum {SERIAL_OPEN_FAIL = 0,
              SERIAL_OPEN_OK = 1,
              SERIAL_CLOSE }

             SERIAL_OPEN_STATUS;


static const QString  SET_STYLE_GREEN = "background-color: green";
static const QString  SET_STYLE_RED =  "background-color: red";
static const QString  SET_STYLE_YELLOW =  "background-color: yellow";
static const QString  SET_STYLE_DEFAULT =  " background-color: rgba( 255, 255, 255, 0% );";
static const QString  SET_STYLE_BUTTON_DEFAULT = "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa) );";


typedef enum {Button_A = 128, Button_B = 64, Button_X = 32, Button_Y = 16, Button_LB = 8,  Button_RB = 4, Button_BACK = 2,  Button_START = 1} ButonsMSB_t;
typedef enum {Button_ZMT = 128, Button_LeftPush = 64, Button_RightPush = 32, Button_L = 16, Button_R = 8,  Button_UP = 4, Button_DOWN = 2,  Button_none = 1} ButonsLSB_t;

static const bool SERIAL_TYPE = true;
static const bool UDP_TYPE = false;


static const int OUT_FRAME_SIZE = 8;



#pragma pack(push, 1)


//typedef union {
//    struct {
//        unsigned char axis_0;
//        unsigned char axis_1;
//        unsigned char axis_2;
//        unsigned char axis_3;
//        unsigned char axis_4;
//        unsigned char axis_5;
//        unsigned char buttons_msb;
//        unsigned char buttons_lsb;
//    } struct_frame;

//    unsigned char table_frame[OUT_FRAME_SIZE];

//} gamepad_t;



typedef struct {
    QHostAddress ip;
    unsigned int out_port;
    unsigned int in_port;

} udp_param_t;



#pragma pack(pop)





#endif // DEFINES_H
