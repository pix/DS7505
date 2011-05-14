#include "DS7505.h"
#include <Wire.h>
#include <math.h>

// Initialize DS7505
// a2, a1, a0 are either HIGH (1) or LOW (0) depending
// on the pin setup.
void DS7505::init(uint8_t a2, uint8_t a1, uint8_t a0, DS7505::Resolution res)
{
  _configByte = res << 5;

  // 1001A2A1A0
  _i2cAddr = 0x48 | (a2 & 0x1) << 2 | (a1 & 0x1) << 1 | (a0 & 0x1);

  setConfigRegister(_configByte);
}

//set configuration byte
//bit 7: NVB (NV Memory Status), read-only
//bit 6: R1 (Conversion Resolution Bit 1)
//bit 5: R0 (Conversion Resolution Bit 0)
//bit 4: F1 (Thermostat Fault Tolerance Bit 1)
//bit 3: F0 (Thermostat Fault Tolerance Bit 0)
//bit 2: POL (Thermostat Output Polarity)
//bit 1: TM (Thermostat Operating Mode)
//bit 0: SD (Shutdown)
void DS7505::setConfigRegister(byte configByte)
{
  Wire.beginTransmission(_i2cAddr);
  Wire.send(P_CONF);
  Wire.send(_configByte);
  Wire.endTransmission();
}

//cmdSet:
//	CMD_RECALL_DATA
//	CMD_COPY_DATA
//	CMD_POR
void DS7505::sendCommand(byte cmdSet)
{
  Wire.beginTransmission(_i2cAddr);
  Wire.send(cmdSet);
  Wire.endTransmission();
}

//get temperature in Celsius
float DS7505::getTemp(DS7505::Register regPdef)
{
  float temp = 0.0;
  float s = 1.0;

  Wire.beginTransmission(_i2cAddr);
  Wire.send(regPdef);
  Wire.endTransmission();
  Wire.requestFrom(_i2cAddr, 2u);

  while (!Wire.available()) {  };

  byte h = Wire.receive();

  while (!Wire.available()) {  };

  byte l = Wire.receive();
  Wire.endTransmission();

  if (h & 0x80 == 0x80) {
    s = -1.0;
    h = h & 0x7f;
  }
  else {
    s = 1.0;
  }

  temp = 0.5 * ((l & 0x80 )>> 7) + 0.25 * ((l & 0x40 )>> 6)+ 0.125 * ((l & 0x20 )>> 5) + 0.0625 * ((l & 0x10 )>> 4) + (float) h;

  return temp;
}

//set thermostat, temperatures are in Celsius
//tos: trip point temperature (must be higher than thyst)
//thyst: hysteresis temperature
//ft: fault tolerance configuration
//	FT_1, FT_2, FT_4, FT_6
void DS7505::setThermostat(float tos, float thyst, FaultTolerance ft)
{
  if (tos < thyst || tos < -55.0 || thyst < -55.0 || tos > 125.0 || thyst > 125.0) return;
  float tosr, thystr;
  byte tos_h, tos_l, thyst_h, thyst_l;

  tos_l = 0;
  thyst_l = 0;

  if (tos >= 0) {
    tos_h = (byte) tos;
  }
  else {
    tos_h = 0x80 & (byte) abs(tos);
  }

  tosr = fabs((double) tos) -  (int) abs(tos);

  if (tosr >= 0.5 ) {
    tos_l |= 0x80;
    tosr -= 0.5;
  }

  if (tosr >= 0.25) {
    tos_l |= 0x40;
    tosr -= 0.25;
  }

  if (tosr >= 0.125) {
    tos_l |= 0x20;
    tosr -= 0.125;
  }

  if (tosr >= 0.0625) {
    tos_l |= 0x10;
  }

  if (thyst >= 0) {
    thyst_h = (byte) thyst;
  }
  else {
    thyst_h = 0x80 & (byte) abs(thyst);
  }

  thystr = fabs((double) thyst) - (int) abs(thyst);

  if (thystr >= 0.5 ) {
    thyst_l |= 0x80;
    thystr -= 0.5;
  }

  if (thystr >= 0.25) {
    thyst_l |= 0x40;
    thystr -= 0.25;
  }

  if (thystr >= 0.125) {
    thyst_l |= 0x20;
    thystr -= 0.125;
  }

  if (thystr >= 0.0625) {
    thyst_l |= 0x10;
  }

  Wire.beginTransmission(_i2cAddr);
  Wire.send(P_TOS);
  Wire.send(tos_h);
  Wire.send(tos_l);
  Wire.endTransmission();

  Wire.beginTransmission(_i2cAddr);
  Wire.send(P_THYST);
  Wire.send(thyst_h);
  Wire.send(thyst_l);
  Wire.endTransmission();

  //clear F1F0 then set F1F0 using ft
  _configByte &= 0xE7;
  _configByte |= ft << 3;

  setConfigRegister(_configByte);
}

