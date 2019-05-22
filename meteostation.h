#ifndef METEOSTATION_H
#define METEOSTATION_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

#pragma pack(push, 1)


typedef enum {
    NODE_LETTER = 0,
    WIND_DIR,
    WIND_SPEED,
    CORRECTED_DIR,
    GPS_SPEED,
    PREASSURE,
    RELATIVE_HUMIDITY,
    TEMPERATURE,
    DEWPOINT,
    GPS_LOCATION,
    DATE_AND_TIME,
    SUPPLY_VOLTAGE,
    STATUS,
    CHECK,
} value_types_t;


typedef enum
{
    WAIT_FOR_START_CMD = 0,
    COLLECT_DATA,
    COUNT_CRC
} collect_data_statemachine_t;


typedef struct {

    char type[100];
    bool satus;
    char units[16][10];
    char values[16][50];

} data_meteo_t;


typedef struct{
    int year;
    int month;
    int day;

} meteo_date_t;

typedef struct{
    int time_zone;
    int hours;
    int min;
    int sec;
} meteo_time_t;


typedef struct {

    char node;
    unsigned int wind_dir;
    float wind_speed;
    int corrected_dir;
    float gps_speed;
    float pressure;
    int reletive_humidity;
    float temperature;
    float devpoint;
    float gps_latitude;
    float gps_longitude;
    float gps_height;
    meteo_date_t meteo_date;
    meteo_time_t meteo_time;
    float supply_voltage;
    int status;

    char type[50];
    char units[16][4];

} meteo_station_data_t;


#pragma pack(pop)

class MeteoStation
{
public:
    MeteoStation();
    MeteoStation(meteo_station_data_t *meteo_station_data);


    bool parse_data(char data);




private:

       meteo_station_data_t *temp_meteo_station_data;

       collect_data_statemachine_t collect_data_statemachine;
       data_meteo_t  data_meteo;
       value_types_t value_types;

       bool data_receive_status;
       uint8_t computeCheckSum(uint8_t *data, int no);
       void fillData(  data_meteo_t * raw_data);

       void chenge_dot_to_coma(char *c);



};

#endif // METEOSTATION_H
