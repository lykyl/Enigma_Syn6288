/*
 *    enigmaSyn6288.h - A library for controling Syn6288
 *    Copyright (c) 2021 lykyl
*/

#ifndef Enigma_Syn6288_h
#define Enigma_Syn6288_h
#if defined(ARDUINO) && ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SoftwareSerial.h>

#define HEADLEN      5
#define GB2312_CODE 0x00
#define GBK_CODE 0x01
#define BIG5_CODE 0x02
#define UNICODE_CODE 0x03
#define STATE_SEND_ERROR  -1
#define STATE_NORMAL  0
#define STATE_SERIAL_ERROR  1
#define GET_SUCCESS 0x41
#define UNKNOW_CMD 0x45
#define INIT_SUCCESS 0x4a
#define BUSYING 0x4e
#define WAITING 0X4f

class Enigma_Syn6288
{
  private:
    uint8_t _txPin=12;
    uint8_t _rxPin=13;
    unsigned int _boudr;
    bool _isHardSerial=true;
    byte _dataHead[5] = {0xfd,0x00,0x00,0x01,0x00}; //合成播放命令
    byte _boud[5]={0xfd,0x00,0x03,0x31,0x00};  //设置波特率
    byte _stop[5] = {0xfd,0x00,0x02,0x02,0xfd}; //停止合成命令
    byte _pause[5] = {0xfd,0x00,0x02,0x03,0xfc}; //暂停合成命令
    byte _resume[5] = {0xfd,0x00,0x02,0x04,0xfb}; //恢复合成命令
    byte _ask[5] = {0xfd,0x00,0x02,0x21,0xde}; //状态问询命令
    byte _powerDown[5] = {0xfd,0x00,0x02,0x88,0x77}; //休眠命令
    SoftwareSerial _serial=SoftwareSerial(_rxPin,_txPin);

    uint8_t insertCmd(byte *buff,uint8_t index,char cmd,uint8_t value);
    int8_t setBoud(unsigned int boudr);
    int8_t sendData(byte *data,uint8_t len,bool sendChecker=true,int8_t checker=0);
    int8_t sendCmd(byte *data);

  public:
    unsigned int voiceVolume=8;  //前景音量,0~16
    unsigned int backVolume=2;   //背景音量,0~16
    unsigned int speed=3;    //语带，0～5，5为最高语数
    unsigned int numbType=0;  //数字处理方式，0：自动；1：做为号码；2：做为数值
    bool ArabOne=true;   //1是否读成阿拉伯数字
    bool noticeSound=false;  //播放提示音
    bool readPunctuation=false;    //读出标点符号
    bool readWord=false;   //是否一个字一个字读

    Enigma_Syn6288();
    Enigma_Syn6288(uint8_t rxPin,uint8_t txPin,unsigned int boudr=9600);
    int8_t init();
    int8_t play(byte *content,uint8_t len,uint8_t code=GBK_CODE,uint8_t music=0);
    int8_t setVolume(uint8_t voice,uint8_t back);
    bool stop();
    bool restore();
    bool isBusy();
    bool pause();
    bool sleep();
};
#endif
