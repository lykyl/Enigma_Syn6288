/*
 *    enigmaSyn6288.cpp - A library for controling Syn6288
 *    Copyright (c) 2021 lykyl
*/

#if defined(ARDUINO) && ARDUINO>=100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <SoftwareSerial.h>
#include "enigmaSyn6288.h"


Enigma_Syn6288::Enigma_Syn6288()
{
  _rxPin=0;
  _txPin=1;
  _boudr=9600;
}

Enigma_Syn6288::Enigma_Syn6288(uint8_t rxPin,uint8_t txPin,unsigned int boudr)
{
  if (rxPin==txPin)
  {
    _rxPin=0;
    _txPin=1;
  }
  else
  {
    if (rxPin>13)
      rxPin=0;
    if (txPin>13)
      txPin=1;
    _rxPin=rxPin;
    _txPin=txPin;
  }
  if (boudr!=9600 && boudr!=19200)
    _boudr=9600;
  else
    _boudr=boudr;
}

int8_t Enigma_Syn6288::sendData(byte *data,uint8_t len,bool sendChecker,int8_t checker)
{
  if (data)
  {
    if (!_isHardSerial)
    {
      if (!(_serial.isListening()))
        _serial.listen();
    }
    for (int i=0;i<len;i++)
    {
      checker^=data[i];
      if (_isHardSerial)
        Serial.write(data[i]);
      else
      {
        _serial.write(data[i]);
      }
    }
    if (sendChecker)
    {
      if (_isHardSerial)
        Serial.write(checker);
      else
        _serial.write(checker);
      //接收SYN6288反馈
      int8_t get=STATE_SEND_ERROR;
      if (_isHardSerial)
      {
        if (Serial.available())
        {
          while (Serial.available())
          {
            get=Serial.read();
          }
        }
      }
      else
      {
        if (_serial.available())
        {
          while (_serial.available())
          {
            get=_serial.read();
          }
        }
      }
      return get;
    }
    return checker;
  }
  return STATE_SEND_ERROR;
}

int8_t Enigma_Syn6288::setBoud(unsigned int boudr)
{
  if (boudr==19200)
  {
    _boud[4]=0x01;
  }
  else
  {
    _boud[4]=0x00;
  }
  return sendData(_boud,HEADLEN);
}

uint8_t Enigma_Syn6288::insertCmd(byte *buff,uint8_t index,char cmd,uint8_t value)
{
  uint8_t newIndex=index;

  buff[newIndex++]='[';
  buff[newIndex++]=cmd;
  if (value>9)
  {
    buff[newIndex++]=0x31;
    value-=10;
  }
  buff[newIndex++]=48+value;
  buff[newIndex++]=']';
  return newIndex;
}

int8_t Enigma_Syn6288::init()
{
  if (_rxPin==0 && _txPin==1)
  {
    Serial.begin(_boudr);
    if (!Serial)
    {
      return STATE_SERIAL_ERROR;
    }
    _isHardSerial=true;
  }
  else
  {
    _serial=SoftwareSerial(_rxPin,_txPin);
    _serial.begin(_boudr);
    _isHardSerial=false;
  }
  byte data[35];    //MAX is 34
  uint8_t f=0;
  if (voiceVolume>16)
    voiceVolume=16;
  f=insertCmd(data,f,'v',voiceVolume);
  if (backVolume>16)
    backVolume=16;
  f=insertCmd(data,f,'m',backVolume);
  if (speed>5)
    speed=5;
  f=insertCmd(data,f,'t',speed);
  if (numbType>2)
    numbType=0;
  f=insertCmd(data,f,'n',numbType);
  f=insertCmd(data,f,'y',ArabOne);
  f=insertCmd(data,f,'x',noticeSound);
  f=insertCmd(data,f,'b',readPunctuation);
  f=insertCmd(data,f,'o',readWord);
  return play(data,f,0);
}

int8_t Enigma_Syn6288::play(byte *content,uint8_t len,uint8_t code,uint8_t music)
{
  if (music>15)
    music=0;
  if (code>3)
    code=3;
  _dataHead[2]=len+3;
  _dataHead[4]=(music<<3)+code;
  int8_t ctmp=sendData(_dataHead,HEADLEN,false);
  return sendData(content,len,true,ctmp);
}

int8_t Enigma_Syn6288::setVolume(uint8_t voice,uint8_t back)
{
  byte data[10];
  int f=0;
  if (voice>16)
    voice=16;
  if (back>16)
    back=16;
  voiceVolume=voice;
  backVolume=back;
  f=insertCmd(data,f,'v',voiceVolume);
  f=insertCmd(data,f,'m',backVolume);
  return play(data,f,0);
}

int8_t Enigma_Syn6288::sendCmd(byte *data)
{
  int8_t get=STATE_SEND_ERROR;

  if (data)
  {
    if (!_isHardSerial)
    {
      if (!(_serial.isListening()))
        _serial.listen();
    }
    for (int i=0;i<HEADLEN;i++)
    {
      if (_isHardSerial)
        Serial.write(data[i]);
      else
      {
        _serial.write(data[i]);
      }
    }
    if (_isHardSerial)
    {
      if (Serial.available())
      {
        while (Serial.available())
        {
          get=Serial.read();
        }
      }
    }
    else
    {
      if (_serial.available())
      {
        while (_serial.available())
        {
          get=_serial.read();
        }
      }
    }
  }
  return get;
}

bool Enigma_Syn6288::isBusy()
{
  int8_t get;
  get=sendCmd(_ask);
  return (get==BUSYING);
}

bool Enigma_Syn6288::stop()
{
  int8_t get;
  get=sendCmd(_stop);
  return (get==GET_SUCCESS);
}

bool Enigma_Syn6288::restore()
{
  int8_t get;
  get=sendCmd(_resume);
  return (get==GET_SUCCESS);
}

bool Enigma_Syn6288::pause()
{
  int8_t get;
  get=sendCmd(_pause);
  return (get==GET_SUCCESS);
  sendCmd(_pause);
}

bool Enigma_Syn6288::sleep()
{
  int8_t get;
  get=sendCmd(_powerDown);
  return (get==GET_SUCCESS);
}