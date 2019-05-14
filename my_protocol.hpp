#ifndef MY_PROTOCOL_HPP
#define MY_PROTOCOL_HPP

#include <cstdint>
#include <vector>

#include <cdelegate.h>

//**************** QUICK EXAMPLE *************

//Application::Application()
//{
//protocol = new MyProtocol();
//protocol->connect<Application, &Application::analyzeRecivedFrame>(this);
//}
//
//Application::~Application()
//{
// delete protocol;
//}
//
//
//void Application::analyzeRecivedFrame(std::vector<char> frame_data_with_header)
//{
//
//}
//
//
//void Application::sendFrameData( vector<char> &sendData)
//{
//    protocol->codeFrame(sendData);
//    if (serialPort->isOpen() && serialPort->isWritable())
//    {
//        //cout << "serialPort->serialWrite(&sendData);" << endl;
//        serialPort->serialWrite(&sendData);
//    }
//}
//
//*******************************************


//VERSION 1.0
//Byte 1 - HEADER 0x31
//Byte 2 - HEADER 0x31
//Byte 3 - ROZMIAR PRZESYŁANYCH DANYCH uint8_t
//-------------------------
//Byte 4 - Poczatek danych
//Byte n - Koniec danych
//-------------------------
//Byte n+1  - CRC (lo) - jest liczone tylko dla danych
//Byte n+2  - CRC (hi) - jest liczone tylko dla danych
//Uwaga, gdy którykolwiek z bajtów od 3 do n+2 jest równy 0x31 to po nim dodaje się 0x30

//VERSION 1.1
//Byte 1 - HEADER 0x31
//Byte 2 - HEADER 0x31
//Byte 3 - Wersja protokołu, przesylana na dwóch półbajtach, np. wersja 1.1 to 0b00010001, wersja 2.3 to 0b00100011, starszy połbajt to major wersji, młodszy półbajt to minor wersji
//Byte 4 - ROZMIAR PRZESYŁANYCH DANYCH uint16_t (lo)
//Byte 5 - ROZMIAR PRZESYŁANYCH DANYCH uint16_t (hi)
//-------------------------
//Byte 6 - Poczatek danych
//Byte n - Koniec danych
//-------------------------
//Byte n+1  - CRC (lo) - jest liczone tylko dla danych
//Byte n+2  - CRC (hi) - jest liczone tylko dla danych
//Uwaga, gdy którykolwiek z bajtów od 3 do n+2 jest równy 0x31 to po nim dodaje się 0x30



class MyProtocol_frameArriveListener;



class MyProtocol{
    typedef CDelegate<void, std::vector<char>> FrameArriveRecipient_t;
public:

    typedef enum{
        VER_1_0 = 0b00010000,
        VER_1_1 = 0b00010001
    }ProtocolVersion;



    typedef enum{
        CONNECTION_LOST ,
        CONNECTION_ESTABLISH,
        CRC_ERROR,
        CRC_OK,
        FRAME_FORMAT_ERROR
    }ProtocolState;


public:
   // MyProtocol(); //domyslnie obsługuje wersje 1.0 protokolu
    MyProtocol(ProtocolVersion version = MyProtocol::VER_1_0); //wybranie wersji protokołu
    ~MyProtocol();
    //send frames
    void codeFrame(char * frameToCode, uint16_t sizeIn, char * codedFrame, uint16_t &sizeOut);
    void codeFrame(std::vector<char> &frame);


    //receive frames
    void parseRecivedByte(char byte);
    void addReceivedData(std::vector<char> data);

    void setFrameArriveListener(MyProtocol_frameArriveListener * frameArriveListener);
    void clearFrameArriveListener();

    template <void (*Function)(std::vector<char>)>
    void connect()
    {
        _recipient.Bind<Function>();
    }

    template<class C, void (C::*Function)(std::vector<char>)>
    void connect(C *instance)
    {
        _recipient.Bind<C, Function>(instance);
    }

private:
    uint16_t crc_ccitt_update (uint16_t crc, uint8_t data);
    uint16_t calculateCRC(char * buff[], uint16_t frame_len);
    uint16_t calculateCRC(char * buff, uint16_t frameSize);
    uint16_t calculateCRC(std::vector<char> const &buff);
    //std::vector<char> arrivedFrame;

    MyProtocol_frameArriveListener * listener = nullptr;
    FrameArriveRecipient_t _recipient;


    typedef enum{
        WAIT_FOR_SYNCHRO_BYTE = 0,
        WAIT_FOR_SECOND_SYNCHRO_BYTE = 1,
        WAIT_FOR_VERSION = 2,
        WAIT_FOR_SIZE_BYTE = 3,
        WAIT_FOR_SIZE_SECOND_BYTE = 4,
        FILL_BUFFER = 5,
        ONE_DATA_DETECTED = 6
    } ParserState ;



    ProtocolVersion current_protocol_version = VER_1_0;

    ParserState mparserState = WAIT_FOR_SYNCHRO_BYTE;
    uint16_t mindex;
    char pbuffer[65000];
    uint16_t receivedFrameFullSize = 0;
    uint16_t receivedFrameDataSize = 0;
    // float number_of_crc_error_frames = 0;
    uint8_t frameDataSize_lo = 0;
    uint8_t frameDataSize_hi = 0;

    const uint32_t DATA_OFFSET_VER_1_0 = 3;
    const uint32_t DATA_OFFSET_VER_1_1 = 5;

};

class MyProtocol_frameArriveListener{
public:
    virtual void MyProtocol_onFrameArrive(MyProtocol *, char * data, int size) = 0;
    virtual void MyProtocol_protocolState(MyProtocol *, MyProtocol::ProtocolState) {}
};


#endif // MY_PROTOCOL_HPP

