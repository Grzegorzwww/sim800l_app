#include "meteostation.h"


#include "QDebug"
#include "iostream"


MeteoStation::MeteoStation()
{
    collect_data_statemachine =  WAIT_FOR_START_CMD;
    value_types = NODE_LETTER;
    data_receive_status = false;
    temp_meteo_station_data = nullptr;

}
MeteoStation::MeteoStation(meteo_station_data_t *meteo_station_data)
{
    temp_meteo_station_data = meteo_station_data;
    collect_data_statemachine =  WAIT_FOR_START_CMD;
    value_types = NODE_LETTER;
    data_receive_status = false;

}


bool MeteoStation::parse_data(char data){
    static int counter = 0;
    static char buffor_data_in[300];
    int i, j = 0;

    static int debug_counter = 0;

    switch(collect_data_statemachine){

    case WAIT_FOR_START_CMD :
        if(data == 0x02){
            collect_data_statemachine = COLLECT_DATA;
            counter = 0;
        }
        else{

            buffor_data_in[counter++] = data;
            if(data == '\n'){
                if(strstr( buffor_data_in, "MAXIMET") != NULL) {
                    qDebug() << "type = "<<debug_counter++;
                    data_meteo.type[0] ='\0';
                    strcpy(data_meteo.type, buffor_data_in);
                    counter = 0;
                }
                else if(strstr( buffor_data_in, "STARTUP") != NULL) {
                    qDebug() << "startup = "<<debug_counter++;
                    if(strstr( buffor_data_in, "OK") != NULL) {

                        data_meteo.satus = true;
                    }else{
                        data_meteo.satus = false;
                    }
                    counter = 0;
                }
                else if((strstr( buffor_data_in, "-,") != NULL)) {
                    qDebug() << "units = "<<debug_counter++;
                    char *temp_ptr  = strstr( buffor_data_in, "-,");
                    value_types = NODE_LETTER;
                    for(i = 0; i < counter; i++){
                        if(temp_ptr[i] == ','){
                            if(value_types < CHECK){
                                int temp = value_types;
                                temp++;
                                value_types = (value_types_t)temp;
                                j = 0;
                                continue;
                            }
                        }else{
                            data_meteo.units[value_types][j++] = temp_ptr[i];
                        }
                    }
                    counter = 0;
                    value_types = NODE_LETTER;
                }
                else{
                    // counter = 0;
                    ;
                }
            }
        }
        break;
    case COLLECT_DATA :
        buffor_data_in[counter++] = data;
        qDebug() << "collect data segment";
        if(data == 'ÿ'){
            counter = 0;
            collect_data_statemachine = WAIT_FOR_START_CMD;
            return false;
        }
        if(data == '\n'){
            value_types = NODE_LETTER;
            for(i = 0; i < counter; i++){
                if( buffor_data_in[i] == ','){
                    if(value_types < CHECK){
                        int temp = value_types;
                        temp++;
                        value_types = (value_types_t)temp;
                        j = 0;
                        continue;
                    }
                }else{
                    if(buffor_data_in[i] == 0x03){
                        i++;
                        data_meteo.values[CHECK][j++] = buffor_data_in[i];
                        i++;
                        data_meteo.values[CHECK][j++] = buffor_data_in[i];
                        collect_data_statemachine = COUNT_CRC;
                        break;
                    }else{
                        data_meteo.values[value_types][j++] = buffor_data_in[i];
                    }
                }
            }
            collect_data_statemachine = COUNT_CRC;
        }
        break;

    case COUNT_CRC :
        uint8_t crc = 0;
        crc = computeCheckSum((uint8_t *)buffor_data_in, counter - 5);
        char temp_str[2] = {buffor_data_in[counter - 4],buffor_data_in[counter - 3] };
        int count_crc = (int)strtol(temp_str, NULL, 16);

        if(count_crc == crc ){
            collect_data_statemachine = WAIT_FOR_START_CMD;
            data_receive_status = true;
            fillData(&data_meteo);
             counter = 0;
            return true;

        }else{
            data_receive_status = false;
            collect_data_statemachine = WAIT_FOR_START_CMD;
             counter = 0;
            return false;
        }
        collect_data_statemachine = WAIT_FOR_START_CMD;
        break;
    }
    return false;

}


