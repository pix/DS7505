#ifndef DS7505_H
#define DS7505_H

#include <inttypes.h>
#include <WProgram.h>

/*
* DS7505 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * 3/2011
 */
class DS7505
{
public:
  //Resolution Definition
  //R1 R0
  static const byte RES_09 = 0x0; // 9 bit res;
  static const byte RES_10 = 0x1; //10 bit res;
  static const byte RES_11 = 0x2; //11 bit res;
  static const byte RES_12 = 0x3; //12 bit res;

  //Fault Tolerance Configuration
  //F1 F0
  static const byte FT_1 = 0x0; //fault tolerance consecutive out of limits 1
  static const byte FT_2 = 0x1; //fault tolerance consecutive out of limits 2
  static const byte FT_4 = 0x2; //fault tolerance consecutive out of limits 4
  static const byte FT_6 = 0x3; //fault tolerance consecutive out of limits 6

  //Register Pointer Definition
  //P1 P0
  static const byte P_TEMP = 0x0; //temperature
  static const byte P_CONF = 0x1; //configuration
  static const byte P_THYST = 0x2; //Thyst
  static const byte P_TOS = 0x3; //Tos

  //command set
  static const byte CMD_RECALL_DATA = 0xB8;
  static const byte CMD_COPY_DATA = 0x48;
  static const byte CMD_POR = 0x54;

  DS7505() {};

  //get temperature in Celsius
  float getTempC() { return getTemp(P_TEMP); }

  //get temperature in Fahrenheit
  float getTempF() { return 9.0/5.0 * getTemp(P_TEMP) + 32.0; }

  //get temperature, overloaded (Celsius)
  //	regPdef:
  //		P_TEMP: get temperature in Celsius
  //		P_THYST: get hysteresis temperature
  //		P_OS: get trip temperature
  float getTempC(byte regPdef) { return getTemp(regPdef); }

  //get temperature, overloaded (Fahrenheit)
  //	regPdef:
  //		P_TEMP: get temperature in Celsius
  //		P_THYST: get hysteresis temperature
  //		P_OS: get trip temperature
  float getTempF(byte regPdef) { return 9.0/5.0 * getTemp(regPdef) + 32.0;}

  //set thermostat trip temperature (Celsius)
  //hysteresis is set to be 5 degree below trip temperature
  float setThermostatC(float t) { setThermostatC(t, t - 5.0); }

  //set thermostat temperature (Fahrenheit)
  //hysteresis is set to be 5 degree below trip temperature
  float setThermostatF(float t) { setThermostatF(t, t - 5.0); }

  //set thermostat temperature, overloaded (Celsius)
  //	tos: trip temperature
  //	thyst: hysteresis temperature
  float setThermostatC(float tos, float thyst) { setThermostatC(tos, thyst, FT_1); }

  //set thermostat temperature, overloaded (Fahrenheit)
  //	tos: trip temperature
  //	thyst: hysteresis temperature
  float setThermostatF(float tos, float thyst) { setThermostatF(tos, thyst, FT_1); }

  //set thermostat temperature, overloaded (Celsius)
  //	tos: trip temperature
  //	thyst: hysteresis temperature
  //	ft: fault tolerance (consecutive out-of-limits conversions before tripping)
  //		FT_1, FT_2, FT_4, FT_6
  float setThermostatC(float tos, float thyst, byte ft) { setThermostat(tos, thyst, ft); }

  //set thermostat temperature, overloaded (Fahrenheit)
  //	tos: trip temperature
  //	thyst: hysteresis temperature
  //	ft: fault tolerance (consecutive out-of-limits conversions before tripping)
  //		FT_1, FT_2, FT_4, FT_6
  float setThermostatF(float tos, float thyst, byte ft) { setThermostat((tos - 32.0) * 5.0 / 9.0, (thyst - 32.0) * 5.0 / 9.0, ft); }

  //set configuration register
  //	configByte:
  //	NVB R1 R0 F1 F0 POL TM SD (see DS7505 data-sheet)
  void setConfigRegister(byte configByte);

  //send command
  //	cmdSet:
  //		CMD_RECALL_DATA
  //		CMD_COPY_DATA
  //		CMD_POR
  void sendCommand(byte cmdSet);

  //initialization
  //	a2, a1, a0: the hardware configured I2C address (e.g. B110)
  //	res: temperature resolution (9, 10, 11 or 12 bits)
  void init(byte a2, byte a1, byte a0, byte res);
private:
  byte Resolution;
  byte A2, A1, A0;
  byte I2CAddr;
  byte ConfigByte;

  //get temperature, overloaded (Celsius)
  //	regPdef:
  //		P_TEMP: get temperature in Celsius
  //		P_THYST: get hysteresis temperature
  //		P_OS: get trip temperature
  float getTemp(byte regPdef);

  //set thermostat temperature, overloaded (Celsius)
  //	tos: trip temperature
  //	thyst: hysteresis temperature
  //	ft: fault tolerance (consecutive out-of-limits conversions before tripping)
  //		FT_1, FT_2, FT_4, FT_6
  void setThermostat(float tos, float thyst, byte ft);
};

#endif
