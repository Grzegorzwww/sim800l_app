
#include "my_protocol.hpp"

#include <cstdint>

//#include <library_api/CDebug.hpp>

#define lo8(x) x & 0x00FF
#define hi8(x) (x & 0xFF00) >> 8



uint16_t MyProtocol::crc_ccitt_update (uint16_t crc, uint8_t data)
{
    data ^= lo8 (crc);
    data ^= data << 4;

    return ((((uint16_t)data << 8) | hi8 (crc)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
}

uint16_t MyProtocol::calculateCRC(char * buff[], uint16_t frame_len){
    uint16_t tmp_crc = 0xFFFF;
    for(uint16_t k = 0; k < frame_len; k++){
        tmp_crc = crc_ccitt_update(tmp_crc, *buff[k]);
    }
    return tmp_crc;
}

uint16_t MyProtocol::calculateCRC(char * buff, uint16_t frameSize){
    uint16_t tmp_crc = 0xFFFF;
    for(uint16_t k = 0; k < frameSize; k++){
        tmp_crc = crc_ccitt_update(tmp_crc, buff[k]);
    }
    return tmp_crc;
}

uint16_t MyProtocol::calculateCRC(std::vector<char> const &buff)
{
    uint16_t tmp_crc = 0xFFFF;
    for (auto const &byte : buff)
    {
        tmp_crc = crc_ccitt_update(tmp_crc, byte);
    }
    return tmp_crc;
}



void MyProtocol::codeFrame(char * frameToCode, uint16_t sizeIn, char * codedFrame, uint16_t & sizeOut){
    //cDebug() << "MyProtocol::codeFrame(char * frameToCode, char sizeIn, char * codedFrame, int & sizeOut)::end" << _endl;

    if(current_protocol_version == VER_1_0)
    {
        sizeOut = 0;

        //ADD HEADERS
        codedFrame[sizeOut++] = '1';
        codedFrame[sizeOut++] = '1';

        //ADD SIZE TO FRAME
        uint16_t size = sizeIn;
        char size_lo;

        size_lo = (char)(size & 0x00FF);

        codedFrame[sizeOut++] = size_lo;
        if(size_lo == '1')
            codedFrame[sizeOut++] = '0';


        //ADD DATA
        for (int i = 0; i < sizeIn; i++) {
            codedFrame[sizeOut++] = frameToCode[i];
            if (frameToCode[i] == '1') {
                codedFrame[sizeOut++] = '0';
            }
        }

        uint16_t send_data_crc;
        //qDebug() << "SFRAME: ";
        send_data_crc = calculateCRC(frameToCode,sizeIn);
        //cDebug() << "SCRC16: " << send_data_crc << _endl;

        codedFrame[sizeOut++] = *((char *) &send_data_crc);

        if(codedFrame[sizeOut-1] == '1'){
            codedFrame[sizeOut++] = '0';
        }

        codedFrame[sizeOut++] = *(((char *) &send_data_crc) + 1);

        if(codedFrame[sizeOut-1] == '1'){
            codedFrame[sizeOut++] = '0';
        }
    }else if(current_protocol_version == VER_1_1)
    {

        sizeOut = 0;

        //ADD HEADERS
        codedFrame[sizeOut++] = '1';
        codedFrame[sizeOut++] = '1';

        //ADD VERSION TO FRAME
        codedFrame[sizeOut++] = (char)VER_1_1;
        if((char)VER_1_1 == '1')
            codedFrame[sizeOut++] = '0';

        //ADD SIZE TO FRAME
        uint16_t size = sizeIn;
        char size_lo;
        char size_hi;

        size_lo = (char)(size & 0x00FF);
        size_hi = (char)((size >> 8) & 0x00FF);

        codedFrame[sizeOut++] = size_lo;
        if(size_lo == '1')
            codedFrame[sizeOut++] = '0';

        codedFrame[sizeOut++] = size_hi;
        if(size_hi == '1')
            codedFrame[sizeOut++] = '0';


        //ADD DATA
        for (int i = 0; i < sizeIn; i++) {
            codedFrame[sizeOut++] = frameToCode[i];
            if (frameToCode[i] == '1') {
                codedFrame[sizeOut++] = '0';
            }
        }

        uint16_t send_data_crc;
        //qDebug() << "SFRAME: ";
        send_data_crc = calculateCRC(frameToCode,sizeIn);
        //cDebug() << "SCRC16: " << send_data_crc << _endl;

        codedFrame[sizeOut++] = *((char *) &send_data_crc);

        if(codedFrame[sizeOut-1] == '1'){
            codedFrame[sizeOut++] = '0';
        }

        codedFrame[sizeOut++] = *(((char *) &send_data_crc) + 1);

        if(codedFrame[sizeOut-1] == '1'){
            codedFrame[sizeOut++] = '0';
        }



    }
    // cDebug() << "MyProtocol::codeFrame(char * frameToCode, char sizeIn, char * codedFrame, int & sizeOut)::end" << _endl;
}

void MyProtocol::codeFrame(std::vector<char> &frame)
{
    // cDebug() << "MyProtocol::codeFrame(std::vector<char> &frame)::start" << _endl;
    if(current_protocol_version == VER_1_0)
    {

        std::vector<char>_tmp_frame;
        uint16_t crc = 0;
        char *crc_8 = (char *)&crc;

        crc = calculateCRC(frame);

        //    for (std::vector<char>::iterator byte_it = frame.begin(); byte_it != frame.end(); byte_it++)
        //    {
        //        if (*byte_it == '1')
        //        {
        //            frame.insert(byte_it+1, '0');
        //        }
        //    }

        //ADD HEADERS
        _tmp_frame.push_back('1');
        _tmp_frame.push_back('1');

        //ADD SIZE TO FRAME

        uint8_t size_lo = frame.size();

        _tmp_frame.push_back((char)size_lo);
        if(size_lo == '1')
            _tmp_frame.push_back('0');

        //ADD DATA
        for (auto const &byte : frame)
        {
            _tmp_frame.push_back(byte);
            if (byte == '1')
            {
                _tmp_frame.push_back('0');
            }
        }

        //ADD CRC
        _tmp_frame.push_back(crc_8[0]);

        if (crc_8[0] == '1')
        {
            _tmp_frame.push_back('0');
        }

        _tmp_frame.push_back(crc_8[1]);

        if (crc_8[1] == '1')
        {
            _tmp_frame.push_back('0');
        }

        // cDebug() << "MyProtocol::codeFrame(std::vector<char> &frame)::end" << _endl;
        //return _tmp_frame;
        frame.clear();
        frame.assign(_tmp_frame.begin(), _tmp_frame.end());

    }else if(current_protocol_version == VER_1_1)
    {
        std::vector<char>_tmp_frame;
        uint16_t crc;
        char *crc_8;

        crc = calculateCRC(frame);
        crc_8 = (char *)&crc;

        //    for (std::vector<char>::iterator byte_it = frame.begin(); byte_it != frame.end(); byte_it++)
        //    {
        //        if (*byte_it == '1')
        //        {
        //            frame.insert(byte_it+1, '0');
        //        }
        //    }

        //ADD HEADERS
        _tmp_frame.push_back('1');
        _tmp_frame.push_back('1');

        //ADD VERSION TO FRAME
        _tmp_frame.push_back((char)VER_1_1);
        if((char)VER_1_1 == '1')
            _tmp_frame.push_back('0');


        //ADD SIZE TO FRAME

        uint16_t size = (uint16_t)frame.size();
        char size_lo;
        char size_hi;

        size_lo = (char)(size & 0x00FF);
        size_hi = (char)((size >> 8) & 0x00FF);

        _tmp_frame.push_back((char)size_lo);
        if(size_lo == '1')
            _tmp_frame.push_back('0');

        _tmp_frame.push_back((char)size_hi);
        if(size_hi == '1')
            _tmp_frame.push_back('0');

        //ADD DATA
        for (auto const &byte : frame)
        {
            _tmp_frame.push_back(byte);
            if (byte == '1')
            {
                _tmp_frame.push_back('0');
            }
        }

        //ADD CRC
        _tmp_frame.push_back(crc_8[0]);

        if (crc_8[0] == '1')
        {
            _tmp_frame.push_back('0');
        }

        _tmp_frame.push_back(crc_8[1]);

        if (crc_8[1] == '1')
        {
            _tmp_frame.push_back('0');
        }

        // cDebug() << "MyProtocol::codeFrame(std::vector<char> &frame)::end" << _endl;
        //return _tmp_frame;
        frame.clear();
        frame.assign(_tmp_frame.begin(), _tmp_frame.end());
    }
}

//MyProtocol::MyProtocol(){

//}

MyProtocol::MyProtocol(ProtocolVersion version){
    current_protocol_version = version;
}

MyProtocol::~MyProtocol()
{
    listener = nullptr;
}

void MyProtocol::setFrameArriveListener(MyProtocol_frameArriveListener * frameArriveListener){
    listener = frameArriveListener;
}

void MyProtocol::clearFrameArriveListener(){
    listener = nullptr;
}

void MyProtocol::parseRecivedByte(char byte){
    //  cDebug() << "void MyProtocol::parseRecivedByte(char byte)::start" << _endl;

   if(mindex == 725)
   {
        mindex = 725;
   }

    if(current_protocol_version == VER_1_0)
    {

        switch(mparserState){
        case WAIT_FOR_SYNCHRO_BYTE:
             //cDebug() << "p 2" << _endl;
            if(byte == '1'){
                mparserState = WAIT_FOR_SECOND_SYNCHRO_BYTE;
                mindex = 0;
                this->pbuffer[mindex++] = '1';
            }else{

            }
            break;
        case WAIT_FOR_SECOND_SYNCHRO_BYTE:
             // cDebug() << "p 3" << _endl;
            if(byte == '1'){
                mparserState = WAIT_FOR_SIZE_BYTE;
                this->pbuffer[mindex++] = '1';
                // cDebug() << "p 3.1" << _endl;
            }else{
                mparserState = WAIT_FOR_SYNCHRO_BYTE;
                // cDebug() << "p 3.2" << _endl;
            }
            break;
        case WAIT_FOR_SIZE_BYTE:
            //cDebug() << "p 4" << _endl;
            receivedFrameDataSize = byte;
            receivedFrameFullSize = receivedFrameDataSize + 5;
            this->pbuffer[mindex++] = receivedFrameDataSize;
            //frameSizeWithCRC = frameSize + 2;
            mparserState = FILL_BUFFER;
            break;
        case FILL_BUFFER:
            //cDebug() << "p 5" << _endl;
            if(byte == '1'){
                //cDebug() << "p 6" << _endl;
                mparserState = ONE_DATA_DETECTED;
            }else{
                // cDebug() << "p 7" << _endl;
                this->pbuffer[mindex++] = byte;
                //cDebug() << "p 8" << _endl;
                if(mindex == receivedFrameFullSize){

                    mparserState = WAIT_FOR_SYNCHRO_BYTE;


                    char * wskaznikNaBajt;
                    uint16_t  received_crc;
                    wskaznikNaBajt = (char*)&received_crc;
                    *wskaznikNaBajt = pbuffer[mindex-2];
                    *(wskaznikNaBajt + 1) = pbuffer[mindex-1];


                    uint16_t calculated_received_crc;
                    calculated_received_crc = calculateCRC(&this->pbuffer[DATA_OFFSET_VER_1_0],receivedFrameDataSize);


                    if(calculated_received_crc != received_crc){
                        //_recipient(arrivedFrame, CRC_ERROR);
                        if (listener)
                        {
                            listener->MyProtocol_protocolState(this, CRC_ERROR);
                        }
                           //std::cout << "MyProtocol CRC errorr: " << std::endl;
                    }else{
                        //arrivedFrame.clear();
                        std::vector<char> arrivedFrame;
                        //std::copy(&this->pbuffer[3], (&this->pbuffer[3])+receivedFrameDataSize, std::back_inserter(arrivedFrame));
                        arrivedFrame.insert(arrivedFrame.end(), &this->pbuffer[DATA_OFFSET_VER_1_0], &this->pbuffer[DATA_OFFSET_VER_1_0 + receivedFrameDataSize]);
                        _recipient(arrivedFrame);
//                        if (listener)
//                        {
//                            listener->MyProtocol_onFrameArrive(this, &this->pbuffer[DATA_OFFSET_VER_1_0], receivedFrameDataSize);
//                        }
                    }
                    mindex = 0;
                }
            }
            break;
        case ONE_DATA_DETECTED:
            if(byte == '0'){
                this->pbuffer[mindex++] = '1';
                mparserState = FILL_BUFFER;
                if(mindex == receivedFrameFullSize){

                    mparserState = WAIT_FOR_SYNCHRO_BYTE;


                    char * wskaznikNaBajt;
                    uint16_t  received_crc;
                    wskaznikNaBajt = (char*)&received_crc;
                    *wskaznikNaBajt = pbuffer[mindex-2];
                    *(wskaznikNaBajt + 1) = pbuffer[mindex-1];



                    uint16_t calculated_received_crc;
                    calculated_received_crc = calculateCRC(&this->pbuffer[DATA_OFFSET_VER_1_0],receivedFrameDataSize);


                    if(calculated_received_crc != received_crc){
                        if (listener)
                        {
                            listener->MyProtocol_protocolState(this, CRC_ERROR);
                        }

                        std::cout << "crc error" << std::endl;
                    }else{
                        //arrivedFrame.clear();
                        std::vector<char> arrivedFrame;
                        //std::copy(&this->pbuffer[3], (&this->pbuffer[3])+receivedFrameDataSize, std::back_inserter(arrivedFrame));
                        arrivedFrame.insert(arrivedFrame.end(), &this->pbuffer[DATA_OFFSET_VER_1_0], &this->pbuffer[DATA_OFFSET_VER_1_0+receivedFrameDataSize]);
                        _recipient(arrivedFrame);
                        if (listener)
                        {
                            listener->MyProtocol_onFrameArrive(this, &this->pbuffer[DATA_OFFSET_VER_1_0], receivedFrameDataSize);
                        }
                    }
                      mindex = 0;
                }
            }else{
                mparserState = WAIT_FOR_SYNCHRO_BYTE;
                mindex = 0;
            }
            break;
        default:
            break;
        }

    }else if(current_protocol_version == VER_1_1){

        switch(mparserState){
        case WAIT_FOR_SYNCHRO_BYTE:
            // cDebug() << "p 2" << _endl;
            if(byte == '1'){
                mparserState = WAIT_FOR_SECOND_SYNCHRO_BYTE;
                mindex = 0;
                this->pbuffer[mindex++] = '1';
            }else{

            }
            break;
        case WAIT_FOR_SECOND_SYNCHRO_BYTE:
            // cDebug() << "p 3" << _endl;
            if(byte == '1'){
                mparserState = WAIT_FOR_VERSION;
                this->pbuffer[mindex++] = '1';
                // cDebug() << "p 3.1" << _endl;
            }else{
                mparserState = WAIT_FOR_SYNCHRO_BYTE;
                // cDebug() << "p 3.2" << _endl;
            }
            break;
        case WAIT_FOR_VERSION:
            this->pbuffer[mindex++] = byte;
            mparserState = WAIT_FOR_SIZE_BYTE;
            break;
        case WAIT_FOR_SIZE_BYTE:
            //cDebug() << "p 4" << _endl;
            frameDataSize_lo = (uint8_t)byte;
            this->pbuffer[mindex++] = byte;
            //frameSizeWithCRC = frameSize + 2;
            mparserState = WAIT_FOR_SIZE_SECOND_BYTE;
            break;
        case WAIT_FOR_SIZE_SECOND_BYTE:
            //cDebug() << "p 4" << _endl;
            frameDataSize_hi = (uint8_t)byte;
            receivedFrameDataSize = (((uint16_t)frameDataSize_hi) << 8) | (uint16_t)frameDataSize_lo;
            receivedFrameFullSize = receivedFrameDataSize + 7;
            this->pbuffer[mindex++] = byte;
            //frameSizeWithCRC = frameSize + 2;
            mparserState = FILL_BUFFER;
            break;
        case FILL_BUFFER:
            //cDebug() << "p 5" << _endl;
            if(byte == '1'){
                //cDebug() << "p 6" << _endl;
                mparserState = ONE_DATA_DETECTED;
            }else{
                // cDebug() << "p 7" << _endl;
                this->pbuffer[mindex++] = byte;
                //cDebug() << "p 8" << _endl;
                if(mindex == receivedFrameFullSize){

                    mparserState = WAIT_FOR_SYNCHRO_BYTE;


                    char * wskaznikNaBajt;
                    uint16_t  received_crc;
                    wskaznikNaBajt = (char*)&received_crc;
                    *wskaznikNaBajt = pbuffer[mindex-2];
                    *(wskaznikNaBajt + 1) = pbuffer[mindex-1];


                    uint16_t calculated_received_crc;
                    calculated_received_crc = calculateCRC(&this->pbuffer[DATA_OFFSET_VER_1_1],receivedFrameDataSize);


                    if(calculated_received_crc != received_crc){
                        //_recipient(arrivedFrame, CRC_ERROR);
                        if (listener)
                        {
                            listener->MyProtocol_protocolState(this, CRC_ERROR);
                        }
                        //   cDebug() << "MyProtocol CRC errorr: " << crc_errors << _endl;
                    }else{
                        //arrivedFrame.clear();
                        std::vector<char> arrivedFrame;
                        //std::copy(&this->pbuffer[3], (&this->pbuffer[3])+receivedFrameDataSize, std::back_inserter(arrivedFrame));
                        arrivedFrame.insert(arrivedFrame.end(), &this->pbuffer[DATA_OFFSET_VER_1_1], &this->pbuffer[DATA_OFFSET_VER_1_1+receivedFrameDataSize]);
                        _recipient(arrivedFrame);
                        if (listener)
                        {
                            listener->MyProtocol_onFrameArrive(this, &this->pbuffer[DATA_OFFSET_VER_1_1], receivedFrameDataSize);
                        }
                    }
                    mindex = 0;
                }
            }
            break;
        case ONE_DATA_DETECTED:
            if(byte == '0'){
                this->pbuffer[mindex++] = '1';
                mparserState = FILL_BUFFER;
                if(mindex == receivedFrameFullSize){

                    mparserState = WAIT_FOR_SYNCHRO_BYTE;


                    char * wskaznikNaBajt;
                    uint16_t  received_crc;
                    wskaznikNaBajt = (char*)&received_crc;
                    *wskaznikNaBajt = pbuffer[mindex-2];
                    *(wskaznikNaBajt + 1) = pbuffer[mindex-1];



                    uint16_t calculated_received_crc;
                    calculated_received_crc = calculateCRC(&this->pbuffer[DATA_OFFSET_VER_1_1],receivedFrameDataSize);


                    if(calculated_received_crc != received_crc){
                        if (listener)
                        {
                            listener->MyProtocol_protocolState(this, CRC_ERROR);
                        }


                    }else{
                        //arrivedFrame.clear();
                        std::vector<char> arrivedFrame;
                        //std::copy(&this->pbuffer[3], (&this->pbuffer[3])+receivedFrameDataSize, std::back_inserter(arrivedFrame));
                        arrivedFrame.insert(arrivedFrame.end(), &this->pbuffer[DATA_OFFSET_VER_1_1], &this->pbuffer[DATA_OFFSET_VER_1_1+receivedFrameDataSize]);
                        _recipient(arrivedFrame);
                        if (listener)
                        {
                            listener->MyProtocol_onFrameArrive(this, &this->pbuffer[DATA_OFFSET_VER_1_1], receivedFrameDataSize);
                        }
                    }
                                        mindex = 0;
                }
            }else{
                mparserState = WAIT_FOR_SYNCHRO_BYTE;
                mindex = 0;
            }
            break;
        }

    }
    //  cDebug() << "void MyProtocol::parseRecivedByte(char byte)::end" << _endl;
}

void MyProtocol::addReceivedData(std::vector<char> data)
{
    //cout << "void MyProtocol::addReceivedData(std::vector<char> data)::start" << _endl;
    for (auto const &byte : data)
    {
        parseRecivedByte(byte);
    }
   //cout << "void MyProtocol::addReceivedData(std::vector<char> data)::end" << _endl;
}