uint8_t MeteoStation::computeCheckSum(uint8_t *data, int no){
    int i;
    uint8_t crc = 0;
    for(i = 0; i < no ; i++){
        crc = crc ^ data[i];
    }
    return crc;
}

void MeteoStation::fillData(data_meteo_t *raw_data){
    temp_meteo_station_data->node = raw_data->values[NODE_LETTER][0];

    temp_meteo_station_data->wind_dir =  atoi(raw_data->values[WIND_DIR]);

    chenge_dot_to_coma(raw_data->values[WIND_SPEED]);
    temp_meteo_station_data->wind_speed =  atof(raw_data->values[WIND_SPEED]);

    temp_meteo_station_data->corrected_dir =  atoi(raw_data->values[CORRECTED_DIR]);

    chenge_dot_to_coma(raw_data->values[GPS_SPEED]);
    temp_meteo_station_data->gps_speed =  atof(raw_data->values[GPS_SPEED]);

    chenge_dot_to_coma(raw_data->values[PREASSURE]);
    temp_meteo_station_data->pressure =  atof(raw_data->values[PREASSURE]);

    temp_meteo_station_data->reletive_humidity =  atoi(raw_data->values[RELATIVE_HUMIDITY]);

    chenge_dot_to_coma(raw_data->values[TEMPERATURE]);
    temp_meteo_station_data->temperature =  atof(raw_data->values[TEMPERATURE]);

    chenge_dot_to_coma(raw_data->values[DEWPOINT]);
    temp_meteo_station_data->devpoint =  atof(raw_data->values[DEWPOINT]);

    int i, j = 0;
    int k = 0;
    char  temp_str [strlen(raw_data->values[GPS_LOCATION])  + 1];

    for(i = 0; i < strlen(raw_data->values[GPS_LOCATION]); i++){
        if(raw_data->values[GPS_LOCATION][i] == ':' ||
                i == strlen(raw_data->values[GPS_LOCATION]) -1  ){

            temp_str[j] = '\0';
            j = 0;
            switch(k){
            case 0:
                chenge_dot_to_coma(temp_str);
                temp_meteo_station_data->gps_latitude =  atof(temp_str);
                k++;
                break;
            case 1:
                chenge_dot_to_coma(temp_str);
                temp_meteo_station_data->gps_longitude =  atof(temp_str);
                k++;
                break;
            case 2:
                chenge_dot_to_coma(temp_str);
                temp_meteo_station_data->gps_height =  atof(temp_str);
                break;
            }
            continue;
        }
        temp_str[j] = raw_data->values[GPS_LOCATION][i];
        j++;
    }
    j = 0;
    k = 0;
    char separator_char = '-';

    for(i = 0; i < strlen(raw_data->values[DATE_AND_TIME]); i++){
        if(raw_data->values[DATE_AND_TIME][i] == separator_char ||
                i == strlen(raw_data->values[DATE_AND_TIME]) -1  ){
            temp_str[j] = '\0';
            j = 0;
            switch(k){
            case 0:
                temp_meteo_station_data->meteo_date.year =  atoi(temp_str);
                k++;
                break;
            case 1:
                temp_meteo_station_data->meteo_date.month =  atoi(temp_str);
                separator_char = 'T';
                k++;
                break;
            case 2:
                temp_meteo_station_data->meteo_date.day =  atoi(temp_str);
                separator_char = ':';
                k++;
                break;
            case 3:
                temp_meteo_station_data->meteo_time.hours =  atoi(temp_str);
                k++;
                break;
            case 4:
                temp_meteo_station_data->meteo_time.min =  atoi(temp_str);
                k++;
                break;
            case 5:
                temp_meteo_station_data->meteo_time.sec =  atoi(temp_str);
                k++;
                break;
            }
            continue;
        }
        temp_str[j] = raw_data->values[DATE_AND_TIME][i];
        j++;
    }
    chenge_dot_to_coma(raw_data->values[SUPPLY_VOLTAGE]);
    temp_meteo_station_data->supply_voltage=  atof(raw_data->values[SUPPLY_VOLTAGE]);
    temp_meteo_station_data->status=  atoi(raw_data->values[STATUS]);

    for(i = 0; i < 1;i++){
        strcpy( temp_meteo_station_data->units[i], raw_data->units[i]);
    }
}

void MeteoStation::chenge_dot_to_coma(char *c){
    while(*c++){
        if(*c == '.'){
            *c = ',';
        }
    }
}