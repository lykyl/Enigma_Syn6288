#include "enigmaSyn6288.h"

Enigma_Syn6288 syn(/* rxPin */ 12,/* txPin */ 13);  //syn()  HW Serial
uint8_t text1[]={0xB4,0xBA,0xC3,0xDF,0xB2,0xBB,0xBE,0xF5, 0xCF,0xFE,0xA3,0xAC,0xB4,0xA6,0xB4,0xA6
            ,0xCE,0xC5,0xCC,0xE4,0xC4,0xF1,0xA1,0xA3, 0xD2,0xB9,0xC0,0xB4,0xB7,0xE7,0xD3,0xEA
            ,0xC9,0xF9,0xA3,0xAC,0xBB,0xA8,0xC2,0xE4, 0xD6,0xAA,0xB6,0xE0,0xC9,0xD9,0xA1,0xA3};
uint8_t text2[]={0x77,0x77,0x77,0x2E,0x77,0x75,0x68,0x75, 0x6E,0x65,0x77,0x73,0x2E,0x63,0x6E}; //www.wuhunews.cn
uint8_t text3[]={0x36,0x35,0x35,0x33,0x31};//65531

void setup()
{
    Serial.begin(9600);
    syn.voiceVolume=2;
    syn.speed=5;
    syn.ArabOne=false;
    printState(syn.init());
    delay(1000);
}
void loop()
{
    int get;
    get=syn.play(text1,sizeof(text1),1);
    printState(get);
    delay(2000);
    if (syn.pause())
        Serial.println("pause success");
    else
        Serial.println("pause fail");
    delay(3000);
    if (syn.restore())
        Serial.println("restore success");
    else
        Serial.println("restore fail");
    get=syn.play(text2,sizeof(text2),2);
    printState(get);
    delay(5000);
    get=syn.play(text3,sizeof(text3),3);
    printState(get);
    delay(5000);
}

void printState(int stateNumb)
{

    switch (stateNumb)
    {
        case STATE_SEND_ERROR :
            Serial.println("STATE_SEND_ERROR");
            break;
        case STATE_NORMAL :
            Serial.println("STATE_NORMAL");
            break;
        case STATE_SERIAL_ERROR :
            Serial.println("STATE_SERIAL_ERROR");
            break;
        case GET_SUCCESS :
            Serial.println("GET_SUCCESS");
            break;
        case UNKNOW_CMD :
            Serial.println("UNKNOW_CMD");
            break;
        case INIT_SUCCESS :
            Serial.println("INIT_SUCCESS");
            break;
        case BUSYING :
            Serial.println("BUSYING");
            break;
        case WAITING :
            Serial.println("WAITING");
            break;
        default:
            Serial.println("Ops");
            break;
    }
}